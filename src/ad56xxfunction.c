/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ad56xxfunction.c                                               **/
/** 版    本：V2.0.0                                                         **/
/** 简    介：声明和定义适用于ADI公司AD56xx系列数模转换芯片的函数和操作。    **/
/**           采用SPI接口相关器件的特点是均具备24位输入寄存器，具有相同的    **/
/**           命令格式。适用的器件有：                                       **/
/**           AD5662，单通道16位数模转换器                                   **/
/**           AD5623，双通道12位数模转换器                                   **/
/**           AD5643，双通道14位数模转换器                                   **/
/**           AD5663，双通道16位数模转换器                                   **/
/**           AD5624，四通道12位数模转换器                                   **/
/**           AD5644，四通道14位数模转换器                                   **/
/**           AD5664，四通道16位数模转换器                                   **/
/**                                                                          **/
/**     其中，AD5662的24位移位寄存器数据格式如下：                           **/
/**     +-+-+-+-+-+-+---+---+---+---+---+---+-------+--+--+--+--+            **/
/**     |  ------   |  模式 |                数据位             |            **/
/**     +-+-+-+-+-+-+---+---+---+---+---+---+-------+--+--+--+--+            **/
/**     |x|x|x|x|x|x|PD1|PD0|D15|D14|D13|D12| ......|D3|D2|D1|D0|            **/
/**     +-+-+-+-+-+-+---+---+---+---+---+---+-------+--+--+--+--+            **/
/**                                                                          **/
/**     其中，AD5663和AD5664的24位移位寄存器数据格式如下：                   **/
/**     +-+-+--+--+--+--+--+--+---+---+---+---+-------+--+--+--+--+          **/
/**     | - | 命令位 | 地址位 |                数据位             |          **/
/**     +-+-+--+--+--+--+--+--+---+---+---+---+-------+--+--+--+--+          **/
/**     |x|x|C2|C1|C0|A2|A1|A0|D15|D14|D13|D12|.......|D3|D2|D1|D0|          **/
/**     +-+-+--+--+--+--+--+--+---+---+---+---+-------+--+--+--+--+          **/
/**                                                                          **/
/**     其中，AD5643和AD5644的24位移位寄存器数据格式如下：                   **/
/**     +-+-+--+--+--+--+--+--+---+---+---+---+-------+--+--+--+--+-+-+      **/
/**     | - | 命令位 | 地址位 |                数据位             | - |      **/
/**     +-+-+--+--+--+--+--+--+---+---+---+---+-------+--+--+--+--+-+-+      **/
/**     |x|x|C2|C1|C0|A2|A1|A0|D13|D12|D11|D10|.......|D3|D2|D1|D0|x|x|      **/
/**     +-+-+--+--+--+--+--+--+---+---+---+---+-------+--+--+--+--+-+-+      **/
/**                                                                          **/
/**     其中，AD5623和AD5624的24位移位寄存器数据格式如下：                   **/
/**     +-+-+--+--+--+--+--+--+---+---+--+--+-------+--+--+--+--+-+-+-+-+    **/
/**     | - | 命令位 | 地址位 |                数据位           | ----- |    **/
/**     +-+-+--+--+--+--+--+--+---+---+--+--+-------+--+--+--+--+-+-+-+-+    **/
/**     |x|x|C2|C1|C0|A2|A1|A0|D11|D10|D9|D8|.......|D3|D2|D1|D0|x|x|x|x|    **/
/**     +-+-+--+--+--+--+--+--+---+---+--+--+-------+--+--+--+--+-+-+-+-+    **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    1、声明AD56xx对象实体，并调用函数AD56xxInitialization初始化对象实体。 **/
/**    初始化时，指定设备类型（DACTypeDef）                                  **/
/**    2、实现形如void f(uint8_t *tData,uint16_t tSize)的函数，并将其赋值    **/
/**    给AD56xx对象实体的WriteDataToDAC函数指针。                            **/
/**    3、实现形如void f(bool en)的函数，并将其赋值AD56xx对象实体的ChipSelcet**/
/**    函数指针。                                                            **/
/**    4、调用相应的函数实现需要的操作。                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2015-07-28          木南              创建文件               **/
/**     V1.0.1  2017-03-17          木南              作适应性修改           **/
/**     V1.5.0  2018-01-05          木南              修改使适用于同类器件   **/
/**     V2.0.0  2018-05-13          木南              修改为对象定义及操作   **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "ad56xxfunction.h"

/*命令定义*/
#define Write_to_Input_Register         0x000000
#define Update_DAC_Register             0x080000
#define Software_LDAC                   0x100000
#define WriteTo_Update_DAC_Channel      0x180000
#define Power_Down                      0x200000
#define RESET                           0x280000
#define LDAC_Register_Setup             0x300000
#define Reference_Set                   0x380000

