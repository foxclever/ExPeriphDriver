/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：mlx90614function.h                                             **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现MLX90614红外温度传感器的通讯                           **/
/**           采用SMBus通讯通讯接口                                          **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    SMBus通讯通讯接口的定义与低速I2C接口定义相同，可以采用硬件I2C接口和   **/
/**    软件模拟I2C接口实现通讯。                                             **/
/**    1、声明MLX90614对象实体，并调用函数MLXInitialization初始化对象实体。  **/
/**       初始化时，给定对象的地址。                                         **/
/**    2、实现形如void f(struct MLXObject *mlx,uint8_t cmd,uint8_t *rData,   **/
/**       uint16_t rSize)的函数，并将其赋值给MLX90614对象实体的Read函数指针。**/
/**       实现数据的读取操作。                                               **/
/**    3、实现形如void f(struct MLXObject *mlx,uint8_t cmd,uint8_t *wData,   **/
/**       uint16_t wSize)的函数，并将其赋值给MLX90614对象实体的Write函数指   **/
/**       针。实现数据的写操作。                                             **/
/**    4、实现形如oid f(volatile uint32_t nTime)的函数，并将其赋值给MLX90614t**/
/**       对象实体的Delay函数指针。实现延时操作，时间单位为毫秒。            **/
/**    5、调用相应的函数实现需要的操作。                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2019-05-05          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __MLX90614FUNCTION_H
#define __MLX90614FUNCTION_H

#include "stdint.h"

/*定义MLX90614对象类型*/
typedef struct MLXObject {
  uint8_t devAddress;           //对象的地址
  uint8_t flags;                //对象状态标志
  uint16_t pwmctrl;             //PWM控制寄存器
  uint16_t ConfigRegister;      //配置寄存器
  uint16_t ID[4];               //对象的ID值
  float tempAmbient;            //温度值
  float tempObject1;            //温度值
  float tempObject2;            //温度值
  void (*Read)(struct MLXObject *mlx,uint8_t cmd,uint8_t *rData,uint16_t rSize);    //读数据操作指针
  void (*Write)(struct MLXObject *mlx,uint8_t cmd,uint8_t *wData,uint16_t wSize);   //写数据操作指针
  void (*Delayus)(volatile uint32_t nTime);       //延时操作指针
}MLXObjectType;

/*定义读MLX90614数据操作指针类型*/
typedef void (*MLXRead)(struct MLXObject *mlx,uint8_t cmd,uint8_t *rData,uint16_t rSize);
/*定义写MLX90614数据操作指针类型*/
typedef void (*MLXWrite)(struct MLXObject *mlx,uint8_t cmd,uint8_t *wData,uint16_t wSize);
/*定义微秒延时操作指针类型*/
typedef void (*MLXDelayus)(volatile uint32_t nTime);

/*读取温度值*/
void GetMLXTemperature(MLXObjectType *mlx);

/* 读状态 */
uint16_t ReadFlagFromMLX(MLXObjectType *mlx);

/* 使设备进入休眠状态 */
void EnterSleepModeForMLX(MLXObjectType *mlx);

/* 配置PWM控制 */
void SetPWMControll(MLXObjectType *mlx,uint16_t data);

/* 设置配置寄存器 */
void SetConfigRegister(MLXObjectType *mlx,uint16_t data);

/* 修改设备地址 */
void ModifyDeviceAddress(MLXObjectType *mlx,uint16_t address);

/* 红外温度传感器对象初始化 */
void MLXInitialization(MLXObjectType *mlx,      //MLX90614对象
                       uint8_t address,         //设备地址
                       MLXRead read,            //读数据函数指针
                       MLXWrite write,          //写数据函数指针
                       MLXDelayus delayus       //微秒延时函数指针
                      );

#endif
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
