/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：dht11function.h                                                **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现DHT11温湿度传感器的通讯                                **/
/**           采用单总线通讯方式，数据格式如下：                             **/
/**           湿度整数+湿度小数+温度整数+温度小数+校验和                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-03-07          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "dht11function.h"

/*从DHT11读取一个位,返回值：1/0*/
static uint8_t ReadBitFromDHT11(Dht11ObjectType *dht);
/*从DHT11读取一个字节,返回值：读到的数据*/
static uint8_t ReadByteFromDHT11(Dht11ObjectType *dht);
/*复位DHT11*/
static void ResetDHT11(Dht11ObjectType *dht);
/*等待DHT11的回应，返回1：未检测到DHT11的存在；返回0：存在*/
static DHT11ErrorType CheckDHT11Status(Dht11ObjectType *dht);

/*从DHT11读取数据,temp:温度值(0-50),humi:湿度值(20%-90%),返回值：0,正常;1,失败*/
DHT11ErrorType GetProcessValueFromDHT11(Dht11ObjectType *dht)    
{
    DHT11ErrorType error=DHT11_None;
    uint8_t readBuffer[5];
    
    ResetDHT11(dht);
    if(CheckDHT11Status(dht)==DHT11_NoError)
    {
        for(int i=0;i<5;i++)
        {
            readBuffer[i]=ReadByteFromDHT11(dht);
        }
        
        uint8_t checkSum=0;
        checkSum=readBuffer[0]+readBuffer[1]+readBuffer[2]+readBuffer[3];
        error=DHT11_DataError;
        if(checkSum==readBuffer[4])
        {
            dht->temperature=(float)readBuffer[0];
            dht->humidity=(float)readBuffer[2];
            error=DHT11_NoError;
        }
    }
    return error;
}

/*DHT11初始化操作*/
DHT11ErrorType InitializeDHT11(Dht11ObjectType *dht,    //需要初始化对象
                               Dht11SetPinOutValueType setPinStatus,    //设置总线输出值
                               Dht11ReadPinBitType getPinStatus,        //读取总线输入值
                               Dht11SetPinModeType mode,        //配置总线的输入输出模式
                               Dht11DelayType delayms,  //毫秒延时
                               Dht11DelayType delayus   //微秒延时
                                   )
{
    if((dht==NULL)||(setPinStatus==NULL)||(getPinStatus==NULL)||(mode==NULL)||(delayms==NULL)||(delayus==NULL))
    {
        return DHT11_InitError;
    }
    dht->SetPinOutValue=setPinStatus;
    dht->ReadPinBit=getPinStatus;
    dht->SetPinMode=mode;
    dht->Delayms=delayms;
    dht->Delayus=delayus;
    
    dht->humidity=0.0;
    dht->temperature=0.0;
    
    ResetDHT11(dht);
    return CheckDHT11Status(dht);
}

/*复位DHT11，开始通讯*/
static void ResetDHT11(Dht11ObjectType *dht)
{
    dht->SetPinMode(DHT11_Out);   //设置为输出方式
    dht->SetPinOutValue(DHT11_Reset);     //将引脚点位拉低
    dht->Delayms(20);     //拉低至少18ms
    dht->SetPinOutValue(DHT11_Set);       //拉高
    dht->Delayus(30);     //主机拉高20至40us
}

/*等待DHT11的回应，返回1：未检测到DHT11的存在；返回0：存在*/
static DHT11ErrorType CheckDHT11Status(Dht11ObjectType *dht)
{
    uint8_t retry=0;
    dht->SetPinMode(DHT11_In);    //设置为输入方式
    while(dht->ReadPinBit()&&(retry<100))
    {
        retry++;
        dht->Delayus(1);
    }
    if(retry>=100)
    {
        return DHT11_None;
    }
    retry=0;
    while(!dht->ReadPinBit()&&(retry<100))
    {
        retry++;
        dht->Delayus(1);
    }
    if(retry>=100)
    {
        return DHT11_None;
    }
    return DHT11_NoError;  
}

/*从DHT11读取一个位,返回值：1/0*/
static uint8_t ReadBitFromDHT11(Dht11ObjectType *dht)			 
{
    uint8_t retry=0;
    /*等待变为低电平*/
    while(dht->ReadPinBit()&&(retry<100))
    {
        retry++;
        dht->Delayus(1);
    }
    retry=0;
    /*等待变高电平*/
    while(!dht->ReadPinBit()&&(retry<100))
    {
        retry++;
        dht->Delayus(1);
    }
    dht->Delayus(40);     //延时判断此位是0还是1
    
    return dht->ReadPinBit();
}

/*从DHT11读取一个字节,返回值：读到的数据*/
static uint8_t ReadByteFromDHT11(Dht11ObjectType *dht)
{
    uint8_t data=0;
    for(int i=0;i<8;i++)
    {
        data<<=1;
        data|=ReadBitFromDHT11(dht);
    }
    
    return data;
}

/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
