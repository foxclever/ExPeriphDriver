/******************************************************************************/
/** 模块名称：数据检测与处理                                                 **/
/** 文件名称：ad840xfunction.h                                               **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：声明和定义ad8400数字电位器芯片相关的各种函数和操作             **/
/**           采用SPI接口，支持SPI模式0                                      **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2016-11-08          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __AD840XFUNCTION_H
#define __AD840XFUNCTION_H

#include <stdint.h>

/* 定义AD840x类型枚举 */
typedef enum AD840x{
  AD8400,
  AD8402,
  AD8403
}AD840xType;

/* 定义AD840x通道选择枚举 */
typedef enum AD840xLdac{
  AD840x_VR1=0x00,
  AD840x_VR2=0x01,
  AD840x_VR3=0x02,
  AD840x_VR4=0x03
}AD840xLdacType;

/* 定义AD840x标称电阻枚举 */
typedef enum AD840xNR{
  AD840x_NR1k,		//标称电阻为1K
  AD840x_NR10k,		//标称电阻为10K
  AD840x_NR50k,		//标称电阻为50K
  AD840x_NR100k		//标称电阻为100K
}AD840xNRType;

/* 定义AD840x对象类型 */
typedef struct AD840xObject{
  AD840xType type;		//AD840x对象类型
  float nominalValue;	//标称电阻值
  void (*Write)(uint8_t rdac);	//写数据操作函数指针
}AD840xObjectType;

/* 定义写数据操作函数指针类型 */
typedef void (*AD840xWriteByte)(uint8_t rdac);

/*设置AD8400游标的位置*/
void SetAD8400ResistorValue(AD840xObjectType *vr,uint16_t resistorValue,AD840xLdacType ldac);

/* 初始化AD840x对象 */
void AD840xInitialization(AD840xObjectType *vr,AD840xType type,AD840xWriteByte write,AD840xNRType nr);

#endif

/*********** (C) COPYRIGHT 2016 北京卡米特测控技术有限公司 *****END OF FILE****/
