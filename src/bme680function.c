/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：bme680function.c                                               **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现BME280压力、温度及湿度传感器的操作                     **/
/**           BME280有55个可操作寄存器（包括26个校准数据）地址如下：         **/
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
/**    1、声明bme680ObjectType对象实体，并调用函数BME680Initialization初始化 **/
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

#include "stdio.h"
#include "bme680function.h"

/*定义控制寄存器地址*/
#define REG_RESET       0xE0	//复位寄存器
#define REG_BME680_ID   0xD0	//设备ID寄存器
#define REG_CONFIG      0x75	//配置寄存器
#define REG_CTRL_MEAS   0x74	//测量控制寄存器
#define REG_STATUS      0x73	//内存页配置寄存器
#define REG_CTRL_HUM    0x72	//湿度测量控制寄存器
#define REG_CTRL_GAS1   0x71	//气体控制寄存器1
#define REG_CTRL_GAS0   0x70	//气体控制寄存器0
#define REG_GAS_WAIT0   0x64	//气体加热等待时间寄存器0
#define REG_GAS_WAIT1   0x65	//气体加热等待时间寄存器1
#define REG_GAS_WAIT2   0x66	//气体加热等待时间寄存器2
#define REG_GAS_WAIT3   0x67	//气体加热等待时间寄存器3
#define REG_GAS_WAIT4   0x68	//气体加热等待时间寄存器4
#define REG_GAS_WAIT5   0x69	//气体加热等待时间寄存器5
#define REG_GAS_WAIT6   0x6A	//气体加热等待时间寄存器6
#define REG_GAS_WAIT7   0x6B	//气体加热等待时间寄存器7
#define REG_GAS_WAIT8   0x6C	//气体加热等待时间寄存器8
#define REG_GAS_WAIT9   0x6D	//气体加热等待时间寄存器9
#define REG_RES_HEAT_0  0x5A	//气体加热器电阻寄存器0
#define REG_RES_HEAT_1  0x5B	//气体加热器电阻寄存器1
#define REG_RES_HEAT_2  0x5C	//气体加热器电阻寄存器2
#define REG_RES_HEAT_3  0x5D	//气体加热器电阻寄存器3
#define REG_RES_HEAT_4  0x5E	//气体加热器电阻寄存器4
#define REG_RES_HEAT_5  0x5F	//气体加热器电阻寄存器5
#define REG_RES_HEAT_6  0x60	//气体加热器电阻寄存器6
#define REG_RES_HEAT_7  0x61	//气体加热器电阻寄存器7
#define REG_RES_HEAT_8  0x62	//气体加热器电阻寄存器8
#define REG_RES_HEAT_9  0x63	//气体加热器电阻寄存器9
#define REG_IDAC_HEAT_0 0x50	//加热器DAC导入电流0寄存器
#define REG_IDAC_HEAT_1 0x51	//加热器DAC导入电流1寄存器
#define REG_IDAC_HEAT_2 0x52	//加热器DAC导入电流2寄存器
#define REG_IDAC_HEAT_3 0x53	//加热器DAC导入电流3寄存器
#define REG_IDAC_HEAT_4 0x54	//加热器DAC导入电流4寄存器
#define REG_IDAC_HEAT_5 0x55	//加热器DAC导入电流5寄存器
#define REG_IDAC_HEAT_6 0x56	//加热器DAC导入电流6寄存器
#define REG_IDAC_HEAT_7 0x57	//加热器DAC导入电流7寄存器
#define REG_IDAC_HEAT_8 0x58	//加热器DAC导入电流8寄存器
#define REG_IDAC_HEAT_9 0x59	//加热器DAC导入电流9寄存器

/*定义状态寄存器地址*/
#define REG_EAS_STATUS_0 0x1D	//状态寄存器地址

/*定义数据寄存器地址*/
#define REG_TEMP_MSB 	0x22	//温度值寄存器高字节
#define REG_TEMP_LSB 	0x23	//温度值寄存器低字节
#define REG_TEMP_XLSB   0x24	//温度值寄存器最低字节

#define REG_HUMI_LSB	0x26	//湿度值寄存器低字节
#define REG_HUMI_MSB	0x25	//湿度值寄存器高字节

#define REG_PRESS_XLSB	0x21	//压力值寄存器最低字节
#define REG_PRESS_LSB   0x20	//压力值寄存器低字节
#define REG_PRESS_MSB   0x1F	//压力值寄存器高字节

#define REG_GAS_MSB	0x2A				//气体电阻值寄存器高字节
#define REG_GAS_LSB	0x2B				//气体电阻值寄存器低字节
#define REG_GAS_RANGE	0x2B			//气体范围值寄存器高字节
#define REG_RANGE_SW_ERROR 0x04	//气体范围转换错误寄存器
#define REG_RES_HEAT_RANGE 0x02	//加热器电阻范围寄存器
#define REG_RES_HEAT_VAL 0x00		//加热器值寄存器

/*定义校准数据寄存器地址,calib00-calib25:0x88-0xA1共26个;calib26-calib41:0xE1-0xF0共16个*/
#define REG_PAR_T2_LSB	0x8A         //温度校准系数2，有符号16位整数
#define REG_PAR_T2_MSB	0x8B         //温度校准系数2，有符号16位整数
#define REG_PAR_T3      0x8C         //温度校准系数3，有符号16位整数
#define REG_PAR_P1_LSB	0x8E         //压力校准系数1，无符号16位整数
#define REG_PAR_P1_MSB	0x8F         //压力校准系数1，无符号16位整数
#define REG_PAR_P2_LSB	0x90         //压力校准系数2，有符号16位整数
#define REG_PAR_P2_MSB	0x91         //压力校准系数2，有符号16位整数
#define REG_PAR_P3      0x92         //压力校准系数3，有符号16位整数
#define REG_PAR_P4_LSB	0x94         //压力校准系数4，有符号16位整数
#define REG_PAR_P4_MSB	0x95         //压力校准系数4，有符号16位整数
#define REG_PAR_P5_LSB	0x96         //压力校准系数5，有符号16位整数
#define REG_PAR_P5_MSB	0x97         //压力校准系数5，有符号16位整数
#define REG_PAR_P7      0x98         //压力校准系数7，有符号16位整数
#define REG_PAR_P6      0x99         //压力校准系数6，有符号16位整数
#define REG_PAR_P8_LSB	0x9C         //压力校准系数8，有符号16位整数
#define REG_PAR_P8_MSB	0x9D         //压力校准系数8，有符号16位整数
#define REG_PAR_P9_LSB	0x9E         //压力校准系数9，有符号16位整数
#define REG_PAR_P9_MSB	0x9F         //压力校准系数9，有符号16位整数
#define REG_PAR_P10     0xA0         //压力校准系数10，有符号16位整数

