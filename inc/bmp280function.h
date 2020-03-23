/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：bmp280function.c                                               **/
/** 版    本：V1.1.0                                                         **/
/** 简    介：用于实现BMP280压力及温度传感器的操作                           **/
/**           BMP280有37个可操作寄存器（包括26个校准数据）地址如下：         **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |寄存器名  |地址|bit7|bit6|bit5|bit4|bit3|bit2|bit1|bit0|复位状态|读写| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |temp_xlsb |0xFC|    temp_xlsb<7:4> | 0  | 0  | 0  | 0  |  0x00  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |temp_lsb  |0xFB|              temp_lsb<7:0>            |  0x00  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |temp_msb  |0xFA|              temp_msb<7:0>            |  0x80  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |press_xlsb|0xF9|   press_xlsb<7:4> | 0  | 0  | 0  | 0  |  0x00  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |press_lsb |0xF8|             press_lsb<7:0>            |  0x00  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |press_msb |0xF7|             press_msb<7:0>            |  0x80  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |config    |0xF5|   t_sb[2:0]  |  filter[2:0] |    |e[0]|  0x00  |读写| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |ctrl_meas |0xF4|  osrs_t[2:0] |  osrs_p[2:0] |mode[1:0]|  0x00  |读写| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  | status   |0xF3|                   |m[0]|         |u[0]|  0x00  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  | reset    |0xE0|             reset[7:0]                |  0x00  |只写| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |   id     |0xD0|             chip_id[7:]               |  0x58  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |calib25   |0xA1|        calibration data[共26]         |出厂设定|只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |calib00   |0x88|        calibration data[共26]         |出厂设定|只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  控制测量寄存器压力测量和温度测量的配置：                                **/
/**    +-----------+----+------------+    +-----------+----+-------------+   **/
/**    |osrs_p[2:0]|倍数|    精度    |    |osrs_t[2:0]|倍数|     精度    |   **/
/**    +-----------+----+------------+    +-----------+----+-------------+   **/
/**    |   000     |  - |      -     |    |   000     |  - |     -       |   **/
/**    +-----------+----+------------+    +-----------+----+-------------+   **/
/**    |   001     | x1 |16bit/2.62Pa|    |   001     | x1 |16bit/0.0050C|   **/
/**    +-----------+----+------------+    +-----------+----+-------------+   **/
/**    |   010     | x2 |17bit/1.31Pa|    |   010     | x2 |17bit/0.0025C|   **/
/**    +-----------+----+------------+    +-----------+----+-------------+   **/
/**    |   011     | x4 |18bit/0.66Pa|    |   011     | x4 |18bit/0.0012C|   **/
/**    +-----------+----+------------+    +-----------+----+-------------+   **/
/**    |   100     | x8 |19bit/0.33Pa|    |   100     | x8 |19bit/0.0006C|   **/
/**    +-----------+----+------------+    +-----------+----+-------------+   **/
/**    |101,110,111| x16|20bit/0.16Pa|    |101,110,111| x16|20bit/0.0003C|   **/
/**    +-----------+----+------------+    +-----------+----+-------------+   **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    1、声明BMP280ObjectType对象实体，并调用函数BMP280Initialization初始化 **/
/**       对象实体。初始化时，需要传递如下参数：                             **/
/**       bmpAddress,采用I2C接口必须的设备地址，0xEC或0xEE。SPI接口时不需要  **/
/**       port,通讯接口类型，I2C或者SPI                                      **/
/**       t_sb,间隔周期，配置寄存器配置参数                                  **/
/**       filter,过滤器滤波设置，配置寄存器配置参数                          **/
/**       spi3W_en,是否适用3线SPI控制，配置寄存器配置参数                    **/
/**       osrs_t,温度采样精度设置，测量控制寄存器配置参数                    **/
/**       osrs_p,压力采样精度，测量控制寄存器配置参数                        **/
/**       mode,电源模式，测量控制寄存器配置参数                              **/
/**       Read,读数据操作指针，其函数类型见后续描述                          **/
/**       Write,写数据操作指针，其函数类型见后续描述                         **/
/**       Delayms,延时操作指针，其函数类型见后续描述                         **/
/**       ChipSelect,片选操作指针，SPI接口必需。I2C接口不需要，可置NULL      **/
/**    2、实现形如void f(struct BMP280Object *bmp,uint8_t regAddress,        **/
/**       uint8_t *rData,uint16_t rSize)的函数，并将其赋值给BMP280ObjectType **/
/**       对象实体的Read函数指针。                                           **/
/**    3、实现形如void f(struct BMP280Object *bmp,uint8_t regAddress,        **/
/**       uint8_t command)的函数，并将其赋值给BMP280ObjectType对象实体       **/
/**       的Write函数指针。                                                  **/
/**    4、实现形如void f(volatile uint32_t nTime)的函数，并将其赋值给        **/
/**       BMP280ObjectType对象实体的Delayms函数指针。实现毫秒延时。          **/
/**    5、实现形如void f(bool en)的函数，并将其赋值BMP280ObjectType对象实体  **/
/**       的ChipSelcet函数指针。                                             **/
/**    6、调用相应的函数实现需要的操作。                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-07-17          木南              创建文件               **/
/**     V1.0.1  2018-05-20          木南              将设备定义为结构对象   **/
/**     V1.1.0  2019-05-30          木南              修改并增加类型定义     **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __BMP280FUNCTION_H
#define __BMP280FUNCTION_H

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"

/* 定义压力采样设定枚举类型 */
typedef enum BMP280PressureSample {
  BMP280_PRES_SAMPLE_X0=0x00,   //压力不采样
  BMP280_PRESE_SAMPLE_X1=0x04,  //压力采样16位
  BMP280_PRES_SAMPLE_X2=0x08,   //压力采样17位
  BMP280_PRES_SAMPLE_X4=0x0C,   //压力采样18位
  BMP280_PRES_SAMPLE_X8=0x10,   //压力采样19位
  BMP280_PRES_SAMPLE_X16=0x14   //压力采样20位
}BMP280PressureSampleType;

