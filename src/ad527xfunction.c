/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ad527xfunction.c                                               **/
/** 版    本：V1.0.2                                                         **/
/** 简    介：声明和定义AD527x系列数字电位器芯片相关的各种函数和操作         **/
/**           本软件适用于AD5270、AD5271、AD5272、AD5274芯片                 **/
/**           其中：AD5270、AD5271采用SPI接口(CPOL = 0, CPHA = 1)            **/
/**                 AD5272、AD5274采用I2C接口                                **/
/**           输入一位寄存器的格式如下：                                     **/
/**           +---+---+--+--+--+--+--+--+--+--+--+--+--+--+--+--+            **/
/**           |未定义 |  控制位   |          数据位             |            **/
/**           +---+---+--+--+--+--+--+--+--+--+--+--+--+--+--+--+            **/
/**           | 0 | 0 |C3|C2|C1|C0|D9|D8|D7|D6|D5|D4|D3|D2|D1|D0|            **/
/**           +---+---+--+--+--+--+--+--+--+--+--+--+--+--+--+--+            **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    1、声明AD527x对象实体，并调用函数AD527xInitialization初始化对象实体。 **/
/**    2、初始化时，若对象类型为AD5272、AD5274则给定设备地址，以备I2C通讯。  **/
/**       若对象类型为AD5270、AD5271则实现形如void f(bool en)的函数，并将其  **/
/**       赋值AD527x对象实体的ChipSelcet函数指针。以备SPI通讯。              **/
/**    3、实现形如void f(struct AD527xObject *rx,uint8_t *rData,             **/
/**       uint16_t rSize)的函数，并将其赋值给AD527x对象实体的Receive函数     **/
/**       指针。完成收数据操作                                               **/
/**    4、实现形如void f(struct AD527xObject *rx,uint8_t *wData,             **/
/**       uint16_t wSize)的函数，并将其赋值给AD527x对象实体的Transmit函数    **/
/**       指针。完成写数据操作                                               **/
/**    5、实现形如void f(volatile uint32_t nTime)延时函数，并将其赋值给      **/
/**       AD527x对象实体的Delay函数指针。单位为毫秒。                        **/
/**    6、调用相应的函数实现需要的操作。                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2019-04-01          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "ad527xfunction.h"

/* 定义操作命令 */
#define COMMAND_NOP                     (0x00<<10)
#define COMMAND_W_RDAC                  (0x01<<10)
#define COMMAND_R_RDAC                  (0x02<<10)
#define COMMAND_STORE_RDAC_50TP         (0x03<<10)
#define COMMAND_SOFTWARE_RESET          (0x04<<10)
#define COMMAND_R_50TP_NEXT_FRAME       (0x05<<10)
#define COMMAND_R_LAST_50TP_LOCATION    (0x06<<10)
#define COMMAND_W_CONTROL               (0x07<<10)
#define COMMAND_R_CONTROL               (0x08<<10)
#define COMMAND_SOFTWARE_SHUTDOWN       (0x09<<10)

/* 定义写控制寄存器操作 */
#define PROGRAM_50TP_DISABLE		0x00	//0 = 50-TP编程禁用(默认) 
#define PROGRAM_50TP_ENABLE			0x01	//1 = 使能器件进行50-TP编程
#define PROGRAM_RDAC_DISABLE		0x00	//0 = 游标位置冻结至50-TP存储器中的值(默认)
#define PROGRAM_RDAC_ENABLE			0x02	//1 = 允许通过数字接口更新游标位置
#define RDAC_RESISTANCE_ENABLE	0x00	//0 = RDAC电阻容差校准使能(默认) 
#define RDAC_RESISTANCE_DISABLE	0x04	//1 = RDAC电阻容差校准禁用

/* 定义软件关断模式 */
#define SOFT_NORMAL_MODE				0x00	//正常模式
#define SOFT_SHUTOFF_MODE				0x01	//关断模式

/* 读取控制寄存器的值 */
static void ReadControlRegister(AD527xObjectType *rx);
/* 设置控制寄存器*/
static void SetControlRegister(AD527xObjectType *rx,uint16_t value);
/* 设置软件关断模式 */
static void SetSoftShutMode(AD527xObjectType *rx,uint16_t mode);
/* 默认片旬操作函数 */
static void DefaultChipSelect(AD527xCSType en);
/* 写寄存器操作 */
static void AD527xWriteRegister(AD527xObjectType *rx,uint16_t cmd);
/* 读寄存器操作 */
static void AD527xReadRegister(AD527xObjectType *rx,uint16_t cmd,uint8_t *rData);
	
/* 设置AD527x游标位置 */
void SetRDACForAd527x(AD527xObjectType *rx,uint16_t data)
{
  uint16_t temp=0;
  
  if((rx->type==AD5271)||(rx->type==AD5274))	//256档
  {
    temp=data>255?255:data;
  }
  else if((rx->type==AD5270)||(rx->type==AD5272))	//1024档
  {
    temp=data>1023?1023:data;
  }
	
  temp=COMMAND_W_RDAC|temp;
	
  if(((rx->conreg)&0x02)!=0x02)
  {
    SetControlRegister(rx,PROGRAM_RDAC_ENABLE|rx->conreg);
  }
	
  AD527xWriteRegister(rx,temp);
}

