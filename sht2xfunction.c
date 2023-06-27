/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：sht2xfunction.c                                                **/
/** 版    本：V1.0.1                                                         **/
/** 简    介：用于实现SHT2X温湿度计的操作                                    **/
/**           默认分辨率14bit(温度)和12bit(湿度)可以通过用户寄存器修改       **/
/**           相对湿度的计算公式：RH=-6+125*SRH/Pow(2,16)，计算基于液态水的  **/
/**           相对湿度，而基于冰的相对湿度可以计算得到：                     **/
/**           RHi=RHw*exp（17.62*t/(243.12+t))/exp(22.46*t/(272.62+t));      **/
/**           温度的计算公式：T=-46.85+175.72*ST/Pow(2,16);                  **/
/**           不同分辨率下ADC转换时间：                                      **/
/**           +------+--------+--------+-------+-------+----+                **/
/**           |分辨率|RH典型值|RH最大值|T典型值|T最大值|单位|                **/
/**           +------+--------+--------+-------+-------+----+                **/
/**           | 14bit| ——   | ——   |  66   |  85   | ms |                **/
/**           +------+--------+--------+-------+-------+----+                **/
/**           | 13bit| ——   | ——   |  33   |  43   | ms |                **/
/**           +------+--------+--------+-------+-------+----+                **/
/**           | 12bit|   22   |   29   |  17   |  22   | ms |                **/
/**           +------+--------+--------+-------+-------+----+                **/
/**           | 11bit|   12   |   15   |   9   |  11   | ms |                **/
/**           +------+--------+--------+-------+-------+----+                **/
/**           | 10bit|   7    |    9   | ——  | ——  | ms |                **/
/**           +------+--------+--------+-------+-------+----+                **/
/**           | 8bit |   3    |    4   | ——  | ——  | ms |                **/
/**           +------+--------+--------+-------+-------+----+                **/
/**           SHT2x命令代码：                                                **/
/**           +------------+----------+----------+                           **/
/**           |    命令    |   描述   | 命令代码 |                           **/
/**           +------------+----------+----------+                           **/
/**           | 触发T测量  | 保持主机 | 11100011 |                           **/
/**           +------------+----------+----------+                           **/
/**           | 触发RH测量 | 保持主机 | 11100101 |                           **/
/**           +------------+----------+----------+                           **/
/**           | 触发T测量  |非保持主机| 11110011 |                           **/
/**           +------------+----------+----------+                           **/
/**           | 触发RH测量 |非保持主机| 11110101 |                           **/
/**           +------------+----------+----------+                           **/
/**           |写用户寄存器|          | 11100110 |                           **/
/**           +------------+----------+----------+                           **/
/**           |读用户寄存器|          | 11100111 |                           **/
/**           +------------+----------+----------+                           **/
/**           |   软复位   |          | 11111110 |                           **/
/**           +------------+----------+----------+                           **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    1、声明SHT2xDevice对象实体，并调用函数SHT2xInitialization初始化对象   **/
/**       实体。SHT2x设备地址固定为0x80                                      **/
/**    2、实现形如void f(uint8_t devAddress,uint8_t *rData,uint16_t rSize)的 **/
/**       函数，并将其赋值给SHT2xDevice对象实体的Receive函数指针。实现数据接 **/
/**       收。                                                               **/
/**    3、实现形如void f(uint8_t devAddress,uint8_t *tData,uint16_t tSize)的 **/
/**       函数，并将其赋值给SHT2xDevice对象实体的Transmit函数指针。实现数据  **/
/**       发送。                                                             **/
/**    4、实现形如void f(volatile uint32_t nTime)的函数，并将其赋值给        **/
/**       SHT2xDevice对象实体的Delay函数指针。实现操作延时，单位毫秒         **/
/**    5、调用相应的函数实现需要的操作。                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-07-17          木南              创建文件               **/
/**     V1.0.1  2018-04-17          木南              添加SHT2x对象定义并修  **/
/**                                                   改各函数形参           **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "sht2xfunction.h"

#define SHT2X_ADDRESS   0X80//SHT2X的地址

/* 定义序列号存储地址宏 */
#define FIRST_MEM_ADDRESS       0xFA    //序列号第一段存储地址
#define FIRST_MEM_COMMAND       0x0F    //序列号第一段存储地址
#define SECOND_MEM_ADDRESS      0xFC    //序列号第二段存储地址
#define SECOND_MEM_COMMAND      0xC9    //序列号第二段存储地址

