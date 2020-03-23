/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ltc2400function.h                                              **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现LTC2400 ADC的数据获取                                  **/
/**           LTC2400采用SPI电气接口                                         **/
/**           输出数据的结构如下：                                           **/
/**           Bit31 Bit30 Bit29 Bit28 Bit27 … Bit4 Bit3-0                    **/
/**           EOC   DMY   SIG   EXR   MSB      LSB  SUB LSBs                 **/
/**           0     0     0/1   0/1   0/1      0/1  X                        **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-01-18          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef _ltc2400function_h
#define _ltc2400function_h

#include "stdint.h"

/* 定义LTC2400片选信号枚举 */
typedef enum LTC2400CS {
  LTC2400CS_Enable,
  LTC2400CS_Disable
}LTC2400CSType;

/* 定义LTC2400时钟信号 */
typedef enum LTC2400Clock {
  INTERNAL_CLOCK50Hz,
  INTERNAL_CLOCK60Hz,
  EXTERNAL_CLOCK
}LTC2400ClockType;

/* 定义LTC2400对象类型 */
typedef struct Ltc2400Object {
  LTC2400ClockType clock;       //使用的时钟
  uint32_t dataCode;            //数据编码
  void (*Receive)(uint8_t *rData);      //接收数据
  void (*ChipSelect)(LTC2400CSType cs); //实现片选
  void (*Delayms)(volatile uint32_t nTime);     //实现ms延时操作
}Ltc2400ObjectType;

/*定义接收数据函数指针类型*/
typedef void (*LTC2400Receive)(uint8_t *rData);
/*定义片选信号函数指针类型*/
typedef void (*LTC2400ChipSelect)(LTC2400CSType cs);
/*定义延时操作函数指针类型*/
typedef void (*LTC2400Delay)(volatile uint32_t nTime);

/* 获取LTC2400转换数据,返回量程数据的比例值 */
float GetLtc2400Data(Ltc2400ObjectType *ltc);

/* LTC2400对象初始化函数 */
void LTC2400Initialization(Ltc2400ObjectType *ltc,
                           LTC2400ClockType clock,
                           LTC2400Receive receive,
                           LTC2400ChipSelect cs,
                           LTC2400Delay msDelay);

#endif
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
