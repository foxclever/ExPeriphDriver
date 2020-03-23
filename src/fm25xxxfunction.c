/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：fm25xxxfunction.c                                              **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现FM25系列串行FRAM的操作，包括4K、16K、64K、128K、256K、 **/
/**           512K、1M等多种容量，不同容量其采用的设备地址位、寄存器地址位存 **/
/**           在差异，具体配置如下：                                         **/
/**           +----------+---------------+-----------------+--------+        **/
/**           |   型号   |   容量结构    |    寄存器地址   |每页字节|        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25L04B | 512x8(4K)     | 9位(1个寄存器)  |  8字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25040B | 512x8(4K)     | 9位(1个寄存器)  |  8字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25C160B| 2048x8(16K)   | 11位(1个寄存器) | 32字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25L16B | 2048x8(16K)   | 11位(1个寄存器) | 32字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25CL64B| 8192x8(64K)   | 13位(2个寄存器) | 32字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25640B | 8192x8(64K)   | 13位(2个寄存器) | 32字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25V01A | 16384x8(128K) | 14位(2个寄存器) | 64字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25W256 | 32768x8(256K) | 15位(2个寄存器) | 64字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25V02A | 32768x8(256K) | 15位(2个寄存器) | 64字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25V05  | 65536x8(512K) | 16位(2个寄存器) | 128字节|        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25V10  | 131072x8(1M)  | 17位(3个寄存器) | 256字节|        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           FM25系列FRAM采用SPI接口，SPI接口模式支持Mode0和Mode3：         **/
/**                   SPI Mode 0 (CPOL = 0, CPHA = 0)                        **/
/**                   SPI Mode 3 (CPOL = 1, CPHA = 1)                        **/
/**           状态寄存器不仅反映状态还用来设置写保护，转台寄存器各位：       **/
/**           +-------+------+------+------+------+------+------+------+     **/
/**           |  Bit7 | Bit6 | Bit5 | Bit4 | Bit3 | Bit2 | Bit1 | Bit0 |     **/
/**           +-------+------+------+------+------+------+------+------+     **/
/**           |WPEN(0)| X(0) | X(0) | X(0) |BP1(0)|BP0(0)|WEL(0)| X(0) |     **/
/**           +-------+------+------+------+------+------+------+------+     **/
/**           其中WEL由写使能所存器决定，WPEN、BP1、BP0可以设定。            **/
/**           BP1和BP0决定保护区域与非保护区域的大小：                       **/
/**             +-----+-----+------------------+                             **/
/**             | BP1 | BP0 |  被保护区域范围  |                             **/
/**             +-----+-----+------------------+                             **/
/**             |  0  |  0  | None             |                             **/
/**             +-----+-----+------------------+                             **/
/**             |  0  |  1  | 1800h-1FFFh(1/4) |                             **/
/**             +-----+-----+------------------+                             **/
/**             |  1  |  0  | 1000h-1FFFh(1/2) |                             **/
/**             +-----+-----+------------------+                             **/
/**             |  1  |  1  | 0000h-1FFFh(全部)|                             **/
/**             +-----+-----+------------------+                             **/
/**           FM25Cxx系列存储器采用多重写保护，关系如下：                    **/
/**           +-----+------+----+----------+----------+----------+           **/
/**           | WEL | WPEN | WP |被保护区域|不保护区域|状态寄存器|           **/
/**           +-----+------+----+----------+----------+----------+           **/
/**           |  0  |  X   | X  |  被保护  |  被保护  |  被保护  |           **/
/**           +-----+------+----+----------+----------+----------+           **/
/**           |  1  |  0   | X  |  被保护  |  不保护  |  不保护  |           **/
/**           +-----+------+----+----------+----------+----------+           **/
/**           |  1  |  1   | 0  |  被保护  |  不保护  |  被保护  |           **/
/**           +-----+------+----+----------+----------+----------+           **/
/**           |  1  |  1   | 1  |  被保护  |  不保护  |  不保护  |           **/
/**           +-----+------+----+----------+----------+----------+           **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-07-17          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "fm25xxxfunction.h"

/*缺省片选处理函数*/
static void FM25ChipSelectDefault(FM25CSType cs);

