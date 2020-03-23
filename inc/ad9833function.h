/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ad9833function.c                                               **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：声明和定义AD9833可编程波形发生器相关的各种函数和操作           **/
/**           AD9833可编程波形发生器采用SPI通讯接口，可以生成不同频率和幅值  **/
/**           的正弦播、三角波和方波。具有一个16位控制寄存器如下：           **/
/**           高位字节如下：                                                 **/
/**           +----+----+----+----+-------+-------+---+-----+                **/
/**           |DB15|DB14|DB13|DB12|  DB11 |  DB10 |DB9| DB8 |                **/
/**           +----+----+----+----+-------+-------+---+-----+                **/
/**           | 0  | 0  | B28| HLB|FSELECT|PSELECT| 0 |RESET|                **/
/**           +----+----+----+----+-------+-------+---+-----+                **/
/**           低位字节如下：                                                 **/
/**           +------+-------+-------+---+----+---+----+---+                 **/
/**           |  DB7 |  DB6  |  DB5  |DB4| DB3|DB2| DB1|DB0|                 **/
/**           +------+-------+-------+---+----+---+----+---+                 **/
/**           |SLEEP1|SLEEP12|OPBITEN| 0 |DIV2| 0 |MODE| 0 |                 **/
/**           +------+-------+-------+---+----+---+----+---+                 **/
/**           其中DB12要与DB13配合使用，DB3和DB1要与DB5配合使用。            **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    1、声明AD9833对象实体，并调用函数AD9833Initialization初始化对象实体。 **/
/**    初始化时，给定始终频率MCLK，单位为MHz。                               **/
/**    2、实现形如void f(uint8_t *tData,uint16_t tSize)的函数，并将其赋值    **/
/**    给AD9833对象实体的WriteDataToDAC函数指针。                            **/
/**    3、实现形如void f(bool en)的函数，并将其赋值AD9833对象实体的ChipSelcet**/
/**    函数指针。                                                            **/
/**    4、调用相应的函数实现需要的操作。                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2019-05-16          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __AD9833FUNCTION_H
#define __AD9833FUNCTION_H

#include "stdint.h"

/* 定义AD9833输出模式枚举 */
typedef enum Ad9833Out {
  sinusoid=0,
  triangular,
  square_msb_2,
  square_msb
}Ad9833OutMode;

/* 定义AD9833休眠模式枚举 */
typedef enum Ad9833Sleep {
  NoSleep=0,
  DACTurnOff,
  MCLKTurnOff,
  DACMCLKTurnOff
}Ad9833SleepMode;

/* 定义AD9833频率寄存器枚举 */
typedef enum Ad9833Freq {
  FREQ0,
  FREQ1
}Ad9833FreqReg;

/* 定义AD9833相位寄存器枚举 */
typedef enum Ad9833Phase {
  PHASE0,
  PHASE1
}Ad9833PhaseReg;

/*定义片选操作枚举*/
typedef enum AD9833CS {
  AD9833CS_ENABLE,
  AD9833CS_DISABLE
}AD9833CSType;

/* 定义可写频率寄存器枚举 */
typedef enum WriteAd9833Freq {
  FREQ0_B28,            //频率寄存器0全28位
  FREQ0_B14_LSB,        //频率寄存器0低14位
  FREQ0_B14_MSB,        //频率寄存器0高14位
  FREQ1_B28,            //频率寄存器1全28位
  FREQ1_B14_LSB,        //频率寄存器1低14位
  FREQ1_B14_MSB         //频率寄存器1高14位
}WriteAd9833FreqReg;

/* 定义AD9833对象类型 */
typedef struct Ad9833Object{
  uint16_t ctlRegister;         //控制寄存器
  float freqConstant;           //频率计算常数
  void (*WriteData)(uint8_t *tData,uint16_t tSize);     //向DAC发送数据
  void (*ChipSelcet)(AD9833CSType en);  //片选信号
  void (*Delayms)(volatile uint32_t nTime);     //ms延时操作指针
}Ad9833ObjectType;

/* 定义AD9833写数据指针类型 */
typedef void (*AD9833WriteData)(uint8_t *tData,uint16_t tSize);
/* 定义AD9833片选操作指针类型 */
typedef void (*AD9833ChipSelcet)(AD9833CSType en);
/* 定义AD9833 ms延时操作指针类型 */
typedef void (*AD9833Delayms)(volatile uint32_t nTime);

/* 定义AD9833控制寄存器操作宏 */
#define AD9833_CTRLB28          (0x1 << 13)		//设置频率寄存器的写入方式
#define AD9833_CTRLHLB          (0x1 << 12)		//频率寄存器高低为选择
#define AD9833_CTRLFSEL         (0x1 << 11)		//选择频率寄存器
#define AD9833_CTRLPSEL         (0x1 << 10)		//选择相位寄存器
#define AD9833_CTRLRESET        (0x1 << 8)		//复位设备
#define AD9833_CTRLSLEEP1       (0x1 << 7)		//MCLK控制
#define AD9833_CTRLSLEEP12      (0x1 << 6)		//DAC输出控制
#define AD9833_CTRLOPBITEN      (0x1 << 5)		//输出连接方式选择
#define AD9833_CTRLDIV2         (0x1 << 3)		//方波输出大小选择
#define AD9833_CTRLMODE         (0x1 << 1)		//输出模式选择

/* 定义频率寄存器和香味寄存器地址 */
#define FREQ0_Address           (0x4000)			//频率寄存器0的地址
#define FREQ1_Address           (0x8000)			//频率寄存器1的地址
#define PHASE0_Address          (0xC000)			//相位寄存器0的地址
#define PHASE1_Address          (0xE000)			//相位寄存器1的地址

/* 设置频率寄存器的值 */
void SetAD9833FreqRegister(Ad9833ObjectType *dev,WriteAd9833FreqReg reg,uint32_t freqValue);
/* 设置相位寄存器的值 */
void SetAD9833PhaseRegister(Ad9833ObjectType *dev,Ad9833PhaseReg reg,float phaseValue);
/* 频率寄存器选择 */
void SelectAD9833FregRegister(Ad9833ObjectType *dev,Ad9833FreqReg reg);
/* 相位寄存器选择 */
void SelectAD9833PhaseRegister(Ad9833ObjectType *dev,Ad9833PhaseReg reg);
/* 复位AD9833对象 */
void ResetAD9833Object(Ad9833ObjectType *dev);
/* 设置AD9833休眠状态 */
void SetAD9833SleepMode(Ad9833ObjectType *dev,Ad9833SleepMode mode);
/* 设置AD9833的输出模式 */
void SetAD9833OutputMode(Ad9833ObjectType *dev,Ad9833OutMode mode);
/* 初始化AD9833对象 */
void AD9833Initialization(Ad9833ObjectType *dev,
                          float mclk,
                          AD9833WriteData write,
                          AD9833ChipSelcet cs,
                          AD9833Delayms delayms);

#endif
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
