/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：bme280function.h                                               **/
/** 版    本：V1.1.0                                                         **/
/** 简    介：用于实现BME280压力、温度及湿度传感器的操作                     **/
/**           BME280有55个可操作寄存器（包括26个校准数据）地址如下：         **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |寄存器名  |地址|bit7|bit6|bit5|bit4|bit3|bit2|bit1|bit0|复位状态|读写| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |hum_lsb   |0xFE|               hum_lsb<7:0>            |  0x00  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |hum_msb   |0xFD|               hum_msb<7:0>            |  0x80  |只读| **/
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
/**  |ctrl_hum  |0xF2|                        |  osrs_h[2:0] |  0x00  |读写| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |calib41   |0xF0|        calibration data[共41]         |出厂设定|只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |calib26   |0xE1|        calibration data[共41]         |出厂设定|只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  | reset    |0xE0|             reset[7:0]                |  0x00  |只写| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |   id     |0xD0|             chip_id[7:0]              |  0x60  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |calib25   |0xA1|        calibration data[共41]         |出厂设定|只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |calib00   |0x88|        calibration data[共41]         |出厂设定|只读| **/
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
/**    1、声明BME280ObjectType对象实体，并调用函数BME280Initialization初始化 **/
/**       对象实体。初始化时，需要传递如下参数：                             **/
/**       bmpAddress,采用I2C接口必须的设备地址，0xEC或0xEE。SPI接口时不需要  **/
/**       port,通讯接口类型，I2C或者SPI                                      **/
/**       t_sb,间隔周期，配置寄存器配置参数                                  **/
/**       filter,过滤器滤波设置，配置寄存器配置参数                          **/
/**       spi3W_en,是否适用3线SPI控制，配置寄存器配置参数                    **/
/**       osrs_t,温度采样精度设置，测量控制寄存器配置参数                    **/
/**       osrs_p,压力采样精度，测量控制寄存器配置参数                        **/
/**       mode,电源模式，测量控制寄存器配置参数                              **/
/**       osrs_h,湿度采样精度设置，湿度测量控制寄存器配置参数                **/
/**       Read,读数据操作指针，其函数类型见后续描述                          **/
/**       Write,写数据操作指针，其函数类型见后续描述                         **/
/**       Delayms,延时操作指针，其函数类型见后续描述                         **/
/**       ChipSelect,片选操作指针，SPI接口必需。I2C接口不需要，可置NULL      **/
/**    2、实现形如void f(struct BME280Object *bmp,uint8_t regAddress,        **/
/**       uint8_t *rData,uint16_t rSize)的函数，并将其赋值给BME280ObjectType **/
/**       对象实体的Read函数指针。                                           **/
/**    3、实现形如void f(struct BME280Object *bmp,uint8_t regAddress,        **/
/**       uint8_t command)的函数，并将其赋值给BME280ObjectType对象实体       **/
/**       的Write函数指针。                                                  **/
/**    4、实现形如void f(volatile uint32_t nTime)的函数，并将其赋值给        **/
/**       BME280ObjectType对象实体的Delayms函数指针。实现毫秒延时。          **/
/**    5、实现形如void f(bool en)的函数，并将其赋值BME280ObjectType对象实体  **/
/**       的ChipSelcet函数指针。                                             **/
/**    6、调用GetBME280Measure函数实现数据的读取。                           **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-07-17          木南              创建文件               **/
/**     V1.0.1  2018-05-20          木南              将设备定义为结构对象   **/
/**     V1.1.0  2018-06-01          木南              修改类型定义及数据返回 **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __BME280FUNCTION_H
#define __BME280FUNCTION_H

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"

/* 定义压力采样设定枚举类型 */
typedef enum BME280PresSample {
  BME280_PRES_SAMPLE_X0=0x00,    //压力不采样
  BME280_PRES_SAMPLE_X1=0x04,    //压力采样1倍，不滤波时16位
  BME280_PRES_SAMPLE_X2=0x08,    //压力采样2倍，不滤波时17位
  BME280_PRES_SAMPLE_X4=0x0C,    //压力采样4倍，不滤波时18位
  BME280_PRES_SAMPLE_X8=0x10,    //压力采样8倍，不滤波时19位
  BME280_PRES_SAMPLE_X16=0x14    //压力采样16倍，不滤波时20位
}BME280PresSampleType;

/* 定义温度采样设定枚举类型 */
typedef enum BME280TempSample {
  BME280_TEMP_SAMPLE_X0=0x00,    //温度不采样
  BME280_TEMP_SAMPLE_X1=0x20,    //温度采样1倍，不滤波时16位
  BME280_TEMP_SAMPLE_X2=0x40,    //温度采样2倍，不滤波时17位
  BME280_TEMP_SAMPLE_X4=0x60,    //温度采样4倍，不滤波时18位
  BME280_TEMP_SAMPLE_X8=0x80,    //温度采样8倍，不滤波时19位
  BME280_TEMP_SAMPLE_X16=0xA0    //温度采样16倍，不滤波时20位
}BME280TempSampleType;

/* 定义湿度采样设定枚举类型 */
typedef enum BME280HumiSample {
  BME280_HUMI_SAMPLE_X0=0x00,    //湿度不采样
  BME280_HUMI_SAMPLE_X1=0x01,    //湿度采样1倍
  BME280_HUMI_SAMPLE_X2=0x02,    //湿度采样2倍
  BME280_HUMI_SAMPLE_X4=0x03,    //湿度采样4倍
  BME280_HUMI_SAMPLE_X8=0x04,    //湿度采样8倍
  BME280_HUMI_SAMPLE_X16=0x05    //湿度采样16倍
}BME280HumiSampleType;

