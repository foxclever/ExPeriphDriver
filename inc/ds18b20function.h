/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ds18b20function.h                                              **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：实现温度检测模块DS18B20的数据读取                              **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2012-07-28          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef _DS18B20FUNCTION_H
#define _DS18B20FUNCTION_H

#include "stdint.h"

/* 定义DS18B20引脚输出值枚举 */
typedef enum Ds18b20PinValue{
  DS18B20_Set=1,
  DS18B20_Reset=(!DS18B20_Set)
}Ds18b20PinValueType;

/* 定义DS18B20引脚方向枚举 */
typedef enum Ds18b20IOMode{
  DS18B20_Out=1,
  DS18B20_In=(!DS18B20_Out)
}Ds18b20IOModeType;

/* 定义DS18B20操作状态枚举 */
typedef enum Ds18b20Status{
  DS18B20_OK,   //正常
  DS18B20_InitialError, //初始值错误
  DS18B20_NoResponse    //设备无应答
}Ds18b20StatusType;

/*定义分辨率枚举*/
typedef enum Ds18b20Resolution {
  BITS9,
  BITS10,
  BITS11,
  BITS12
}Ds18b20ResolutionType;

/* 定义DS18B20对象类型 */
typedef struct Ds18b20Object {
  uint8_t sn[6];        //Ds18b20元件序列号
  float temperature;    //温度数据
  void (*SetBit)(Ds18b20PinValueType vBit);     //写数据位到DS18B20
  uint8_t (*GetBit)(void);      //从DS18B20读取一位数据
  void (*SetPinMode)(Ds18b20IOModeType mode);   //设置DS18B20的数据引脚的输入输出模式
  void (*Delayus)(volatile uint32_t nTime);     //延时us操作指针
}Ds18b20ObjectType;

/*写数据位到DS18B20*/
typedef void (*Ds18b20SetBitType)(Ds18b20PinValueType vBit);
/*从DS18B20读取一位数据*/
typedef uint8_t (*Ds18b20GetBitType)(void);
/*设置DS18B20的数据引脚的输入输出模式*/
typedef void (*Ds18b20SetPinModeType)(Ds18b20IOModeType mode);
/* 定义延时操作函数指针类型 */
typedef void (*Ds18b20DelayType)(volatile uint32_t nTime);

/*温度数据获取、转换函数*/
float GetDS18b20TemperatureValue(Ds18b20ObjectType *ds18b20);

/*对DS18B20操作进行初始化*/
Ds18b20StatusType Ds18b20Initialization(Ds18b20ObjectType *ds18b20,     //被初始化的对象变量
                                        Ds18b20SetBitType setBit,       //向总线写一位操作
                                        Ds18b20GetBitType getBit,       //从总线读一位操作
                                        Ds18b20SetPinModeType pinDirection,     //总线输入输出模式控制
                                        Ds18b20DelayType delayus        //为秒延时操作
                                       );

#endif
/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