#define REG_PAR_H2_MSB          0xE1         //湿度校准系数2，有符号16位整数
#define REG_PAR_H1H2_LSB        0xE2         //湿度校准系数2，有符号16位整数
#define REG_PAR_H1_MSB		0xE3         //湿度校准系数1，无符号8位整数
#define REG_PAR_H3              0xE4         //湿度校准系数3，无符号8位整数
#define REG_PAR_H4              0xE5         //湿度校准系数4，有符号16位整数
#define REG_PAR_H5              0xE6         //湿度校准系数5，有符号16位整数
#define REG_PAR_H6              0xE7         //湿度校准系数6，有符号8位整数
#define REG_PAR_H7              0xE8         //湿度校准系数7，有符号8位整数
#define REG_PAR_T1_LSB		0xE9         //温度校准系数1，无符号16位整数
#define REG_PAR_T1_MSB		0xEA         //温度校准系数1，无符号16位整数
#define REG_PAR_G2_LSB		0xEB         //气体校准系数1，无符号16位整数
#define REG_PAR_G2_MSB		0xEC         //气体校准系数1，无符号16位整数
#define REG_PAR_G1              0xED         //气体校准系数1，无符号16位整数
#define REG_PAR_G3              0xEE         //气体校准系数1，无符号16位整数

#define REG_CALIB_START_ADDR1	0x8A            //第一段校准数据的起始地址
#define REG_CALIB_START_ADDR2	0xE1		//第二段校准数据的起始地址

/*定义校准数据寄存器枚举*/
enum calib1 {
  PAR_T2_LSB,           //温度校准系数2，有符号16位整数
  PAR_T2_MSB,           //温度校准系数2，有符号16位整数
  PAR_T3,               //温度校准系数3，有符号16位整数
  PAR_SPARE_1,          //系统保留
  PAR_P1_LSB,           //压力校准系数1，无符号16位整数
  PAR_P1_MSB,           //压力校准系数1，无符号16位整数
  PAR_P2_LSB,           //压力校准系数2，有符号16位整数
  PAR_P2_MSB,           //压力校准系数2，有符号16位整数
  PAR_P3,               //压力校准系数3，有符号16位整数
  PAR_SPARE_2,          //系统保留
  PAR_P4_LSB,           //压力校准系数4，有符号16位整数
  PAR_P4_MSB,           //压力校准系数4，有符号16位整数
  PAR_P5_LSB,           //压力校准系数5，有符号16位整数
  PAR_P5_MSB,           //压力校准系数5，有符号16位整数
  PAR_P7,               //压力校准系数7，有符号16位整数
  PAR_P6,               //压力校准系数6，有符号16位整数
  PAR_SPARE_3,          //系统保留
  PAR_SPARE_4,          //系统保留
  PAR_P8_LSB,           //压力校准系数8，有符号16位整数
  PAR_P8_MSB,           //压力校准系数8，有符号16位整数
  PAR_P9_LSB,           //压力校准系数9，有符号16位整数
  PAR_P9_MSB,           //压力校准系数9，有符号16位整数
  PAR_P10,              //压力校准系数10，有符号16位整数
  REG_CALIB_LENGTH1     //第一段校准数据的长度
};

/*定义校准数据寄存器枚举*/
enum calib2 {
  PAR_H2_MSB,             //湿度校准系数2，有符号16位整数
  PAR_H1H2_LSB,           //湿度校准系数2，有符号16位整数
  PAR_H1_MSB,             //湿度校准系数1，无符号8位整数
  PAR_H3,                 //湿度校准系数3，无符号8位整数
  PAR_H4,                 //湿度校准系数4，有符号16位整数
  PAR_H5,                 //湿度校准系数5，有符号16位整数
  PAR_H6,                 //湿度校准系数6，有符号8位整数
  PAR_H7,                 //湿度校准系数7，有符号8位整数
  PAR_T1_LSB,             //温度校准系数1，无符号16位整数
  PAR_T1_MSB,             //温度校准系数1，无符号16位整数
  PAR_G2_LSB,             //气体校准系数1，无符号16位整数
  PAR_G2_MSB,             //气体校准系数1，无符号16位整数
  PAR_G1,                 //气体校准系数1，无符号16位整数
  PAR_G3,                 //气体校准系数1，无符号16位整数
  REG_CALIB_LENGTH2       //第二段校准数据的长度
};

/*定义测量数据寄存器枚举*/
enum tphMeas {
  PRESS_MSB,      //压力值寄存器高字节
  PRESS_LSB,      //压力值寄存器低字节
  PRESS_XLSB,     //压力值寄存器最低字节
  TEMP_MSB,       //温度值寄存器高字节
  TEMP_LSB,       //温度值寄存器低字节
  TEMP_XLSB,      //温度值寄存器最低字节
  HUMI_MSB,       //湿度值寄存器高字节
  HUMI_LSB,       //湿度值寄存器低字节
  REG_TPH_LENGTH  //TPH数据寄存器字节长度
};

/*定义测量数据寄存器枚举*/
enum gasMeas {
  GAS_MSB,        //气体电阻值高字节寄存器
  GAS_LSB,        //气体电阻值低字节寄存器
  REG_GAS_LENGTH  //气体寄存器字节长度
};

