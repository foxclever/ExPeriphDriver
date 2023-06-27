/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ds18b20function.c                                              **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：实现温度检测模块DS18B20的数据读取                              **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2012-07-28          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "ds18b20function.h"

#define RESOLUTION_BITS9        0x1F
#define RESOLUTION_BITS10       0x3F
#define RESOLUTION_BITS11       0x5F
#define RESOLUTION_BITS12       0x7F

#define Search_Rom      0xF0
#define Read_Rom        0x33
#define Match_Rom       0x55
#define Skip_Rom        0xCC
#define Alarm_Search    0xEC

#define Convert_T       0x44
#define Write_Scratchpad        0x4E
#define Read_Scratchpad		0xBE
#define Copy_Scratchpad		0x48
#define Recall_EE               0xB8
#define Read_Power_Supply	0xB4

//CRC = X8 + X5 + X4 + 1

/*主机给从机发送复位脉冲*/
static void ResetDs18b20(Ds18b20ObjectType *ds18b20);
/*检测从机给主机返回的存在脉冲 0：成功;1：失败*/
static uint8_t PresenceDs18b20(Ds18b20ObjectType *ds18b20);
/*从DS18B20读取一个位,返回值：1/0*/
static uint8_t ReadBitFromDs18b20(Ds18b20ObjectType *ds18b20);
/*向DS18B20写一个字节*/
static void WriteByteToDs1820(Ds18b20ObjectType *ds18b20,uint8_t commond);
/*从DS18B20读取一个字节*/
static uint8_t ReadByteFromDs18b20(Ds18b20ObjectType *ds18b20);
/*设置单总线输入输出模式的缺省函数*/
static void SetPinModeDefault(Ds18b20IOModeType mode);
/*读取DS18B20对象的序列号*/
static void GetDs18b20SerialNumber(Ds18b20ObjectType *ds18b20);

/*温度数据获取、转换函数*/
float GetDS18b20TemperatureValue(Ds18b20ObjectType *ds18b20)
{
    uint8_t temph,temp1;
    uint16_t itemp;
    float tFactor=0.0625;
    
    ResetDs18b20(ds18b20);        //复位DS18B20
    PresenceDs18b20(ds18b20);
    WriteByteToDs1820(ds18b20,Skip_Rom);  //发跳过ROM匹配命令
    WriteByteToDs1820(ds18b20,Convert_T); //发温度转换命令
    
    ResetDs18b20(ds18b20);        //复位DS18B20
    PresenceDs18b20(ds18b20);
    WriteByteToDs1820(ds18b20,Skip_Rom);  //发跳过ROM匹配命令
    WriteByteToDs1820(ds18b20,Read_Scratchpad);   //发出读温度命令
    
    temp1=ReadByteFromDs18b20(ds18b20);   //将读出的温度数据低位字节
    temph=ReadByteFromDs18b20(ds18b20);   //将读出的温度数据高位字节
    itemp=(temph<<8) | temp1;
    
    if(itemp<=0x7FF)
    {
        ds18b20->temperature=(float)itemp*tFactor;
    }
    else
    {
        itemp=~itemp+1;
        ds18b20->temperature=0.0-(float)itemp*tFactor;
    }
    
    return ds18b20->temperature;
}

/*主机给从机发送复位脉冲*/
static void ResetDs18b20(Ds18b20ObjectType *ds18b20)
{
    /* 主机设置为推挽输出*/
    ds18b20->SetPinMode(DS18B20_Out);
    /* 主机至少产生480us 的低电平复位信号*/
    ds18b20->SetBit(DS18B20_Reset);
    ds18b20->Delayus(550);
    /* 主机在产生复位信号后，需将总线拉高*/
    ds18b20->SetBit(DS18B20_Set);
    /*从机接收到主机的复位信号后，会在15~60us 后给主机发一个存在脉冲*/
    ds18b20->Delayus(15);
}

