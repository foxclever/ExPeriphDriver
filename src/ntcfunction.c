/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ntcfunction.c                                                  **/
/** 版    本：V1.0.1                                                         **/
/** 简    介：声明和定义负温度系数热电阻测温计算的各种函数和操作             **/
/**           NTC热敏电阻器的B值一般在2000K－6000K之间。10K的常见B只有3435、 **/
/**           3380、3370、3950，100K的常见B值4100，B值有NTC厂家提供。        **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-09-01          木南              创建文件               **/
/**     V1.0.0  2019-07-23          木南              修改为对象操作         **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "ntcfunction.h"

#define KELVIN_CONSTANT  273.15         /*开氏温度常数*/

/*公式法计算NTC温度值*/
float NTCTemperature(NTCObjectType *ntc,float resistance)
{
  float temp;
  float result=0.0;
  
  result=resistance/ntc->nominalR;
  result=(log(result)/ntc->nominalR)+(1/(ntc->nominalT+KELVIN_CONSTANT));
  temp=1/result-KELVIN_CONSTANT;
  
  return temp;
}

/* NTC对象初始化函数 */
void NTCInitialization(NTCObjectType *ntc,float bValue,float nt,float nr)
{
  if(ntc==NULL)
  {
    return;
  }
	
  ntc->BValue=bValue;
  ntc->nominalT=nt;
  ntc->nominalR=nr;
}

/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
