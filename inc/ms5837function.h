/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ms5837function.h                                               **/
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
/**                                                                          **/
/******************************************************************************/ 

#ifndef __MS5837FUNCTION_H
#define __MS5837FUNCTION_H

#include "stdint.h"
#include "math.h"

/* 定义转换精度枚举 */
typedef enum MS5837OSR{
  MS5837_OSR256,
  MS5837_OSR512,
  MS5837_OSR1024,
  MS5837_OSR2048,
  MS5837_OSR4096,
  MS5837_OSR8192
}MS5837OSRType;

/* 定义MS5837对象类型 */
typedef struct MS5837Object {
  uint8_t devAddress;   //设备地址
  uint16_t caliData[6]; //校准数据
  float temperature;
  float pressure;
  void (*Write)(struct MS5837Object *ms,uint8_t command);//向MS5837下发指令，指令格式均为1个字节
  void (*Read)(struct MS5837Object *ms,uint8_t *rData,uint16_t rSize);//从MS5837读取多个字节数据的值
  void (*Delayms)(volatile uint32_t nTime);     //毫秒秒延时函数
}MS5837ObjectType;

/*向MS5837下发指令，指令格式均为1个字节*/
typedef void (*MS5837Write)(struct MS5837Object *ms,uint8_t command);
/*从MS5837读取多个字节数据的值*/
typedef void (*MS5837Read)(struct MS5837Object *ms,uint8_t *rData,uint16_t rSize);
/*毫秒秒延时函数*/
typedef void (*MS5837Delayms)(volatile uint32_t nTime);

/*获取转换值，包括温度和压力*/
void GetMS5837ConversionValue(MS5837ObjectType *ms,MS5837OSRType pOSR,MS5837OSRType tOSR);

/*复位MS5837操作*/
void ResetForMs5837(MS5837ObjectType *ms);

/* 初始化MS5837对象 */
void MS5837Initialization(MS5837ObjectType *ms, //MS5837对象
                          MS5837Write write,    //向MS5837写数据函数指针
                          MS5837Read read,      //从MS5837读数据函数指针
                          MS5837Delayms delayms //毫秒延时函数指针
                         );
#endif
/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
