/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：sht3xfunction.h                                                **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现SHT3X数字温湿度计的操作                                **/
/**           单次读取命令如下：                                             **/
/**           +---------------+------------+                                 **/
/**           |     条件      |十六进制代码|                                 **/
/**           +------+--------+------+-----+                                 **/
/**           |重复性|时钟拉伸| MSB  | LSB |                                 **/
/**           +------+--------+------+-----+                                 **/
/**           | High | enabled| 0x2C | 06  |                                 **/
/**           +------+--------+------+-----+                                 **/
/**           |Medium| enabled| 0x2C | 0D  |                                 **/
/**           +------+--------+------+-----+                                 **/
/**           |  Low | enabled| 0x2C | 10  |                                 **/
/**           +------+--------+------+-----+                                 **/
/**           | High |disabled| 0x24 | 00  |                                 **/
/**           +------+--------+------+-----+                                 **/
/**           |Medium|disabled| 0x24 | 0B  |                                 **/
/**           +------+--------+------+-----+                                 **/
/**           |  Low |disabled| 0x24 | 16  |                                 **/
/**           +------+--------+------+-----+                                 **/
/**           周期性读取命令如下：                                           **/
/**           +-------------------+------------+                             **/
/**           |        条件       |十六进制代码|                             **/
/**           +------+------------+------+-----+                             **/
/**           |重复性|每秒测量次数| MSB  | LSB |                             **/
/**           +------+------------+------+-----+                             **/
/**           | High |     0.5    | 0x20 |  32 |                             **/
/**           +------+------------+------+-----+                             **/
/**           |Medium|     0.5    | 0x20 |  24 |                             **/
/**           +------+------------+------+-----+                             **/
/**           | Low  |     0.5    | 0x20 |  2F |                             **/
/**           +------+------------+------+-----+                             **/
/**           | High |      1     | 0x21 |  30 |                             **/
/**           +------+------------+------+-----+                             **/
/**           |Medium|      1     | 0x21 |  26 |                             **/
/**           +------+------------+------+-----+                             **/
/**           | Low  |      1     | 0x21 |  2D |                             **/
/**           +------+------------+------+-----+                             **/
/**           | High |      2     | 0x22 |  36 |                             **/
/**           +------+------------+------+-----+                             **/
/**           |Medium|      2     | 0x22 |  20 |                             **/
/**           +------+------------+------+-----+                             **/
/**           | Low  |      2     | 0x22 |  2B |                             **/
/**           +------+------------+------+-----+                             **/
/**           | High |      4     | 0x23 |  34 |                             **/
/**           +------+------------+------+-----+                             **/
/**           |Medium|      4     | 0x23 |  22 |                             **/
/**           +------+------------+------+-----+                             **/
/**           | Low  |      4     | 0x23 |  29 |                             **/
/**           +------+------------+------+-----+                             **/
/**           | High |      10    | 0x27 |  37 |                             **/
/**           +------+------------+------+-----+                             **/
/**           |Medium|      10    | 0x27 |  21 |                             **/
/**           +------+------------+------+-----+                             **/
/**           | Low  |      10    | 0x27 |  2A |                             **/
/**           +------+------------+------+-----+                             **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    1、声明SHT3xObject对象实体，并调用函数SHT3xInitialization初始化对象   **/
/**       实体。初始化时，指定设备地址：0x44,0x45,0x88和0x8A或使用相应的宏   **/
/**       定义：I2CADDRESSA、I2CADDRESSB、I2CADDRESSC和I2CADDRESSD           **/
/**    2、实现形如void f(uint8_t devAddress,uint8_t *rData,uint16_t rSize)的 **/
/**       函数，并将其赋值给SHT3xObject对象实体的Receive函数指针。实现数据接 **/
/**       收。                                                               **/
/**    3、实现形如void f(uint8_t devAddress,uint8_t *tData,uint16_t tSize)的 **/
/**       函数，并将其赋值给SHT3xObject对象实体的Transmit函数指针。实现数据  **/
/**       发送。                                                             **/
/**    4、实现形如void f(volatile uint32_t nTime)的函数，并将其赋值给        **/
/**       SHT3xObject对象实体的Delay函数指针。实现操作延时，单位毫秒         **/
/**    5、调用相应的函数实现需要的操作。                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2018-12-01          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __SHT3XFUNCTION_H
#define __SHT3XFUNCTION_H

#include "stdint.h"

/* SHT3x传感器操作故障状态定义 */
typedef enum SHT3xError{
  SHT3X_NO_ERROR       = 0x00,  // 无错误
  SHT3X_ACK_ERROR      = 0x01,  // 应答确认错误
  SHT3X_CHECKSUM_ERROR = 0x02,  // 校验错误
  SHT3X_TIMEOUT_ERROR  = 0x04,  // 超时错误
  SHT3X_PARM_ERROR     = 0x80,  // 参数超限错误
	SHT3X_INIT_ERROR		 = 0x03,	// SHT3X对象初始化错误
}SHT3xErrorType;