/* 定义电源模式设定枚举类型 */
typedef enum BME280PowerMode {
  BME280_POWER_SLEEP_MODE=0x00,    	//睡眠模式
  BME280_POWER_FORCED_MODE=0x01,		//强制模式
  BME280_POWER_NORMAL_MODE=0x03    	//正常模式
}BME280PowerModeType;

/* 定义等待时间设定枚举类型 */
typedef enum BME280TimeSB {
  BME280_T_SB_0P5=0x00,		//等待时间0.5ms
  BME280_T_SB_62P5=0x20,	//等待时间62.5ms
  BME280_T_SB_125=0x40,		//等待时间125ms
  BME280_T_SB_250=0x60,		//等待时间250ms
  BME280_T_SB_500=0x80,		//等待时间500ms
  BME280_T_SB_1000=0xA0,	//等待时间1000ms
  BME280_T_SB_10=0xC0,		//等待时间10ms
  BME280_T_SB_20=0xE0			//等待时间20ms
}BME280TimeSBType;

/* 定义IIR滤波系数设定枚举类型 */
typedef enum BME280IIRFilter {
  BME280_IIR_FILTER_COEFF_OFF=0x00,     //关闭滤波器
  BME280_IIR_FILTER_COEFF_X2=0x04,      //滤波器系数2
  BME280_IIR_FILTER_COEFF_X4=0x08,      //滤波器系数4
  BME280_IIR_FILTER_COEFF_X8=0x0C,      //滤波器系数8
  BME280_IIR_FILTER_COEFF_X16=0x10      //滤波器系数16
}BME280IIRFilterType;

/* 定义3线SPI使能枚举类型 */
typedef enum BME280SPI3wUse {
  BME280_SPI3W_DISABLE=0x00,   //不启用3线SPI
  BME280_SPI3W_ENABLE=0x01     //启用3线SPI
}BME280SPI3wUseType;

/* 定义BMP280通讯接口枚举 */
typedef enum BME280Port {
  BME280_I2C,
  BME280_SPI
}BME280PortType;

/* 定义修正参数操作对象类型 */
typedef struct BME280CalibParam {
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
  uint8_t  dig_h1;      //湿度修正系数1	
  int16_t  dig_h2;      //湿度修正系数2
  uint8_t  dig_h3;      //湿度修正系数3	
  int16_t  dig_h4;      //湿度修正系数4
  int16_t  dig_h5;      //湿度修正系数5
  int8_t   dig_h6;      //湿度修正系数6
  int32_t  t_fine;      //用于压力修正的温度数据
}BME280CalibParamType;

/*定义BME280片选操作枚举*/
typedef enum BME280CS{
  BME280CS_ENABLE,
  BME280CS_DISABLE
}BME280CSType;

/*定义Bme280操作对象*/
typedef struct BME280Object{
  uint8_t chipID;       //芯片ID
  uint8_t bmeAddress;   //I2C通讯时的设备地址
  uint8_t config;       //配置寄存器
  uint8_t ctrlMeas;     //测量控制寄存器
  uint8_t ctrlHumi;     //湿度测量控制寄存器
  BME280PortType port;  //接口选择
  BME280CalibParamType caliPara;        //校准参数
  float temperature;    //温度值
  float pressure;       //压力值
  float humidity;       //湿度值
  void (*Read)(struct BME280Object *bme,uint8_t regAddress,uint8_t *rData,uint16_t rSize);      //读数据操作指针
  void (*Write)(struct BME280Object *bme,uint8_t regAddress,uint8_t command);   //写数据操作指针
  void (*Delayms)(volatile uint32_t nTime);     //延时操作指针
  void (*ChipSelect)(BME280CSType cs);  //使用SPI接口时，片选操作
}BME280ObjectType;

/* 定义读数据操作函数指针类型 */
typedef void (*BME280Read)(struct BME280Object *bme,uint8_t regAddress,uint8_t *rData,uint16_t rSize);
/* 定义写数据操作函数指针类型 */
typedef void (*BME280Write)(struct BME280Object *bme,uint8_t regAddress,uint8_t command);
/* 定义延时操作函数指针类型 */
typedef void (*BME280Delayms)(volatile uint32_t nTime);
/* 定义使用SPI接口时，片选操作函数指针类型 */
typedef void (*BME280ChipSelect)(BME280CSType cs);

/*读取压力和温度数据*/
void GetBME280Measure(BME280ObjectType *bme);

/*实现BME280初始化配置*/
void BME280Initialization(BME280ObjectType *bme,        //BMP280对象
                          uint8_t bmeAddress,           //I2C接口是设备地址
                          BME280PortType port,          //接口选择
                          BME280TimeSBType t_sb,        //间隔周期
                          BME280IIRFilterType filter,   //过滤器
                          BME280SPI3wUseType spi3W_en,  //3线SPI控制
                          BME280TempSampleType osrs_t,  //温度精度
                          BME280PresSampleType osrs_p,  //压力精度
                          BME280PowerModeType mode,     //电源模式
                          BME280HumiSampleType osrs_h,  //湿度精度
                          BME280Read Read,              //读数据操作指针
                          BME280Write Write,            //写数据操作指针
                          BME280Delayms Delayms,        //延时操作指针
                          BME280ChipSelect ChipSelect   //片选操作指针
                         );

#endif
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