/*判断对象是否有效*/
static bool ObjectIsValid(BME680ObjectType *bme);
/*向BME680寄存器写一个字节*/
static void WriteBME680Register(BME680ObjectType *bme,uint8_t regAddress,uint8_t command);
/*从BME680寄存器读取数据*/
static uint8_t ReadBME680Register(BME680ObjectType *bme,uint8_t regAddress,uint8_t *rDatas,uint16_t rSize);
/*BME680软件复位*/
static void BME680SoftReset(BME680ObjectType *bme);
/*读取校准数据*/
static void GetBME680CalibrationData(BME680ObjectType *bme);
/*缺省片选处理函数*/
static void BME680ChipSelectDefault(BME680CSType cs);
/*在SPI模式下，获取当前内存页*/
static void GetMemeryPageNumber(BME680ObjectType *bme);
/*在SPI模式下，设置当前内存页*/
static void SetMemeryPageNumber(BME680ObjectType *bme,uint8_t regAdd);
/*配置温度、压力、湿度控制寄存器*/
static void ConfigControlRegister(BME680ObjectType *bme,                //BMP280对象
                                  BME680IIRFilterType filter,           //过滤器
                                  BME680SPI3wUseType spi3W_en,          //3线SPI控制
                                  BME680TempSampleType osrs_t,          //温度精度
                                  BME680PresSampleType osrs_p,          //压力精度
                                  BME680SPI3wIntType spi3wint_en,       //3线SPI中断控制
                                  BME680HumiSampleType osrs_h,          //湿度精度
                                  BME680GasRunType run_gas,             //气体运行设置
                                  BME680HeaterSPType nb_conv,           //加热器设定点选择
                                  BME680HeaterOffType heat_off,         //加热器关闭
                                  uint8_t waitTime,                     //等待时间，取值0到63，factor为其倍数
                                  uint8_t tempTarget                    //加热器的目标温度
                                 );

#if BME680_COMPENSATION_SELECTED > (0)
/*计算实际温度值,精度0.01GegC*/
static int32_t CompensateTemperature(BME680ObjectType *bme,uint32_t adcCode);
/*计算实际的压力值*/
static int32_t CompensatePressure(BME680ObjectType *bme,uint32_t adcCode);
/*计算实际的湿度值*/
static int32_t CompensateHumidity(BME680ObjectType *bme,uint32_t adcCode);
/*计算实际的气体电阻值*/
static int32_t CompensateGasResistance(BME680ObjectType *bme,uint32_t adcCode,uint16_t gasRange);
#else
/*计算实际温度值,精度0.01GegC*/
static float CompensateTemperature(BME680ObjectType *bme,uint32_t adcCode);
/*计算实际的压力值，修正后为Q24.8格式*/
static float CompensatePressure(BME680ObjectType *bme,uint32_t adcCode);
/*计算实际的湿度值，修正后为Q22.10格式*/
static float CompensateHumidity(BME680ObjectType *bme,uint32_t adcCode);
/*计算实际的气体电阻值*/
static float CompensateGasResistance(BME680ObjectType *bme,uint32_t adcCode,uint16_t gasRange);
#endif
/*计算加热器电阻值,设置到res_wait_x寄存器的值*/
static uint8_t CompensateHeaterResistance(BME680ObjectType *bme,uint16_t tempTarget);
/*计算加热器等待时间，设到gas_wait_x寄存器的值*/
static uint8_t CalcHeaterWaitTime(uint16_t waitTime);
/*配置气体加热器寄存器*/
static void ConfigGasHeater(BME680ObjectType *bme,BME680HeaterSPType nbconv,uint16_t tempTarget,uint16_t waitTime);
/*计算加热等待时间*/
static uint16_t CalcProfileDuration(BME680ObjectType *bme,
                                    uint16_t duration,                  //TPHG测量循环周期
                                    BME680TempSampleType osrs_t,        //温度精度
                                    BME680PresSampleType osrs_p,        //压力精度
                                    BME680HumiSampleType osrs_h         //湿度精度
                                   );

/*设置传感器电源模式*/
static uint8_t SetSensorPowerMode(BME680ObjectType *bme,BME680PowerModeType powerMode);
/*读取传感器电源模式*/
static uint8_t GetSensorPowerMode(BME680ObjectType *bme);


/*读取压力和温度数据*/
void GetBME680Measure(BME680ObjectType *bme)
{
  uint8_t regAddress;
  uint8_t tphDatas[8];
  uint8_t gasDatas[8];
  
  uint32_t adcTemp=0;
  uint32_t adcPres=0;
  uint32_t adcHumi=0;
  uint32_t adcGas=0;
  uint16_t gasRange;		//气体测量范围
	
  if(!ObjectIsValid(bme))
  {
    return;
  }
  
  /*读取状态检测AD转换是否完成*/
  uint8_t status=0xFF;
  ReadBME680Register(bme,REG_EAS_STATUS_0,&status,1);
	
  if(status&0x80)
  {
    regAddress=REG_PRESS_MSB;
    ReadBME680Register(bme,regAddress,tphDatas,REG_TPH_LENGTH);
    
    regAddress=REG_GAS_MSB;
    ReadBME680Register(bme,regAddress,gasDatas,REG_GAS_LENGTH);

    /*计算温度、压力和湿度的ADC值*/
    adcTemp=((uint32_t)tphDatas[TEMP_MSB]<<16)+((uint32_t)tphDatas[TEMP_LSB]<<8)+(uint32_t)tphDatas[TEMP_XLSB];
    adcPres=((uint32_t)tphDatas[PRESS_MSB]<<16)+((uint32_t)tphDatas[PRESS_LSB]<<8)+(uint32_t)tphDatas[PRESS_XLSB];
    adcHumi=((uint32_t)tphDatas[HUMI_MSB]<<8)+(uint32_t)tphDatas[HUMI_LSB];
    
    adcGas=((uint32_t)gasDatas[GAS_MSB]<<8)+(uint32_t)gasDatas[GAS_LSB];
  
    gasRange=(uint16_t)(gasDatas[GAS_LSB]&0x0F);
	
    adcTemp=(adcTemp>>4);
    adcPres=(adcPres>>4);
    adcGas=(adcGas>>6);
  
    /*计算压力和温度的物理量值*/
    bme->temperature=CompensateTemperature(bme,adcTemp);
  
    bme->pressure=CompensatePressure(bme,adcPres);
  
    bme->humidity=CompensateHumidity(bme,adcHumi);
	
    bme->gasResistence=CompensateGasResistance(bme,adcGas,gasRange);
  }
}