/*地址模式,选择DAC通道*/
#define DAC_Channel_A                   0x000000
#define DAC_Channel_B                   0x010000
#define DAC_Channel_C                   0x020000
#define DAC_Channel_D                   0x030000
#define DAC_Channel_ALL                 0x070000

/*power down模式选择*/
#define Normal_Operation                0x000000
#define _1K_GND                         0x000010
#define _100K_GND                       0x000020
#define Three_State                     0x000030

/*软件复位模式*/
#define Register_Reset_Software         0x000000
#define Register_Reset_Poweron          0x000001

/*power down模式通道或者LDAC模式引脚选择*/
#define DAC_None                        0x00000000
#define DAC_A                           0x00000001
#define DAC_B                           0x00000002
#define DAC_C                           0x00000004
#define DAC_D                           0x00000008
#define DAC_ALL                         0x0000000F

/* 内部参考电压源设置 */
#define Reference_OFF   0x00; //内部参考电压源关闭
#define Reference_ON    0x01; //内部参考电压源开启

/* 默认片选操作函数 */
static void DefaultChipSelect(AD56xxCSType cs);

/* 对AD56xx进行软件复位 */
void Ad56xxSoftwareReset(AD56xxObjectType *dacObj,AD56xxResetType resetMode)
{
  uint32_t inputShiftData=0;
  
  if(resetMode==ResetSoftware)
  {
    inputShiftData=RESET|Register_Reset_Software;
  }
  
  if(resetMode==ResetPoweron)
  {
    inputShiftData=RESET|Register_Reset_Poweron;
  }
  uint8_t txData[3];
  txData[0]=inputShiftData>>16;
  txData[1]=inputShiftData>>8;
  txData[2]=inputShiftData;
  
  dacObj->ChipSelcet(AD56xxCS_Enable);
  dacObj->WriteDataToDAC(txData,3);
  dacObj->ChipSelcet(AD56xxCS_Disable);
}

/* 设置AD56xx上电/掉电工作模式 */
void Ad56xxPowerUpDownMode(AD56xxObjectType *dacObj,AD56xxChannelType channel,AD56xxPowerdownType powerdownType)
{
  uint32_t inputShiftData=0;
  uint32_t pdc=0;
  uint32_t pdm=0;
  uint32_t cmd=Power_Down;
   
  uint32_t pdChannel[]={DAC_A,DAC_B,DAC_C,DAC_D,DAC_ALL,DAC_None};
  
  pdc=pdChannel[channel];
  
  uint32_t pdMode[]={Normal_Operation,_1K_GND,_100K_GND,Three_State};
  
  pdm=pdMode[powerdownType];
  
  if(dacObj->objectType==AD5662)
  {
    pdc=DAC_None;
    pdm=(pdm<<12);
    cmd=Write_to_Input_Register;
  }
  
  inputShiftData=cmd|pdc|pdm;
   
  uint8_t txData[3];
  txData[0]=inputShiftData>>16;
  txData[1]=inputShiftData>>8;
  txData[2]=inputShiftData;
  
  dacObj->ChipSelcet(AD56xxCS_Enable);
  dacObj->WriteDataToDAC(txData,3);
  dacObj->ChipSelcet(AD56xxCS_Disable);
}