/* 定义用户寄存器配置宏 */
#define DPI_RH12_T14    0x00    //湿度12位温度14位精度
#define DPI_RH8_T12     0x01    //湿度8位温度13位精度
#define DPI_RH10_T13    0x80    //湿度10位温度12位精度
#define DPI_RH11_T11    0x81    //湿度11位温度11位精度
#define END_OF_BATTERY_H        0x00    //电池状态VDD>2.25
#define END_OF_BATTERY_L        0x40    //电池状态VDD<2.25
#define ONCHIPHEATERDISABLE     0x00    //禁用片内加热
#define ONCHIPHEATERENABLE      0x04    //启用片内加热
#define OTPENABLE       0x00    //能启动OTP加载
#define OTPDISABLE      0x02    //不能启动OTP加载

const uint16_t SHT2xPOLYNOMIAL = 0x131;      ////定义校验多项式P(x) = x^8 + x^5 + x^4 + 1 = 100110001

/*根据指定的多项式计算CRC校验*/
static bool CheckCRC8ForSHT2x(uint8_t *data, uint8_t numOfBytes, uint8_t checksum);
/*计算温度的物理量值*/
static float CalcSHT2xTemperature(uint16_t sTemp);
/*计算相对湿度的物理量值*/
static float CalcSHT2xHumidity(uint16_t sHumi);
/*获取SHT2X电子识别码（序列号）*/
static void GetSHT2xSerialNumber(SHT2xObjectType *sht);
/*读取用户寄存器*/
static void GetSHT2xUserRegister(SHT2xObjectType *sht);
/*配置用户寄存器*/
static void SetSHT2xUserRegister(SHT2xObjectType *sht,uint8_t cmd);

/*读取SHT2x的温度数据*/
float GetSHT2xTemperatureValue(SHT2xObjectType *sht,uint8_t cmd)
{
    uint8_t data[3]={0,0,0};
    uint16_t result=0;
    
    /*设置转换命令*/
    sht->Transmit(sht,&cmd,1);
    
    sht->Delayms(85);
    
    /*读取数据*/
    sht->Receive(sht,data,3);
    
    if(CheckCRC8ForSHT2x(data,2,data[2]))
    {
        result=(uint16_t)(data[0]);
        result=(result<<8)+(uint16_t)(data[1]);
    }
    
    sht->tempCode=result;
    
    return CalcSHT2xTemperature(result);
}

/*读取SHT2x的湿度数据*/
float GetSHT2xHumidityValue(SHT2xObjectType *sht,uint8_t cmd)
{
    uint8_t data[3]={0,0,0};
    uint16_t result=0;
    
    /*设置转换命令*/
    sht->Transmit(sht,&cmd,1);
    
    sht->Delayms(29);
    
    /*读取数据*/
    sht->Receive(sht,data,3);
    
    if(CheckCRC8ForSHT2x(data,2,data[2]))
    {
        result=(uint16_t)(data[0]);
        result=(result<<8)+(uint16_t)(data[1]);
    }
    
    sht->humiCode=result;
    
    return CalcSHT2xHumidity(result);
}

/*软件复位*/
void SoftResetSHT2x(SHT2xObjectType *sht)
{
    uint8_t tData=SOFT_RESET;
    sht->Transmit(sht,&tData,1);
}

/* 初始化配置SHT2x */
void SHT2xInitialization(SHT2xObjectType *sht,
                         SHT2xDPIType dpi,
                         SHT2xBatteryType endBat,
                         SHT2xHeaterType heater,
                         SHT2xOTPType otp,
                         SHT2xTransmit write,
                         SHT2xReceive read,
                         SHT2xDelayms delayms)
{
    uint8_t userReg=0;
    uint8_t dpiSet[4]={DPI_RH12_T14,DPI_RH8_T12,DPI_RH10_T13,DPI_RH11_T11};
    uint8_t batEnd[2]={END_OF_BATTERY_H,END_OF_BATTERY_L};
    uint8_t heaterSet[2]={ONCHIPHEATERDISABLE,ONCHIPHEATERENABLE};
    uint8_t otpSet[2]={OTPENABLE,OTPDISABLE};
    
    if((sht==NULL)||(write==NULL)||(read==NULL)||(delayms==NULL))
    {
        return;
    }
    
    sht->Transmit=write;
    sht->Receive=read;
    sht->Delayms=delayms;
    
    sht->devAddress=SHT2X_ADDRESS;
    sht->tempCode=0;
    sht->humiCode=0;
    
    SoftResetSHT2x(sht);
    
    userReg=dpiSet[dpi]|batEnd[endBat]|heaterSet[heater]|otpSet[otp];
    
    SetSHT2xUserRegister(sht,userReg);	//配置用户寄存器
    
    sht->Delayms(20);
    
    //GetSHT2xUserRegister(sht);			//读取用户寄存器
    
    GetSHT2xSerialNumber(sht);			//获取电子识别码
}

