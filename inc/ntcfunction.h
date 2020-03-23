/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ntcfunction.h                                                  **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：声明和定义负温度系数热电阻测温计算的各种函数和操作             **/
/**           NTC热敏电阻器的B值一般在2000K－6000K之间。10K的常见B只有3435、 **/
/**           3380、3370、3950，100K的常见B值4100，B值有NTC厂家提供。        **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-09-01          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __NTCFUNCTION_H
#define __NTCFUNCTION_H

#include "math.h"
#include "stdint.h"

/* 定义NTC电阻测温对象 */
typedef struct NTCObject{
  float BValue;         //NTC的B常熟
  float nominalT;       //NTC标称温度（摄氏度）
  float nominalR;       //NTC标称电阻
}NTCObjectType;

/*公式法计算NTC温度值*/
float NTCTemperature(NTCObjectType *ntc,float resistance);

/* NTC对象初始化函数 */
void NTCInitialization(NTCObjectType *ntc,float bValue,float nt,float nr);

#endif
/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
