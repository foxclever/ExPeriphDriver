/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：sht3xfunction.c                                                **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现SHT3X数字温湿度计的操作                                **/
/**           单次读取命令如下：                                             **/
/**           +---------------+------------+                                 **/
/**           |     条件      |十六进制代码|                                 **/
/**           +------+--------+------+-----+                                 **/
/**           |重复性|时钟拉伸| MSB  | LSB |                                 **/
/**           +------+--------+------+-----+                                 **/
/**           | High | enabled| 0x2C | 06  |                                 **/
/**           +------+--------+------+-----+                                 **/
/**           |Medium| enabled| 0x2C | 0D  |                                 **/
/**           +------+--------+------+-----+                                 **/
/**           |  Low | enabled| 0x2C | 10  |                                 **/
/**           +------+--------+------+-----+                                 **/
/**           | High |disabled| 0x24 | 00  |                                 **/
/**           +------+--------+------+-----+                                 **/
/**           |Medium|disabled| 0x24 | 0B  |                                 **/
/**           +------+--------+------+-----+                                 **/
/**           |  Low |disabled| 0x24 | 16  |                                 **/
/**           +------+--------+------+-----+                                 **/
/**           周期性读取命令如下：                                           **/
/**           +-------------------+------------+                             **/
/**           |        条件       |十六进制代码|                             **/
/**           +------+------------+------+-----+                             **/
/**           |重复性|每秒测量次数| MSB  | LSB |                             **/
/**           +------+------------+------+-----+                             **/
/**           | High |     0.5    | 0x20 |  32 |                             **/
/**           +------+------------+------+-----+                             **/
/**           |Medium|     0.5    | 0x20 |  24 |                             **/
/**           +------+------------+------+-----+                             **/
/**           | Low  |     0.5    | 0x20 |  2F |                             **/
/**           +------+------------+------+-----+                             **/
/**           | High |      1     | 0x21 |  30 |                             **/
/**           +------+------------+------+-----+                             **/
/**           |Medium|      1     | 0x21 |  26 |                             **/
/**           +------+------------+------+-----+                             **/
/**           | Low  |      1     | 0x21 |  2D |                             **/
/**           +------+------------+------+-----+                             **/
/**           | High |      2     | 0x22 |  36 |                             **/
/**           +------+------------+------+-----+                             **/
/**           |Medium|      2     | 0x22 |  20 |                             **/
/**           +------+------------+------+-----+                             **/
/**           | Low  |      2     | 0x22 |  2B |                             **/
/**           +------+------------+------+-----+                             **/
/**           | High |      4     | 0x23 |  34 |                             **/
/**           +------+------------+------+-----+                             **/
/**           |Medium|      4     | 0x23 |  22 |                             **/
/**           +------+------------+------+-----+                             **/
/**           | Low  |      4     | 0x23 |  29 |                             **/
/**           +------+------------+------+-----+                             **/
/**           | High |      10    | 0x27 |  37 |                             **/
/**           +------+------------+------+-----+                             **/
/**           |Medium|      10    | 0x27 |  21 |                             **/
/**           +------+------------+------+-----+                             **/
/**           | Low  |      10    | 0x27 |  2A |                             **/
/**           +------+------------+------+-----+                             **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    1、声明SHT3xObject对象实体，并调用函数SHT3xInitialization初始化对象   **/
/**       实体。初始化时，指定设备地址：0x44,0x45,0x88和0x8A或使用相应的宏   **/
/**       定义：I2CADDRESSA、I2CADDRESSB、I2CADDRESSC和I2CADDRESSD           **/
/**    2、实现形如void f(uint8_t devAddress,uint8_t *rData,uint16_t rSize)的 **/
/**       函数，并将其赋值给SHT3xObject对象实体的Receive函数指针。实现数据接 **/
/**       收。                                                               **/
/**    3、实现形如void f(uint8_t devAddress,uint8_t *tData,uint16_t tSize)的 **/
/**       函数，并将其赋值给SHT3xObject对象实体的Transmit函数指针。实现数据  **/
/**       发送。                                                             **/
/**    4、实现形如void f(volatile uint32_t nTime)的函数，并将其赋值给        **/
/**       SHT3xObject对象实体的Delay函数指针。实现操作延时，单位毫秒         **/
/**    5、调用相应的函数实现需要的操作。                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2018-12-01          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "sht3xfunction.h"

const uint16_t SHT3xPOLYNOMIAL = 0x31;      //定义校验多项式P(x) = x^8 + x^5 + x^4 + 1 = 100110001

/*根据指定的多项式计算CRC校验*/
static SHT3xErrorType CheckCRC8ForSHT3x(uint8_t *data, uint8_t numOfBytes, uint8_t checksum);
/* 读取SHT3x对象的序列号*/
static SHT3xErrorType SHT3xReadSerialNumber(SHT3xObjectType *sht);
/* 向SHT3x发送命令 */
static void SHT3xWriteCommand(SHT3xObjectType *sht,SHT3xCommands cmd);
/* 计算摄氏温度最终值*/
static float CalcTemperatureValue(uint16_t code);
/* 计算湿度最终值 */
static float CalcHumidityValue(uint16_t code);

