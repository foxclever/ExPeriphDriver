/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：sdp800function.h                                               **/
/** 版    本：V1.1.2                                                         **/
/** 简    介：实现SDP800系列差压传感器的功能                                 **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期          作者          说明                           **/
/**     V1.0.0  2022-09-08      木南      创建文件                           **/
/**                                                                          **/
/******************************************************************************/

#include "stddef.h"
#include "sdp800function.h"

/*定义SDP800传感器命令枚举*/
typedef enum {
    //无操作命令
    COMMAND_UNDEFINED = 0x0000,
    //启动带温度补偿的质量流量持续平均值连续测量
    COMMAND_START_MEASUREMENT_MF_AVERAGE = 0x3603,
    //启动带温度补偿的质量流量1ms更新值连续测量
    COMMAND_START_MEASUREMENT_MF_NONE = 0x3608,
    //启动带温度补偿的差压持续平均值连续测量
    COMMAND_START_MEASUREMENT_DP_AVERAGE = 0x3615,
    //启动带温度补偿的差压1ms更新值连续测量
    COMMAND_START_MEASUREMENT_DP_NONE = 0x361E,
    //停止连续测量
    COMMAND_STOP_CONTINOUS_MEASUREMENT = 0x3FF9,
    //触发单次无时钟拉伸流量测量
    COMMAND_TRIGGERED_MEASUREMENT_MF_STRETCH = 0x3624,
    //触发单次有时钟拉伸流量测量
    COMMAND_TRIGGERED_MEASUREMENT_MF_POLL = 0x3726,
    //触发单次无时钟拉伸差压测量
    COMMAND_TRIGGERED_MEASUREMENT_DP_STRETCH = 0x362F,
    //触发单次有时钟拉伸差压测量
    COMMAND_TRIGGERED_MEASUREMENT_DP_POLL = 0x372D,
    //软件复位
    COMMAND_SOFT_RESET = 0x0006,
    //进入睡眠模式
    COMMAND_ENTER_SLEEP_MODE = 0x3677,
    //读产品标识符
    COMMAND_READ_PRODUCT_IDENTIFIER1 = 0x367C,
    COMMAND_READ_PRODUCT_IDENTIFIER2 = 0xE102
} SDP800Command;

/*读取设备编号*/
static SDP800ErrorType Sdp800ReadSerialNumber(SDP800ObjectType *sdp);
/* 向SDP800发送命令 */
static void Sdp800WriteCommand(SDP800ObjectType *sdp,SDP800Command cmd);
/*验证校验码*/
static SDP800ErrorType CheckCRC8ForSDP800(const uint8_t data[], uint8_t size, uint8_t checksum);

/*SDP800对象初始化配置*/
SDP800ErrorType Sdp800Initialization(SDP800ObjectType *sdp,     //SDP800对象
                                     uint8_t i2cAddress,        //设备地址
                                     SDP800Receive recieve,     //接收函数指针
                                     SDP800Transmit transmit,   //发送函数指针
                                     SDP800Delayms delayms      //毫秒演示函数
                                         )
{
    SDP800ErrorType error=SDP800_ERROR_NONE;
    
    if((sdp==NULL)||(recieve==NULL)||(transmit==NULL)||(delayms==NULL))
    {
        return SDP800_ERROR_IVALID_PARAMETER;
    }
    
    sdp->Receive=recieve;
    sdp->Transmit=transmit;
    sdp->Delayms=delayms;
    
    sdp->temperature=0.0;
    sdp->dpressure=0.0;
    
    if((i2cAddress==0x25)||(i2cAddress==0x26))
    {
        sdp->devAddress=(i2cAddress<<1);
    }
    else if((i2cAddress==0x4A)||(i2cAddress==0x4C))
    {
        sdp->devAddress=i2cAddress;
    }
    else
    {
        sdp->devAddress=0;
        error|=SDP800_ERROR_IVALID_PARAMETER;
    }
    
    if(error==SDP800_ERROR_NONE)
    {
        error|=Sdp800ReadSerialNumber(sdp);
    }
    
    return error;
}

/*连续读取测量值*/
SDP800ErrorType Sdp800ReadContinousMeasurement(SDP800ObjectType *sdp)
{
    SDP800ErrorType error=SDP800_ERROR_NONE;
    uint8_t rDatas[9];
    int16_t  diffPressureTicks;
    int16_t  temperatureTicks;
    uint16_t scaleFactorDiffPressure;
    
    sdp->Receive(sdp,rDatas,9);
    
    if((rDatas[0]==0xFF)&&(rDatas[1]==0xFF)&&(rDatas[2]==0xAC)
       &&(rDatas[3]==0xFF)&&(rDatas[4]==0xFF)&&(rDatas[5]==0xAC)
           &&(rDatas[6]==0xFF)&&(rDatas[7]==0xFF)&&(rDatas[8]==0xAC))
    {
        sdp->status=0;
        return SDP800_ERROR_ACK;
    }
    
    error|=CheckCRC8ForSDP800(&rDatas[0],2,rDatas[2]);
    error|=CheckCRC8ForSDP800(&rDatas[3],2,rDatas[5]);
    error|=CheckCRC8ForSDP800(&rDatas[6],2,rDatas[8]);
    
    if(error==SDP800_ERROR_NONE)
    {
        diffPressureTicks=rDatas[0]*256+rDatas[1];
        temperatureTicks=rDatas[3]*256+rDatas[4];
        scaleFactorDiffPressure=rDatas[6]*256+rDatas[7];
        
        sdp->temperature=(float)temperatureTicks/200.0;
        sdp->dpFactor=(float)scaleFactorDiffPressure;
        sdp->dpressure=(float)diffPressureTicks/sdp->dpFactor;
    }

    return error;
}