/* 初始化AD527x对象,I2C接口必须初始化devAddress，SPI接口必需初始化void (*ChipSelcet)(bool) */
void AD527xInitialization(AD527xObjectType *rx,
                          uint8_t address,
                          AD527xType type,
                          AD527xReceive recieve,
                          AD527xTransmit transmit,
                          AD527xChipSelect cs,
                          AD527xDelayms delayms)
{
  if((rx==NULL)||(recieve==NULL)||(transmit==NULL)||(delayms==NULL))
  {
    return;
  }
	
  if((type==AD5270)||(type==AD5271))	//使用SPI接口
  {
    if(cs==NULL)	//硬件电路实现片选
    {
      rx->ChipSelect=DefaultChipSelect;
    }
    else
    {
      rx->ChipSelect=cs;
    }
    rx->devAddress=0x00;
  }
  else	//使用I2C接口
  {
    if((address==0x58)||(address==0x5C)||(address==0x5E))
    {
      rx->devAddress=address;
    }
    else if((address==0x2C)||(address==0x2E)||(address==0x2F))
    {
      rx->devAddress=(address<<1);
    }
    else
    {
      rx->devAddress=0x00;
    }
  
    rx->ChipSelect=NULL;
  }	
  
  rx->type=type;
	
  rx->conreg=0x00;
  rx->rdac=0x0000;
	
  rx->Receive=recieve;
  rx->Transmit=transmit;
  rx->Delayms=delayms;
	
  ReadControlRegister(rx);
	
  SetSoftShutMode(rx,SOFT_NORMAL_MODE);
}

/* 读取RDAC游标寄存器的内容 */
uint16_t ReadRDACFromAd527x(AD527xObjectType *rx)
{
  uint8_t rData[2];

  uint16_t cmd=COMMAND_R_RDAC;

  AD527xReadRegister(rx,cmd,rData);

  rx->rdac=(rData[0]<<8)+rData[1];
  
  return rx->rdac;
}

/* 存储游标设置：将RDAC设置存储到50-TP中 */
void StoreRDACTo50TP(AD527xObjectType *rx)
{
  uint16_t cmd=COMMAND_STORE_RDAC_50TP;
	
  if(((rx->conreg)&0x01)!=0x01)
  {
    SetControlRegister(rx,PROGRAM_50TP_ENABLE|rx->conreg);
  }
	
  AD527xWriteRegister(rx,cmd);
}

/* 软件复位 */
void AD527xSoftwareReset(AD527xObjectType *rx)
{
  uint16_t cmd=COMMAND_SOFTWARE_RESET;

  AD527xWriteRegister(rx,cmd);
}

/* 读指定存储位置的数据 */
uint16_t ReadGivenAddressValue(AD527xObjectType *rx,uint8_t address)
{
  uint8_t rData[2];
  uint16_t cmd=COMMAND_R_50TP_NEXT_FRAME|address;
	
  AD527xReadRegister(rx,cmd,rData);
	
  return (rData[0]<<8)+rData[1];
}

/* 读取最后一次50-TP编程存储器位置的地址 */
uint8_t ReadLast50TPAddress(AD527xObjectType *rx)
{
  uint8_t rData[2];
  uint16_t cmd=COMMAND_R_LAST_50TP_LOCATION;
	
  AD527xReadRegister(rx,cmd,rData);
	
  return rData[1];
}

/* 设置软件关断模式 */
static void SetSoftShutMode(AD527xObjectType *rx,uint16_t mode)
{
  uint16_t cmd=COMMAND_SOFTWARE_SHUTDOWN|mode;
	
  AD527xWriteRegister(rx,cmd);
}

/* 设置控制寄存器*/
static void SetControlRegister(AD527xObjectType *rx,uint16_t value)
{
  uint16_t cmd=COMMAND_W_CONTROL|value;
	
  AD527xWriteRegister(rx,cmd);
	
  ReadControlRegister(rx);
}

/* 读取控制寄存器的值 */
static void ReadControlRegister(AD527xObjectType *rx)
{
  uint8_t rData[2];

  uint16_t cmd=COMMAND_R_CONTROL;
  
  AD527xReadRegister(rx,cmd,rData);
	
  rx->conreg=rData[1];
}

/* 写寄存器操作 */
static void AD527xWriteRegister(AD527xObjectType *rx,uint16_t cmd)
{
  uint8_t tData[2];
	
  tData[0]=(uint8_t)(cmd>>8);
  tData[1]=(uint8_t)cmd;
	
  if((rx->type==AD5270)||(rx->type==AD5271))	//SPI接口
  {
    rx->ChipSelect(AD527xCS_ENABLE);
    rx->Delayms(1);
  }

  rx->Transmit(rx,tData,2);
	
  if((rx->type==AD5270)||(rx->type==AD5271))	//SPI接口
  {
    rx->Delayms(1);
    rx->ChipSelect(AD527xCS_DISABLE);
  }
}

/* 读寄存器操作 */
static void AD527xReadRegister(AD527xObjectType *rx,uint16_t cmd,uint8_t *rData)
{
  uint8_t tData[2];
	
  if((rx->type==AD5270)||(rx->type==AD5271))	//SPI接口
  {
    rx->ChipSelect(AD527xCS_ENABLE);
    rx->Delayms(1);
  }
	
  rx->Transmit(rx,tData,2);

  rx->Receive(rx,rData,2);
	
  if((rx->type==AD5270)||(rx->type==AD5271))	//SPI接口
  {
    rx->Delayms(1);
    rx->ChipSelect(AD527xCS_DISABLE);
  }
}

/* 默认片选操作函数 */
static void DefaultChipSelect(AD527xCSType en)
{
  return;
}

/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
