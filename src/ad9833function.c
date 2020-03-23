/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ad9833function.c                                               **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：声明和定义AD9833可编程波形发生器相关的各种函数和操作           **/
/**           AD9833可编程波形发生器采用SPI通讯接口，可以生成不同频率和幅值  **/
/**           的正弦播、三角波和方波。具有一个16位控制寄存器如下：           **/
/**           高位字节如下：                                                 **/
/**           +----+----+----+----+-------+-------+---+-----+                **/
/**           |DB15|DB14|DB13|DB12|  DB11 |  DB10 |DB9| DB8 |                **/
/**           +----+----+----+----+-------+-------+---+-----+                **/
/**           | 0  | 0  | B28| HLB|FSELECT|PSELECT| 0 |RESET|                **/
/**           +----+----+----+----+-------+-------+---+-----+                **/
/**           低位字节如下：                                                 **/
/**           +------+-------+-------+---+----+---+----+---+                 **/
/**           |  DB7 |  DB6  |  DB5  |DB4| DB3|DB2| DB1|DB0|                 **/
/**           +------+-------+-------+---+----+---+----+---+                 **/
/**           |SLEEP1|SLEEP12|OPBITEN| 0 |DIV2| 0 |MODE| 0 |                 **/
/**           +------+-------+-------+---+----+---+----+---+                 **/
/**           其中DB12要与DB13配合使用，DB3和DB1要与DB5配合使用。            **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    1、声明AD9833对象实体，并调用函数AD9833Initialization初始化对象实体。 **/
/**    初始化时，给定始终频率MCLK，单位为MHz。                               **/
/**    2、实现形如void f(uint8_t *tData,uint16_t tSize)的函数，并将其赋值    **/
/**    给AD9833对象实体的WriteDataToDAC函数指针。                            **/
/**    3、实现形如void f(bool en)的函数，并将其赋值AD9833对象实体的ChipSelcet**/
/**    函数指针。                                                            **/
/**    4、调用相应的函数实现需要的操作。                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2019-05-16          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "ad9833function.h"

/* 发送一个16位数据到AD9833 */
static void SendToAD9833(Ad9833ObjectType *dev,uint16_t data);
/* 频率寄存器的操作配置：是28位或14位，高字段或低字段 */
static void ConfigFreqRegisterStyle(Ad9833ObjectType *dev,WriteAd9833FreqReg reg);
/* 默认片选操作函数，用于硬件片选时 */
static void DefaultChipSelcet(AD9833CSType en);

/* 设置频率寄存器的值 */
void SetAD9833FreqRegister(Ad9833ObjectType *dev,WriteAd9833FreqReg reg,uint32_t freqValue)
{
  uint16_t msbFreq,lsbFreq;
  uint32_t freqReg;
	
  freqReg =(uint32_t)(dev->freqConstant*freqValue);
  lsbFreq = (freqReg & 0x0003FFF);
  msbFreq = ((freqReg & 0xFFFC000) >> 14);
	
  ConfigFreqRegisterStyle(dev,reg);
	
  switch(reg)
  {
    case FREQ0_B28:
    {
      lsbFreq |=FREQ0_Address;
      SendToAD9833(dev,lsbFreq);
      msbFreq |=FREQ0_Address;
      SendToAD9833(dev,msbFreq);
      break;
    }
    case FREQ0_B14_LSB:
    {
      lsbFreq |=FREQ0_Address;
      SendToAD9833(dev,lsbFreq);
      break;
    }
    case FREQ0_B14_MSB:
    {
      msbFreq |=FREQ0_Address;
      SendToAD9833(dev,msbFreq);
      break;
    }
    case FREQ1_B28:
    {
      lsbFreq |=FREQ1_Address;
      SendToAD9833(dev,lsbFreq);
      msbFreq |=FREQ1_Address;
      SendToAD9833(dev,msbFreq);
      break;
    }
    case FREQ1_B14_LSB:
    {
      lsbFreq |=FREQ1_Address;
      SendToAD9833(dev,lsbFreq);
      break;
    }
    case FREQ1_B14_MSB:
    {
      msbFreq |=FREQ1_Address;
      SendToAD9833(dev,msbFreq);
      break;
    }
    default:
    {
      break;
    }
  }
	
}
	
/* 频率寄存器的操作配置：是28位或14位，高字段或低字段 */
static void ConfigFreqRegisterStyle(Ad9833ObjectType *dev,WriteAd9833FreqReg reg)
{
  uint16_t regValue=dev->ctlRegister;
//	uint8_t wData[2];
  regValue&=(~(AD9833_CTRLB28|AD9833_CTRLHLB));
	
  switch(reg)
  {
    case FREQ0_B28:
    case FREQ1_B28:
    {
      regValue |=AD9833_CTRLB28;
      break;
    }
    case FREQ0_B14_MSB:
    case FREQ1_B14_MSB:
    {
      regValue |=AD9833_CTRLHLB;
      break;
    }
    default:
    {
      break;
    }
  }
	
  SendToAD9833(dev,regValue);
  dev->ctlRegister=regValue;
}
	
