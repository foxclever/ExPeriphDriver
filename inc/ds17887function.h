/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ds17887function.h                                              **/
/** 版    本：V1.1.0                                                         **/
/** 简    介：实现实时时钟模块DS17887的操作                                  **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2012-07-28          木南              创建文件               **/
/**     V1.1.0  2019-08-07          木南              修改为机遇对象的操作   **/
/**                                                                          **/
/******************************************************************************/ 

#include "stdint.h"

#define  DS17887_Base   0x00     //定义时钟芯片的片选基址

#define  DS17887_Second   DS17887_Base+0x00  // 秒
#define  DS17887_Minute   DS17887_Base+0x02  // 分
#define  DS17887_Hour     DS17887_Base+0x04  // 时
#define  DS17887_Date     DS17887_Base+0x07  // 日期
#define  DS17887_Month    DS17887_Base+0x08  //月份
#define  DS17887_Year     DS17887_Base+0x09  //年

#define  DS17887_Reg_A DS17887_Base+0x0A //控制寄存器A
#define  DS17887_Reg_B DS17887_Base+0x0B //控制寄存器B
#define  DS17887_Reg_C DS17887_Base+0x0C //控制寄存器C
#define  DS17887_Reg_D DS17887_Base+0x0D //控制寄存器D

/* 定义DS17887控制寄存器枚举 */
typedef enum Ds17887CtlReg{
  Reg_A,
  Reg_B,
  Reg_C,
  Reg_D
}Ds17887CtlRegType;

/* 定义DS17887总线的方向类型 */
typedef enum Ds17887BusDirection{
  DS17887_Out=1,
  DS17887_In=!DS17887_Out,
}Ds17887BusDirectionType;

/* 定义DS17887控制引脚的种类 */
typedef enum Ds17887CtlPins{
  DS17887_CS,
  DS17887_WR,
  DS17887_RD,
  DS17887_ALE,
  DS17887_KS,
  DS17887_RCLR  
}Ds17887CtlPinsType;

/* 定义DS17887引脚设定值 */
typedef enum Ds17887PinValue{
  DS17887_Set=1,
  DS17887_Reset=(!DS17887_Set)
}Ds17887PinValueType;

/* 定义DS17887对象类型 */
typedef struct Ds17887Object{
  uint8_t ctlReg[4];    //控制寄存器
  uint8_t dateTime[6];  //读取的系统时间
  void (*SetCtlPin[6])(Ds17887PinValueType value);      //控制引脚操作
  void (*WriteByte)(uint16_t data);     //写一个字节
  uint16_t (*ReadByte)(void);           //读一个字节
  void (*SetBusDirection)(Ds17887BusDirectionType direction);   //设置总线方向
  void (*Delayus)(volatile uint32_t nTime);     //延时ms操作指针
}Ds17887ObjectType;

/* 定义DS17887控制引脚操作函数指针 */
typedef void (*DS17887CtlPinOperationType)(Ds17887PinValueType value);
/* 定义DS17887写数据操作函数指针 */
typedef void (*WriteByteToDs17887Type)(uint16_t data);
/* 定义DS17887读数据操作函数指针 */
typedef uint16_t (*ReadByteFromDs17887Type)(void);
/* 定义设置数据地址总线方向函数指针 */
typedef void (*Ds17887SetBusDirectionType)(Ds17887BusDirectionType direction);
/* 定义延时操作函数指针类型 */
typedef void (*Ds17887DelayusType)(volatile uint32_t nTime);

/* 从实时时钟模块读取时间 */
void GetDateTimeFromDs17887(Ds17887ObjectType *ds17887);
/* 校准DS17887的时间 */
void CalibrationDs17887DateTime(Ds17887ObjectType *ds17887,uint16_t * dateTime);
/* 对DS17887进行初始化配置 */
void Ds17887Initialization(Ds17887ObjectType *ds17887,
                           DS17887CtlPinOperationType *SetCtlPin,
                           WriteByteToDs17887Type WriteByte,
                           ReadByteFromDs17887Type ReadByte,
                           Ds17887SetBusDirectionType SetBusDirection,
                           Ds17887DelayusType Delayus);
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
