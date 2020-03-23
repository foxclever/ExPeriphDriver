/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ad7192function.h                                               **/
/** 版    本：V1.0.2                                                         **/
/** 简    介：声明和定义AD7192模数转换芯片相关的各种函数和操作               **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-07-26          木南              创建文件               **/
/**     V1.0.1  2017-12-06          木南              将寄存器数组变为参数   **/
/**     V1.0.2  2018-05-09          木南              添加操作延时函数       **/
/**     V2.0.0  2018-05-23          木南              添加操作对象，并修改全 **/
/**                                                   部相关函数的形参       **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __AD7192FUNCTION_H
#define __AD7192FUNCTION_H

#include "stdint.h"

/* 定义AD7192错误类型 */
typedef enum AD7192Error {
  AD7192_OK,            //无错误
  AD7192_InitError,     //初始化错误
}AD7192ErrorType;

/* 定义AD7192片选信号枚举 */
typedef enum AD719xCS {
  AD7192CS_Enable,
  AD7192CS_Disable
}AD7192CSType;

/*定义AD7192校准方式枚举*/
typedef enum AD7192Calibration {
  AD7192_Internal,	//内部校准模式
  AD7192_External		//外部校准模式
}AD7192CalibrationType;

/*定义AD7192寄存器枚举*/
typedef enum AD7192Register {
  AD7192_REG_COM_STA=0x00,      //读为状态寄存器，写为通讯寄存器*/
  AD7192_REG_MODE=0x01,         //操作模式寄存器*/
  AD7192_REG_CONF=0x02,         //操作配置寄存器*/
  AD7192_REG_DATA=0x03,         //操作数据寄存器*/
  AD7192_REG_ID=0x04,           //操作ID寄存器*/
  AD7192_REG_GPOCON=0x05,       //操作GPOCON寄存器*/
  AD7192_REG_OFFSET=0x06,       //操作失调寄存器*/
  AD7192_REG_FS=0x07,           //操作满量程寄存器*/
  AD7192RegisterNumber          //寄存器数量
}AD7192RegisterType;

/*定义AD719x输入通道枚举*/
typedef enum AD7192Channel{
  CHAIN1AIN2=0x000100,  //差分通道1
  CHAIN3AIN4=0x000200,  //差分通道2
  CHTEMP=0x000400,      //内部温度
  CHAIN2AIN2=0x000800,  //模拟通道2
  CHAIN1COM=0x001000,   //单端输入1
  CHAIN2COM=0x002000,   //单端输入2
  CHAIN3COM=0x004000,   //单端输入3
  CHAIN4COM=0x008000    //单端输入4
}AD7192ChannelType;

/* 定义AD719x增益枚举类型 */
typedef enum AD7192Gain {
  AD7192Gain1,          //1倍增益
  AD7192Gain8,          //8倍增益
  AD7192Gain16,         //16倍增益
  AD7192Gain32,         //32倍增益
  AD7192Gain64,         //64倍增益
  AD7192Gain128         //128倍增益
}AD7192GainType;

/* 定义AD719x极性枚举类型 */
typedef enum AD7192Polar {
  AD7192_Unipolar,      //单极性
  AD7192_Bipolar        //双极性
}AD7192PolarType;


/*定义用于操作的结构体*/
typedef struct Ad7192Object {
  uint8_t polar;        //通道的极性
  uint8_t gain;         //通道增益
  uint16_t fs;          //数据输出速度
  uint32_t Registers[8];        //存放寄存器值的数组
  void (*ReadWrite)(uint8_t *wData,uint8_t *rData,uint16_t size);       //实现读写操作
  void (*ChipSelect)(AD7192CSType cs);  //实现片选
  uint16_t (*GetReadyInput)(void);      //实现Ready状态监视
  void (*Delay)(volatile uint32_t nTime);       //实现ms延时操作
}Ad7192ObjectType;

/*定义读写操作函数指针类型*/
typedef void (*AD7192ReadWriteType)(uint8_t *wData,uint8_t *rData,uint16_t size);
/*实现片选*/
typedef void (*AD7192ChipSelectType)(AD7192CSType cs);
/*实现Ready状态监视*/
typedef uint16_t (*AD7192GetReadyInputType)(void);
/*实现ms延时操作*/
typedef void (*AD7192DelaymsType)(volatile uint32_t nTime);

/*连续转换数据获取*/
void GettContinuousConvertionValue(Ad7192ObjectType *adObj,uint32_t Channels,uint32_t *dataCodes,int number);

/*单次转换数据获取*/
uint32_t GetSingleConvertionValue(Ad7192ObjectType *adObj,uint32_t Channels);

/*读取内部温度数据，返回摄氏度温度*/
float GetTemperatureValue(Ad7192ObjectType *adObj);

/*AD7192初始化配置*/
AD7192ErrorType AD7192Initialization(Ad7192ObjectType *adObj,           //AD7192对象
                                     uint32_t Channels,                 //通道
                                     AD7192PolarType polar,             //极性
                                     AD7192GainType gain,               //增益
                                     AD7192CalibrationType cali,        //校准方式
                                     uint16_t fs,                       //数据输出速率设定：1-1023，数越大频率越低
                                     AD7192ReadWriteType readWrite,     //读写函数指针
                                     AD7192ChipSelectType cs,           //片选函数指针
                                     AD7192GetReadyInputType ready,     //就绪函数指针
                                     AD7192DelaymsType delayms          //延时函数指针
                                    );
#endif
/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