/*实现BME680初始化配置*/
void BME680Initialization(BME680ObjectType *bme,                //BMP280对象
                          uint8_t bmeAddress,                   //I2C接口是设备地址
                          BME680PortType port,                  //接口选择
                          BME680IIRFilterType filter,           //过滤器
                          BME680SPI3wUseType spi3W_en,          //3线SPI控制
                          BME680TempSampleType osrs_t,          //温度精度
                          BME680PresSampleType osrs_p,          //压力精度
                          BME680SPI3wIntType spi3wint_en,       //3线SPI中断控制
                          BME680HumiSampleType osrs_h,		//湿度精度
                          BME680GasRunType run_gas,             //气体运行设置
                          BME680HeaterSPType nb_conv,           //加热器设定点选择
                          BME680HeaterOffType heat_off,         //加热器关闭
                          uint16_t duration,                    //TPHG测量循环周期，ms单位
                          uint8_t tempTarget,                   //加热器的目标温度
                          BME680Read Read,                      //读数据操作指针
                          BME680Write Write,                    //写数据操作指针
                          BME680Delayms Delayms,                //延时操作指针
                          BME680ChipSelect ChipSelect           //片选操作指针
                         )
{
  uint8_t try_count = 5;
  uint8_t regValue=0;
	
  if((bme==NULL)||(Read==NULL)||(Write==NULL)||(Delayms==NULL))
  {
    return;
  }
  bme->Read=Read;
  bme->Write=Write;
  bme->Delayms=Delayms;
	
  bme->port=port;
  if(bme->port==BME680_I2C)
  {
    if((bmeAddress==0xEC)||(bmeAddress==0xEE))
    {
      bme->bmeAddress=bmeAddress;
    }
    else if((bmeAddress==0x76)||(bmeAddress==0x77))
    {
      bme->bmeAddress=(bmeAddress<<1);
    }
    else
    {
      return;
    }
    bme->ChipSelect=NULL;
  }
  else
  {
    if(ChipSelect!=NULL)
    {
      bme->ChipSelect=ChipSelect;
    }
    else
    {
      bme->ChipSelect=BME680ChipSelectDefault;
    }
  }
	
  bme->chipID=0x00;
  bme->pressure=0.0;
  bme->temperature=25.0;
  bme->humidity=0.0;
  bme->bmeAddress=0x00;
  bme->caliPara.t_fine=0;
		
  if(!ObjectIsValid(bme))
  {
    return;
  }
  	
  while(try_count--)
  {
		ReadBME680Register(bme,REG_BME680_ID,&regValue,1);
    bme->chipID=regValue;
    if(0x61==bme->chipID)
    {
      BME680SoftReset(bme);
      
      break;
    }
  }
  
  if(try_count)
  {
    uint8_t waitTime;
    waitTime=CalcProfileDuration(bme,duration,osrs_t,osrs_p,osrs_h);
		
    //控制寄存器配置
    ConfigControlRegister(bme,filter,spi3W_en,osrs_t,osrs_p,spi3wint_en,osrs_h,run_gas,nb_conv,heat_off,waitTime,tempTarget);
		
    //读取校准值
    GetBME680CalibrationData(bme);
  }
}

/*读取校准数据*/
static void GetBME680CalibrationData(BME680ObjectType *bme)
{
  uint8_t calib1Data[REG_CALIB_LENGTH1];
  uint8_t calib2Data[REG_CALIB_LENGTH2];
  
  if(!ObjectIsValid(bme))
  {
    return;
  }
	
  ReadBME680Register(bme,REG_CALIB_START_ADDR1,calib1Data,REG_CALIB_LENGTH1);
  ReadBME680Register(bme,REG_CALIB_START_ADDR2,calib2Data,REG_CALIB_LENGTH2);
	

  bme->caliPara.dig_t1=(uint16_t)(((uint16_t)calib2Data[PAR_T1_MSB]<<8)+((uint16_t)calib2Data[PAR_T1_LSB]));
  bme->caliPara.dig_t2=(int16_t)(((int16_t)calib1Data[PAR_T2_MSB]<<8)+((int16_t)calib1Data[PAR_T2_LSB]));
  bme->caliPara.dig_t3=(int16_t)calib1Data[PAR_T3];
  
  bme->caliPara.dig_p1=(uint16_t)(((uint16_t)calib1Data[PAR_P1_MSB]<<8)+((uint16_t)calib1Data[PAR_P1_LSB]));
  bme->caliPara.dig_p2=(int16_t)(((int16_t)calib1Data[PAR_P2_MSB]<<8)+((int16_t)calib1Data[PAR_P2_LSB]));
  bme->caliPara.dig_p3=(int16_t)calib1Data[PAR_P3];
  bme->caliPara.dig_p4=(int16_t)(((int16_t)calib1Data[PAR_P4_MSB]<<8)+((int16_t)calib1Data[PAR_P4_LSB]));
  bme->caliPara.dig_p5=(int16_t)(((int16_t)calib1Data[PAR_P5_MSB]<<8)+((int16_t)calib1Data[PAR_P5_LSB]));
  bme->caliPara.dig_p6=(int16_t)calib1Data[PAR_P6];
  bme->caliPara.dig_p7=(int16_t)calib1Data[PAR_P7];
  bme->caliPara.dig_p8=(int16_t)(((int16_t)calib1Data[PAR_P8_MSB]<<8)+((int16_t)calib1Data[PAR_P8_LSB]));
  bme->caliPara.dig_p9=(int16_t)(((int16_t)calib1Data[PAR_P9_MSB]<<8)+((int16_t)calib1Data[PAR_P9_LSB]));
  bme->caliPara.dig_p10=(int16_t)calib1Data[PAR_P10];

  bme->caliPara.dig_h1=(int16_t)(((int16_t)calib2Data[PAR_H1_MSB]<<4)+(((int16_t)calib2Data[PAR_H1H2_LSB])&0x0F));
  bme->caliPara.dig_h2=(int16_t)(((int16_t)calib2Data[PAR_H2_MSB]<<4)+((int16_t)calib2Data[PAR_H1H2_LSB]>>4));
  bme->caliPara.dig_h3=calib2Data[PAR_H3];
  bme->caliPara.dig_h4=calib2Data[PAR_H4];
  bme->caliPara.dig_h5=calib2Data[PAR_H5];
  bme->caliPara.dig_h6=calib2Data[PAR_H6];
  bme->caliPara.dig_h7=calib2Data[PAR_H7];
  
  bme->caliPara.dig_g1=calib2Data[PAR_G1];
  bme->caliPara.dig_g2=(int16_t)(((int16_t)calib2Data[PAR_G2_MSB]<<8)+((int16_t)calib2Data[PAR_G2_LSB]));
  bme->caliPara.dig_g3=calib2Data[PAR_G3];
	
  uint8_t regValue;
	
  ReadBME680Register(bme,REG_RANGE_SW_ERROR,&regValue,1);
  bme->caliPara.range_sw_err=regValue;
  
  ReadBME680Register(bme,REG_RES_HEAT_VAL,&regValue,1);
  bme->caliPara.res_heat_val=regValue;
  
  ReadBME680Register(bme,REG_RES_HEAT_RANGE,&regValue,1);
  bme->caliPara.res_heat_range=regValue&0x30;
}

