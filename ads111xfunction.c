/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ads111xfunction.c                                              **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：声明和定义适用于TI公司ADS111x系列模数转换芯片的函数和操作。    **/
/**           采用I2C接口，ADS111x个寄存器如下：                             **/
/**     +----+------------------------+----+------+-----+                    **/
/**     |编号|          寄存器        |地址|复位值| 位数|                    **/
/**     +----+------------------------+----+------+-----+                    **/
/**     |  1 |Address Pointer Register|N/A | N/A  | 7Bit|                    **/
/**     +----+------------------------+----+------+-----+                    **/
/**     |  2 |Conversion Register     | 0H |0000h |16Bit|                    **/
/**     +----+------------------------+----+------+-----+                    **/
/**     |  3 |Config Register         | 1H |8583h |16Bit|                    **/
/**     +----+------------------------+----+------+-----+                    **/
/**     |  4 |Lo_thresh Register      | 2H |8000h |16Bit|                    **/
/**     +----+------------------------+----+------+-----+                    **/
/**     |  5 |Hi_thresh Register      | 3H |7FFFh |16Bit|                    **/
/**     +----+------------------------+----+------+-----+                    **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2021-02-15          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "ads111xfunction.h"

/*定义ADS111寄存器枚举*/
typedef enum Ads111xRegister {
    ConversionRegister=0,
    ConfigRegister=1,
    Lo_threshRegister=2,
    Hi_threshRegister=3
}Ads111xRegisterType;

/*MCP4725通用操作*/
static void Ads111xReset(Ads111xObjectType *ads);
/*读ADS111x寄存器*/
static uint16_t Ads111xReadRegister(Ads111xObjectType *ads,Ads111xRegisterType reg);
/*写ADS111x寄存器*/
static void Ads111xWriteRegister(Ads111xObjectType *ads,Ads111xRegisterType reg,uint16_t regValue);

/*获取ADS111x转换数据*/
void Ads111xGetDataCode(Ads111xObjectType *ads,Ads111xChannelType channel)
{
    uint16_t channels[]={0x0000,0x1000,0x2000,0x3000,0x4000,0x5000,0x6000,0x7000};
    Ads111xChannelType ch;
    uint16_t config;
    
    ads->config=Ads111xReadRegister(ads,ConfigRegister);
    
    if(((ads->config)&0x8000)==0x0000)
    {
        return;
    }
    
    ch=(Ads111xChannelType)((ads->config>>12)&0x07);
    ads->dataCode[ch]=Ads111xReadRegister(ads,ConversionRegister);

    config=(((ads->config)&0x0FFF)|0x8000);
    config=config|channels[channel];
    Ads111xWriteRegister(ads,ConfigRegister,config);
}

/*计算AD11x数据对应的物理量值*/
float Ads111xCalcPhysicalValue(Ads111xObjectType *ads,Ads111xChannelType channel,float phyRange,float phyZero)
{
    float result=0.0;
    uint16_t dataCode=0;
    
    if(ads->dataCode[channel] > 0x7FFF)
    {
        dataCode=(0xFFFF-ads->dataCode[channel])+0x01;
        result=0.0-((float)dataCode*(phyRange-phyZero)/32768.0+phyZero);
        return result;
    }
    
    dataCode=ads->dataCode[channel];
    result=(float)dataCode*(phyRange-phyZero)/32767.0+phyZero;
    return result;
}

/*ADS111x初始化配置*/
void Ads111xInitialization(Ads111xObjectType *ads,  //ADS111x对象变量
                           uint8_t devAddress,      //设备地址
                           Ads111xGainType gain,    //增益
                           Ads111xDataRateType dr,  //输出速率
                           Ads111xTransmit transmit,//发送函数指针
                           Ads111xReceive receive,  //接收函数指针
                           Ads111xDelayus delayus   //us延时函数指针
                           )
{
    uint16_t channels[]={0x0000,0x1000,0x2000,0x3000,0x4000,0x5000,0x6000,0x7000};
    uint16_t gains[]={0x0000,0x0200,0x0400,0x0600,0x0800,0x0A00};
    uint16_t dataRates[]={0x0000,0x0020,0x0040,0x0060,0x0080,0x00A0,0x00C0,0x00E0};
    uint16_t config=0x8103;

    if((ads==NULL)||(transmit==NULL)||(receive==NULL)||(delayus==NULL))
    {
        return ;
    }
    
    ads->Transmit=transmit;
    ads->Receive=receive;
    ads->Delayus=delayus;
    
    Ads111xReset(ads);
    
    if((devAddress==0x48)||(devAddress==0x49)||(devAddress==0x4A)||(devAddress==0x4B))
    {
        ads->devAddress=(devAddress<<1);
    }
    else if((devAddress==0x90)||(devAddress==0x92)||(devAddress==0x94)||(devAddress==0x96))
    {
        ads->devAddress=devAddress;
    }
    else
    {
        ads->devAddress=0x00;
    }

    config=config|channels[ADS111X_AIN0_AIN1]|gains[gain]|dataRates[dr];
    
    Ads111xWriteRegister(ads,ConfigRegister,config);
    ads->Delayus(200);
    ads->config=Ads111xReadRegister(ads,ConfigRegister);
}

/*读ADS111x寄存器*/
static uint16_t Ads111xReadRegister(Ads111xObjectType *ads,Ads111xRegisterType reg)
{
     uint8_t wData;
     uint8_t rData[2];
     uint16_t result=0;
     
     wData=(uint8_t)reg;
     
     ads->Transmit(ads,&wData,1);
     ads->Delayus(200);
     ads->Receive(ads,rData,2);
     
     result=rData[0];
     result=(result<<8)+rData[1];
     
     return result;
}

/*写ADS111x寄存器*/
static void Ads111xWriteRegister(Ads111xObjectType *ads,Ads111xRegisterType reg,uint16_t regValue)
{
    uint8_t wData[3];
    
    wData[0]=(uint8_t)reg;
    wData[1]=(uint8_t)(regValue>>8);
    wData[2]=(uint8_t)regValue;
    
    ads->Transmit(ads,wData,3);
}

/*MCP4725通用操作*/
static void Ads111xReset(Ads111xObjectType *ads)
{
    uint8_t wData=0x06;
    uint8_t devAdd=ads->devAddress;
    
    ads->devAddress=0x00;
    
    ads->Transmit(ads,&wData,1);
    
    ads->devAddress=devAdd;
}

/*********** (C) COPYRIGHT 1999-2021 Moonan Technology *********END OF FILE****/