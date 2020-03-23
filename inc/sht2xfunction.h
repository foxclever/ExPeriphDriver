/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：sht2xfunction.h                                                **/
/** 版    本：V1.0.1                                                         **/
/** 简    介：用于实现SHT2X温湿度计的操作                                    **/
/**           默认分辨率14bit(温度)和12bit(湿度)可以通过用户寄存器修改       **/
/**           相对湿度的计算公式：RH=-6+125*SRH/Pow(2,16)，计算基于液态水的  **/
/**           相对湿度，而基于冰的相对湿度可以计算得到：                     **/
/**           RHi=RHw*exp（17.62*t/(243.12+t))/exp(22.46*t/(272.62+t));      **/
/**           温度的计算公式：T=-46.85+175.72*ST/Pow(2,16);                  **/
/**           不同分辨率下ADC转换时间：                                      **/
/**           +------+--------+--------+-------+-------+----+                **/
/**           |分辨率|RH典型值|RH最大值|T典型值|T最大值|单位|                **/
/**           +------+--------+--------+-------+-------+----+                **/
/**           | 14bit| ——   | ——   |  66   |  85   | ms |                **/
/**           +------+--------+--------+-------+-------+----+                **/
/**           | 13bit| ——   | ——   |  33   |  43   | ms |                **/
/**           +------+--------+--------+-------+-------+----+                **/
/**           | 12bit|   22   |   29   |  17   |  22   | ms |                **/
/**           +------+--------+--------+-------+-------+----+                **/
/**           | 11bit|   12   |   15   |   9   |  11   | ms |                **/
/**           +------+--------+--------+-------+-------+----+                **/
/**           | 10bit|   7    |    9   | ——  | ——  | ms |                **/
/**           +------+--------+--------+-------+-------+----+                **/
/**           | 8bit |   3    |    4   | ——  | ——  | ms |                **/
/**           +------+--------+--------+-------+-------+----+                **/
/**           SHT2x命令代码：                                                **/
/**           +------------+----------+----------+                           **/
/**           |    命令    |   描述   | 命令代码 |                           **/
/**           +------------+----------+----------+                           **/
/**           | 触发T测量  | 保持主机 | 11100011 |                           **/
/**           +------------+----------+----------+                           **/
/**           | 触发RH测量 | 保持主机 | 11100101 |                           **/
/**           +------------+----------+----------+                           **/
/**           | 触发T测量  |非保持主机| 11110011 |                           **/
/**           +------------+----------+----------+                           **/
/**           | 触发RH测量 |非保持主机| 11110101 |                           **/
/**           +------------+----------+----------+                           **/
/**           |写用户寄存器|          | 11100110 |                           **/
/**           +------------+----------+----------+                           **/
/**           |读用户寄存器|          | 11100111 |                           **/
/**           +------------+----------+----------+                           **/
/**           |   软复位   |          | 11111110 |                           **/
/**           +------------+----------+----------+                           **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    1、声明SHT2xDevice对象实体，并调用函数SHT2xInitialization初始化对象   **/
/**       实体。SHT2x设备地址固定为0x80                                      **/
/**    2、实现形如void f(uint8_t devAddress,uint8_t *rData,uint16_t rSize)的 **/
/**       函数，并将其赋值给SHT2xDevice对象实体的Receive函数指针。实现数据接 **/
/**       收。                                                               **/
/**    3、实现形如void f(uint8_t devAddress,uint8_t *tData,uint16_t tSize)的 **/
/**       函数，并将其赋值给SHT2xDevice对象实体的Transmit函数指针。实现数据  **/
/**       发送。                                                             **/
/**    4、实现形如void f(volatile uint32_t nTime)的函数，并将其赋值给        **/
/**       SHT2xDevice对象实体的Delay函数指针。实现操作延时，单位毫秒         **/
/**    5、调用相应的函数实现需要的操作。                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-07-17          木南              创建文件               **/
/**     V1.0.1  2018-04-17          木南              添加SHT2x对象定义并修  **/
/**                                                   改各函数形参           **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __SHT2XFUNCTION_H
#define __SHT2XFUNCTION_H

#include "stdint.h"
#include "stdbool.h"

/* 定义操作命令宏 */
#define MEASURE_RH_COMMAND_HOST         0XE5//主机模式读取相对湿度
#define MEASURE_T_COMMAND_HOST          0XE3//主机模式读温度
#define MEASURE_RH_COMMAND_NOHOST       0XF5//非主机模式读相对湿度
#define MEASURE_T_COMMAND_NOHOST        0XF3//非主机模式读温度
#define READ_USER_REGISTER              0XE7//读用户寄存器
#define WRITE_USER_REGISTER             0XE6//写用户寄存器
#define SOFT_RESET                      0XFE//软复位

