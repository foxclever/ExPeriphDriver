/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：luxfunction.c                                                  **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：声明和定义S1336-5BQ光敏二极管作为光度计的各种函数和操作        **/
/**           S1336-5BQ光敏二极管到通电流基准：100lx为5微安                  **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-09-01          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "luxfunction.h"
#include "stddef.h"

/*计算光照强度*/
float CalcLxIllumination(LuxObjectType *lm,float mVoltage)
{
  float lux=0.0;
  lux=(mVoltage-lm->vref)*100/(lm->rnf*lm->isc100lux);
  lm->lux=lux;
  return lux;
}

/* 光度检测对象初始化 */
void LuxInitialization(LuxObjectType *lm,float isc,float vref,float rnf)
{
  if(lm==NULL)
  {
    return;
  }
	
  lm->lux=0.0;
  lm->isc100lux=isc;
  lm->rnf=rnf;
  lm->vref=vref;
}

/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
