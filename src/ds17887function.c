/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ds17887function.h                                              **/
/** 版    本：V1.1.0                                                         **/
/** 简    介：实现实时时钟模块DS17887的操作                                  **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2012-07-28          木南              创建文件               **/
/**     V1.1.0  2019-08-07          木南              修改为机遇对象的操作   **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "ds17887function.h"

/*从DS17887读数据*/
static uint8_t ReadDataFromDS17887(Ds17887ObjectType *ds17887,uint16_t address);
/*向DS17887写数据*/
static void WriteDataToDS17887(Ds17887ObjectType *ds17887,uint16_t address,uint16_t data);

/*从实时时钟模块读取时间*/
void GetDateTimeFromDs17887(Ds17887ObjectType *ds17887)
{
  /*读取系统时间值*/
  ds17887->dateTime[0]=ReadDataFromDS17887(ds17887,DS17887_Year);//系统时间年
  ds17887->Delayus(5);
  ds17887->dateTime[1]=ReadDataFromDS17887(ds17887,DS17887_Month);//系统时间月
  ds17887->Delayus(5);
  ds17887->dateTime[2]=ReadDataFromDS17887(ds17887,DS17887_Date);//系统时间日
  ds17887->Delayus(5);
  ds17887->dateTime[3]=ReadDataFromDS17887(ds17887,DS17887_Hour);//系统时间时
  ds17887->Delayus(5);
  ds17887->dateTime[4]=ReadDataFromDS17887(ds17887,DS17887_Minute);//系统时间分
  ds17887->Delayus(5);
  ds17887->dateTime[5]=ReadDataFromDS17887(ds17887,DS17887_Second);//系统时间秒
  ds17887->Delayus(5);
	
  /*读取控制寄存器*/
  ds17887->ctlReg[Reg_A]=ReadDataFromDS17887(ds17887,DS17887_Reg_A);
  ds17887->Delayus(5);
  ds17887->ctlReg[Reg_B]=ReadDataFromDS17887(ds17887,DS17887_Reg_B);
  ds17887->Delayus(5);
  ds17887->ctlReg[Reg_C]=ReadDataFromDS17887(ds17887,DS17887_Reg_C);
  ds17887->Delayus(5);
  ds17887->ctlReg[Reg_D]=ReadDataFromDS17887(ds17887,DS17887_Reg_D);
  ds17887->Delayus(5);
}

/*校准DS17887的时间*/
void CalibrationDs17887DateTime(Ds17887ObjectType *ds17887,uint16_t * dateTime)
{
  /*将ALE、RD与WR复位*/
  ds17887->SetCtlPin[DS17887_ALE](DS17887_Reset);
  ds17887->SetCtlPin[DS17887_WR](DS17887_Reset);
  ds17887->SetCtlPin[DS17887_RD](DS17887_Reset);
  
  /*初始化控制寄存器，以便校准时间*/
  WriteDataToDS17887(ds17887,DS17887_Reg_A,0x20);
  WriteDataToDS17887(ds17887,DS17887_Reg_B,0x06);
  WriteDataToDS17887(ds17887,DS17887_Reg_B,0x80);
  
  /*设置系统时间值*/
  WriteDataToDS17887(ds17887,DS17887_Year,dateTime[0]);//系统时间年
  WriteDataToDS17887(ds17887,DS17887_Month,dateTime[1]);//系统时间月
  WriteDataToDS17887(ds17887,DS17887_Date,dateTime[2]);//系统时间日
  WriteDataToDS17887(ds17887,DS17887_Hour,dateTime[3]);//系统时间时
  WriteDataToDS17887(ds17887,DS17887_Minute,dateTime[4]);//系统时间分
  WriteDataToDS17887(ds17887,DS17887_Second,dateTime[5]);//系统时间秒
  
  /*设置寄存器B和A的值，启动DS17887*/
  WriteDataToDS17887(ds17887,DS17887_Reg_B,0x06);
  WriteDataToDS17887(ds17887,DS17887_Reg_A,0x20);
	
  //读取DS17887的时间
  GetDateTimeFromDs17887(ds17887);
}

