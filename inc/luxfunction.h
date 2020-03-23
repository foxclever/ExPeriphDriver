/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：luxfunction.h                                                  **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：声明和定义S1336-5BQ光敏二极管作为光度计的各种函数和操作        **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-09-01          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __LUXFUNCTION_H
#define __LUXFUNCTION_H

#include "stdint.h"

/* 定义光度检测对象类型 */
typedef struct LuxObject {
  float isc100lux;      //每100个光度变化所对应的电流变化
  float rnf;            //采样电阻的值
  float vref;           //参考电压
  float lux;            //光度值
}LuxObjectType;

/*计算光照强度*/
float CalcLxIllumination(LuxObjectType *lm,float mVoltage);

/* 光度检测对象初始化 */
void LuxInitialization(LuxObjectType *lm,float isc,float vref,float rnf);

#endif
/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