/* 定义温度采样设定枚举类型 */
typedef enum BMP280TemperatureSample {
  BMP280_TEMP_SAMPLE_X0=0x00,   //温度不采样
  BMP280_TEMP_SAMPLE_X1=0x20,   //温度采样16位
  BMP280_TEMP_SAMPLE_X2=0x40,   //温度采样17位
  BMP280_TEMP_SAMPLE_X4=0x60,   //温度采样18位
  BMP280_TEMP_SAMPLE_X8=0x80,   //温度采样19位
  BMP280_TEMP_SAMPLE_X16=0xA0   //温度采样20位
}BMP280TemperatureSampleType;

/* 定义电源模式设定枚举类型 */
typedef enum BMP280PowerMode {
  BMP280_POWER_SLEEP_MODE=0x00,         //睡眠模式
  BMP280_POWER_FORCED_MODE=0x01,        //强制模式
  BMP280_POWER_NORMAL_MODE=0x03         //正常模式
}BMP280PowerModeType;

/* 定义等待时间设定枚举类型 */
typedef enum BMP280TimeStandby {
  BMP280_T_SB_0P5=0x00,		//等待时间0.5ms
  BMP280_T_SB_62P5=0x20,	//等待时间62.5ms
  BMP280_T_SB_125=0x40,		//等待时间125ms
  BMP280_T_SB_250=0x60,		//等待时间250ms
  BMP280_T_SB_500=0x80,		//等待时间500ms
  BMP280_T_SB_1000=0xA0,	//等待时间1000ms
  BMP280_T_SB_2000=0xC0,	//等待时间2000ms
  BMP280_T_SB_4000=0xE0		//等待时间4000ms
}BMP280TimeStandbyType;

/* 定义IIR滤波设定枚举类型 */
typedef enum BMP280IIRFilterCoeff {
  BMP280_IIR_FILTER_COEFF_OFF=0x00,   //关闭滤波器
  BMP280_IIR_FILTER_COEFF_X2=0x04,    //滤波器系数2
  BMP280_IIR_FILTER_COEFF_X4=0x08,    //滤波器系数4
  BMP280_IIR_FILTER_COEFF_X8=0x0C,    //滤波器系数8
  BMP280_IIR_FILTER_COEFF_X16=0x10    //滤波器系数16
}BMP280IIRFilterCoeffType;

