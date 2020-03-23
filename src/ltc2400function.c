/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ltc2400function.c                                              **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现LTC2400 ADC的数据获取                                  **/
/**           LTC2400采用SPI电气接口                                         **/
/**           输出数据的结构如下：                                           **/
/**           Bit31 Bit30 Bit29 Bit28 Bit27 … Bit4 Bit3-0                    **/
/**           EOC   DMY   SIG   EXR   MSB      LSB  SUB LSBs                 **/
/**           0     0     0/1   0/1   0/1      0/1  X                        **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-01-18          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "ltc2400function.h"

#define LTC2400_OUTPUT_SIGNBIT 0x20000000
#define LTC2400_OUTPUT_EXRNBIT 0x10000000
#define LTC2400_OUTPUT_DATABITS 0xFFFFFF0
#define LTC2400_DIGITAL_RANGE 0xFFFFFF
#define LTC2400_DIGITAL_ZERO 0x0

/*LTC2400数据的合成*/
static float CompoundLTC2400Data(Ltc2400ObjectType *ltc,uint8_t *dataCode);
/* 默认片选操作函数 */
static void DefaultChipSelect(LTC2400CSType en);

/* 获取LTC2400转换数据,返回量程数据的比例值 */
float GetLtc2400Data(Ltc2400ObjectType *ltc)
{
  uint8_t rData[4];
  
  ltc->ChipSelect(LTC2400CS_Enable);
  ltc->Delayms(1);
  
  ltc->Receive(rData);
  
  ltc->Delayms(1);
  ltc->ChipSelect(LTC2400CS_Disable);
  
  return CompoundLTC2400Data(ltc,rData);
}

/*LTC2400数据的合成*/
static float CompoundLTC2400Data(Ltc2400ObjectType *ltc,uint8_t *dataCode)
{
  uint32_t temp=0;
  float result=0.0;
  
  temp=(dataCode[0]<<24)+(dataCode[1]<<16)+(dataCode[2]<<8)+dataCode[3];
  
  ltc->dataCode=temp;
    
  result=(float)(((temp&LTC2400_OUTPUT_DATABITS)>>4)-LTC2400_DIGITAL_ZERO)/(float)(LTC2400_DIGITAL_RANGE-LTC2400_DIGITAL_ZERO);
    
  if((temp&LTC2400_OUTPUT_SIGNBIT)!=LTC2400_OUTPUT_SIGNBIT)
  {
    //负数
    result=result-1.0;
  }
  else
  {
    if((temp&LTC2400_OUTPUT_EXRNBIT)!=LTC2400_OUTPUT_EXRNBIT)
    {
      //超量程
      result=result+1.0;
    }
  }
  
  return result;
}

/* LTC2400对象初始化函数 */
void LTC2400Initialization(Ltc2400ObjectType *ltc,
                           LTC2400ClockType clock,
                           LTC2400Receive receive,
                           LTC2400ChipSelect cs,
                           LTC2400Delay msDelay)
{
  if((ltc==NULL)||(receive==NULL)||(msDelay==NULL))
  {
    return;
  }
	
  ltc->dataCode=0;
  ltc->clock=clock;
  
  if(cs==NULL)	//硬件电路实现片选
  {
    ltc->ChipSelect=DefaultChipSelect;
  }
  else
  {
    ltc->ChipSelect=cs;
  }
	
  ltc->Receive=receive;
  ltc->Delayms=msDelay;
}

/* 默认片选操作函数 */
static void DefaultChipSelect(LTC2400CSType en)
{
  return;
}

/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
