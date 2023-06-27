/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：pm3003function.c                                               **/
/** 版    本：V1.0.1                                                         **/
/** 简    介：实现PM3003S系列激光粉尘传感器的功能                            **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期          作者          说明                           **/
/**     V1.0.0  2022-09-23      木南      创建文件                           **/
/**                                                                          **/
/******************************************************************************/

#include "stddef.h"
#include "pm3003function.h"

/*计算校验和*/
static uint8_t CalculateChecksum(uint8_t *data,uint8_t length);
/*计算传感器编码*/
static void ParsingSensorCode(PM3003ObjectType *pm,uint8_t *rxbuffer);
/*计算测量数据*/
static void ParsingMeasureResult(PM3003ObjectType *pm,uint8_t *rxbuffer);


/*对象初始化配置*/
PM3003SErrorType Pm3003sInitialization(PM3003ObjectType *pm,Pm3003sSendCommandType send)
{
    if((pm==NULL)||(send==NULL))
    {
        return PM3003S_PARAMETER_ERROR;
    }

    pm->SendCommand=send;

    pm->status=0;
    pm->alarm=0;
    for(int i=0;i<5;i++)
    {
        pm->sensorCode[i]=0;
    }
    pm->pm1d0=0;
    pm->pm2d5=0;
    pm->pm10=0;
    pm->tsp=0;
    pm->up0d3um=0;
    pm->up0d5um=0;
    pm->up1d0um=0;
    pm->up2d5um=0;
    pm->up5d0um=0;
    pm->up10um=0;
    pm->dustFactor=0.0;

    return PM3003S_NO_ERROR;
}

/*读取测量结果*/
void Pm3003sReadMeasureResult(PM3003ObjectType *pm)
{
    uint8_t cmd[]={0x11,0x02,0x0B,0x07,0xDB};
    pm->SendCommand(cmd,5);
}

/*停止粉尘测量*/
void Pm3003sTurnoffDustMeasurement(PM3003ObjectType *pm)
{
    uint8_t cmd[]={0x11,0x03,0x0C,0x01,0x1E,0xC1};
    pm->SendCommand(cmd,6);
}

/*开启粉尘测量*/
void Pm3003sTurnonDustMeasurement(PM3003ObjectType *pm)
{
    uint8_t cmd[]={0x11,0x03,0x0C,0x02,0x1E,0xC0};
    pm->SendCommand(cmd,6);
}

/*读取粉尘校准系数*/
void Pm3003sGetDustCalibrationFactor(PM3003ObjectType *pm)
{
    uint8_t cmd[]={0x11,0x01,0x07,0xE7};
    pm->SendCommand(cmd,4);
}

/*设置粉尘校准系数*/
void Pm3003sSetDustCalibrationFactor(PM3003ObjectType *pm,uint8_t data)
{
    uint8_t cmd[]={0x11,0x02,0x07,0x00,0x00};

    cmd[3]=data;
    cmd[4]=CalculateChecksum(cmd,4);

    pm->SendCommand(cmd,5);
}

/*查询传感器编码*/
void Pm3003sQueryingSensorCode(PM3003ObjectType *pm)
{
    uint8_t cmd[]={0x11,0x01,0x1F,0xCF};
    pm->SendCommand(cmd,4);
}

/*解析接收到的报文*/
PM3003SErrorType Pm3003sParsingMessage(PM3003ObjectType *pm,uint8_t *rxbuffer,uint8_t rxLength)
{
    uint8_t dataLength=rxbuffer[1];
    uint8_t cmd=rxbuffer[2];
    uint8_t cs;
    PM3003SErrorType error=PM3003S_NO_ERROR;

    if(rxbuffer[0]!=0x16)   //报文错误
    {
        return PM3003S_HEAD_ERROR;
    }

    if(rxLength<(dataLength+3))    //尚未接收完整
    {
        return PM3003S_INCOMPLETE_ERROR;
    }

    cs=CalculateChecksum(rxbuffer,rxLength-1);

    if(cs!=rxbuffer[rxLength-1])
    {
        return PM3003S_CHECK_ERROR;
    }

    switch (cmd)
    {
    case 0x0B:  //测量结果
        {
            ParsingMeasureResult(pm,rxbuffer);
            break;
        }
    case 0x0C:  //启停状态
        {
            pm->status=rxbuffer[3];
            break;
        }
        
    case 0x07:  //粉尘校准系数
        {
            pm->dustFactor=(float)rxbuffer[3]/(float)1.0;
            break;
        }
    case 0x1F:  //传感器编码
        {
            ParsingSensorCode(pm,rxbuffer);
            break;
        }
    default:
        {
            error=PM3003S_COMMAND_ERROR;
            break;
        }
    }

    return error;
}

/*计算测量数据*/
static void ParsingMeasureResult(PM3003ObjectType *pm,uint8_t *rxbuffer)
{
    pm->pm1d0=(((rxbuffer[3]*256+rxbuffer[4])*256+rxbuffer[5])*256+rxbuffer[6]);
    pm->pm2d5=(((rxbuffer[7]*256+rxbuffer[8])*256+rxbuffer[9])*256+rxbuffer[10]);
    pm->pm10=(((rxbuffer[11]*256+rxbuffer[12])*256+rxbuffer[13])*256+rxbuffer[14]);
    pm->tsp=(((rxbuffer[15]*256+rxbuffer[16])*256+rxbuffer[17])*256+rxbuffer[18]);
    pm->up0d3um=(((rxbuffer[27]*256+rxbuffer[28])*256+rxbuffer[29])*256+rxbuffer[30]);
    pm->up0d5um=(((rxbuffer[31]*256+rxbuffer[32])*256+rxbuffer[33])*256+rxbuffer[34]);
    pm->up1d0um=(((rxbuffer[35]*256+rxbuffer[36])*256+rxbuffer[37])*256+rxbuffer[38]);
    pm->up2d5um=(((rxbuffer[39]*256+rxbuffer[40])*256+rxbuffer[41])*256+rxbuffer[42]);
    pm->up5d0um=(((rxbuffer[43]*256+rxbuffer[44])*256+rxbuffer[45])*256+rxbuffer[46]);
    pm->up10um=(((rxbuffer[47]*256+rxbuffer[48])*256+rxbuffer[49])*256+rxbuffer[50]);
    pm->alarm=rxbuffer[51];
}

/*计算传感器编码*/
static void ParsingSensorCode(PM3003ObjectType *pm,uint8_t *rxbuffer)
{
    pm->sensorCode[0]=rxbuffer[3]*256+rxbuffer[4];
    pm->sensorCode[1]=rxbuffer[5]*256+rxbuffer[6];
    pm->sensorCode[2]=rxbuffer[7]*256+rxbuffer[8];
    pm->sensorCode[3]=rxbuffer[9]*256+rxbuffer[10];
    pm->sensorCode[4]=rxbuffer[11]*256+rxbuffer[12];
}

/*计算校验和*/
static uint8_t CalculateChecksum(uint8_t *data,uint8_t length)
{
    uint8_t checksum=0x00;

    for(int i=0;i<length;i++)
    {
        checksum += data[i];
    }

    checksum=256-checksum;

    return checksum;
}

/*********** (C) COPYRIGHT 1999-2022 Moonan Technology *********END OF FILE****/