/*配置温度、压力、湿度控制寄存器*/
static void ConfigControlRegister(BME680ObjectType *bme,                //BMP280对象
                                  BME680IIRFilterType filter,           //过滤器
                                  BME680SPI3wUseType spi3W_en,          //3线SPI控制
                                  BME680TempSampleType osrs_t,          //温度精度
                                  BME680PresSampleType osrs_p,          //压力精度
                                  BME680SPI3wIntType spi3wint_en,       //3线SPI中断控制
                                  BME680HumiSampleType osrs_h,          //湿度精度
                                  BME680GasRunType run_gas,             //气体运行设置
                                  BME680HeaterSPType nb_conv,           //加热器设定点选择
                                  BME680HeaterOffType heat_off,         //加热器关闭
                                  uint8_t waitTime,                     //等待时间，取值0到63，factor为其倍数
                                  uint8_t tempTarget                    //加热器的目标温度
                                 )
{
  uint8_t command=0; 
  uint8_t regValue[6];
  BME680PowerModeType mode;
	
  if(!ObjectIsValid(bme))
  {
    return;
  }
	
  //步骤1：设置操作模式为SLEEP MODE
  do
  {
  mode=(BME680PowerModeType)SetSensorPowerMode(bme,BME680_POWER_SLEEP_MODE);
  }while(mode!=BME680_POWER_SLEEP_MODE);
	
  //步骤2：配置测量控制寄存器:温度20位，压力20位，电源正常模式
  command=(uint8_t)osrs_t|(uint8_t)osrs_p|(uint8_t)mode;
  WriteBME680Register(bme,REG_CTRL_MEAS,command);
  
  //步骤2：配置湿度测量控制寄存器：3线SPI中断不启用，湿度精度16位
  command=(uint8_t)spi3wint_en|(uint8_t)osrs_h;
  WriteBME680Register(bme,REG_CTRL_HUM,command);
	
  //步骤3：配置配置寄存器:IIR滤波系数16、不使用SPI3线通讯
  command=(uint8_t)filter|(uint8_t)spi3W_en;
  WriteBME680Register(bme,REG_CONFIG,command);
	
  //步骤4：配置气体寄存器0：加热器是否启用
  command=heat_off;
  WriteBME680Register(bme,REG_CTRL_GAS0,command);
	
  //步骤5：配置气体控制寄存器1：气体运行和设定点
  command=(uint8_t)run_gas|(uint8_t)nb_conv;
  WriteBME680Register(bme,REG_CTRL_GAS1,command);
	
  //步骤6：配置气体加热器寄存器
  ConfigGasHeater(bme,nb_conv,tempTarget,waitTime);
	
  bme->Delayms(10);
	
  //步骤7：设置操作模式为FORCED MODE
  do
  {
    mode=(BME680PowerModeType)SetSensorPowerMode(bme,BME680_POWER_FORCED_MODE);
  }while(mode!=BME680_POWER_FORCED_MODE);
	
  bme->Delayms(10);
	
  //同时读取气体控制1（0x70）、气体控制1（0x71）、湿度控制（0x72）、状态（0x73）、测量控制（0x74）、配置（0x75）寄存器值
  ReadBME680Register(bme,REG_CTRL_GAS0,regValue,0);
  bme->ctrlGas0=regValue[0];
  bme->ctrlGas1=regValue[1];
  bme->ctrlHumi=regValue[2];
  bme->memeryPage=regValue[3];
  bme->ctrlMeas=regValue[4];
  bme->config=regValue[5];	
}

/*BME680软件复位*/
static void BME680SoftReset(BME680ObjectType *bme)
{
  uint8_t command=0xB6;
  
  if(ObjectIsValid(bme))
  {
    WriteBME680Register(bme,REG_RESET,command);
  }
	
  GetMemeryPageNumber(bme);
}

/*在SPI模式下，获取当前内存页*/
static void GetMemeryPageNumber(BME680ObjectType *bme)
{
  uint8_t status=0xEF;
  if(bme->port==BME680_SPI)
  {
    ReadBME680Register(bme,REG_STATUS,&status,1);
    status=status&0x10;
  }
	
  bme->memeryPage=status;
}

/*在SPI模式下，设置当前内存页*/
static void SetMemeryPageNumber(BME680ObjectType *bme,uint8_t regAdd)
{
  uint8_t status=0xFF;
  uint8_t setPage=0xFF;
  
  if(regAdd>0x7F)
  {
    setPage=BME680_MEMERY_PAGE0;
  }
  else
  {
    setPage=BME680_MEMERY_PAGE1;
  }
  
  GetMemeryPageNumber(bme);
  
  if(setPage!=bme->memeryPage)
  {
    if(bme->port==BME680_SPI)
    {
      ReadBME680Register(bme,REG_STATUS,&status,1);
      status=status&0xEF;
    }
		
    setPage=status|(setPage&0x10);
		
    WriteBME680Register(bme,REG_STATUS,setPage);
  }
  
  GetMemeryPageNumber(bme);
}

/*从BME680寄存器读取数据*/
static uint8_t ReadBME680Register(BME680ObjectType *bme,uint8_t regAddress,uint8_t *rDatas,uint16_t rSize)
{
  uint8_t bmeError=0xFF;
  
  if(ObjectIsValid(bme))
  {
    if(bme->port==BME680_SPI)
    {
      bme->ChipSelect(BME680CS_Enable);
      bme->Delayms(1);
      SetMemeryPageNumber(bme,regAddress);
      regAddress |= 0x80;
      bme->Delayms(1);
      bme->Read(bme,regAddress,rDatas,rSize);
      bme->Delayms(1);
      bme->ChipSelect(BME680CS_Disable);
    }
    else
    {
      bme->Read(bme,regAddress,rDatas,rSize);
    }
  
    bmeError=0x00;
  }

  return bmeError;
}

