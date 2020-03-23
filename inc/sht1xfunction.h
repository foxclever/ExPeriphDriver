/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：sht1xfunction.h                                                **/
/** 版    本：V1.1.0                                                         **/
/** 简    介：用于实现SHT1X温湿度计的操作，采用类似I2C的通讯方式             **/
/**           包括SHT10,SHT11和SHT15以及SHT7x                                **/
/**           默认分辨率14bit(温度)和12bit(湿度)可以被降低为12和8bit         **/
/**           温度转换公式：T=d1+d2*SOT，其中SOT为数字输出，T为温度值        **/
/**           d1和d2的取值如下：                                             **/
/**           VDD               d1 (°C)         d1 (°F)                    **/
/**           5V                -40.1           -40.2                        **/
/**           4V                -39.8           -39.6                        **/
/**           3.5V              -39.7           -39.5                        **/
/**           3V                -39.6           -39.3                        **/
/**           2.5V              -39.4           -38.9                        **/
/**                                                                          **/
/**           SOT               d2 (°C)         d2 (°F)                    **/
/**           14bit             0.01            0.018                        **/
/**           12bit             0.04            0.072                        **/
/**           湿度数据的转换公式：RHlinear=c1+c2*SORH+c3*SORH*SORH，         **/
/**           其中SORH为数字输出，RHlinear为转换后的现对湿度                 **/
/**           SORH	  c1	          c2	          c3                 **/
/**           12bit	-2.0468         0.0367          -1.5955E-6           **/
/**           8bit	-2.0468         0.5872          -4.0845E-4           **/
/**           湿度温度补偿公式：RHtrue=(T-25)*(t1+t2*SORH)+RHlinear          **/
/**           其中RHtrue为温度补偿后的相对湿度                               **/
/**           SORH	  t1	  t2                                         **/
/**           12bit	0.01	0.00008                                      **/
/**           8bit	0.01	0.00128                                      **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    1、声明Sht1xObjectType对象实体，并调用函数SHT1xInitialization初始化   **/
/**       对象实体。给定设备的工作电源电压VDD以及温度单位（摄氏度或华氏度）  **/
/**    2、实现两个形如void f(BusPinValue value)的引脚输出操作函数，分别操    **/
/**       作SCK和SDA。并声明BusPinOperation类型的函数指针数组，SCK的操作函   **/
/**       数作为第一个元素，SDA的操作函数作为第二个元素。并将函数指针数组的  **/
/**       指针赋值给Sht1xObjectType对象实体的SetBusPin函数指针。实现对SCK引  **/
/**       脚和SDA引脚的操作。                                                **/
/**    3、实现形如uint8_t f(void)的函数，并将其赋值给Sht1xObjectType对象实体 **/
/**       的ReadSDABit函数指针。实现数据的读取。                             **/
/**    4、实现形如void f(IODirection direction);的函数，并将其赋值给         **/
/**       Sht1xObjectType对象实体的SetSDADirection函数指针。实现动态改变SDA引**/
/**       脚的输入输出方向                                                   **/
/**    5、实现形如void f(volatile uint32_t period)的函数，并将其赋值给       **/
/**       Sht1xObjectType对象实体的Delayus函数指针。实现操作延时，单位微秒   **/
/**    6、实现形如void f(volatile uint32_t nTime)的函数，并将其赋值给        **/
/**       Sht1xObjectType对象实体的Delayms函数指针。实现操作延时，单位毫秒   **/
/**    7、调用相应的函数实现需要的操作。                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-01-13          木南              创建文件               **/
/**     V1.1.0  2019-05-19          木南              修改为对象操作         **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef _sht1xfunction_h
#define _sht1xfunction_h

#include "stdint.h"
#include "stdbool.h"

/* 定义SHT1X总线引脚的设置值枚举 */
typedef enum SHT1xPinValue{
  SHT1xSet=1,
  SHT1xReset=(!SHT1xSet)
}SHT1xPinValueType;