/*检测从机给主机返回的存在脉冲 0：成功;1：失败*/
static uint8_t PresenceDs18b20(Ds18b20ObjectType *ds18b20)
{
    uint8_t pulse_time = 0;
    /* 主机设置为上拉输入*/
    ds18b20->SetPinMode(DS18B20_In);
    
    /* 等待存在脉冲的到来，存在脉冲为一个60~240us 的低电平信号*/
    /*如果存在脉冲没有来则做超时处理，从机接收到主机的复位信号后，会在15~60us 后给主机发一个存在脉冲*/
    while( ds18b20->GetBit() && pulse_time<100 )
    {
        pulse_time++;
        ds18b20->Delayus(1);
    }
    /* 经过100us 后，存在脉冲都还没有到来*/
    if( pulse_time >=100 )
        return 1;
    else
        pulse_time = 0;
    /* 存在脉冲到来，且存在的时间不能超过240us*/
    while(!ds18b20->GetBit() && (pulse_time<240))
    {
        pulse_time++;
        ds18b20->Delayus(1);
    }
    if( pulse_time >=240 )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*从DS18B20读取一个位,返回值：1/0*/
static uint8_t ReadBitFromDs18b20(Ds18b20ObjectType *ds18b20) 
{
    uint8_t data;
    
    ds18b20->SetPinMode(DS18B20_Out);
    ds18b20->SetBit(DS18B20_Reset);
    ds18b20->Delayus(2);
    ds18b20->SetBit(DS18B20_Set);
    ds18b20->SetPinMode(DS18B20_In);
    ds18b20->Delayus(12);
    data=ds18b20->GetBit();
    ds18b20->Delayus(50);
    return data;
}

/*向DS18B20写一个字节*/
static void WriteByteToDs1820(Ds18b20ObjectType *ds18b20,uint8_t commond)
{
    uint8_t i, testb;
    
    ds18b20->SetPinMode(DS18B20_Out);
    
    for(i=0; i<8; i++)
    {
        testb = commond&0x01;
        commond = commond>>1;
        // 写0和写1的时间至少要大于60us
        if (testb)
        {
            ds18b20->SetBit(DS18B20_Reset);
            // 1us < 这个延时 < 15us
            ds18b20->Delayus(10);
            ds18b20->SetBit(DS18B20_Set);
            ds18b20->Delayus(45);
        }
        else
        {
            ds18b20->SetBit(DS18B20_Reset);
            // 60us < Tx 0 < 120us
            ds18b20->Delayus(60);
            ds18b20->SetBit(DS18B20_Set);
            // 1us < Trec(恢复时间) < 无穷大
        }
        ds18b20->Delayus(2);
    }
}

/*从DS18B20读取一个字节*/
static uint8_t ReadByteFromDs18b20(Ds18b20ObjectType *ds18b20)
{
    uint8_t i, j, data = 0;
    for(i=0; i<8; i++)
    {
        j = ReadBitFromDs18b20(ds18b20);
        data = (data) | (j<<i);
    }
    return data;
}

/*读取DS18B20对象的序列号*/
static void GetDs18b20SerialNumber(Ds18b20ObjectType *ds18b20)
{
    uint8_t data[8];
    
    WriteByteToDs1820(ds18b20,Read_Rom);
    
    for(int i=0;i<8;i++)
    {
        data[i]=ReadByteFromDs18b20(ds18b20);
    }
    
    for(int i=0;i<6;i++)
    {
        ds18b20->sn[i]=data[i+1];
    }
}

/*对DS18B20操作进行初始化*/
Ds18b20StatusType Ds18b20Initialization(Ds18b20ObjectType *ds18b20,
                                        Ds18b20SetBitType setBit,
                                        Ds18b20GetBitType getBit,
                                        Ds18b20SetPinModeType pinDirection,
                                        Ds18b20DelayType delayus)
{
    if((ds18b20==NULL)||(setBit==NULL)||(getBit==NULL)||(delayus==NULL))
    {
        return DS18B20_InitialError;
    }
    
    ds18b20->SetBit=setBit;
    ds18b20->GetBit=getBit;
    ds18b20->Delayus=delayus;
    
    if(pinDirection==NULL)
    {
        ds18b20->SetPinMode=SetPinModeDefault;
    }
    else
    {
        ds18b20->SetPinMode=pinDirection;
    }
    
    ds18b20->temperature=0.0;
    
    ResetDs18b20(ds18b20);
    if(PresenceDs18b20(ds18b20))
    {
        return DS18B20_NoResponse;
    }
    
    GetDs18b20SerialNumber(ds18b20);
    
    return DS18B20_OK;
}

/*设置单总线输入输出模式的缺省函数*/
static void SetPinModeDefault(Ds18b20IOModeType mode)
{
    //当引脚可以配置为开楼输出模式时，无需修改输入输出模式，使用此缺省函数
}
/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