/*对DS17887进行初始化配置*/
void Ds17887Initialization(Ds17887ObjectType *ds17887,
                           DS17887CtlPinOperationType *SetCtlPin,
                           WriteByteToDs17887Type WriteByte,
                           ReadByteFromDs17887Type ReadByte,
                           Ds17887SetBusDirectionType SetBusDirection,
                           Ds17887DelayusType Delayus)
{
  if((ds17887==NULL)||(SetCtlPin==NULL)||(WriteByte==NULL)||(ReadByte==NULL)||(SetBusDirection==NULL)||(Delayus==NULL))
  {
    return;
  }
	
  ds17887->ctlReg[Reg_A]=0x00;
  ds17887->ctlReg[Reg_B]=0x00;
  ds17887->ctlReg[Reg_C]=0x00;
  ds17887->ctlReg[Reg_D]=0x00;
	
  for(int i=0;i<6;i++)
  {
    ds17887->dateTime[0]=0;
    ds17887->SetCtlPin[i]=SetCtlPin[i];
  }
  ds17887->WriteByte=WriteByte;
  ds17887->ReadByte=ReadByte;
  ds17887->SetBusDirection=SetBusDirection;
  ds17887->Delayus=Delayus;
	
  /*将ALE、RD与WR复位*/
  SetCtlPin[DS17887_ALE](DS17887_Reset);
  SetCtlPin[DS17887_WR](DS17887_Reset);
  SetCtlPin[DS17887_RD](DS17887_Reset);
  
  /*设置寄存器B和A的值，启动DS17887*/
  WriteDataToDS17887(ds17887,DS17887_Reg_B,0x06);
  WriteDataToDS17887(ds17887,DS17887_Reg_A,0x20);
	
  //读取DS17887的时间
  GetDateTimeFromDs17887(ds17887);
}

/*从DS17887读数据*/
static uint8_t ReadDataFromDS17887(Ds17887ObjectType *ds17887,uint16_t address)
{
  /*将片选信号置位，失能片选*/
  ds17887->SetCtlPin[DS17887_CS](DS17887_Set);
  /*将RD与WR置位*/
  ds17887->SetCtlPin[DS17887_WR](DS17887_Set);
  ds17887->SetCtlPin[DS17887_RD](DS17887_Set);
  ds17887->Delayus(2);
  /*置位ALE*/
  ds17887->SetCtlPin[DS17887_ALE](DS17887_Set);
  
  /*将地址数据总线的模式改为输出*/
  ds17887->SetBusDirection(DS17887_Out);
  /*写寄存器地址*/
  ds17887->WriteByte(address);
  /*将片选信号置位，使能片选*/
  ds17887->SetCtlPin[DS17887_CS](DS17887_Reset);
  ds17887->Delayus(2);
  /*复位ALE*/
  ds17887->SetCtlPin[DS17887_ALE](DS17887_Reset);
  ds17887->Delayus(2);
  /*复位RD*/
  ds17887->SetCtlPin[DS17887_RD](DS17887_Reset);
  ds17887->Delayus(10);
  
  /*将地址数据总线的模式改为输入*/
  ds17887->SetBusDirection(DS17887_In);
  ds17887->Delayus(40);
  /*读取数据*/
  uint16_t readData=0;
  readData=ds17887->ReadByte();
  ds17887->Delayus(4);
  /* 将RD置位，并将CS信号置位，失能芯片 */
  ds17887->SetCtlPin[DS17887_RD](DS17887_Set);
  ds17887->SetCtlPin[DS17887_CS](DS17887_Set);
  ds17887->Delayus(4);
  /*将ALE置位*/
  ds17887->SetCtlPin[DS17887_ALE](DS17887_Set);
  ds17887->Delayus(20);
  return readData;
}

/*向DS17887写数据*/
static void WriteDataToDS17887(Ds17887ObjectType *ds17887,uint16_t address,uint16_t data)
{
  /*将DS17887的片选信号失能*/
  ds17887->SetCtlPin[DS17887_CS](DS17887_Set);
  /*将RD与WR置位*/
  ds17887->SetCtlPin[DS17887_WR](DS17887_Set);
  ds17887->SetCtlPin[DS17887_RD](DS17887_Set);
  ds17887->Delayus(2);
  /*将ALE信号置高*/
  ds17887->SetCtlPin[DS17887_ALE](DS17887_Set);
  
  /*将地址数据总线的模式改为输出*/
  ds17887->SetBusDirection(DS17887_Out);
  /*写寄存器地址*/
  ds17887->WriteByte(address);
  /*将片选信号置位，使能片选*/
  ds17887->SetCtlPin[DS17887_CS](DS17887_Reset);
  ds17887->Delayus(4);
  /*复位ALE信号*/
  ds17887->SetCtlPin[DS17887_ALE](DS17887_Reset);
  ds17887->Delayus(4);
  /*复位WR*/
  ds17887->SetCtlPin[DS17887_WR](DS17887_Reset);
  /*写数据*/
  ds17887->WriteByte(data);
  ds17887->Delayus(4);
  
  /* 将WR置位，并将CS信号置位，失能芯片 */
  ds17887->SetCtlPin[DS17887_WR](DS17887_Set);
  ds17887->SetCtlPin[DS17887_CS](DS17887_Set);  
  ds17887->Delayus(4);
  /*将ALE置位*/
  ds17887->SetCtlPin[DS17887_ALE](DS17887_Set);
  ds17887->Delayus(10);
}

/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
