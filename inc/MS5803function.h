/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ms5803function.h                                               **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现压力变送器（MS5803）的相关操作                         **/
/**           MS5803压力变送器使用I2C或SPI接口通讯方式，由硬件接线决定       **/
/**           MS5803采用SPI通讯时，支持Mode0和Mode3                          **/
/**           MS5803通讯的命令格式如下：                                     **/
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
/**           使用I2C通讯时，最多带2台。地址为0xEC和0xEE                     **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    1、声明MS5803ObjectType对象实体，并调用函数MS5803Initialization初始化 **/
/**       对象实体。需要给定类型，类型由enum MS5803Model定义。给定通讯接口， **/
/**       通讯接口由enum MS5803Port定义。如果通讯接口为I2C则需要给定设备地   **/
/**       址，若为SPI接口则应赋值ChipSelcet指针。并读取校准数据              **/
/**    2、实现形如uint8_t f(struct MS5803Object *ms,uint8_t command)的函数， **/
/**       并将其赋值给MS5803ObjectType对象实体的SendCommand函数指针。实现命  **/
/**       令的发送。                                                         **/
/**    3、实现形如void f(struct MS5803Object *ms,uint8_t *rData,             **/
/**       uint16_t rSize)的函数，并将其赋值给MS5803ObjectType对象实体的      **/
/**       GetDatas函数指针。实现数据的读取                                   **/
/**    4、若使用SPI端口，则实现形如void f(bool en)的函数，并将其赋值给       **/
/**       MS5803ObjectType对象实体的ChipSelcet函数指针。实现片选信号。若采   **/
/**       用I2C接口，则ChipSelcet函数指针会自动赋值为NULL。                  **/
/**    5、实现形如void f(volatile uint32_t nTime)的函数，并将其赋值给        **/
/**       MS5837ObjectType对象实体的Delayms函数指针。实现操作延时，单位毫秒  **/
/**    6、调用相应的函数实现需要的操作。                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-01-16          木南              创建文件               **/
/**     V1.1.0  2019-05-26          木南              修改为对象操作         **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef _MS5803function_h
#define _MS5803function_h

#include "stdint.h"
#include "stdbool.h"
#include "math.h"
#include "stddef.h"

/*MS5803传感器的类型*/
typedef enum MS5803Model{
  MS580301BA,
  MS580302BA,
  MS580305BA,
  MS580314BA,
  MS580330BA
}MS5803ModelType;

/*MS5803的接口类型*/
typedef enum MS5803Port{
  SPI,
  I2C
}MS5803PortType;

/*MS5803的ADC转换精度类型*/
typedef enum MS5803OSR{
  MS5803_OSR256,
  MS5803_OSR512,
  MS5803_OSR1024,
  MS5803_OSR2048,
  MS5803_OSR4096
}MS5803OSRType;

/* 定义MS5803片选信号枚举 */
typedef enum MS5803CS {
  MS5803CS_Enable,
  MS5803CS_Disable
}MS5803CSType;

typedef struct MS5803Object {
  uint8_t devAddress;           //设备地址
  MS5803PortType port;          //通讯端口协议
  MS5803ModelType model;        //设备的类型
  uint16_t caliData[6];         //校准数据
  float temperature;
  float pressure;
  void (*Write)(struct MS5803Object *ms,uint8_t command);       //向MS5803写信息
  void (*Read)(struct MS5803Object *ms,uint8_t *rData,uint16_t rSize);  //从MS5803读数据
  void (*ChipSelcet)(MS5803CSType en);  //片选信号,用于SPI接口
  void (*Delayms)(volatile uint32_t nTime);     //毫秒秒延时函数
}MS5803ObjectType;

/*向MS5803下发指令，指令格式均为1个字节*/
typedef void (*MS5803Write)(MS5803ObjectType *ms,uint8_t command);
/*从MS5803读取多个字节数据的值*/
typedef void (*MS5803Read)(MS5803ObjectType *ms,uint8_t *rData,uint16_t rSize);
/*片选信号,用于SPI接口*/
typedef void (*MS5803ChipSelcet)(MS5803CSType en);     
/*毫秒秒延时函数*/
typedef void (*MS5803Delayms)(volatile uint32_t nTime);     

/*获取转换值，包括温度和压力*/
void GetMS5803ConversionValue(MS5803ObjectType *ms,MS5803OSRType osrT,MS5803OSRType osrP);

/*复位MS5803操作*/
void ResetForMS5803(MS5803ObjectType *ms);

/* MS5803对象初始化 */
void MS5803Initialization(MS5803ObjectType *ms,         //MS5803对象
                          MS5803ModelType model,        //类型
                          MS5803PortType port,          //通讯端口
                          uint8_t address,              //I2C设备地址
                          MS5803Write write,            //写数据函数
                          MS5803Read read,              //读数据函数
                          MS5803ChipSelcet cs,          //SPI片选信号
                          MS5803Delayms delayms         //毫秒延时
                         );

#endif
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