/* SHT3x传感器操作命令 */
typedef enum{
  CMD_MEAS_CLOCKSTR_H = 0x2C06,  // 单次数据采集模式，时钟延展，高重复性
  CMD_MEAS_CLOCKSTR_M = 0x2C0D,  // 单次数据采集模式，时钟延展，中重复性
  CMD_MEAS_CLOCKSTR_L = 0x2C10,  // 单次数据采集模式，时钟延展，低重复性
  CMD_MEAS_POLLING_H = 0x2400,   // 单次数据采集模式，轮询，高重复性
  CMD_MEAS_POLLING_M = 0x240B,   // 单次数据采集模式，轮询，中重复性
  CMD_MEAS_POLLING_L = 0x2416,   // 单次数据采集模式，轮询，低重复性
  CMD_MEAS_PERI_05_H = 0x2032,   // 周期数据采集模式，0.5 mps, 高重复性
  CMD_MEAS_PERI_05_M = 0x2024,   // 周期数据采集模式，0.5 mps, 中重复性
  CMD_MEAS_PERI_05_L = 0x202F,   // 周期数据采集模式，0.5 mps, 低重复性
  CMD_MEAS_PERI_1_H = 0x2130,    // 周期数据采集模式，1 mps, 高重复性
  CMD_MEAS_PERI_1_M = 0x2126,    // 周期数据采集模式，1 mps, 中重复性
  CMD_MEAS_PERI_1_L = 0x212D,    // 周期数据采集模式，1 mps, 低重复性
  CMD_MEAS_PERI_2_H = 0x2236,    // 周期数据采集模式，2 mps, 高重复性
  CMD_MEAS_PERI_2_M = 0x2220,    // 周期数据采集模式，2 mps, 中重复性
  CMD_MEAS_PERI_2_L = 0x222B,    // 周期数据采集模式，2 mps, 低重复性
  CMD_MEAS_PERI_4_H = 0x2334,    // 周期数据采集模式，4 mps, 高重复性
  CMD_MEAS_PERI_4_M = 0x2322,    // 周期数据采集模式，4 mps, 中重复性
  CMD_MEAS_PERI_4_L = 0x2329,    // 周期数据采集模式，4 mps, 低重复性
  CMD_MEAS_PERI_10_H = 0x2737,   // 周期数据采集模式，10 mps, 高重复性
  CMD_MEAS_PERI_10_M = 0x2721,   // 周期数据采集模式，10 mps, 中重复性
  CMD_MEAS_PERI_10_L = 0x272A,   // 周期数据采集模式，10 mps, 低重复性
  CMD_FETCH_DATA = 0xE000,       // 读出周期模式的测量结果
  CMD_ART=0x2B32,                // 加速响应时间命令
  CMD_BREAK_STOP=0x3093,         // 中断命令或停止周期采集模式
  CMD_SOFT_RESET = 0x30A2,       // 软件复位
  CMD_GENERAL_CALL_RESET=0x0006, // 广播复位
  CMD_HEATER_ENABLE = 0x306D,    // 启用加热器
  CMD_HEATER_DISABLE = 0x3066,   // 禁用加热器
  CMD_READ_STATUS = 0xF32D,      // 读状态寄存器
  CMD_CLEAR_STATUS = 0x3041,     // 清状态寄存器
  CMD_READ_SERIALNBR = 0x3780,   // 读序列号
  CMD_R_AL_LIM_LS = 0xE102,      // 读报警限制，低设置
  CMD_R_AL_LIM_LC = 0xE109,      // 读报警限制，低清除
  CMD_R_AL_LIM_HS = 0xE11F,      // 读报警限制，高设置
  CMD_R_AL_LIM_HC = 0xE114,      // 读报警限制，高清除
  CMD_W_AL_LIM_HS = 0x611D,      // 写报警限制，高设置
  CMD_W_AL_LIM_HC = 0x6116,      // 写报警限制，高清除
  CMD_W_AL_LIM_LS = 0x6100,      // 写报警限制，低设置
  CMD_W_AL_LIM_LC = 0x610B,      // 写报警限制，低清除
  CMD_NO_SLEEP = 0x303E,         // 无休眠
}SHT3xCommands;

/* SHT3x传感器测量重复性 */
typedef enum SHT3xRepeatability{
  REPEATAB_HIGH,        // 高重复性
  REPEATAB_MEDIUM,      // 中重复性
  REPEATAB_LOW,         // 低重复性
}SHT3xRepeatability;

/* SHT3x传感器单次测量模式 */
typedef enum SHT3xMode{
  MODE_CLKSTRETCH,      // clock stretching
  MODE_POLLING,         // polling
}SHT3xMode;

/* 周期性数据采集的频率定义 */
typedef enum SHT3xFrequency{
  FREQUENCY_HZ5,        // 每秒0.5次
  FREQUENCY_1HZ,        // 每秒1.0次
  FREQUENCY_2HZ,        // 每秒2.0次
  FREQUENCY_4HZ,        // 每秒4.0次
  FREQUENCY_10HZ,       // 每秒10.0次
}SHT3xFrequency;