/*从FM25XXX读取单个字节,从随机地址读取*/
uint8_t ReadByteFromFM25xxx(FM25ObjectType *fram,uint32_t regAddress)
{
  uint8_t rData;

  ReadBytesFromFM25xxx(fram,regAddress,&rData,1);
	  
  return rData;
}

/*向FM25XXX写入单个字节*/
void WriteByteToFM25xxx(FM25ObjectType *fram,uint32_t regAddress,uint8_t data)
{
  WriteBytesToFM25xxx(fram,regAddress,&data,1);
}

/* 设置写使能所存器*/
void SetWriteEnableLatchForFM25xxx(FM25ObjectType *fram)
{
  uint8_t opCode=FM25_WREN;
  
  fram->ChipSelect(FM25CS_Enable);
  fram->Write(&opCode,1);
  fram->ChipSelect(FM25CS_Enable);
  
  ReadStatusForFM25xxx(fram);
}

/* 复位写使能所存器*/
void ResetWriteEnableLatchForFM25xxx(FM25ObjectType *fram)
{
  uint8_t opCode=FM25_WRDI;
  
  fram->ChipSelect(FM25CS_Enable);
  fram->Write(&opCode,1);
  fram->ChipSelect(FM25CS_Enable);
  
  ReadStatusForFM25xxx(fram);
}

/*读FM25xxx状态寄存器*/
void ReadStatusForFM25xxx(FM25ObjectType *fram)
{
  uint8_t opCode=FM25_RDSR;
  uint8_t status;
  
  fram->ChipSelect(FM25CS_Enable);
  fram->Write(&opCode,1);
  fram->Delayms(1);
  fram->Read(&status,1);
  fram->ChipSelect(FM25CS_Enable);
  
  fram->status=status;
}

/*写FM25xxx状态寄存器*/
void WriteStatusForFM25xx(FM25ObjectType *fram,uint8_t cmd)
{
  uint8_t data[2];
	
  data[0]=FM25_WRSR;
  data[1]=cmd;
	
  if(((fram->status)&0x02)!=0x02)
  {
    SetWriteEnableLatchForFM25xxx(fram);
  }
	
  if((((fram->status)&FM25_WPEN)!=FM25_WPEN)&&(fram->WP!=NULL))
  {
    fram->WP(FM25WP_Disable);
  }
	
  fram->ChipSelect(FM25CS_Enable);
  fram->Write(data,2);
  fram->ChipSelect(FM25CS_Disable);
  
  ReadStatusForFM25xxx(fram);
  
  if(fram->WP!=NULL)
  {
    fram->WP(FM25WP_Enable);
  }
}

/*从FM25xxx读取数据*/
void ReadBytesFromFM25xxx(FM25ObjectType *fram,uint32_t regAddress,uint8_t *rData,uint16_t rSize)
{
  uint8_t data[4];
  uint16_t index=0;
  uint8_t temp;

  data[index++]=FM25_READ;
	
  if(fram->memAddLength==FM258BitMemAdd)
  {
    data[index++]=(uint8_t)regAddress;
		
    if((fram->mode==FM25L04B)||(fram->mode==FM25040B))
    {
      temp=(uint8_t)(regAddress>>8);
      data[0]|=((temp&0x01)<<3);
    }
  }
  else if(fram->memAddLength==FM2516BitMemAdd)
  {
    data[index++]=(uint8_t)(regAddress>>8);
    data[index++]=(uint8_t)regAddress;
  }
  else
  {
    data[index++]=(uint8_t)(regAddress>>16);
    data[index++]=(uint8_t)(regAddress>>8);
    data[index++]=(uint8_t)regAddress;
  }
	
  fram->ChipSelect(FM25CS_Enable);
  fram->Write(data,index);
  fram->Delayms(1);
  fram->Read(rData,rSize);
  fram->ChipSelect(FM25CS_Disable);
}