/* 获取一次性采集 */
SHT3xErrorType SHT3xGetSingleShotData(SHT3xObjectType *sht,SHT3xRepeatability repeatability,SHT3xMode mode)
{
    SHT3xErrorType error=SHT3X_NO_ERROR;
    uint8_t data[6];
    uint16_t tempCode;
    uint16_t humiCode;
    
    SHT3xCommands commands[2][3]={{CMD_MEAS_CLOCKSTR_H,  // 单次数据采集模式，时钟延展，高重复性
    CMD_MEAS_CLOCKSTR_M,  // 单次数据采集模式，时钟延展，中重复性
    CMD_MEAS_CLOCKSTR_L,  // 单次数据采集模式，时钟延展，低重复性
    },
    {CMD_MEAS_POLLING_H,   // 单次数据采集模式，轮询，高重复性
    CMD_MEAS_POLLING_M,   // 单次数据采集模式，轮询，中重复性
    CMD_MEAS_POLLING_L,   // 单次数据采集模式，轮询，低重复性
    }};
    
    SHT3xWriteCommand(sht,commands[mode][repeatability]);
    
    if(mode==MODE_CLKSTRETCH)
    {
        sht->Delayms(1);
    }
    else if(mode==MODE_POLLING)
    {
        sht->Delayms(20);
    }
    
    sht->Receive(sht,data,6);
    
    error|=CheckCRC8ForSHT3x(&data[0],2,data[2]);
    
    error|=CheckCRC8ForSHT3x(&data[3],2,data[5]);
    
    if(error==SHT3X_NO_ERROR)
    {
        tempCode=(data[0]<<8)+data[1];
        humiCode=(data[3]<<8)+data[4];
        
        sht->temperature=CalcTemperatureValue(tempCode);
        sht->humidity=CalcHumidityValue(humiCode);
    }
    
    return error;
}

/* 启动周期性数据采集 */
void SHT3xStartPeriodicMeasurment(SHT3xObjectType *sht,SHT3xRepeatability repeatability,SHT3xFrequency frequency)
{
    SHT3xCommands commands[5][3]={{CMD_MEAS_PERI_05_H,   // 周期数据采集模式，0.5 mps, 高重复性
    CMD_MEAS_PERI_05_M,   // 周期数据采集模式，0.5 mps, 中重复性
    CMD_MEAS_PERI_05_L,   // 周期数据采集模式，0.5 mps, 低重复性
    },
    {CMD_MEAS_PERI_1_H,    // 周期数据采集模式，1 mps, 高重复性
    CMD_MEAS_PERI_1_M,    // 周期数据采集模式，1 mps, 中重复性
    CMD_MEAS_PERI_1_L,    // 周期数据采集模式，1 mps, 低重复性
    },
    {CMD_MEAS_PERI_2_H,    // 周期数据采集模式，2 mps, 高重复性
    CMD_MEAS_PERI_2_M,    // 周期数据采集模式，2 mps, 中重复性
    CMD_MEAS_PERI_2_L,    // 周期数据采集模式，2 mps, 低重复性
    },
    {CMD_MEAS_PERI_4_H,    // 周期数据采集模式，4 mps, 高重复性
    CMD_MEAS_PERI_4_M,    // 周期数据采集模式，4 mps, 中重复性
    CMD_MEAS_PERI_4_L,    // 周期数据采集模式，4 mps, 低重复性
    },
    {CMD_MEAS_PERI_10_H,   // 周期数据采集模式，10 mps, 高重复性
    CMD_MEAS_PERI_10_M,   // 周期数据采集模式，10 mps, 中重复性
    CMD_MEAS_PERI_10_L,   // 周期数据采集模式，10 mps, 低重复性
    }};
    
    SHT3xWriteCommand(sht,commands[frequency][repeatability]);
}