/* 设置相位寄存器的值 */
void SetAD9833PhaseRegister(Ad9833ObjectType *dev,Ad9833PhaseReg reg,float phaseValue)
{
  uint16_t phaseReg=0;
  float phaseConstant=651.8986469;
  
  phaseReg=(uint16_t)(phaseValue*phaseConstant);
  phaseReg&=0x0FFF;
	
  if(reg==PHASE0)
  {
    phaseReg|=PHASE0_Address;
  }
  else
  {
    phaseReg|=PHASE1_Address;
  }
	
  SendToAD9833(dev,phaseReg);
}
	
/* 频率寄存器选择 */
void SelectAD9833FregRegister(Ad9833ObjectType *dev,Ad9833FreqReg reg)
{
  uint16_t regValue=dev->ctlRegister;

  regValue&=(~AD9833_CTRLFSEL);
	
  if(reg==FREQ1)
  {
    regValue|=AD9833_CTRLFSEL;
  }
	
  SendToAD9833(dev,regValue);
	
  dev->ctlRegister=regValue;
}
	
/* 相位寄存器选择 */
void SelectAD9833PhaseRegister(Ad9833ObjectType *dev,Ad9833PhaseReg reg)
{
  uint16_t regValue=dev->ctlRegister;

  regValue&=(~AD9833_CTRLPSEL);
	
  if(reg==PHASE1)
  {
    regValue|=AD9833_CTRLPSEL;
  }
	
  SendToAD9833(dev,regValue);
	
  dev->ctlRegister=regValue;
}
	
/* 复位AD9833对象 */
void ResetAD9833Object(Ad9833ObjectType *dev)
{
  uint16_t regValue=dev->ctlRegister;
	
  regValue|=AD9833_CTRLRESET;
  SendToAD9833(dev,regValue);
	
  dev->Delayms(1);
	
  regValue&=(~AD9833_CTRLRESET);
  SendToAD9833(dev,regValue);
	
  dev->ctlRegister=regValue;
}

/* 设置AD9833休眠状态 */
void SetAD9833SleepMode(Ad9833ObjectType *dev,Ad9833SleepMode mode)
{
  uint16_t regValue=dev->ctlRegister;

  regValue&=(~(AD9833_CTRLSLEEP1|AD9833_CTRLSLEEP12));
	
  switch(mode)
  {
    case DACTurnOff:
    {
      regValue|=AD9833_CTRLSLEEP12;
      break;
    }
    case MCLKTurnOff:
    {
      regValue|=AD9833_CTRLSLEEP1;
      break;
    }
    case DACMCLKTurnOff:
    {
      regValue|=(AD9833_CTRLSLEEP1|AD9833_CTRLSLEEP12);
      break;
    }
    default:
    {
      break;
    }
  }
  SendToAD9833(dev,regValue);
  
  dev->ctlRegister=regValue;
}
	
/* 设置AD9833的输出模式 */
void SetAD9833OutputMode(Ad9833ObjectType *dev,Ad9833OutMode mode)
{
  uint16_t regValue=dev->ctlRegister;
  
  regValue&=(~(AD9833_CTRLOPBITEN|AD9833_CTRLDIV2|AD9833_CTRLMODE));
	
  switch(mode)
  {
    case triangular:
    {
      regValue|=AD9833_CTRLMODE;
      break;
    }
    case square_msb_2:
    {
      regValue|=AD9833_CTRLOPBITEN;
      break;
    }
    case square_msb:
    {
      regValue|=(AD9833_CTRLOPBITEN|AD9833_CTRLDIV2);
      break;
    }
    default:
    {
      break;
    }
  }

  SendToAD9833(dev,regValue);
	
  dev->ctlRegister=regValue;
}

/* 初始化AD9833对象 */
void AD9833Initialization(Ad9833ObjectType *dev,
                          float mclk,
                          AD9833WriteData write,
                          AD9833ChipSelcet cs,
                          AD9833Delayms delayms)
{
  if((dev==NULL)||(write==NULL)||(delayms==NULL))
  {
    return;
  }
  dev->WriteData=write;
  dev->Delayms=delayms;
  
  dev->ctlRegister=0x0000;
  
  if(mclk>0)
  {
    dev->freqConstant=268.435456/mclk;
  }
  else
  {
    dev->freqConstant=10.73741824;	//默认是25M
  }
	
  if(cs!=NULL)
  {
    dev->ChipSelcet=cs;
  }
  else
  {
    dev->ChipSelcet=DefaultChipSelcet;
  }
	
  ResetAD9833Object(dev);
}

/* 发送一个16位数据到AD9833 */
static void SendToAD9833(Ad9833ObjectType *dev,uint16_t data)
{
  uint8_t wData[2];
  
  wData[0]=(uint8_t)(data>>8);
  wData[1]=(uint8_t)data;
  
  dev->ChipSelcet(AD9833CS_ENABLE);
  dev->WriteData(wData,2);
  dev->ChipSelcet(AD9833CS_DISABLE);
}

/* 默认片选操作函数，用于硬件片选时 */
static void DefaultChipSelcet(AD9833CSType en)
{
  return;
}
	
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
