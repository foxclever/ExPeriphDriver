/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：hlptfunction.h                                                 **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现TSEV01CL55红外温度传感器的通讯                         **/
/**           采用I2C通讯通讯接口                                            **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    1、声明HLPTObjectType对象实体，并调用函数HLPTInitialization初始化     **/
/**       对象实体。初始化时，给定对象的地址。                               **/
/**    2、实现形如void f(struct HLPTObject *hlpt,uint8_t *rData,             **/
/**       uint16_t rSize)的函数，并将其赋值给HLPTObjectType对象实体的Receive **/
/**       函数指针。实现读数据的操作。                                       **/
/**    3、实现形如void f(struct HLPTObject *hlpt,uint8_t wData)的函数，并将  **/
/**       其赋值给HLPTObjectType对象实体的Transmit函数指针。实现发送命令操作 **/
/**    4、实现形如oid f(volatile uint32_t nTime)的函数，并将其赋值给         **/
/**       HLPTObjectType对象实体的Delay函数指针。实现延时操作，单位为毫秒。  **/
/**    5、调用相应的函数实现需要的操作。                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2019-02-26          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __HLPT_FUNCTION_H
#define __HLPT_FUNCTION_H

#include "stdint.h"

//定义TSEV01CL55红外温度传感器对象类型
typedef struct HLPTObject{
  uint8_t devAddress;           //对象的地址
  float tempObject;             //对象温度值
  float tempAmbient;            //环境温度
  void (*Delayms)(volatile uint32_t nTime);       //延时操作指针
  void (*Receive)(struct HLPTObject *hlpt,uint8_t *rData,uint16_t rSize);		//接收数据操作指针
  void (*Transmit)(struct HLPTObject *hlpt,uint8_t *wData,uint16_t wSize);		//发送数据操作指针
}HLPTObjectType;

/*延时操作指针类型*/
typedef void (*HLPTDelayms)(volatile uint32_t nTime);
/*接收数据操作指针类型*/
typedef void (*HLPTReceive)(struct HLPTObject *hlpt,uint8_t *rData,uint16_t rSize);
/*发送数据操作指针类型*/
typedef void (*HLPTTransmit)(struct HLPTObject *hlpt,uint8_t *wData,uint16_t wSize);

//读取温度值
void GetHLPTemperature(HLPTObjectType *hlpt);

/* 红外温度传感器对象初始化 */
void HLPTInitialization(HLPTObjectType *hlpt,   //初始化的对象变量
                        uint8_t address,        //设备I2C地址
                        HLPTReceive receive,    //接收函数指针
                        HLPTTransmit transmit,  //发送函数指针
                        HLPTDelayms delayms     //毫秒延时函数指针
                       );

#endif

/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