/* 读取周期性采集的数据 */
SHT3xErrorType SHT3xFetchPeriodicMeasurmentData(SHT3xObjectType *sht)
{
    SHT3xErrorType error=SHT3X_NO_ERROR;
    uint8_t data[6];
    uint16_t tempCode;
    uint16_t humiCode;
    
    SHT3xWriteCommand(sht,CMD_FETCH_DATA);
    
    sht->Delayms(1);
    
    sht->Receive(sht,data,6);
    
    error|=CheckCRC8ForSHT3x(&data[0],2,data[2]);
    
    error|=CheckCRC8ForSHT3x(&data[3],2,data[5]);
    
    if(error==SHT3X_NO_ERROR)
    {
        tempCode=(data[0]<<8)+data[1];
        humiCode=(data[3]<<8)+data[4];
        
        sht->temperature=CalcTemperatureValue(tempCode);
        sht->humidity=CalcHumidityValue(humiCode);
    }
    
    return error;
}

/* 设置周期采集使用ART */
void SHT3xARTCommand(SHT3xObjectType *sht)
{
    SHT3xWriteCommand(sht,CMD_ART);
}

/* 停止周期采集模式*/
void SHT3xBreakCommand(SHT3xObjectType *sht)
{
    SHT3xWriteCommand(sht,CMD_BREAK_STOP);
}


/* SHT3x接口复位 */
void SHT3xInterfaceReset(SHT3xObjectType *sht)
{
    //SDA发送不少于9个时钟周期的低电平
    uint8_t cmdWrite[2];
    cmdWrite[0]=0x00;
    cmdWrite[1]=0x00;
    
    sht->Transmit(sht,cmdWrite,2);
}

/* SHT3x广播复位 */
void SHT3xGeneralCallReset(SHT3xObjectType *sht)
{
    uint8_t cmd=0x06;
    sht->Transmit(0x00,&cmd,1);
}

/* SHT3x软件复位 */
void SHT3xSoftReset(SHT3xObjectType *sht)
{
    SHT3xWriteCommand(sht,CMD_SOFT_RESET);
}

/* SHT3x启用加热器 */
void SHT3xEnableHeater(SHT3xObjectType *sht)
{
    SHT3xWriteCommand(sht,CMD_HEATER_ENABLE);
}

/* SHT3x禁用加热器 */
void SHT3xDisableHeater(SHT3xObjectType *sht)
{
    SHT3xWriteCommand(sht,CMD_HEATER_DISABLE);
}

/* 读取状态寄存器的值 */
SHT3xErrorType SHT3xReadStatusRegister(SHT3xObjectType *sht,SHT3xStatusRegister *status)
{
    SHT3xErrorType error=SHT3X_NO_ERROR;
    uint8_t sta[3];
    
    SHT3xWriteCommand(sht,CMD_READ_STATUS);
    
    sht->Delayms(1);
    
    sht->Receive(sht,sta,3);
    
    error|=CheckCRC8ForSHT3x(&sta[0],2,sta[2]);
    
    if(error==SHT3X_NO_ERROR)
    {
        status->word=(sta[0]<<8)+sta[1];
    }
    else
    {
        status->word=0;
    }
    
    return error;
}