/*配置用户寄存器*/
static void SetSHT2xUserRegister(SHT2xObjectType *sht,uint8_t cmd)
{
    uint8_t status;
    uint8_t command;
    uint8_t pData[2];
    
    GetSHT2xUserRegister(sht);
    
    status=sht->userReg&0x38;
    
    command=cmd&0xC7;
    command=command|status;
    
    if(command!=sht->userReg)
    {
        pData[0]=WRITE_USER_REGISTER;
        pData[1]=command;
        
        sht->Transmit(sht,pData,2);
        
        sht->Delayms(10);
        
        GetSHT2xUserRegister(sht);
    }
}

/*读取用户寄存器*/
static void GetSHT2xUserRegister(SHT2xObjectType *sht)
{
    uint8_t pData;
    uint8_t rData=0;
    
    pData=READ_USER_REGISTER;
    sht->Transmit(sht,&pData,1);

    sht->Receive(sht,&rData,1);
    sht->userReg=rData;
}

/*计算温度的物理量值*/
static float CalcSHT2xTemperature(uint16_t sTemp)
{
    float temperatureC;
    
    sTemp &= 0xFFFC;
    /*T= -46.85 + 175.72 * ST/2^16*/
    temperatureC = (float)-46.85 + (float)sTemp*((float)175.72 / 65536);
    
    return temperatureC;
}

/*计算相对湿度的物理量值*/
static float CalcSHT2xHumidity(uint16_t sHumi)
{
    float humidityRH;
    
    sHumi &= 0xFFFC;
    /* RH= -6 + 125 * SRH/2^16*/
    humidityRH = (float)-6.0 + (float)sHumi*((float)125.0/65536); 
    
    return humidityRH;
}

/*根据指定的多项式计算CRC校验*/
static bool CheckCRC8ForSHT2x(uint8_t *data, uint8_t numOfBytes, uint8_t checksum)
{
    uint8_t crc = 0x00;
    uint8_t bit=0;
    bool error;
    
    for(uint8_t i = 0; i < numOfBytes; i++)
    {
        crc ^= (data[i]);
        for ( bit = 8; bit > 0; --bit)
        {
            if (crc & 0x80)
            {
                crc = (crc << 1) ^ SHT2xPOLYNOMIAL;
            }
            else
            {
                crc = (crc << 1);
            }
        }
    }
    error=(crc != checksum)?false:true;
    return error;
}

/*获取SHT2X电子识别码（序列号）*/
static void GetSHT2xSerialNumber(SHT2xObjectType *sht)
{
    uint8_t snbBuff[8];
    uint8_t snacBuff[6];
    uint8_t pData[2];
    
    /*访问第一个存储位置*/
    pData[0]=FIRST_MEM_ADDRESS;
    pData[1]=FIRST_MEM_COMMAND;
    sht->Transmit(sht,pData,2);
    sht->Delayms(100);
    sht->Receive(sht,snbBuff,8);
    
    /*访问第二个存储位置*/
    pData[0]=SECOND_MEM_ADDRESS;
    pData[1]=SECOND_MEM_COMMAND;
    sht->Transmit(sht,pData,2);
    sht->Delayms(100);
    sht->Receive(sht,snacBuff,6);
    
    sht->sn[0]=snacBuff[4];
    sht->sn[1]=snacBuff[3];
    sht->sn[2]=snbBuff[6];
    sht->sn[3]=snbBuff[4];
    sht->sn[4]=snbBuff[2];
    sht->sn[5]=snbBuff[0];
    sht->sn[6]=snacBuff[1];
    sht->sn[7]=snacBuff[0];
}

/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