/* 设置AD56xx及同类器件LDAC功能 */
void SetAd56xxLdacFunction(AD56xxObjectType *dacObj,AD56xxChannelType channel)
{
  uint32_t inputShiftData=0;
  
  uint32_t pdChannel[]={DAC_A,DAC_B,DAC_C,DAC_D,DAC_ALL,DAC_None};
  
  inputShiftData=pdChannel[channel];
  
  inputShiftData=inputShiftData|LDAC_Register_Setup;
  
  uint8_t txData[3];
  txData[0]=(uint8_t)(inputShiftData>>16);
  txData[1]=(uint8_t)(inputShiftData>>8);
  txData[2]=(uint8_t)inputShiftData;
  
  dacObj->ChipSelcet(AD56xxCS_Enable);
  dacObj->WriteDataToDAC(txData,3);
  dacObj->ChipSelcet(AD56xxCS_Disable);
}

/* 开启或关闭内部参考电压源 */
void SetInternalReference(AD56xxObjectType *dacObj,AD56xxRefType ref)
{
  uint32_t inputShiftData=0;
	
  inputShiftData=Reference_Set;
	
  if(ref==AD56xxRef_ON)
  {
    inputShiftData=inputShiftData|Reference_ON;
  }
	
  uint8_t txData[3];
  txData[0]=(uint8_t)(inputShiftData>>16);
  txData[1]=(uint8_t)(inputShiftData>>8);
  txData[2]=(uint8_t)inputShiftData;
  
  dacObj->ChipSelcet(AD56xxCS_Enable);
  dacObj->WriteDataToDAC(txData,3);
  dacObj->ChipSelcet(AD56xxCS_Disable);
}

/* 设置DA通道的值 */
AD56xxErrorType SetAD56xxChannelValue(AD56xxObjectType *dacObj,AD56xxChannelType channel,uint16_t data)
{
  uint32_t inputShiftData=0;
  uint32_t dac=0;
  uint32_t cmd=WriteTo_Update_DAC_Channel;
  
  uint32_t dacChannel[]={DAC_Channel_A,DAC_Channel_B,DAC_Channel_C,DAC_Channel_D,DAC_Channel_ALL};
  
  uint32_t shiftV[]={0,4,2,0,4,0};
  
  if(channel>=ChannelNone)
  {
    return AD56xx_ChannelError;
  }
  
  dac=dacChannel[channel];
  
  if(dacObj->objectType==AD5662)
  {
    dac=DAC_Channel_A;
    cmd=Write_to_Input_Register;
  }

  inputShiftData=dac|cmd|(data<<shiftV[dacObj->objectType]);
   
  uint8_t txData[3];
  txData[0]=(uint8_t)(inputShiftData>>16);
  txData[1]=(uint8_t)(inputShiftData>>8);
  txData[2]=(uint8_t)inputShiftData;
  
  dacObj->ChipSelcet(AD56xxCS_Enable);
  dacObj->WriteDataToDAC(txData,3);
  dacObj->ChipSelcet(AD56xxCS_Disable);
	
  return AD56xx_OK;
}

/* 初始化AD56xx对象 */
AD56xxErrorType AD56xxInitialization(AD56xxObjectType *dacObj,AD56xxType objectType,AD56xxWrite write,AD56xxChipSelcet cs)
{
  if((dacObj==NULL)||(write==NULL))
  {
    return AD56xx_InitError;
  }
  dacObj->WriteDataToDAC=write;
  
  if(cs!=NULL)
  {
    dacObj->ChipSelcet=cs;
  }
  else
  {
    dacObj->ChipSelcet=DefaultChipSelect;
  }
  
  if(objectType<TypeNumber)
  {
    dacObj->objectType=objectType;
  }

  return AD56xx_OK;
}

/* 默认片选操作函数 */
static void DefaultChipSelect(AD56xxCSType cs)
{
  return;
}
/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/
