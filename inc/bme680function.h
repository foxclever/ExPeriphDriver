/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：bme680function.h                                               **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现BME680压力、温度及湿度传感器的操作                     **/
/**           BME680有55个可操作寄存器（包括26个校准数据）地址如下：         **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |寄存器名  |地址|bit7|bit6|bit5|bit4|bit3|bit2|bit1|bit0|复位状态|读写| **/
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
/**  |config    |0x75|              |  filter[2:0] |    |e[0]|  0x00  |读写| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |ctrl_meas |0x74|  osrs_t[2:0] |  osrs_p[2:0] |mode[1:0]|  0x00  |读写| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  | status   |0x73|              |m[0]|                   |  0x00  |读写| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |ctrl_hum  |0x72|    |e[0]|              |  osrs_h[2:0] |  0x00  |读写| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |ctrl_gas_1|0x71|              |r[0]|    nb_conv<3:0>   |  0x00  |读写| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |ctrl_gas_0|0x70|                   |o[0]|              |  0x00  |读写| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |gas_wait_9|0x6D|           gas_wait_x[共10]            |  0x00  |读写| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |gas_wait_0|0x64|           gas_wait_x[共10]            |  0x00  |读写| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |res_heat_9|0x63|           res_heat_x[共10]            |  0x00  |读写| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |res_heat_0|0x5A|           res_heat_x[共10]            |  0x00  |读写| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |idac_heat9|0x59|           idac_heat_x[共10]           |  0x00  |读写| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |idac_heat0|0x50|           idac_heat_x[共10]           |  0x00  |读写| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |gas_lsb   |0x2B| hum_lsb |v[0]|s[0]|   gas_range<4:0>  |  0x00  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |gas_msb   |0x2A|               gas_msb<7:0>            |  0x00  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |hum_lsb   |0x26|               hum_lsb<7:0>            |  0x00  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |hum_msb   |0x25|               hum_msb<7:0>            |  0x80  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |temp_xlsb |0x24|    temp_xlsb<7:4> | 0  | 0  | 0  | 0  |  0x00  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |temp_lsb  |0x23|              temp_lsb<7:0>            |  0x00  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |temp_msb  |0x22|              temp_msb<7:0>            |  0x80  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |press_xlsb|0x21|   press_xlsb<7:4> | 0  | 0  | 0  | 0  |  0x00  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |press_lsb |0x20|             press_lsb<7:0>            |  0x00  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |press_msb |0x1F|             press_msb<7:0>            |  0x80  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |eas_status|0x1D|n[0]|m[0]|m[0]|    |     index<3:0>    |  0x00  |只读| **/
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
/**    1、声明BME680ObjectType对象实体，并调用函数BME680Initialization初始化 **/
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
/**    2、实现形如void f(struct BME680Object *bmp,uint8_t regAddress,        **/
/**       uint8_t *rData,uint16_t rSize)的函数，并将其赋值给BME680ObjectType **/
/**       对象实体的Read函数指针。                                           **/
/**    3、实现形如void f(struct BME680Object *bmp,uint8_t regAddress,        **/
/**       uint8_t command)的函数，并将其赋值给BME680ObjectType对象实体       **/
/**       的Write函数指针。                                                  **/
/**    4、实现形如void f(volatile uint32_t nTime)的函数，并将其赋值给        **/
/**       BME680ObjectType对象实体的Delayms函数指针。实现毫秒延时。          **/
/**    5、实现形如void f(bool en)的函数，并将其赋值BME680ObjectType对象实体  **/
/**       的ChipSelcet函数指针。                                             **/
/**    6、调用GetBME680Measure函数实现数据的读取。                           **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2019-06-01          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __BME680FUNCTION_H
#define __BME680FUNCTION_H


#include "stdint.h"
#include "stdbool.h"

//温压补偿计算采用的数据类型：0为浮点型，1为32位整型
#define BME680_COMPENSATION_SELECTED (0)

/* 定义压力采样设定枚举类型 */
typedef enum BME680PresSample {
  BME680_PRES_SAMPLE_X0=0x00,    //压力不采样
  BME680_PRES_SAMPLE_X1=0x04,    //压力采样1倍，不滤波时16位
  BME680_PRES_SAMPLE_X2=0x08,    //压力采样2倍，不滤波时17位
  BME680_PRES_SAMPLE_X4=0x0C,    //压力采样4倍，不滤波时18位
  BME680_PRES_SAMPLE_X8=0x10,    //压力采样8倍，不滤波时19位
  BME680_PRES_SAMPLE_X16=0x14    //压力采样16倍，不滤波时20位
}BME680PresSampleType;

