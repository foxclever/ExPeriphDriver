/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ms5837function.c                                               **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现压力变送器（MS5837）的相关操作                         **/
/**           MS5837压力变送器使用I2C接口通讯方式                            **/
/**           MS5837通讯的命令格式如下：                                     **/
/**           +-+--------------+-+-+--------------+-+-+                      **/
/**           |S|Device Address|W|A|    Command   |A|P|                      **/
/**           +-+--------------+-+-+--------------+-+-+                      **/
/**           接收内存寄存器数据的报文格式如下：                             **/
/**           +-+--------------+-+-+--------------+-+--------------+-+-+     **/
/**           |S|Device Address|R|A|Menory bit15-8|A|Menory bit15-8|N|P|     **/
/**           +-+--------------+-+-+--------------+-+--------------+-+-+     **/
/**           ADC数据接收的报文格式如下：                                    **/
/**      +-+--------------+-+-+------------+-+-----------+-+----------+-+-+  **/
/**      |S|Device Address|R|A| Data 23-16 |A| Data 15-8 |A| Data 7-0 |N|P|  **/
/**      +-+--------------+-+-+------------+-+-----------+-+----------+-+-+  **/
/**           S=Start Condition   W=Write   A=Acknowledge                    **/
/**           P=Stop Condition    R=Read    N=Not Acknowledge                **/
/**           其中Device Address为7位，Command为8位                          **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    1、声明MS5837ObjectType对象实体，并调用函数MS5837Initialization初始化 **/
/**       对象实体。给定设备地址，并读取校准数据                             **/
/**    2、实现形如uint8_t f(struct MS5837Object *ms,uint8_t command)的函数， **/
/**       并将其赋值给MS5837ObjectType对象实体的SendCommand函数指针。实现命  **/
/**       令的发送。                                                         **/
/**    3、实现形如void f(struct MS5837Object *ms,uint8_t *rData,             **/
/**       uint16_t rSize)的函数，并将其赋值给MS5837ObjectType对象实体的      **/
/**       GetDatas函数指针。实现数据的读取                                   **/
/**    4、实现形如void f(volatile uint32_t nTime)的函数，并将其赋值给        **/
/**       MS5837ObjectType对象实体的Delayms函数指针。实现操作延时，单位毫秒  **/
/**    5、调用相应的函数实现需要的操作。                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-01-16          木南              创建文件               **/
/**     V1.0.0  2019-05-16          木南              改为对象操作           **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "ms5837function.h"

#define COMMAND_RESET 0x1E
#define COMMAND_CONVERTD1OSR256 0x40
#define COMMAND_CONVERTD1OSR512 0x42
#define COMMAND_CONVERTD1OSR1024 0x44
#define COMMAND_CONVERTD1OSR2048 0x46
#define COMMAND_CONVERTD1OSR4096 0x48
#define COMMAND_CONVERTD1OSR8192 0x4A
#define COMMAND_CONVERTD2OSR256 0x50
#define COMMAND_CONVERTD2OSR512 0x52
#define COMMAND_CONVERTD2OSR1024 0x54
#define COMMAND_CONVERTD2OSR2048 0x56
#define COMMAND_CONVERTD2OSR4096 0x58
#define COMMAND_CONVERTD2OSR8192 0x5A
#define COMMAND_ADC_READ 0x00
#define COMMAND_PROM_READ_CRC 0xA0
#define COMMAND_PROM_READ_C1 0xA2
#define COMMAND_PROM_READ_C2 0xA4
#define COMMAND_PROM_READ_C3 0xA6
#define COMMAND_PROM_READ_C4 0xA8
#define COMMAND_PROM_READ_C5 0xAA
#define COMMAND_PROM_READ_C6 0xAC

/*从MS5837的PROM中读取校准数据*/
static void GetCalibrationData(MS5837ObjectType *ms);
/*读取MS5837内存寄存器的数据*/
uint16_t ReadPromFromMs5837(MS5837ObjectType *ms,uint8_t command);
/*读取MS5837ADC的转换值*/
uint32_t ReadConversionFromMs5837(MS5837ObjectType *ms,uint8_t command);

