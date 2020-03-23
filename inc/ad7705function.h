/******************************************************************************/
/** 模块名称：数据检测与处理                                                 **/
/** 文件名称：ad7705function.h                                               **/
/** 版    本：V1.1.0                                                         **/
/** 简    介：声明和定义ad7705模数转换芯片相关的各种函数和操作               **/
/**         AD7705拥有8个片上寄存器，其中通讯、配置、时钟等寄存器结构如下：  **/
/**         +------+----+----+----+------+------+----+---+-----+             **/
/**         |寄存器| DB7| DB6| DB5| DB4  |  DB3 | DB2|DB1| DB0 |             **/
/**         +------+----+----+----+------+------+----+---+-----+             **/
/**         | 通讯 |DRDY| RS2| RS1|  RS0 |  R/W |STBY|CH1| CH0 |             **/
/**         +------+----+----+----+------+------+----+---+-----+             **/
/**         | 配置 | MD1| MD0| G2 |  G1  |  G0  | B/U|BUF|FSYNC|             **/
/**         +------+----+----+----+------+------+----+---+-----+             **/
/**         | 时钟 |ZERO|ZERO|ZERO|CLKDIS|CLKDIV| CLK|FS1| FS0 |             **/
/**         +------+----+----+----+------+------+----+---+-----+             **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2015-07-28          木南              创建文件               **/
/**     V1.1.0  2019-08-28          木南              改为基于对象的操作     **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __ad7705function_h
#define __ad7705function_h

#include "stdint.h"

/* 定义AD7705寄存器枚举 */
typedef enum AD7705Reg {
  REG_COMM,
  REG_SETUP,
  REG_CLOCK,
  REG_DATA,
  REG_TEST,
  REG_NOUSE,
  REG_ZERO,
  REG_FULL
}AD7705RegType;

/* 定义AD7705通道枚举 */
typedef enum AD7705Channel {
  Channel1,
  Channel2
}AD7705ChannelType;

/* 定义AD7705工作模式枚举 */
typedef enum AD7705Mode {
  Normal,
  SelfCali,
  ZeroCali,
  FullCali
}AD7705ModeType;

/* 定义AD7705增益枚举 */
typedef enum AD7705Gain {
  Gain_1,       //1倍增益
  Gain_2,       //2倍增益
  Gain_4,       //4倍增益
  Gain_8,       //8倍增益
  Gain_16,      //16倍增益
  Gain_32,      //32倍增益
  Gain_64,      //64倍增益
  Gain_128      //128倍增益
}AD7705GainType;

/* 定义AD77705片选信号枚举 */
typedef enum AD7705CS {
  AD7705CS_Enable,
  AD7705CS_Disable
}AD7705CSType;

/* 定义AD7705输出频率枚举 */
typedef enum AD7705OutRate {
  Rate20Hz,
  Rate25Hz,
  Rate100Hz,
  Rate200Hz,
  Rate50Hz,
  Rate60Hz,
  Rate250Hz,
  Rate500Hz
}AD7705OutRateType;

/* 定义AD7705外部时钟枚举 */
typedef enum AD7705Mclk {
  Mclk2457600,  //2.4576MHz
  Mclk4915200,  //4.9152MHz
  Mclk1000000,  //1MHz
  Mclk2000000   //2MHz
}AD7705MclkType;

/* 定义AD7705对象类型 */
typedef struct AD7705Object {
  uint8_t registers[3]; //用于存储通讯、配置和时钟寄存器
  uint8_t (*ReadWriteByte)(uint8_t data);       //读写操作
  uint8_t (*CheckDataIsReady)(void);            //就绪信号检测
  void (*ChipSelect)(AD7705CSType cs);          //实现片选
  void (*Delayms)(volatile uint32_t nTime);     //实现ms延时操作
  void (*Delayus)(volatile uint32_t nTime);     //实现us延时操作
}AD7705ObjectType;

/*定义读写AD7705函数指针类型*/
typedef uint8_t (*AD7705ReadWriteByteType)(uint8_t data);
/*定义就绪检测函数指针类型*/
typedef uint8_t (*AD7705CheckDataIsReadyType)(void);
/*定义片选信号函数指针类型*/
typedef void (*AD7705ChipSelect)(AD7705CSType cs);
/*定义延时操作函数指针类型*/
typedef void (*AD7705Delay)(volatile uint32_t nTime);

//读取AD7705个通道的值
uint16_t GetAD7705ChannelValue(AD7705ObjectType *ad,AD7705ChannelType channel);

/* AD7705对象初始化函数 */
void AD7705Initialization(AD7705ObjectType *ad,
                          AD7705GainType gain,
                          AD7705MclkType mclk,
                          AD7705OutRateType rate,
                          AD7705ReadWriteByteType spiReadWrite,
                          AD7705CheckDataIsReadyType checkReady,
                          AD7705ChipSelect cs,
                          AD7705Delay msDelay,
                          AD7705Delay usDelay);

#endif
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