/* 定义温度采样设定枚举类型 */
typedef enum BME680TempSample {
  BME680_TEMP_SAMPLE_X0=0x00,    //温度不采样
  BME680_TEMP_SAMPLE_X1=0x20,    //温度采样1倍，不滤波时16位
  BME680_TEMP_SAMPLE_X2=0x40,    //温度采样2倍，不滤波时17位
  BME680_TEMP_SAMPLE_X4=0x60,    //温度采样4倍，不滤波时18位
  BME680_TEMP_SAMPLE_X8=0x80,    //温度采样8倍，不滤波时19位
  BME680_TEMP_SAMPLE_X16=0xA0    //温度采样16倍，不滤波时20位
}BME680TempSampleType;

/* 定义湿度采样设定枚举类型 */
typedef enum BME680HumiSample {
  BME680_HUMI_SAMPLE_X0=0x00,    //湿度不采样
  BME680_HUMI_SAMPLE_X1=0x01,    //湿度采样1倍
  BME680_HUMI_SAMPLE_X2=0x02,    //湿度采样2倍
  BME680_HUMI_SAMPLE_X4=0x03,    //湿度采样4倍
  BME680_HUMI_SAMPLE_X8=0x04,    //湿度采样8倍
  BME680_HUMI_SAMPLE_X16=0x05    //湿度采样16倍
}BME680HumiSampleType;

/* 定义IIR滤波系数设定枚举类型 */
typedef enum BME680IIRFilter {
  BME680_IIR_FILTER_COEFF_OFF=0x00,     //关闭滤波器0
  BME680_IIR_FILTER_COEFF_X1=0x04,      //滤波器系数1
  BME680_IIR_FILTER_COEFF_X3=0x08,      //滤波器系数3
  BME680_IIR_FILTER_COEFF_X7=0x0C,      //滤波器系数7
  BME680_IIR_FILTER_COEFF_X15=0x10,     //滤波器系数15
  BME680_IIR_FILTER_COEFF_X31=0x14,     //滤波器系数31
  BME680_IIR_FILTER_COEFF_X63=0x18,     //滤波器系数63
  BME680_IIR_FILTER_COEFF_X127=0x1C     //滤波器系数127
}BME680IIRFilterType;

/* 定义3线SPI使能枚举类型 */
typedef enum BME680SPI3wUse {
  BME680_SPI3W_DISABLE=0x00,   //不启用3线SPI
  BME680_SPI3W_ENABLE=0x01     //启用3线SPI
}BME680SPI3wUseType;

/* 定义等待时间设定枚举类型 */
typedef enum BME680GasWaitTime {
  BME680_GAS_WAIT_TIME1=0x00,   //气体传感器等待时间1倍
  BME680_GAS_WAIT_TIME4=0x40,   //气体传感器等待时间4倍
  BME680_GAS_WAIT_TIME16=0x80,  //气体传感器等待时间16倍
  BME680_GAS_WAIT_TIME64=0xC0   //气体传感器等待时间64倍
}BME680GasWaitTimeType;

