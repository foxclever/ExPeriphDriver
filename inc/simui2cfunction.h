/******************************************************************************/
/** 模块名称：自定义通用功能                                                 **/
/** 文件名称：simui2cfunction.h                                              **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于使用GPIO模拟I2C硬件接口通讯                                **/
/**           基于C语言编写，适用于通用MCU                                   **/
/**           SCL引脚配置为推挽输出                                          **/
/**           SDA引脚配置为开漏输出，在读取时设置为高电平                    **/
/**           速度可设置高于0K，低于400K的整数，默认值为100K                 **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    1、声明GPIO模拟I2C对象实体（如：SimuI2CTypeDef si2c），并调用函数     **/
/**       SimuI2CInitialization初始化对象实体。初始化时，给定工作频率1-500K  **/
/**       以kHz为单位，最小始终周期为2微秒，最大为500k微秒。                 **/
/**    2、实现形如void f(SimuI2CPinValue op)的函数，并将其赋值               **/
/**       给SimuI2CTypeDef对象实体的SetSCLPin函数指针。用以操作SCL引脚。     **/
/**    3、实现形如void f(SimuI2CPinValue op)的函数，并将其赋值               **/
/**       给SimuI2CTypeDef对象实体的SetSDAPin函数指针。用以操作SDA引脚。     **/
/**    4、实现形如uint8_t f(void)的函数，并将其赋值给SimuI2CTypeDef对象实体  **/
/**       的ReadSDAPin函数指针。用以读取SDA引脚。                            **/
/**    5、实现形如void f(volatile uint32_t period)的函数，并将其赋值         **/
/**       给SimuI2CTypeDef对象实体的Delayus函数指针。用于周期延时，单位为微秒**/
/**    6、调用相应的函数实现需要的操作。                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-05-15          木南              创建文件               **/
/**     V1.0.1  2018-08-28          木南              修改为对象操作         **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __SIMUI2C_FUNCTION_H
#define __SIMUI2C_FUNCTION_H

#include "stdint.h"
#include "stdbool.h"

typedef enum SimuI2CPinValue{
  Set=1,
  Reset=(!Set)
}SimuI2CPinValue;

typedef enum SimuI2CPin{
  SCL=0,
  SDA=1
}SimuI2CPin;

typedef enum SimuI2CStatus{
  I2C_OK=0,
  I2C_ERROR=1,
}SimuI2CStatus;

typedef struct SimuI2CObject{
  uint32_t period;                              //确定速度为大于0K小于等于400K的整数，默认为100K
  void (*SetSCLPin)(SimuI2CPinValue op);        //设置SCL引脚
  void (*SetSDAPin)(SimuI2CPinValue op);        //设置SDA引脚
  uint8_t (*ReadSDAPin)(void);                  //读取SDA引脚位
  void (*Delayus)(volatile uint32_t period);    //速度延时函数
}SimuI2CObjectType;

typedef void (*SimuI2CSetPin)(SimuI2CPinValue op);      //设置SDA引脚
typedef uint8_t (*SimuI2CReadSDAPin)(void);             //读取SDA引脚位
typedef void (*SimuI2CDelayus)(volatile uint32_t period);       //速度延时函数

/* GPIO模拟I2C通讯初始化 */
void SimuI2CInitialization(SimuI2CObjectType *simuI2CInstance,
                           uint32_t speed,
                           SimuI2CSetPin setSCL,
                           SimuI2CSetPin setSDA,
                           SimuI2CReadSDAPin readSDA,
                           SimuI2CDelayus delayus);

/* 通过模拟I2C向从站写数据 */
SimuI2CStatus WriteDataBySimuI2C(SimuI2CObjectType *simuI2CInstance,uint8_t deviceAddress,uint8_t *wData,uint16_t wSize);

/* 通过模拟I2C自从站读数据 */
SimuI2CStatus ReadDataBySimuI2C(SimuI2CObjectType *simuI2CInstance,uint8_t deviceAddress,uint8_t *rData, uint16_t rSize);

/* 通过模拟I2C实现对从站先写数据紧接读数据组合操作 */
SimuI2CStatus WriteReadDataBySimuI2C(SimuI2CObjectType *simuI2CInstance,uint8_t deviceAddress, uint8_t *wData,uint16_t wSize,uint8_t *rData, uint16_t rSize);

#endif  /*__SIMUI2C_FUNCTION_H 结束*/

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/
