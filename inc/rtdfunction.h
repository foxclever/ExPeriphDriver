/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：rtdfunction.c                                                  **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现铂热电阻温度测量的相关函数和操作                       **/
/**           所面向的热电阻遵循GB/T 30121-2013(IEC 60751:2008)标准          **/
/**           适用于温度系数为0.003851的铂热电阻                             **/
/**           温度与电阻的关系表达式如下：                                   **/
/**               -200到0摄氏度：Rt=R0*[1+A*t+B*t^2+C*(t-100)*t^3]           **/
/**               0到850摄氏度：Rt=R0*(1+A*t+B*t^2)                          **/
/**           上述表达式中温度单位均为摄氏度,式中各常数的取值如下：          **/
/**               A=3.9083*10^-3                                             **/
/**               B=-5.775*10^-7                                             **/
/**               C=-4.183*10^-13                                            **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2018-01-11          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __RTDFUNCTION_H
#define __RTDFUNCTION_H

#include <stdint.h>
#include <math.h>

/* 定义RTD类型枚举 */
typedef enum RTDCategory {
  WZP,
  WZC,
  WZN
}RTDCategoryType;

/* 定义RTD对象 */
typedef struct RTDObject{
  float nominalR;       //RTD标称电阻
  float temperature;    //所测温度
  RTDCategoryType type; //RTD类型
}RTDObjectType;

/*计算热电阻测得的温度*/
float CalcRTDTemperature(RTDObjectType *rtd,float rt);

/* 初始化RTD对象 */
void RTDInitialization(RTDObjectType *rtd,RTDCategoryType type,float nr);
#endif
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