/* 清除状态寄存器的状态标志 */
void SHT3xClearStatusRegister(SHT3xObjectType *sht)
{
    SHT3xWriteCommand(sht,CMD_CLEAR_STATUS);
}

/* 向SHT3x发送命令 */
static void SHT3xWriteCommand(SHT3xObjectType *sht,SHT3xCommands cmd)
{
    uint8_t cmdWrite[2];
    cmdWrite[0]=(uint8_t)(cmd>>8);
    cmdWrite[1]=(uint8_t)cmd;
    
    sht->Transmit(sht,cmdWrite,2);
}

/* 读取SHT3x对象的序列号*/
static SHT3xErrorType SHT3xReadSerialNumber(SHT3xObjectType *sht)
{
    SHT3xErrorType error=SHT3X_NO_ERROR;
    uint8_t sn[6];
    
    SHT3xWriteCommand(sht,CMD_READ_SERIALNBR);
    
    sht->Delayms(1);
    
    sht->Receive(sht,sn,6);
    
    error|=CheckCRC8ForSHT3x(&sn[0],2,sn[2]);
    
    error|=CheckCRC8ForSHT3x(&sn[3],2,sn[5]);
    
    if(error==SHT3X_NO_ERROR)
    {
        sht->serialNumber=(sn[0]<<24)+(sn[1]<<16)+(sn[3]<<8)+sn[4];
    }
    else
    {
        sht->serialNumber=0;
    }
    
    return error;
}

/* SHT3x对象初始化 */
SHT3xErrorType SHT3xInitialization(SHT3xObjectType *sht,        //待初始化的SHT3X
                                   uint8_t address,             //设备地址
                                   SHT3xReceive receive,        //接收数据函数指针
                                   SHT3xTransmit transmit,      //发送数据函数指针
                                   SHT3xDelayms delayms         //毫秒延时函数指针
                                       )
{
    SHT3xErrorType error=SHT3X_NO_ERROR;
    
    if((sht==NULL)||(receive==NULL)||(transmit==NULL)||(delayms==NULL))
    {
        return SHT3X_INIT_ERROR;
    }
    sht->Receive=receive;
    sht->Transmit=transmit;
    sht->Delayms=delayms;
    
    sht->temperature=0.0;
    sht->humidity=0.0;
    
    if((address==0x44)||(address==0x45))
    {
        sht->devAddress=(address<<1);
    }
    else if((address==0x88)||(address==0x8A))
    {
        sht->devAddress=address;
    }
    else
    {
        sht->devAddress=0;
        error|=SHT3X_PARM_ERROR;
    }
    
    if(error==SHT3X_NO_ERROR)
    {
        error|=SHT3xReadSerialNumber(sht);
    }
    
    return error;
}

/*根据指定的多项式计算CRC校验*/
static SHT3xErrorType CheckCRC8ForSHT3x(uint8_t *data, uint8_t numOfBytes, uint8_t checksum)
{
    uint8_t crc = 0xFF;
    uint8_t bit;
    SHT3xErrorType error=SHT3X_NO_ERROR;
    
    for(uint8_t i = 0; i < numOfBytes; i++)
    {
        crc ^= (data[i]);
        for ( bit = 8; bit > 0; --bit)
        {
            if (crc & 0x80)
            {
                crc = (crc << 1) ^ SHT3xPOLYNOMIAL;
            }
            else
            {
                crc = (crc << 1);
            }
        }
    }
    
    if(crc != checksum)
    {
        error|=SHT3X_CHECKSUM_ERROR;
    }
    
    return error;
}

/* 计算湿度最终值 */
static float CalcHumidityValue(uint16_t code)
{
    float result=0.0;
    result=code/65535.0*100.0;
    return result;
}

/* 计算摄氏温度最终值*/
static float CalcTemperatureValue(uint16_t code)
{
    float result=0.0;
    result=code/65535.0*175.0-45.0;
    return result;
}

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/