/* 定义SHT1X总线IO方向枚举 */
typedef enum SHT1xIOMode{
  SHT1xOut=1,
  SHT1xIn=(!SHT1xOut)
}SHT1xIOModeType;

/* 定义SHT1X总线引脚枚举 */
typedef enum SHT1XPin{
  SckPin=0,
  DataPin=1
}SHT1XPinType;

/* 定义温度单位枚举 */
typedef enum SHT1xTempUnit{
  DegreeCentigrade,	//摄氏度
  DegreeFahrenheit	//华氏度
}SHT1xTempUnitType;

/* 定义SHT1X内部加热控制枚举 */
typedef enum SHT1xHeater {
  SHT1xHeaterDisable,
  SHT1xHeaterEbable
}SHT1xHeaterType;

/* 定义SHT1X OTP控制枚举 */
typedef enum SHT1xOTP {
  SHT1xOTPEbable,
  SHT1xOTPDisable	
}SHT1xOTPType;

/* 定义SHT1X测量精度枚举 */
typedef enum SHT1xResolution {
  SHT1xHighResolution,
  SHT1xLowResolution
}SHT1xResolutionType;

/* 定义GPIO引脚输出操作的函数指针 */
typedef void(*SHT1xSetBusPin)(SHT1xPinValueType value);
/* 读数据总线函数 */
typedef uint8_t (*SHT1xReadSDABit)(void);
/* 数据总线方向控制函数 */
typedef void (*SHT1xSDADirection)(SHT1xIOModeType direction);
/* 微秒延时函数 */
typedef void (*SHT1xDelay)(volatile uint32_t period);

/* 定义SHT1x对象类型 */
typedef struct Sht1xObject {
  uint8_t statusReg;            //状态寄存器
  SHT1xTempUnitType tempUnit;   //温度单位
  uint32_t period;              //SCK时钟周期
  float vdd;                    //工作电压
  float temperature;            //温度
  float humidity;               //湿度
  float dewPoint;               //露点
  SHT1xSetBusPin *SetBusPin;    //总线操作函数
  uint8_t (*ReadSDABit)(void);	//读数据总线函数
  void (*SDADirection)(SHT1xIOModeType direction);      //数据总线方向控制函数
  void (*Delayus)(volatile uint32_t period);    //微秒延时函数
  void (*Delayms)(volatile uint32_t nTime);     //毫秒秒延时函数
}Sht1xObjectType;

/* 初始化SHT1x对象 */
void SHT1xInitialization(Sht1xObjectType *sht,          //SHT1X对象变量
                         uint32_t sck,                  //SCK时钟频率
                         float vdd,                     //工作电压
                         SHT1xTempUnitType uint,        //温度单位
                         SHT1xHeaterType heater,        //是否启用加热器设置
                         SHT1xOTPType otp,              //是否加在OTP设置
                         SHT1xResolutionType resolution,//测量分辨率设置
                         SHT1xSetBusPin setSckPin,      //SCK引脚操作函数
                         SHT1xSetBusPin setDataPin,     //DATA引脚操作函数
                         SHT1xReadSDABit readSDA,       //读DATA引脚函数
                         SHT1xSDADirection direction,   //DATA引脚方向配置函数
                         SHT1xDelay delayus,            //微秒延时函数
                         SHT1xDelay delayms);           //毫秒延时函数

/* 获取SHT1x测量数据 */
void GetSht1xMeasureValue(Sht1xObjectType *sht);

/*获取SHT1X的湿度值*/
float GetSht1xHumidityValue(Sht1xObjectType *sht);

/*获取SHT1X的温度值*/
float GetSht1xTemperatureValue(Sht1xObjectType *sht);

/*SHT1X通讯复位*/
void ResetSHT1XCommunication(Sht1xObjectType *sht);

/*对SHT1X实现软件复位*/
uint8_t Sht1xSoftwareReset(Sht1xObjectType *sht);

#endif
/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