/* 定义3线SPI使能枚举类型 */
typedef enum BMP280UseSPI3w {
  BMP280_SPI3W_DISABLE=0x00,    //不启用3线SPI
  BMP280_SPI3W_ENABLE=0x01      //启用3线SPI
}BMP280UseSPI3wType;

/* 定义BMP280通讯接口枚举 */
typedef enum BMP280Port{
  BMP280_I2C,
  BMP280_SPI
}BMP280PortType;

/* 定义修正参数操作对象类型 */
typedef struct Bmp280CalibParam {
  uint16_t dig_t1;      //温度修正系数1
  int16_t  dig_t2;      //温度修正系数2
  int16_t  dig_t3;      //温度修正系数3
  uint16_t dig_p1;      //压力修正系数1
  int16_t  dig_p2;      //压力修正系数2
  int16_t  dig_p3;      //压力修正系数3
  int16_t  dig_p4;      //压力修正系数4
  int16_t  dig_p5;      //压力修正系数5
  int16_t  dig_p6;      //压力修正系数6
  int16_t  dig_p7;      //压力修正系数7
  int16_t  dig_p8;      //压力修正系数8
  int16_t  dig_p9;      //压力修正系数9
  int32_t  t_fine;      //用于压力修正的温度数据
}Bmp280CalibParamType;

/* 定义片选信号枚举 */
typedef enum BMP280CS {
  BMP280CS_Enable,
  BMP280CS_Disable
}BMP280CSType;

/*定义BMP280操作对象*/
typedef struct BMP280Object{
  uint8_t bmpAddress;   //I2C通讯时的设备地址
  uint8_t chipID;       //芯片ID
  uint8_t config;       //配置寄存器
  uint8_t ctrlMeas;     //测量控制寄存器
  BMP280PortType port;  //接口选择
  Bmp280CalibParamType caliPara;        //校准参数
  float pressure;       //压力值
  float temperature;    //温度值
  void (*Read)(struct BMP280Object *bmp,uint8_t regAddress,uint8_t *rData,uint16_t rSize);      //读数据操作指针
  void (*Write)(struct BMP280Object *bmp,uint8_t regAddress,uint8_t command);   //写数据操作指针
  void (*Delayms)(volatile uint32_t nTime);     //延时操作指针
  void (*ChipSelect)(BMP280CSType cs);  //使用SPI接口时，片选操作
}BMP280ObjectType;

/* 定义读数据操作函数指针类型 */
typedef void (*BMP280Read)(BMP280ObjectType *bmp,uint8_t regAddress,uint8_t *rData,uint16_t rSize);
/* 定义写数据操作函数指针类型 */
typedef void (*BMP280Write)(BMP280ObjectType *bmp,uint8_t regAddress,uint8_t command);
/* 定义延时操作函数指针类型 */
typedef void (*BMP280Delayms)(volatile uint32_t nTime);
/* 定义使用SPI接口时，片选操作函数指针类型 */
typedef void (*BMP280ChipSelect)(BMP280CSType cs);

/*读取压力和温度数据*/
void GetBMP280Measure(BMP280ObjectType *bmp);

/*实现BMP280初始化配置*/
void BMP280Initialization(BMP280ObjectType *bmp,                //BMP280对象
                          uint8_t bmpAddress,                   //I2C接口是设备地址
                          BMP280PortType port,                  //接口选择
                          BMP280TimeStandbyType t_sb,           //间隔周期
                          BMP280IIRFilterCoeffType filter,      //过滤器
                          BMP280UseSPI3wType spi3W_en,          //3线SPI控制
                          BMP280TemperatureSampleType osrs_t,   //温度精度
                          BMP280PressureSampleType osrs_p,      //压力精度
                          BMP280PowerModeType mode,             //电源模式
                          BMP280Read Read,                      //读数据操作指针
                          BMP280Write Write,                    //写数据操作指针
                          BMP280Delayms Delayms,                //延时操作指针
                          BMP280ChipSelect ChipSelect           //片选操作指针
                         );

#endif
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