/* 向BME680寄存器写一个字节 */
static void WriteBME680Register(BME680ObjectType *bme,uint8_t regAddress,uint8_t command)
{
  if(ObjectIsValid(bme))
  {
    if(bme->port==BME680_SPI)
    {
      bme->ChipSelect(BME680CS_Enable);
      bme->Delayms(1);
      SetMemeryPageNumber(bme,regAddress);
      regAddress&=0x7F;
      bme->Delayms(1);
      bme->Write(bme,regAddress,command);
      bme->Delayms(1);
      bme->ChipSelect(BME680CS_Disable);
    }
    else
    {
      bme->Write(bme,regAddress,command);
    }
  }
}

/*计算加热等待时间*/
static uint16_t CalcProfileDuration(BME680ObjectType *bme,
                                    uint16_t duration,  //TPHG测量循环周期
                                    BME680TempSampleType osrs_t,        //温度精度
                                    BME680PresSampleType osrs_p,        //压力精度
                                    BME680HumiSampleType osrs_h //湿度精度
                                   )
{
  uint32_t tphDur;
  uint32_t measCycles;
  uint8_t osrsMeasCycles[6] = {0, 1, 2, 4, 8, 16};
  uint16_t heatDur;

  measCycles=osrsMeasCycles[osrs_t>>5];
  measCycles+=osrsMeasCycles[osrs_p>>2];
  measCycles+=osrsMeasCycles[osrs_h];

  /* TPH测量周期 */
  tphDur = measCycles * 1963;
  tphDur += 477 * 4;	//TPH测量
  tphDur += 477 * 5;	//气体测量
  tphDur += 500;			//四舍五入
  tphDur /= 1000; 		//转为毫秒

  tphDur += 1;				//1ms唤醒时间

  heatDur=duration-(uint16_t)tphDur;	//剩余未加热时间
	
  return heatDur;
}

/*配置气体加热器寄存器*/
static void ConfigGasHeater(BME680ObjectType *bme,BME680HeaterSPType nbconv,uint16_t tempTarget,uint16_t waitTime)
{
  uint8_t regAdd; 
  uint8_t regValue;
	
  uint8_t gasWaitAdd[10]={REG_GAS_WAIT0,REG_GAS_WAIT1,
                          REG_GAS_WAIT2,REG_GAS_WAIT3,
                          REG_GAS_WAIT4,REG_GAS_WAIT5,
                          REG_GAS_WAIT6,REG_GAS_WAIT7,
                          REG_GAS_WAIT8,REG_GAS_WAIT9};
  uint8_t resHeatAdd[10]={REG_RES_HEAT_0,REG_RES_HEAT_1,
                          REG_RES_HEAT_2,REG_RES_HEAT_3,
                          REG_RES_HEAT_4,REG_RES_HEAT_5,
                          REG_RES_HEAT_6,REG_RES_HEAT_7,
                          REG_RES_HEAT_8,REG_RES_HEAT_9};
  
  regAdd=resHeatAdd[nbconv];
  regValue=CompensateHeaterResistance(bme,tempTarget);
  WriteBME680Register(bme,regAdd,regValue);
  
  regAdd=gasWaitAdd[nbconv];
  regValue=CalcHeaterWaitTime(waitTime);
  WriteBME680Register(bme,regAdd,regValue);
  
  ReadBME680Register(bme,resHeatAdd[nbconv],&regValue,1);
  bme->resHeat=regValue;
  
  ReadBME680Register(bme,gasWaitAdd[nbconv],&regValue,1);
  bme->gasWait=regValue;
}

/*设置传感器电源模式*/
static uint8_t SetSensorPowerMode(BME680ObjectType *bme,BME680PowerModeType powerMode)
{
  uint8_t currentMode;
  uint8_t regValue;
  uint8_t regAddr = REG_CTRL_MEAS;
  
  currentMode=GetSensorPowerMode(bme);
  
  if(powerMode!=currentMode)
  {
    regValue=(bme->ctrlMeas&0xFC)|powerMode;
    WriteBME680Register(bme,regAddr,regValue);
    bme->Delayms(10);
    currentMode=GetSensorPowerMode(bme);
  }

  return currentMode;
}

/*读取传感器电源模式*/
static uint8_t GetSensorPowerMode(BME680ObjectType *bme)
{
  uint8_t mode;
  uint8_t regValue;
  
  ReadBME680Register(bme,REG_CTRL_MEAS,&regValue,1);
  
  bme->ctrlMeas=regValue;
  mode=regValue&0x03;
  
  return mode;
}

/*计算加热器等待时间，设到gas_wait_x的值*/
static uint8_t CalcHeaterWaitTime(uint16_t waitTime)
{
  uint8_t factor=0;
  uint8_t waitValue;

  if (waitTime >= 0xFC0)
  {
    waitValue = 0xFF;
  }
  else
  {
    while (waitTime > 0x3F)
    {
      waitTime=waitTime/4;
      factor+=1;
    }
    waitValue = (uint8_t) (waitTime + (factor * 64));
  }

  return waitValue;
}

/*判断对象是否有效*/
static bool ObjectIsValid(BME680ObjectType *bme)
{
  bool valid=true;
  
  if((bme==NULL)||(bme->Delayms==NULL)||(bme->Read==NULL)||(bme->Write==NULL))
  {
    valid=false;
  }

  return valid;
}

/*缺省片选处理函数*/
static void BME680ChipSelectDefault(BME680CSType cs)
{
  //用于在SPI通讯时，片选信号硬件电路选中的情况
  return;
}

#if BME680_COMPENSATION_SELECTED > (0)
/*计算实际温度值,精度0.01GegC*/
static int32_t CompensateTemperature(BME680ObjectType *bme,uint32_t adcCode)
{
  int64_t var1;
  int64_t var2;
  int64_t var3;
  int32_t temperature;
  int32_t temperature_min = -40000;
  int32_t temperature_max = 85000;

  var1=((int32_t)adcCode>>3)-((int32_t)bme->caliPara.dig_t1<<1);
  var2=(var1*(int32_t)bme->caliPara.dig_t2)>>11;
  var3=((var1>>1)*(var1>>1))>>12;
  var3 = ((var3)*((int32_t)bme->caliPara.dig_t3<<4))>>14;
  bme->caliPara.t_fine = (int32_t) (var2 + var3);
  temperature = (int32_t) (((bme->caliPara.t_fine * 5) + 128) >> 8);
	
  if (temperature < temperature_min)
  {
    temperature = temperature_min;
  }
  else if (temperature > temperature_max)
  {
    temperature = temperature_max;
  }
	
  return temperature;
}