/*获取转换值，包括温度和压力*/
void GetMS5837ConversionValue(MS5837ObjectType *ms,MS5837OSRType pOSR,MS5837OSRType tOSR)
{
  uint8_t presOSR[6]={COMMAND_CONVERTD1OSR256,COMMAND_CONVERTD1OSR512,
                      COMMAND_CONVERTD1OSR1024,COMMAND_CONVERTD1OSR2048,
                      COMMAND_CONVERTD1OSR4096,COMMAND_CONVERTD1OSR8192};
  uint8_t tempOSR[6]={COMMAND_CONVERTD2OSR256,COMMAND_CONVERTD2OSR512,
                      COMMAND_CONVERTD2OSR1024,COMMAND_CONVERTD2OSR2048,
                      COMMAND_CONVERTD2OSR4096,COMMAND_CONVERTD2OSR8192};
	
  uint16_t senst1=ms->caliData[0];        //C1压力灵敏度
  uint16_t offt1=ms->caliData[1];         //C2压力补偿值
  uint16_t tcs=ms->caliData[2];           //C3压力灵敏度温度系数
  uint16_t tco=ms->caliData[3];           //C4压力补偿温度系数
  uint16_t tref=ms->caliData[4];          //C5参考温度
  uint16_t tempsens=ms->caliData[5];      //C6温度传感器温度系数
  
  uint32_t digitalPressureValue;
  uint32_t digitalTemperatureValue;
 
  /*读取压力数据*/
  digitalPressureValue=ReadConversionFromMs5837(ms,presOSR[pOSR]);
  
  ms->Delayms(20);
  
  /*读取温度数据*/
  digitalTemperatureValue=ReadConversionFromMs5837(ms,tempOSR[tOSR]);
  
  /*对温度进行一阶修正*/
  int32_t dT;
  int32_t temp;
  dT=digitalTemperatureValue-tref*256;
  temp=(int32_t)(2000+dT*tempsens/pow(2,23));
  
  /*对压力进行一阶修正*/
  int64_t off;
  int64_t sens;
  int32_t pres;
  off=(int64_t)(offt1*pow(2,17)+(tco*dT)/pow(2,6));
  sens=(int64_t)(senst1*pow(2,16)+(tcs*dT)/pow(2,7));
  pres=(int32_t)((digitalPressureValue*sens/pow(2,21)-off)/pow(2,15));
  
  /*对温度和压力进行二阶修正*/
  int64_t ti=0;
  int64_t offi=0;
  int64_t sensi=0;
  int64_t off2=0;
  int64_t sens2=0;  
  
  if(temp<2000)
  {
    ti=(int64_t)(11*dT*dT/pow(2,35));
    offi=(int64_t)(31*(temp-2000)*(temp-2000)/pow(2,3));
    sensi=(int64_t)(63*(temp-2000)*(temp-2000)/pow(2,5));
    
    off2=off-offi;
    sens2=sens-sensi;
    
    temp=temp-(int32_t)ti;
    pres=(int32_t)((digitalPressureValue*sens2/pow(2,21)-off2)/pow(2,15));
  }

  if((-4000<=temp)&&(temp<=8500))
  {
    ms->temperature=(float)temp/100.0f;
  }
  if((1000<=pres)&&(pres<=120000))
  {
    ms->pressure=(float)pres/100.0f;
  }
}

/*复位MS5837操作*/
void ResetForMs5837(MS5837ObjectType *ms)
{
  uint8_t command=COMMAND_RESET;
  /*下发复位命令*/
  ms->Write(ms,command);
}

/* 初始化MS5837对象 */
void MS5837Initialization(MS5837ObjectType *ms,MS5837Write write,MS5837Read read,MS5837Delayms delayms)
{
  if((ms==NULL)||(write==NULL)||(read==NULL)||(delayms==NULL))
  {
    return;	
  }
  
  ms->Write=write;
  ms->Read=read;
  ms->Delayms=delayms;
  
  ms->devAddress=0xEC;
  ms->pressure=0.0;
  ms->temperature=0.0;
  
  ResetForMs5837(ms);
  
  GetCalibrationData(ms);
}

/*从MS5837的PROM中读取校准数据*/
static void GetCalibrationData(MS5837ObjectType *ms)
{
  /*C1压力灵敏度*/
  ms->caliData[0]=ReadPromFromMs5837(ms,COMMAND_PROM_READ_C1);
  /*C2压力补偿值*/
  ms->caliData[1]=ReadPromFromMs5837(ms,COMMAND_PROM_READ_C2);
  /*C3压力灵敏度温度系数*/
  ms->caliData[2]=ReadPromFromMs5837(ms,COMMAND_PROM_READ_C3);
  /*C4压力补偿温度系数*/
  ms->caliData[3]=ReadPromFromMs5837(ms,COMMAND_PROM_READ_C4);
  /*C5参考温度*/
  ms->caliData[4]=ReadPromFromMs5837(ms,COMMAND_PROM_READ_C5);
  /*C6温度传感器温度系数*/
  ms->caliData[5]=ReadPromFromMs5837(ms,COMMAND_PROM_READ_C6);
}

/*读取MS5837内存寄存器的数据*/
static uint16_t ReadPromFromMs5837(MS5837ObjectType *ms,uint8_t command)
{
  /*下发读取指定内存单元的命令*/
  ms->Write(ms,command);
  
  /*接收读取的指定内存单元的值*/
  uint8_t promValue[2];
  ms->Read(ms,promValue,2);
  
  uint16_t result=(uint16_t)promValue[0];
  result=(result<<8)+(uint16_t)promValue[1];
  
  return result;
}

/*读取MS5837ADC的转换值*/
static uint32_t ReadConversionFromMs5837(MS5837ObjectType *ms,uint8_t command)
{
  /*下发转化对象及精度配置命令*/
  ms->Write(ms,command);
  
  ms->Delayms(10);
  
  /*下发读取ADC的命令*/
  ms->Write(ms,COMMAND_ADC_READ);
  
  ms->Delayms(10);
  
  /*接收读取的ADC转换结果*/
  uint8_t adcValue[3];
  ms->Read(ms,adcValue,3);
  
  uint32_t result=(uint32_t)adcValue[0];
  result=(result<<8)+(uint32_t)adcValue[1];
  result=(result<<8)+(uint32_t)adcValue[2];
  
  return result;
}

/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
