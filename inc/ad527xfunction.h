/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ad527xfunction.h                                               **/
/** 版    本：V1.0.2                                                         **/
/** 简    介：声明和定义AD527x系列数字电位器芯片相关的各种函数和操作         **/
/**           本软件适用于AD5270、AD5271、AD5272、AD5274芯片                 **/
/**           其中：AD5270、AD5271采用SPI接口(CPOL = 0, CPHA = 1)            **/
/**                 AD5272、AD5274采用I2C接口                                **/
/**           输入一位寄存器的格式如下：                                     **/
/**           +---+---+--+--+--+--+--+--+--+--+--+--+--+--+--+--+            **/
/**           |未定义 |  控制位   |          数据位             |            **/
/**           +---+---+--+--+--+--+--+--+--+--+--+--+--+--+--+--+            **/
/**           | 0 | 0 |C3|C2|C1|C0|D9|D8|D7|D6|D5|D4|D3|D2|D1|D0|            **/
/**           +---+---+--+--+--+--+--+--+--+--+--+--+--+--+--+--+            **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    1、声明AD527x对象实体，并调用函数AD527xInitialization初始化对象实体。 **/
/**    2、初始化时，若对象类型为AD5272、AD5274则给定设备地址，以备I2C通讯。  **/
/**       若对象类型为AD5270、AD5271则实现形如void f(bool en)的函数，并将其  **/
/**       赋值AD527x对象实体的ChipSelcet函数指针。以备SPI通讯。              **/
/**    3、实现形如void f(struct AD527xObject *rx,uint8_t *rData,             **/
/**       uint16_t rSize)的函数，并将其赋值给AD527x对象实体的Receive函数     **/
/**       指针。完成收数据操作                                               **/
/**    4、实现形如void f(struct AD527xObject *rx,uint8_t *wData,             **/
/**       uint16_t wSize)的函数，并将其赋值给AD527x对象实体的Transmit函数    **/
/**       指针。完成写数据操作                                               **/
/**    5、实现形如void f(volatile uint32_t nTime)延时函数，并将其赋值给      **/
/**       AD527x对象实体的Delay函数指针。单位为毫秒。                        **/
/**    6、调用相应的函数实现需要的操作。                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2019-04-01          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __AD5270_FUNCTION_H
#define __AD5270_FUNCTION_H

#include "stdint.h"
#include "stdbool.h"

/* 定义AD527x类型枚举 */
typedef enum AD527x {
  AD5270,
  AD5271,
  AD5272,
  AD5274
}AD527xType;

/*定义片选操作枚举*/
typedef enum AD527xCS {
  AD527xCS_ENABLE,
  AD527xCS_DISABLE
}AD527xCSType;

/*定义用于SPI接口的对象类型*/
typedef struct AD527xObject {
  AD527xType type;      //设备类型
  uint8_t devAddress;   //设备地址,用于I2C接口
  uint8_t conreg;       //控制寄存器
  uint16_t rdac;        //游标寄存器现值
  void (*ChipSelect)(AD527xCSType en);		//片选信号，用于SPI接口
  void (*Receive)(struct AD527xObject *rx,uint8_t *rData,uint16_t rSize);
  void (*Transmit)(struct AD527xObject *rx,uint8_t *wData,uint16_t wSize);
  void (*Delayms)(volatile uint32_t nTime);       //ms延时操作指针
}AD527xObjectType;

/*定义片选信号函数指针类型*/
typedef void (*AD527xChipSelect)(AD527xCSType en);
/*定义接收数据函数指针类型*/
typedef void (*AD527xReceive)(struct AD527xObject *rx,uint8_t *rData,uint16_t rSize);
/*定义发送数据函数指针类型*/
typedef void (*AD527xTransmit)(struct AD527xObject *rx,uint8_t *wData,uint16_t wSize);
/*定义ms延时操作指针*/
typedef void (*AD527xDelayms)(volatile uint32_t nTime);       

/* 向AD527x写数据 */
void SetRDACForAd527x(AD527xObjectType *rx,uint16_t data);

/* 读取RDAC游标寄存器的内容 */
uint16_t ReadRDACFromAd527x(AD527xObjectType *rx);

/* 软件复位 */
void AD527xSoftwareReset(AD527xObjectType *rx);

/* 存储游标设置：将RDAC设置存储到50-TP中 */
void StoreRDACTo50TP(AD527xObjectType *rx);

/* 读指定存储位置的数据 */
uint16_t ReadGivenAddressValue(AD527xObjectType *rx,uint8_t address);

/* 读取最后一次50-TP编程存储器位置的地址 */
uint8_t ReadLast50TPAddress(AD527xObjectType *rx);

/* 初始化AD527x对象,I2C接口必须初始化devAddress，SPI接口必需初始化void (*ChipSelcet)(bool) */
void AD527xInitialization(AD527xObjectType *rx,
                          uint8_t address,
                          AD527xType type,
                          AD527xReceive recieve,
                          AD527xTransmit transmit,
                          AD527xChipSelect cs,
                          AD527xDelayms delayms);

#endif  //__AD5270_FUNCTION_H

/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