/* 状态寄存器格式定义 */
typedef union {
  uint16_t word;
  struct{
    #ifdef LITTLE_ENDIAN        // 系统为小端模式时，状态寄存器各位的定义
    uint16_t CrcStatus     : 1;     // 写数据校验状态
    uint16_t CmdStatus     : 1;     // 命令状态
    uint16_t Reserve0      : 2;     // 保留位
    uint16_t ResetDetected : 1;     // 系统复位探测
    uint16_t Reserve1      : 5;     // 保留位
    uint16_t T_Alert       : 1;     // 温度追踪报警
    uint16_t RH_Alert      : 1;     // 湿度追踪报警
    uint16_t Reserve2      : 1;     // 保留位
    uint16_t HeaterStatus  : 1;     // 加热器状态
    uint16_t Reserve3      : 1;     // 保留位
    uint16_t AlertPending  : 1;     // 报警未确认状态 
    #else                       // 系统为大端模式时，状态寄存器各位的定义
    uint16_t AlertPending  : 1;     // 报警未确认状态 
    uint16_t Reserve3      : 1;     // 保留位
    uint16_t HeaterStatus  : 1;     // 加热器状态
    uint16_t Reserve2      : 1;     // 保留位
    uint16_t RH_Alert      : 1;     // 湿度追踪报警
    uint16_t T_Alert       : 1;     // 温度追踪报警
    uint16_t Reserve1      : 5;     // 保留位
    uint16_t ResetDetected : 1;     // 系统复位探测
    uint16_t Reserve0      : 2;     // 保留位
    uint16_t CmdStatus     : 1;     // 命令状态
    uint16_t CrcStatus     : 1;     // 写数据校验状态
    #endif
  }bit;
}SHT3xStatusRegister;

/* 定义SHT3x对象类型 */
typedef struct SHT3xObject{
  uint8_t devAddress;           // SHT3x对象的地址
	uint16_t status;							//SHT3x状态及存期的值
  uint32_t serialNumber;        // SHT3x对象的序列号
  float temperature;
  float humidity;
  void (*Delayms)(volatile uint32_t nTime);       //延时操作指针
  void (*Receive)(struct SHT3xObject *sht,uint8_t *rData,uint16_t rSize);    //接收数据操作指针
  void (*Transmit)(struct SHT3xObject *sht,uint8_t *tData,uint16_t tSize);   //发送数据操作指针
}SHT3xObjectType;

/* 毫秒延时函数指针类型 */
typedef void (*SHT3xDelayms)(volatile uint32_t nTime);
/* 接收数据函数指针类型 */
typedef void (*SHT3xReceive)(SHT3xObjectType *sht,uint8_t *rData,uint16_t rSize);
/* 发送数据函数指针类型 */
typedef void (*SHT3xTransmit)(SHT3xObjectType *sht,uint8_t *tData,uint16_t tSize);

/* SHT3x对象初始化 */
SHT3xErrorType SHT3xInitialization(SHT3xObjectType *sht,		//待初始化的SHT3X
                                   uint8_t address,					//设备地址
                                   SHT3xReceive receive,		//接收数据函数指针
                                   SHT3xTransmit transmit,	//发送数据函数指针
                                   SHT3xDelayms delayms			//毫秒延时函数指针
                                  );

/* 清除状态寄存器的状态标志 */
void SHT3xClearStatusRegister(SHT3xObjectType *sht);

/* 读取状态寄存器的值 */
SHT3xErrorType SHT3xReadStatusRegister(SHT3xObjectType *sht,SHT3xStatusRegister *status);

/* SHT3x禁用加热器 */
void SHT3xDisableHeater(SHT3xObjectType *sht);

/* SHT3x启用加热器 */
void SHT3xEnableHeater(SHT3xObjectType *sht);

/* SHT3x软件复位 */
void SHT3xSoftReset(SHT3xObjectType *sht);

/* SHT3x广播复位 */
void SHT3xGeneralCallReset(SHT3xObjectType *sht);

/* SHT3x接口复位 */
void SHT3xInterfaceReset(SHT3xObjectType *sht);

/* 停止周期采集模式*/
void SHT3xBreakCommand(SHT3xObjectType *sht);

/* 设置周期采集使用ART */
void SHT3xARTCommand(SHT3xObjectType *sht);

/* 启动周期性数据采集 */
void SHT3xStartPeriodicMeasurment(SHT3xObjectType *sht,SHT3xRepeatability repeatability,SHT3xFrequency frequency);

/* 读取周期性采集的数据 */
SHT3xErrorType SHT3xFetchPeriodicMeasurmentData(SHT3xObjectType *sht);

/* 获取一次性采集 */
SHT3xErrorType SHT3xGetSingleShotData(SHT3xObjectType *sht,SHT3xRepeatability repeatability,SHT3xMode mode);

#endif
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