/* 定义SHT2X测量分辨率枚举 */
typedef enum SHT2xDPI {
  SHT2x_DPI_RH12_T14,	//湿度12位温度14位精度
  SHT2x_DPI_RH8_T13,	//湿度8位温度13位精度
  SHT2x_DPI_RH10_T12,	//湿度10位温度12位精度
  SHT2x_DPI_RH11_T11	//湿度11位温度11位精度
}SHT2xDPIType;

/* 定义SHT2X电池结束状态枚举 */
typedef enum SHT2xBattery {
  SHT2x_End_High,			//电池状态VDD>2.25
  SHT2x_End_Low				//电池状态VDD<2.25
}SHT2xBatteryType;

/* 定义SHT2X加热器状态枚举 */
typedef enum SHT2xHeater {
  SHT2xHEATERDISABLE,    //禁用片内加热
  SHT2xHEATERENABLE   	 //启用片内加热
}SHT2xHeaterType;

/* 定义SHT2X OTP状态枚举 */
typedef enum SHT2xOTP {
  SHT2xOTPENABLE,    //能启动OTP加载
  SHT2xOTPDISABLE    //不能启动OTP加载
}SHT2xOTPType;

/*定义SHT2x对象*/
typedef struct SHT2xObject{
  uint8_t userReg;      //用户寄存器
  uint8_t devAddress;   //设备地址
  uint8_t sn[8];        //设备序列号
  uint16_t tempCode;    //温度的数值码
  uint16_t humiCode;    //湿度的数值码
  void (*Delayms)(volatile uint32_t nTime);       //毫秒延时操作指针
  void (*Transmit)(struct SHT2xObject *sht,uint8_t *tData,uint16_t tSize);   //发送数据操作指针
  void (*Receive)(struct SHT2xObject *sht,uint8_t *rData,uint16_t rSize);    //接收数据操作指针
}SHT2xObjectType;

/* 毫秒延时函数指针类型 */
typedef void (*SHT2xDelayms)(volatile uint32_t nTime);
/* 发送数据函数指针类型 */
typedef void (*SHT2xTransmit)(struct SHT2xObject *sht,uint8_t *tData,uint16_t tSize);
/* 接收数据函数指针类型 */
typedef void (*SHT2xReceive)(struct SHT2xObject *sht,uint8_t *rData,uint16_t rSize);

/*读取SHT2x的温度数据*/
float GetSHT2xTemperatureValue(SHT2xObjectType *sht,uint8_t cmd);

/*读取SHT2x的湿度数据*/
float GetSHT2xHumidityValue(SHT2xObjectType *sht,uint8_t cmd);

/*软件复位*/
void SoftResetSHT2x(SHT2xObjectType *sht);

/* 初始化配置SHT2x */
void SHT2xInitialization(SHT2xObjectType *sht,          //SHT2X对象变量
                         SHT2xDPIType dpi,              //测量分辨率配置
                         SHT2xBatteryType endBat,       //电池结束状态配置
                         SHT2xHeaterType heater,        //加热器是否启用配置
                         SHT2xOTPType otp,              //是否加载OTP配置
                         SHT2xTransmit write,           //写操作指针
                         SHT2xReceive read,             //读操作指针
                         SHT2xDelayms delayms);         //毫秒延时指针

#endif
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
