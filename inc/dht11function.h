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

#ifndef __DHT11FUNCTION_H
#define __DHT11FUNCTION_H 

#include "stdint.h"

/*定义DHT11错误消息枚举*/
typedef enum DHT11Error{
  DHT11_NoError,        //没有错误
  DHT11_None,           //未检测到DHT11
  DHT11_InitError,      //初始化错误
  DHT11_DataError       //通讯校验错误
}DHT11ErrorType;

/*定义单总线方向枚举*/
typedef enum DHT11IOMode{
  DHT11_Out=1,
  DHT11_In=(!DHT11_Out)
}DHT11IOModeType;

/*定义单总线操作值枚举*/
typedef enum DhtPinValue{
  DHT11_Set=1,
  DHT11_Reset=(!DHT11_Set)
}DhtPinValueType;

/* 定义DHT11对象类型 */
typedef struct Dht11Object {
  float temperature;            //温度值
  float humidity;               //湿度值
  
  void (*SetPinOutValue)(DhtPinValueType setValue);//设置DHT11引脚的输出值
  uint8_t (*ReadPinBit)(void);//读取引脚电平
  void (*SetPinMode)(DHT11IOModeType mode);//设置引脚的输入输出方向
  
  void (*Delayms)(volatile uint32_t nTime);     //实现ms延时操作
  void (*Delayus)(volatile uint32_t nTime);     //实现us延时操作
}Dht11ObjectType;

/*定义设置DHT11引脚的输出值函数类型*/
typedef void (*Dht11SetPinOutValueType)(DhtPinValueType setValue);
/*定义读取引脚电平函数类型*/
typedef uint8_t (*Dht11ReadPinBitType)(void);
/*定义设置引脚的输入输出方向函数类型*/
typedef void (*Dht11SetPinModeType)(DHT11IOModeType mode);
/*定义实现ms延时操作函数类型*/
typedef void (*Dht11DelayType)(volatile uint32_t nTime);      

/*从DHT11读取数据,temp:温度值(0-50),humi:湿度值(20%-90%),返回值：0,正常;1,失败*/
DHT11ErrorType GetProcessValueFromDHT11(Dht11ObjectType *dht);

/*DHT11初始化操作*/
DHT11ErrorType InitializeDHT11(Dht11ObjectType *dht,    //需要初始化对象
                               Dht11SetPinOutValueType setPinStatus,    //设置总线输出值
                               Dht11ReadPinBitType getPinStatus,        //读取总线输入值
                               Dht11SetPinModeType mode,        //配置总线的输入输出模式
                               Dht11DelayType delayms,  //毫秒延时
                               Dht11DelayType delayus   //微秒延时
                              );

#endif
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