/*向FM25xxx写入数据*/
void WriteBytesToFM25xxx(FM25ObjectType *fram,uint32_t regAddress,uint8_t *wData,uint16_t wSize)
{
  uint8_t data[128];
  uint8_t temp;
  uint16_t index=0;
	
  data[index++]=FM25_WRITE;
	
  if(fram->memAddLength==FM258BitMemAdd)
  {
    data[index++]=(uint8_t)regAddress;
		
    if((fram->mode==FM25L04B)||(fram->mode==FM25040B))
    {
      temp=(uint8_t)(regAddress>>8);
      data[0]|=((temp&0x01)<<3);
    }
  }
  else if(fram->memAddLength==FM2516BitMemAdd)
  {
    data[index++]=(uint8_t)(regAddress>>8);
    data[index++]=(uint8_t)regAddress;
  }
  else
  {
    data[index++]=(uint8_t)(regAddress>>16);
    data[index++]=(uint8_t)(regAddress>>8);
    data[index++]=(uint8_t)regAddress;
  }
	
  for(int i=0;i<wSize;i++)
  {
    data[index++]=wData[i];
  }
	
  if(((fram->status)&0x02)!=0x02)
  {
    SetWriteEnableLatchForFM25xxx(fram);
  }
	
  if(((fram->status)&0x0C)!=0x00)
  {
    WriteStatusForFM25xx(fram,fram->status|FM25_BPNONE);
  }
	
  fram->ChipSelect(FM25CS_Enable);
  fram->Write(data,index);
  fram->ChipSelect(FM25CS_Disable);
  
  WriteStatusForFM25xx(fram,fram->status|FM25_BPALL);
}

/*FM25对象初始化*/
void Fm25cxxInitialization(FM25ObjectType *fram,        //FM25xxx对象实体
                           FM25ModeType mode,          //设备类型
                           Fm25Read read,              //读FM25xxx对象操作指针
                           Fm25Write write,            //写FM25xxx对象操作指针
                           Fm25Delayms delayms,        //延时操作指针
                           Fm25WP wp,                  //写保护操作函数指针
                           Fm25ChipSelect cs,          //片选信号函数指针
                           Fm25Hold hold               //保持信号操作函数指针
                          )
{
  if((fram==NULL)||(read==NULL)||(write==NULL)||(delayms==NULL))
  {
    return;
  }
  fram->Read=read;
  fram->Write=write;
  fram->Delayms=delayms;
	
  if(cs!=NULL)
  {
    fram->ChipSelect=cs;
  }
  else
  {
    fram->ChipSelect=FM25ChipSelectDefault;
  }

  if(mode>=FM25Number)
  {
    return;
  }
  fram->mode=mode;
  
  if(mode<FM25C160B)
  {
    fram->memAddLength=FM258BitMemAdd;
  }
  else if(mode<FM25V10)
  {
    fram->memAddLength=FM2516BitMemAdd;
  }
  else
  {
    fram->memAddLength=FM2524BitMemAdd;
  }
	
  ReadStatusForFM25xxx(fram);
	
  //写允许
  SetWriteEnableLatchForFM25xxx(fram);
	
  uint8_t cmd;
  //使能写保护，保护全部区域
  cmd=fram->status|FM25_WPEN|FM25_BPALL;
  WriteStatusForFM25xx(fram,cmd);
  
  ReadStatusForFM25xxx(fram);
}

/*缺省片选处理函数*/
static void FM25ChipSelectDefault(FM25CSType cs)
{
  //用于在SPI通讯时，片选信号硬件电路选中的情况
  return;
}
/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