/*计算实际的压力值*/
static int32_t CompensatePressure(BME680ObjectType *bme,uint32_t adcCode)
{
  int32_t var1;
  int32_t var2;
  int32_t var3;
  int32_t pressure;
//  int32_t pressure_min = 30000.0;
//  int32_t pressure_max = 110000.0;

  var1=(((int32_t)bme->caliPara.t_fine)>>1)-64000;
  var2=((((var1>>2)*(var1>>2))>>11)*(int32_t)bme->caliPara.dig_p6)>>2;
  var2=var2+((var1*(int32_t)bme->caliPara.dig_p5)<<1);
  var2=(var2>>2)+((int32_t)bme->caliPara.dig_p4 <<16);
  var1=(((((var1>>2)*(var1>>2))>>13)*((int32_t)bme->caliPara.dig_p3<<5))>>3)+(((int32_t)bme->caliPara.dig_p2*var1)>>1);
  var1=var1>>18;
  var1 =((32768+var1)*(int32_t)bme->caliPara.dig_p1)>>15;
  pressure=1048576- adcCode;
  pressure=(int32_t)((pressure-(var2>>12))*((uint32_t)3125));
  
  if(pressure>=0x40000000)
  {
    pressure=((pressure/var1)<<1);
  }
  else
  {
    pressure=((pressure<<1)/var1);
  }
  
  var1=((int32_t)bme->caliPara.dig_p9*(int32_t)(((pressure>>3)*(pressure>>3))>>13))>>12;
  var2=((int32_t)(pressure>>2)*(int32_t)bme->caliPara.dig_p8)>>13;
  var3=((int32_t)(pressure>>8)*(int32_t)(pressure>>8)*(int32_t)(pressure>>8)*(int32_t)bme->caliPara.dig_p10)>>17;

  pressure=(int32_t)(pressure)+((var1+var2+var3+((int32_t)bme->caliPara.dig_p7<<7))>>4);

  return pressure;
}

/*计算实际的湿度值*/
static int32_t CompensateHumidity(BME680ObjectType *bme,uint32_t adcCode)
{
  int32_t humidity;
  int32_t humidity_min = 0;
  int32_t humidity_max = 100000;
  int32_t var1;
  int32_t var2;
  int32_t var3;
  int32_t var4;
  int32_t var5;
  int32_t var6;
  int32_t temp_scaled;

  temp_scaled=(((int32_t)bme->caliPara.t_fine*5)+128)>>8;
  var1=(int32_t)(adcCode-((int32_t)((int32_t)bme->caliPara.dig_h1*16)))-(((temp_scaled*(int32_t)bme->caliPara.dig_h3)/((int32_t)100))>>1);
  var2=((int32_t)bme->caliPara.dig_h2* (((temp_scaled * (int32_t) bme->caliPara.dig_h4)/((int32_t)100))
			+(((temp_scaled*((temp_scaled*(int32_t) bme->caliPara.dig_h5)/((int32_t)100)))>>6)/((int32_t)100))+(int32_t)(1<<14)))>>10;
  var3=var1*var2;
  var4=(int32_t)bme->caliPara.dig_h6<<7;
  var4=((var4)+((temp_scaled*(int32_t)bme->caliPara.dig_h7)/((int32_t)100)))>>4;
  var5=((var3>>14)*(var3>>14))>>10;
  var6=(var4*var5)>>1;
  humidity=(((var3+var6)>>10)*((int32_t)1000))>>12;

  if(humidity > humidity_max)
  {
    humidity = humidity_max;
  }
  else if (humidity < humidity_min)
  {
    humidity = humidity_min;
  }
  
  return humidity;
}

/*计算实际的气体电阻值*/
static int32_t CompensateGasResistance(BME680ObjectType *bme,uint32_t adcCode,uint16_t gasRange)
{
  int32_t gasRes;
  int64_t var1 =0;
  int64_t var2 =0;

  /* 气体检测常数数组 */
  uint32_t gasConstantInt[2][16]={{2147483647,2147483647,2147483647,2147483647,
                                   2147483647,2126008810,2147483647,2130303777,
                                   2147483647,2147483647,2143188679,2136746228,
                                   2147483647,2126008810,2147483647,2147483647},
                                   {4096000000,2048000000,1024000000,512000000,
                                   255744255,127110228,64000000,32258064,
                                   16016016,8000000,4000000,2000000,
                                   1000000,500000,250000,125000}};
	
  var1=(int64_t)(((1340+(5*(int64_t)bme->caliPara.range_sw_err))*((int64_t)gasConstantInt[0][gasRange]))>>16);
  var2=(int64_t)(adcCode<<15)-(int64_t)(1<<24)+var1;
  gasRes =(int32_t)((((int64_t)(gasConstantInt[1][gasRange]*var1)>>9)+(var2>>1))/var2);
	
  return gasRes;
}

