/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ad56xxfunction.c                                               **/
/** 版    本：V2.0.0                                                         **/
/** 简    介：声明和定义适用于ADI公司AD56xx系列数模转换芯片的函数和操作。    **/
/**           采用SPI接口相关器件的特点是均具备24位输入寄存器，具有相同的    **/
/**           命令格式。适用的器件有：                                       **/
/**           AD5662，单通道16位数模转换器                                   **/
/**           AD5623，双通道12位数模转换器                                   **/
/**           AD5643，双通道14位数模转换器                                   **/
/**           AD5663，双通道16位数模转换器                                   **/
/**           AD5624，四通道12位数模转换器                                   **/
/**           AD5644，四通道14位数模转换器                                   **/
/**           AD5664，四通道16位数模转换器                                   **/
/**                                                                          **/
/**     其中，AD5662的24位移位寄存器数据格式如下：                           **/
/**     +-+-+-+-+-+-+---+---+---+---+---+---+-------+--+--+--+--+            **/
/**     |  ------   |  模式 |                数据位             |            **/
/**     +-+-+-+-+-+-+---+---+---+---+---+---+-------+--+--+--+--+            **/
/**     |x|x|x|x|x|x|PD1|PD0|D15|D14|D13|D12| ......|D3|D2|D1|D0|            **/
/**     +-+-+-+-+-+-+---+---+---+---+---+---+-------+--+--+--+--+            **/
/**                                                                          **/
/**     其中，AD5663和AD5664的24位移位寄存器数据格式如下：                   **/
/**     +-+-+--+--+--+--+--+--+---+---+---+---+-------+--+--+--+--+          **/
/**     | - | 命令位 | 地址位 |                数据位             |          **/
/**     +-+-+--+--+--+--+--+--+---+---+---+---+-------+--+--+--+--+          **/
/**     |x|x|C2|C1|C0|A2|A1|A0|D15|D14|D13|D12|.......|D3|D2|D1|D0|          **/
/**     +-+-+--+--+--+--+--+--+---+---+---+---+-------+--+--+--+--+          **/
/**                                                                          **/
/**     其中，AD5643和AD5644的24位移位寄存器数据格式如下：                   **/
/**     +-+-+--+--+--+--+--+--+---+---+---+---+-------+--+--+--+--+-+-+      **/
/**     | - | 命令位 | 地址位 |                数据位             | - |      **/
/**     +-+-+--+--+--+--+--+--+---+---+---+---+-------+--+--+--+--+-+-+      **/
/**     |x|x|C2|C1|C0|A2|A1|A0|D13|D12|D11|D10|.......|D3|D2|D1|D0|x|x|      **/
/**     +-+-+--+--+--+--+--+--+---+---+---+---+-------+--+--+--+--+-+-+      **/
/**                                                                          **/
/**     其中，AD5623和AD5624的24位移位寄存器数据格式如下：                   **/
/**     +-+-+--+--+--+--+--+--+---+---+--+--+-------+--+--+--+--+-+-+-+-+    **/
/**     | - | 命令位 | 地址位 |                数据位           | ----- |    **/
/**     +-+-+--+--+--+--+--+--+---+---+--+--+-------+--+--+--+--+-+-+-+-+    **/
/**     |x|x|C2|C1|C0|A2|A1|A0|D11|D10|D9|D8|.......|D3|D2|D1|D0|x|x|x|x|    **/
/**     +-+-+--+--+--+--+--+--+---+---+--+--+-------+--+--+--+--+-+-+-+-+    **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    1、声明AD56xx对象实体，并调用函数AD56xxInitialization初始化对象实体。 **/
/**    初始化时，指定设备类型（DACTypeDef）                                  **/
/**    2、实现形如void f(uint8_t *tData,uint16_t tSize)的函数，并将其赋值    **/
/**    给AD56xx对象实体的WriteDataToDAC函数指针。                            **/
/**    3、实现形如void f(bool en)的函数，并将其赋值AD56xx对象实体的ChipSelcet**/
/**    函数指针。                                                            **/
/**    4、调用相应的函数实现需要的操作。                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2015-07-28          木南              创建文件               **/
/**     V1.0.1  2017-03-17          木南              作适应性修改           **/
/**     V1.5.0  2018-01-05          木南              修改使适用于同类器件   **/
/**     V2.0.0  2018-05-13          木南              修改为对象定义及操作   **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __ad5663function_h
#define __ad5663function_h

#include "stdint.h"

/* 定义AD56XX系列DAC操作错误类型 */
typedef enum AD56xxError {
  AD56xx_OK,            //无错误
  AD56xx_InitError,     //初始化错误
  AD56xx_ChannelError,  //通道错误
}AD56xxErrorType;

/*定义DAC器件的类型*/
typedef enum AD56xx{
  AD5662=0,
  AD5623=1,
  AD5643=2,
  AD5663=3,
  AD5624=4,
  AD5644=5,
  AD5664=6,
  TypeNumber,
}AD56xxType;

/*定义AD5663通道枚举*/
typedef enum AD56xxChannel {
  DAChannelA=0,
  DAChannelB=1,
  DAChannelC=2,
  DAChannelD=3,
  ChannelAll=4,
  ChannelNone=5
}AD56xxChannelType;

/*定义软件复位模式枚举*/
typedef enum AD56xxReset{
  ResetSoftware=0,
  ResetPoweron=1
}AD56xxResetType;

/*定义掉电模式枚举*/
typedef enum AD56xxPowerdown{
  NormalOperation=0,
  R1K2GND=1,
  R100K2GND=2,
  ThreeState=3
}AD56xxPowerdownType;

/* 定义AD56xx片选信号枚举 */
typedef enum AD56xxCS {
  AD56xxCS_Enable,
  AD56xxCS_Disable
}AD56xxCSType;

/* 内部参考电压源设置枚举 */
typedef enum AD56xxRef{
  AD56xxRef_OFF,
  AD56xxRef_ON
}AD56xxRefType;

/* 定义AD56XX对象类型 */
typedef struct AD56xxObject {
  AD56xxType objectType;        //DAC的类型
  void (*WriteDataToDAC)(uint8_t *tData,uint16_t tSize);        //向DAC发送数据
  void (*ChipSelcet)(AD56xxCSType cs);     //片选信号
}AD56xxObjectType;

/* 向DAC发送数据函数指针类型 */
typedef void (*AD56xxWrite)(uint8_t *tData,uint16_t tSize);
/* 片选操作函数指针类型 */
typedef void (*AD56xxChipSelcet)(AD56xxCSType cs);

/*设置DA通道的值*/
AD56xxErrorType SetAD56xxChannelValue(AD56xxObjectType *dacObj,AD56xxChannelType channel,uint16_t data);

/*设置AD56xx上电/掉电工作模式*/
void Ad56xxPowerUpDownMode(AD56xxObjectType *dacObj,AD56xxChannelType channel,AD56xxPowerdownType powerdownType);

/*对AD56xx进行软件复位*/
void Ad56xxSoftwareReset(AD56xxObjectType *dacObj,AD56xxResetType resetMode);

/* 开启或关闭内部参考电压源 */
void SetInternalReference(AD56xxObjectType *dacObj,AD56xxRefType ref);

/*设置AD56xx及同类器件LDAC功能*/
void SetAd56xxLdacFunction(AD56xxObjectType *dacObj,AD56xxChannelType channel);

/* 初始化AD56xx对象 */
AD56xxErrorType AD56xxInitialization(AD56xxObjectType *dacObj,AD56xxType objectType,AD56xxWrite write,AD56xxChipSelcet cs);

#endif
/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