/* 定义加热器配置设定点 */
typedef enum BME680HeaterSP {
  BME680_HEATER_SP0=0x00,       //加热器配置设定点0
  BME680_HEATER_SP1=0x01,       //加热器配置设定点1
  BME680_HEATER_SP2=0x02,       //加热器配置设定点2
  BME680_HEATER_SP3=0x03,       //加热器配置设定点3
  BME680_HEATER_SP4=0x04,       //加热器配置设定点4
  BME680_HEATER_SP5=0x05,       //加热器配置设定点5
  BME680_HEATER_SP6=0x06,       //加热器配置设定点6
  BME680_HEATER_SP7=0x07,       //加热器配置设定点7
  BME680_HEATER_SP8=0x08,       //加热器配置设定点8
  BME680_HEATER_SP9=0x09        //加热器配置设定点9
}BME680HeaterSPType;

/* 定义SPI选择内存页面枚举 */
typedef enum BME680MemeryPage {
  BME680_MEMERY_PAGE0=0x00,     //SPI内存页面0
  BME680_MEMERY_PAGE1=0x01      //SPI内存页面1
}BME680MemeryPageType;

/* 定义电源模式设定枚举类型 */
typedef enum BME680PowerMode {
  BME680_POWER_SLEEP_MODE=0x00,         //睡眠模式
  BME680_POWER_FORCED_MODE=0x01,        //强制模式
  BME680_POWER_NORMAL_MODE=0x03         //正常模式
}BME680PowerModeType;

/* 定义3线SPI新数据中断使能枚举类型 */
typedef enum BME680SPI3wInt {
  BME680_SPI3W_INT_DISABLE=0x00,        //不启用3线SPI新数据中断
  BME680_SPI3W_INT_ENABLE=0x40          //启用3线SPI新数据中断
}BME680SPI3wIntType;

/* 定义气体加热器停止枚举类型 */
typedef enum BME680HeaterOff {
  BME680_HEATER_DISABLE=0x00,   //不使能加热器OFF
  BME680_HEATER_ENABLE=0x04     //使能加热器OFF
}BME680HeaterOffType;

/* 定义气体传感器启动枚举类型 */
typedef enum BME680GasRun {
  BME680_GAS_RUN_DISABLE=0x00,  //不启用气体运行
  BME680_GAS_RUN_ENABLE=0x10    //启用气体运行
}BME680GasRunType;

/* 定义BMP280通讯接口枚举 */
typedef enum BME680Port {
  BME680_I2C,
  BME680_SPI
}BME680PortType;

/* 定义修正参数操作对象类型 */
typedef struct BME680CalibParam {
  uint16_t dig_t1;      //温度修正系数1
  int16_t dig_t2;       //温度修正系数2
  int8_t dig_t3;        //温度修正系数3
  uint16_t dig_p1;      //压力修正系数1
  int16_t dig_p2;       //压力修正系数2
  int8_t dig_p3;        //压力修正系数3
  int16_t dig_p4;       //压力修正系数4
  int16_t dig_p5;       //压力修正系数5
  int8_t dig_p6;        //压力修正系数6
  int8_t dig_p7;        //压力修正系数7
  int16_t dig_p8;       //压力修正系数8
  int16_t dig_p9;       //压力修正系数9
  uint8_t dig_p10;      //压力修正系数10
  uint16_t dig_h1;      //湿度修正系数1	
  uint16_t dig_h2;      //湿度修正系数2
  int8_t dig_h3;        //湿度修正系数3	
  int8_t dig_h4;        //湿度修正系数4
  int8_t dig_h5;        //湿度修正系数5
  uint8_t dig_h6;       //湿度修正系数6
  int8_t dig_h7;        //湿度修正系数7
  int8_t dig_g1;        //气体质量修正系数1
  int16_t dig_g2;       //气体质量修正系数2
  int8_t dig_g3;        //气体质量修正系数3
  
#if BME680_COMPENSATION_SELECTED > (0)
  int32_t  t_fine;      //用于压力修正的温度数据,整形补偿计算
#else
  float t_fine;         //用于压力修正的温度数据，浮点补偿计算
#endif

  uint8_t res_heat_range;	//加热器电阻范围
  int8_t res_heat_val;		//加热器电阻值
  int8_t range_sw_err;		//转换错误范围
	
}BME680CalibParamType;