/*启动连续测量*/
SDP800ErrorType Sdp800StartContinousMeasurement(SDP800ObjectType *sdp,Sdp800TempCompType tempComp,Sdp800AveragingType averaging)
{
    SDP800ErrorType error=SDP800_ERROR_NONE;
    SDP800Command commands[2][2]={{COMMAND_START_MEASUREMENT_MF_AVERAGE,COMMAND_START_MEASUREMENT_MF_NONE},
    {COMMAND_START_MEASUREMENT_DP_AVERAGE,COMMAND_START_MEASUREMENT_DP_NONE}};
    
    switch (commands[tempComp][averaging])
    {
    case COMMAND_START_MEASUREMENT_MF_AVERAGE:
        {
            sdp->status=1;
            break;
        }
    case COMMAND_START_MEASUREMENT_MF_NONE:
        {
            sdp->status=2;
            break;
        }
    case COMMAND_START_MEASUREMENT_DP_AVERAGE:
        {
            sdp->status=3;
            break;
        }
        
    case COMMAND_START_MEASUREMENT_DP_NONE:
        {
            sdp->status=4;
            break;
        }
        
    default:
        {
            sdp->status=0;
            error=SDP800_ERROR_IVALID_PARAMETER;
            break;
        }
    }
    
    if(SDP800_ERROR_NONE==error)
    {
        Sdp800WriteCommand(sdp,commands[tempComp][averaging]);
        sdp->Delayms(20);
    }
    
    if(SDP800_ERROR_NONE!=error)
    {
        sdp->status=0;
    }
    
    return error;
}

/*停止连续测量*/
SDP800ErrorType Sdp800StopContinousMeasurement(SDP800ObjectType *sdp)
{
    Sdp800WriteCommand(sdp,COMMAND_STOP_CONTINOUS_MEASUREMENT);
    
    return SDP800_ERROR_NONE;
}

/*软件复位*/
SDP800ErrorType Sdp800SoftReset(SDP800ObjectType *sdp)
{
    Sdp800WriteCommand(sdp,COMMAND_ENTER_SLEEP_MODE);
    // 等待 20 ms
    sdp->Delayms(20); 
    
    return SDP800_ERROR_NONE;
}

/*进入休眠模式*/
SDP800ErrorType SDP800EnterSleepMode(SDP800ObjectType *sdp)
{
    Sdp800WriteCommand(sdp,COMMAND_ENTER_SLEEP_MODE);
    
    return SDP800_ERROR_NONE;
}

/*退出休眠模式*/
SDP800ErrorType SDP800ExitSleepMode(SDP800ObjectType *sdp)
{
    Sdp800WriteCommand(sdp,COMMAND_UNDEFINED);
    
    return SDP800_ERROR_NONE;
}

/* 向SDP800发送命令 */
static void Sdp800WriteCommand(SDP800ObjectType *sdp,SDP800Command cmd)
{
    uint8_t cmdWrite[2];
    cmdWrite[0]=(uint8_t)(cmd>>8);
    cmdWrite[1]=(uint8_t)cmd;
    
    sdp->Transmit(sdp,cmdWrite,2);
}

/*验证校验码*/
static SDP800ErrorType CheckCRC8ForSDP800(const uint8_t data[], uint8_t size, uint8_t checksum)
{
    uint8_t crc = 0xFF;
    
    //计算CRC8，特征多项式：0x31 (x^8 + x^5 + x^4 + 1)
    for(int i = 0; i < size; i++) {
        crc ^= (data[i]);
        for(uint8_t bit = 8; bit > 0; --bit) {
            if(crc & 0x80) crc = (crc << 1) ^ 0x31;
            else           crc = (crc << 1);
        }
    }
    
    //验证校验码
    return (crc == checksum) ? SDP800_ERROR_NONE : SDP800_ERROR_CHECKSUM;
}

/*读取设备编号*/
static SDP800ErrorType Sdp800ReadSerialNumber(SDP800ObjectType *sdp)
{
    SDP800ErrorType error=SDP800_ERROR_NONE;
    uint8_t data[18];
    
    Sdp800WriteCommand(sdp,COMMAND_READ_PRODUCT_IDENTIFIER1);
    Sdp800WriteCommand(sdp,COMMAND_READ_PRODUCT_IDENTIFIER2);
    
    sdp->Delayms(1);
    
    sdp->Receive(sdp,data,18);
    
    error|=CheckCRC8ForSDP800(&data[0],2,data[2]);
    
    error|=CheckCRC8ForSDP800(&data[3],2,data[5]);
    
    error|=CheckCRC8ForSDP800(&data[6],2,data[8]);
    
    error|=CheckCRC8ForSDP800(&data[9],2,data[11]);
    
    error|=CheckCRC8ForSDP800(&data[12],2,data[14]);
    
    error|=CheckCRC8ForSDP800(&data[15],2,data[17]);
    
    if(error==SDP800_ERROR_NONE)
    {
        sdp->pn[3]=data[0];
        sdp->pn[2]=data[1];
        sdp->pn[1]=data[3];
        sdp->pn[0]=data[4];
        sdp->sn[7]=data[6];
        sdp->sn[6]=data[7];
        sdp->sn[5]=data[9];
        sdp->sn[4]=data[10];
        sdp->sn[3]=data[12];
        sdp->sn[2]=data[13];
        sdp->sn[1]=data[15];
        sdp->sn[0]=data[16];
    }
    
    return error;
}

/*********** (C) COPYRIGHT 1999-2022 Moonan Technology *********END OF FILE****/