/*计算加热器电阻值,设置到res_wait_x寄存器的值*/
static uint8_t CompensateHeaterResistance(BME680ObjectType *bme,uint16_t tempTarget)
{
  uint8_t heatRes;
  int32_t var1;
  int32_t var2;
  int32_t var3;
  int32_t var4;
  int32_t var5;
  int32_t heaterResx100;

  if (tempTarget > 400)
  {
    tempTarget = 400;
  }
	
  var1 = (((int32_t)bme->temperature*bme->caliPara.dig_g3) / 1000) * 256;
  var2 = (bme->caliPara.dig_g1 + 784) * (((((bme->caliPara.dig_g2 + 154009) * tempTarget * 5) / 100) + 3276800) / 10);
  var3 = var1 + (var2 / 2);
  var4 = (var3 / (bme->caliPara.res_heat_range + 4));
  var5 = (131 * bme->caliPara.res_heat_val) + 65536;
  heaterResx100 = (int32_t) (((var4 / var5) - 250) * 34);
  heatRes = (uint8_t) ((heaterResx100 + 50) / 100);

  return heatRes;
}
#else
/*计算实际温度值,精度0.01GegC*/
static float CompensateTemperature(BME680ObjectType *bme,uint32_t adcCode)
{
  double var1;
  double var2;
  double temperature;
  double temperature_min = -40;
  double temperature_max = 85;

  var1 = ((double)adcCode) / 16384.0 - ((double)bme->caliPara.dig_t1) / 1024.0;
  var1 = var1 * ((double)bme->caliPara.dig_t2);
  var2 = (((double)adcCode) / 131072.0 - ((double)bme->caliPara.dig_t1) / 8192.0);
  var2 = (var2 * var2) * ((double)bme->caliPara.dig_t3);
  bme->caliPara.t_fine = (int32_t)(var1 + var2);
  temperature = (var1 + var2) / 5120.0;

  if(temperature < temperature_min)
  {
    temperature = temperature_min;
  }
  else if (temperature > temperature_max)
  {
    temperature = temperature_max;
  }
  return (float)temperature;
}

/*计算实际的压力值，修正后为Q24.8格式*/
static float CompensatePressure(BME680ObjectType *bme,uint32_t adcCode)
{
  double var1;
  double var2;
  double var3;
  double pressure;
  double pressure_min = 30000.0;
  double pressure_max = 110000.0;

  var1 = ((double)bme->caliPara.t_fine / 2.0) - 64000.0;
  var2 = var1 * var1 * ((double)bme->caliPara.dig_p6) / 131072.0;
  var2 = var2 + var1 * ((double)bme->caliPara.dig_p5) * 2.0;
  var2 = (var2 / 4.0) + (((double)bme->caliPara.dig_p4) * 65536.0);
  var1 = ((double)bme->caliPara.dig_p3) * var1 * var1 / 16384.0;
  var1 = (var1 + ((double)bme->caliPara.dig_p2) * var1) / 524288.0;
  var1 = (1.0 + var1 / 32768.0) * ((double)bme->caliPara.dig_p1);
  
  /*避免除零异常 */
  if (var1)
  {
    pressure = 1048576.0 - (double)adcCode;
    pressure = ((pressure - (var2 / 4096.0)) * 6250.0 )/ var1;
    var1 = ((double)bme->caliPara.dig_p9 * pressure * pressure )/ 2147483648.0;
    var2 = pressure * ((double)bme->caliPara.dig_p8) / 32768.0;
    var3 = ((pressure  / 256.0f) * (pressure  / 256.0f) * (pressure  / 256.0f)
			* ((double)bme->caliPara.dig_p10 / 131072.0f));
    pressure = pressure + (var1 + var2 +var3+ ((double)bme->caliPara.dig_p7*128.0)) / 16.0;

    if (pressure < pressure_min)
    {
      pressure = pressure_min;
    }
    
    if (pressure > pressure_max)
    {
      pressure = pressure_max;
    }
  }
  else
  {
    pressure = pressure_min;
  }

  return (float)pressure;
}

/*计算实际的湿度值，修正后为Q22.10格式*/
static float CompensateHumidity(BME680ObjectType *bme,uint32_t adcCode)
{
  double humidity;
  double humidity_min = 0.0;
  double humidity_max = 100.0;
  double var1;
  double var2;
  double var3;
  double var4;
  double temp;

  temp=((double)bme->caliPara.t_fine)/5120.0;
  var1=(double)((double)adcCode)-(((double)bme->caliPara.dig_h1*16.0f)+(((double)bme->caliPara.dig_h3/2.0f)*temp));
  var2=var1*((double)(((double)bme->caliPara.dig_h2/262144.0f)*(1.0f + (((double)bme->caliPara.dig_h4/16384.0f)*temp)+(((double)bme->caliPara.dig_h5/1048576.0f)*temp*temp))));
  var3 = (double) bme->caliPara.dig_h6 / 16384.0f;
  var4 = (double) bme->caliPara.dig_h7 / 2097152.0f;
  humidity = var2 + ((var3 + (var4 * temp)) * var2 * var2);

  if (humidity > humidity_max)
  {
    humidity = humidity_max;
  }
  
  if (humidity < humidity_min)
  {
    humidity = humidity_min;
  }

  return (float)humidity;
}

/*计算实际的气体电阻值*/
static float CompensateGasResistance(BME680ObjectType *bme,uint32_t adcCode,uint16_t gasRange)
{
  double gasRes;
  double var1 =0.0;
  /* 气体检测常数数组 */
  float gasConstantFloat[2][16]={{1,1,1,1,1,0.99,1,0.992,1,1,0.998,0.995,1,0.99,1,1},
                                 {8000000,4000000,2000000,1000000,499500.4995,248262.1648,
                                 125000,63004.03226,31281.28128,15625,7812.5,3906.25,
                                 1953.125,976.5625,488.28125,244.140625}};

  var1 = (1340.0f + (5.0f * bme->caliPara.range_sw_err))*gasConstantFloat[0][gasRange];
  gasRes =var1*gasConstantFloat[1][gasRange]/(adcCode-512.0f+var1);
	
  return (float)gasRes;
}

/*计算加热器电阻值,设置到res_wait_x寄存器的值*/
static uint8_t CompensateHeaterResistance(BME680ObjectType *bme,uint16_t tempTarget)
{
  double var1=0.0;
  double var2=0.0;
  double var3=0.0;
  double var4=0.0;
  double var5=0.0;
  uint8_t heatRes=0.0;

  if (tempTarget > 400)
  {
    tempTarget = 400;
  }
	
  var1 = (((double)bme->caliPara.dig_g1 / (16.0f)) + 49.0f);
  var2 = ((((double)bme->caliPara.dig_g2 / (32768.0f)) * (0.0005f)) + 0.00235f);
  var3 = ((double)bme->caliPara.dig_g3 / (1024.0f));
  var4 = (var1 * (1.0f + (var2 * (double)tempTarget)));
  var5 = (var4 + (var3 * (double)bme->temperature));
  heatRes=(uint8_t)(3.4f*((var5*(4.0/(4.0+(double)bme->caliPara.res_heat_range))*(1.0/(1.0+((double)bme->caliPara.res_heat_val*0.002f))))-25.0));

  return heatRes;
}
#endif
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