/* 定义片选信号枚举 */
typedef enum BME680CS {
  BME680CS_Enable,
  BME680CS_Disable
}BME680CSType;

/*定义BME680操作对象*/
typedef struct BME680Object{
  uint8_t chipID;       //芯片ID
  uint8_t bmeAddress;   //I2C通讯时的设备地址
  uint8_t memeryPage;   //用于在SPI接口时记录当前所处的内存页
  uint8_t config;       //配置寄存器
  uint8_t ctrlMeas;     //测量控制寄存器
  uint8_t ctrlHumi;     //湿度测量控制寄存器
  uint8_t ctrlGas0;     //气体控制寄存器0
  uint8_t ctrlGas1;     //气体控制寄存器1
  uint8_t resHeat;
  uint8_t gasWait;
  
  BME680PortType port;  //接口选择
  BME680CalibParamType caliPara;        //校准参数

#if BME680_COMPENSATION_SELECTED > (0)
  int32_t temperature;          //温度值
  int32_t pressure;             //压力值
  int32_t humidity;             //湿度值
  int32_t gasResistence;        //大气质量电阻值
  int32_t iaq;                  //空气质量水平
#else
  float temperature;            //温度值
  float pressure;               //压力值
  float humidity;               //湿度值
  float gasResistence;          //大气质量电阻值
  float iaq;                    //空气质量水平
#endif

  void (*Read)(struct BME680Object *bme,uint8_t regAddress,uint8_t *rData,uint16_t rSize);       //读数据操作指针
  void (*Write)(struct BME680Object *bme,uint8_t regAddress,uint8_t command);    //谢数据操作指针
  void (*Delayms)(volatile uint32_t nTime);       //延时操作指针
  void (*ChipSelect)(BME680CSType cs);	//使用SPI接口时，片选操作
}BME680ObjectType;

/* 定义读数据操作函数指针类型 */
typedef void (*BME680Read)(struct BME680Object *bme,uint8_t regAddress,uint8_t *rData,uint16_t rSize);
/* 定义写数据操作函数指针类型 */
typedef void (*BME680Write)(struct BME680Object *bme,uint8_t regAddress,uint8_t command);
/* 定义延时操作函数指针类型 */
typedef void (*BME680Delayms)(volatile uint32_t nTime);
/* 定义使用SPI接口时，片选操作函数指针类型 */
typedef void (*BME680ChipSelect)(BME680CSType cs);

/*读取压力和温度数据*/
void GetBME680Measure(BME680ObjectType *bme);

/*实现BME680初始化配置*/
void BME680Initialization(BME680ObjectType *bme,                //BMP680对象
                          uint8_t bmeAddress,                   //I2C接口是设备地址
                          BME680PortType port,                  //接口选择
                          BME680IIRFilterType filter,           //过滤器
                          BME680SPI3wUseType spi3W_en,          //3线SPI控制
                          BME680TempSampleType osrs_t,          //温度精度
                          BME680PresSampleType osrs_p,          //压力精度
                          BME680SPI3wIntType spi3wint_en,       //3线SPI中断控制
                          BME680HumiSampleType osrs_h,          //湿度精度
                          BME680GasRunType run_gas,             //气体运行设置
                          BME680HeaterSPType nb_conv,           //加热器设定点选择
                          BME680HeaterOffType heat_off,         //加热器关闭
                          uint16_t duration,                    //TPHG测量循环周期，ms单位
                          uint8_t tempTarget,                   //加热器的目标温度
                          BME680Read Read,                      //读数据操作指针
                          BME680Write Write,                    //写数据操作指针
                          BME680Delayms Delayms,                //延时操作指针
                          BME680ChipSelect ChipSelect           //片选操作指针
                         );

#endif
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
