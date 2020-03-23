/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ad7192function.c                                               **/
/** 版    本：V1.0.2                                                         **/
/** 简    介：声明和定义AD7192模数转换芯片相关的各种函数和操作               **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-07-26          木南              创建文件               **/
/**     V1.0.1  2017-12-06          木南              将寄存器数组变为参数   **/
/**     V1.0.2  2018-05-09          木南              添加操作延时函数       **/
/**     V2.0.0  2018-05-23          木南              添加操作对象，并修改全 **/
/**                                                   部相关函数的形参       **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "ad7192function.h"

/*     通讯寄存器8位，只写,RS2,RS1,RS0=0,0,0;                                 */
/*     通讯寄存器位功能操作宏定义                                             */
/*     +-------+-------+-------+-------+-------+--------+-------+-------+     */
/*     |  CR7  |  CR6  |  CR5  |  CR4  |  CR3  |  CR2   |  CR1  |  CR0  |     */
/*     +-------+-------+-------+-------+-------+--------+-------+-------+     */
/*     | WEN(0)| R/W(0)| RS2(0)| RS1(0)| RS0(0)|CREAD(0)|  0(0) |  0(0) |     */
/*     +-------+-------+-------+-------+-------+--------+-------+-------+     */
#define	WEN             0x00    //WEN位为0时，允许继续写通讯寄存器
#define WEN_DIS         0x80    //WEN位为1时，禁止继续写通讯寄存器
#define RW_W            0x00    //R/W位为0时，下一操作为写指定寄存器
#define	RW_R            0x40    //R/W位为1时，下一操作读写指定寄存器
#define REG_COM_STA     0x00    //读为状态寄存器，写为通讯寄存器*/
#define	REG_MODE        0x08    //操作模式寄存器
#define	REG_CONF        0x10    //操作配置寄存器
#define	REG_DATA        0x18    //操作数据寄存器
#define	REG_ID          0x20    //操作ID寄存器
#define	REG_GPOCON      0x28    //操作GPOCON寄存器
#define	REG_OFFSET      0x30    //操作失调寄存器
#define	REG_FS          0x38    //操作满量程寄存器
#define	CREAD_EN        0x04    //连续读取
#define	CREAD_DIS       0x00    //单次读取

/*     状态寄存器8位，只读,RS2,RS1,RS0=0,0,0;上电/复位=0x80                   */
/*     状态寄存器位功能取值宏定义                                             */
/*     +-------+-------+--------+---------+-------+-------+-------+-------+   */
/*     |  SR7  |  SR6  |  SR5   |  SR4    |  SR3  |  SR2  |  SR1  |  SR0  |   */
/*     +-------+-------+--------+---------+-------+-------+-------+-------+   */
/*     | RDY(1)| ERR(0)|NOREF(0)|PARITY(0)|  0(0) |CHD2(0)|CHD1(0)|CHD0(0)|   */
/*     +-------+-------+--------+---------+-------+-------+-------+-------+   */
#define	RDY_H			0x80    /*数据未就绪*/
#define	RDY_L			0x00    /*数据准备就绪*/
#define	ERR_H			0x40    /*结果错误*/
#define	ERR_L			0x00    /*结果正确*/
#define	NOREF_H			0x20    /*基准源故障*/
#define	NOREF_L			0x00    /*基准源正确*/
#define	PARITY_H		0x10    /*奇数个1*/
#define	PARITY_L		0x00    /*偶数个1*/
#define	CHDST_AIN1_AIN2         0x00    /*对应通道为第一路差分输入*/
#define	CHDST_AIN3_AIN4         0x01    /*对应通道为第二路差分输入*/
#define	CHDST_TEMP		0x02    /*对应通道为温度输入*/
#define	CHDST_AIN2_AIN2         0x03    /*对应通道为第二路输入*/
#define	CHDST_AIN1_COM          0x04    /*对应通道为第一路单端输入*/
#define	CHDST_AIN2_COM          0x05    /*对应通道为第二路单端输入*/
#define	CHDST_AIN3_COM          0x06    /*对应通道为第三路单端输入*/
#define	CHDST_AIN4_COM          0x07    /*对应通道为第四路单端输入*/

/*   模式寄存器24位，可读可写,RS2,RS1,RS0=0,0,1;上电/复位=0x080060            */
/*   模式寄存器位功能操作宏定义                                               */
/*   +--------+------+--------+----------+---------+--------+------+------+   */
/*   |  MR23  | MR22 |  MR21  |  MR20    |   MR19  |  MR18  | MR17 | MR16 |   */
/*   +--------+------+--------+----------+---------+--------+------+------+   */
/*   | MD2(0) |MD1(0)| MD0(0) |DAT_STA(0)| CLK1(0) | CLK0(0)| 0(0) | 0(0) |   */
/*   +--------+------+--------+----------+---------+--------+------+------+   */
/*   |  MR15  | MR14 |  MR13  |  MR12    |   MR11  |  MR10  | MR9  | MR8  |   */
/*   +--------+------+--------+----------+---------+--------+------+------+   */
/*   |SINC3(0)| 0(0) |ENPAR(0)|CLK_DIV(0)|SINGLE(0)|REJ60(0)|FS9(0)|FS8(0)|   */
/*   +--------+------+--------+----------+---------+--------+------+------+   */
/*   |  MR7   | MR6  |  MR5   |  MR4     |   MR3   |   MR2  | MR1  | MR0  |   */
/*   +--------+------+--------+----------+---------+--------+------+------+   */
/*   | FS7(0) |FS6(0)| FS5(0) |  FS4(0)  |  FS3(0) | FS2(0) |FS1(0)|FS0(0)|   */
/*   +--------+------+--------+----------+---------+--------+------+------+   */
#define	MODE_CONT               0x000000        /*连续转换模式（默认）*/
#define	MODE_SING               0x200000        /*单次转换模式*/
#define	MODE_IDLE               0x400000        /*空闲模式*/
#define	MODE_PD                 0x600000        /*关断模式*/
#define	MODE_INZCL              0x800000        /*内部零电平校准*/
#define	MODE_INFCL              0xA00000        /*内部满量程校准*/
#define	MODE_SYSZCL             0xC00000        /*系统零电平校准*/
#define	MODE_SYSFCL             0xE00000        /*系统满量程校准*/
#define	DAT_STA_EN              0x100000        /*状态与数据寄存器同传*/
#define	DAT_STA_DIS             0x000000        /*状态与数据寄存器不同传*/	  
#define	EXT_XTAL                0x000000        /*外部晶振*/
#define	EXT_CLK                 0x040000        /*外部时钟*/
#define	INCLK_MCLK2TRI          0x080000        /*内部时钟*/
#define	INCLK_MCLK2EN           0x0C0000        /*内部时钟，输出*/
#define	SINC_4                  0x000000        /*使用sinc4滤波*/
#define	SINC_3                  0x008000        /*使用sinc3滤波*/
#define	ENPAR_EN                0x002000        /*使能奇偶校验*/
#define	ENPAR_DIS               0x000000        /*禁止奇偶校验*/
#define	CLK_DIV_2               0x001000        /*主时钟2分频*/
#define	CLK_DIV_DIS             0x000000        /*主时钟不分频*/
#define	SINGLECYCLE_EN          0x000800        /*单周期转换使能*/
#define	SINGLECYCLE_DIS         0x000000        /*单周期转换失能*/
#define	REJ60_EN                0x000400        /*使能60Hz陷波频率*/
#define	REJ60_DIS               0x000000        /*失能60Hz陷波频率*/
										
/*    配置寄存器，24位，可读可写,RS2,RS1,RS0=0,1,0;上电/复位=0x000117         */
/*    配置寄存器位功能操作宏定义                                              */
/*    +-------+---------+------+---------+-------+-------+-------+-------+    */
/*    | CON23 |  CON22  | CON21|  CON20  | CON19 | CON18 | CON17 | CON16 |    */
/*    +-------+---------+------+---------+-------+-------+-------+-------+    */
/*    |CHOP(0)|   0(0)  | 0(0) |REFSEL(0)|  0(0) |  0(0) |  0(0) |  0(0) |    */
/*    +-------+---------+------+---------+-------+-------+-------+-------+    */
/*    | CON15 |  CON14  | CON13|  CON12  | CON11 | CON10 | CON9  | CON8  |    */
/*    +-------+---------+------+---------+-------+-------+-------+-------+    */
/*    | CH7(0)|  CH6(0) |CH5(0)|  CH4(0) | CH3(0)| CH2(0)| CH1(0)| CH0(0)|    */
/*    +-------+---------+------+---------+-------+-------+-------+-------+    */
/*    | CON7  |   CON6  | CON5 |  CON4   | CON3  |  CON2 | CON1  | CON0  |    */
/*    +-------+---------+------+---------+-------+-------+-------+-------+    */
/*    |BURN(0)|REFDET(0)| 0(0) |  BUF(0) | U/B(0)| G2(0) | G1(0) | G0(0) |    */
/*    +-------+---------+------+---------+-------+-------+-------+-------+    */			
#define CHOP_EN                 0x800000        /*斩波使能*/
#define	CHOP_DIS                0x000000        /*斩波禁用*/
#define	REF_IN1                 0x000000        /*基准电压输入1*/
#define	REF_IN2                 0x100000        /*基准电压输入2*/
#define	AIN1_AIN2               0x000100        /*差分通道1*/
#define	AIN3_AIN4               0x000200        /*差分通道2*/
#define	TEMP                    0x000400        /*温度*/
#define	AIN2_AIN2               0x000800        /*模拟通道2*/
#define	AIN1_COM                0x001000        /*单端输入1*/
#define	AIN2_COM                0x002000        /*单端输入2*/
#define	AIN3_COM                0x004000        /*单端输入3*/
#define	AIN4_COM                0x008000        /*单端输入4*/
#define	BURN_EN                 0x000080        /*使能激励电流*/
#define	BURN_DIS                0x000000        /*禁用激励电流*/
#define	REFDET_EN               0x000040        /*使能基准电压检测*/
#define	REFDET_DIS              0x000000        /*禁用基准电压检测*/
#define	BUF_EN                  0x000010        /*使能模拟输入端的缓冲器*/
#define	BUF_DIS                 0x000000        /*禁用模拟输入端的缓冲器*/
#define	UB_UNI                  0x000008        /*单极性工作模式*/
#define UB_BI                   0x000000        /*双极性工作模式*/
#define	GAIN_1                  0x000000        /*1倍增益*/
#define	GAIN_8                  0x000003        /*8倍增益*/
#define	GAIN_16                 0x000004        /*16倍增益*/
#define	GAIN_32                 0x000005        /*32倍增益*/
#define	GAIN_64                 0x000006        /*64倍增益*/
#define	GAIN_128                0x000007        /*128倍增益*/

/*    GPOCON寄存器，8位，只读,RS2,RS1,RS0=1,0,1;上电/复位=0x00                */
/*    数据寄存器位功能操作宏定义                                              */
/*   +----+--------+---------+---------+--------+--------+--------+--------+  */
/*   | GP7|   GP6  |   GP5   |   GP4   |   GP3  |   GP2  |   GP1  |  GP0   |  */
/*   +----+--------+---------+---------+--------+--------+--------+--------+  */
/*   |0(0)|BPDSW(0)|GP32EN(0)|GP10EN(0)|P3DAT(0)|P2DAT(0)|P1DAT(0)|P0DAT(0)|  */
/*   +----+--------+---------+---------+--------+--------+--------+--------+  */
#define BPDSW_CLOSE             0x40    /*闭合电桥关断开关*/
#define	BPDSW_OPEN              0x00    /*断开电桥关断开关*/
#define	GP32EN                  0x20    /*数字输出P3和P2为有效*/
#define	GP32DIS                 0x00    /*数字输出P3和P2被忽略*/
#define	GP10EN                  0x10    /*数字输出P1和P0为有效*/
#define	GP10DIS                 0x00    /*数字输出P1和P0被忽略*/
#define	P3DAT_H                 0x08    /*设置P3为高电平*/
#define	P3DAT_L                 0x00    /*设置P3为低电平*/
#define	P2DAT_H                 0x04    /*设置P2为高电平*/
#define	P2DAT_L                 0x00    /*设置P2为低电平*/
#define	P1DAT_H                 0x02    /*设置P1为高电平*/
#define	P1DAT_L                 0x00    /*设置P1为低电平*/
#define	P0DAT_H                 0x01    /*设置P0为高电平*/
#define	P0DAT_L                 0x00    /*设置P0为低电平*/

#define	NOP                     0x00    /*无操作*/

uint8_t regAdd[]={REG_COM_STA,REG_MODE,REG_CONF,REG_DATA,REG_ID,REG_GPOCON,REG_OFFSET,REG_FS};

/*AD7192软件复位，连续发送40个1就会复位*/
static void AD7192SoftwareReset(Ad7192ObjectType *adObj);
/*写AD7192寄存器值*/
static void WriteAD7192Register(Ad7192ObjectType *adObj,AD7192RegisterType startReg,uint8_t NumberOfRegistersToWrite);
/*读AD7192寄存器值*/
static void ReadAD7192Register(Ad7192ObjectType *adObj,AD7192RegisterType startReg,uint8_t NumberOfRegistersToRead);
/*启动连续读*/
//static void AD7192StartContinuousRead(Ad7192ObjectType *adObj);
/*连续读取*/
//static uint32_t AD7192ContinuousRead(Ad7192ObjectType *adObj);
/*读取转换数据*/
static uint32_t AD7192ReadConvertingData(Ad7192ObjectType *adObj);
/*退出连续读*/
//static void AD7192ExitContinuousRead(Ad7192ObjectType *adObj);
/*内部零点校准*/
static void AD7192InternalZeroScaleCalibration(Ad7192ObjectType *adObj,uint32_t Channels);
/*内部量程校准*/
static void AD7192InternalFullScaleCalibration(Ad7192ObjectType *adObj,uint32_t Channels);
/*外部零点校准*/
static void AD7192ExternalZeroScaleCalibration(Ad7192ObjectType *adObj,uint32_t Channels);
/*外部量程校准*/
static void AD7192ExternalFullScaleCalibration(Ad7192ObjectType *adObj,uint32_t Channels);
/*启动单次转换*/
static void AD7192StartSingleConvertion(Ad7192ObjectType *adObj,uint32_t Channels);
/*启动连续转换*/
static void AD7192StartContinuousConvertion(Ad7192ObjectType *adObj,uint32_t Channels);
/* 默认的片选信号处理函数 */
static void AD719xChipSelect(AD7192CSType cs);

/*连续转换数据获取,dataCodes为8个元素的数组对应8个通道*/
void GettContinuousConvertionValue(Ad7192ObjectType *adObj,uint32_t Channels,uint32_t *dataCodes,int number)
{
  uint32_t dataCode=0;
  uint8_t status=255;
  AD7192StartContinuousConvertion(adObj,Channels&0x00FF00);
  
  for(int i=0;i<number;i++)
  {
    dataCode = AD7192ReadConvertingData(adObj);
    status=((uint8_t)dataCode)&0x07;
    dataCode =(dataCode>>8) & 0x00FFFFFF;
    dataCodes[status]=dataCode;
  }
}

/*单次转换数据获取*/
uint32_t GetSingleConvertionValue(Ad7192ObjectType *adObj,uint32_t Channels)
{
  uint32_t dataCode=0;
  AD7192StartSingleConvertion(adObj,Channels&0x00FF00);
  
  adObj->Delay(1);
  
  dataCode = AD7192ReadConvertingData(adObj);
  dataCode =dataCode & 0x00FFFFFF;

  ReadAD7192Register(adObj,AD7192_REG_DATA, 1);
  
  return dataCode;
}

/*AD7192初始化配置*/
AD7192ErrorType AD7192Initialization(Ad7192ObjectType *adObj,           //AD7192对象
                                     uint32_t Channels,                 //通道
                                     AD7192PolarType polar,             //极性
                                     AD7192GainType gain,               //增益
                                     AD7192CalibrationType cali,        //校准方式
                                     uint16_t fs,                       //数据输出速率设定：1-1023，数越大频率越低
                                     AD7192ReadWriteType readWrite,     //读写函数指针
                                     AD7192ChipSelectType cs,           //片选函数指针
                                     AD7192GetReadyInputType ready,     //就绪函数指针
                                     AD7192DelaymsType delayms          //延时函数指针
                                    )
{
  uint32_t polarity[]={UB_UNI,UB_BI};
  uint32_t gains[]={GAIN_1,GAIN_8,GAIN_16,GAIN_32,GAIN_64,GAIN_128};
  
  if((adObj==NULL)||(readWrite==NULL)||(ready==NULL)||(delayms==NULL))
  {
    return AD7192_InitError;
  }
  adObj->ReadWrite=readWrite;
  adObj->GetReadyInput=ready;
  adObj->Delay=delayms;
	
  if(cs==NULL)
  {
    adObj->ChipSelect=AD719xChipSelect;
  }
  else
  {
    adObj->ChipSelect=cs;
  }
  
  for(int i=0;i<AD7192RegisterNumber;i++)
  {
    adObj->Registers[i]=0x00;
  }
	
  adObj->polar=polarity[polar];
  adObj->gain=gains[gain];
  adObj->fs=fs>1023?1023:(fs<1?1:fs);
		
  AD7192SoftwareReset(adObj);
  adObj->Delay(1);
	
  if(cali==AD7192_Internal)
  {
    AD7192InternalZeroScaleCalibration(adObj,Channels&0x00FF00);
    adObj->Delay(1);
    AD7192InternalFullScaleCalibration(adObj,Channels&0x00FF00);
  }
  else
  {
    AD7192ExternalZeroScaleCalibration(adObj,Channels&0x00FF00);
    adObj->Delay(1);
    AD7192ExternalFullScaleCalibration(adObj,Channels&0x00FF00);
  }
		
  /*读取并存储全部寄存器的值*/
  ReadAD7192Register(adObj,AD7192_REG_COM_STA, 8);

  return AD7192_OK;
}

/*AD7192软件复位，连续发送40个1就会复位*/
static void AD7192SoftwareReset(Ad7192ObjectType *adObj)
{
  uint8_t WriteBuf[1];
  uint8_t ReadBuf[1];
  uint8_t i;
  
  adObj->ChipSelect(AD7192CS_Enable);
  
  WriteBuf[0]	= 0xFF;	 
  
  for(i=0; i<5; i++)
  {
    adObj->ReadWrite(WriteBuf, ReadBuf, 1);
  }
  
  adObj->ChipSelect(AD7192CS_Disable);
}

/*写AD7192寄存器值*/
static void WriteAD7192Register(Ad7192ObjectType *adObj,AD7192RegisterType startReg,uint8_t NumberOfRegistersToWrite)
{
  uint8_t WriteBuf[4];
  uint8_t ReadBuf[4];
  uint8_t i;

  adObj->ChipSelect(AD7192CS_Enable);
  
  for(i=0; i<NumberOfRegistersToWrite; i++)
  {
    WriteBuf[0] = WEN|RW_W|(regAdd[startReg + i])|CREAD_DIS;
    WriteBuf[1] = adObj->Registers[startReg + i]>>16;
    WriteBuf[2] = adObj->Registers[startReg + i]>>8;
    WriteBuf[3] = adObj->Registers[startReg + i];
    adObj->ReadWrite(WriteBuf, ReadBuf, 4);
  }
  
  adObj->ChipSelect(AD7192CS_Disable);
}

/*读AD7192寄存器值*/
static void ReadAD7192Register(Ad7192ObjectType *adObj,AD7192RegisterType startReg,uint8_t NumberOfRegistersToRead)
{
  uint8_t WriteBuf[4];
  uint8_t ReadBuf[4];
  uint8_t i;

  adObj->ChipSelect(AD7192CS_Enable);
  
  for(i=0; i < NumberOfRegistersToRead; i++)
  {
    //写通讯寄存器，下一操作为读
    WriteBuf[0] = WEN|RW_R|(regAdd[startReg + i])|CREAD_DIS;	 
    adObj->ReadWrite(WriteBuf, ReadBuf, 1);

    WriteBuf[0] = NOP;
    WriteBuf[1] = NOP;
    WriteBuf[2] = NOP;
    WriteBuf[3] = NOP;

    switch(regAdd[startReg + i])
    {

      case REG_ID       :
      case REG_COM_STA  : 
      case REG_GPOCON   :
        {
          adObj->ReadWrite(WriteBuf, ReadBuf, 1);
          adObj->Registers[startReg + i ] = ReadBuf[0];
          break;
        }
      case REG_MODE     : 
      case REG_CONF     : 
      case REG_OFFSET   :
      case REG_FS       : 
        {
          adObj->ReadWrite(WriteBuf, ReadBuf, 3);	   
          adObj->Registers[startReg + i ] = ReadBuf[0];
          adObj->Registers[startReg + i ] = (adObj->Registers[startReg + i ]<<8) + ReadBuf[1];
          adObj->Registers[startReg + i ] = (adObj->Registers[startReg + i ]<<8) + ReadBuf[2];  
          break;
        }
      case REG_DATA	 : 
        {
          if (adObj->Registers[AD7192_REG_MODE] & DAT_STA_EN)
          {
            adObj->ReadWrite(WriteBuf, ReadBuf, 4);	  
            adObj->Registers[startReg + i ] = ReadBuf[0];
            adObj->Registers[startReg + i ] = (adObj->Registers[startReg + i ]<<8) + ReadBuf[1];
            adObj->Registers[startReg + i ] = (adObj->Registers[startReg + i ]<<8) + ReadBuf[2];					
            adObj->Registers[startReg + i ] = (adObj->Registers[startReg + i ]<<8) + ReadBuf[3];
            break;
          }
          else
          {
            adObj->ReadWrite(WriteBuf, ReadBuf, 3);
            adObj->Registers[startReg + i ] = ReadBuf[0];
            adObj->Registers[startReg + i ] = (adObj->Registers[startReg + i ]<<8) + ReadBuf[1];
            adObj->Registers[startReg + i ] = (adObj->Registers[startReg + i ]<<8) + ReadBuf[2];
            break;
          }
        }
      default			 : 
        break;
    }
  }
  adObj->ChipSelect(AD7192CS_Disable);
}

/*启动连续读*/
/*static void AD7192StartContinuousRead(Ad7192ObjectType *adObj)
{
  uint8_t WriteBuf[1];
  uint8_t ReadBuf[1];

  adObj->ChipSelected(AD719xCS_Enable);

  WriteBuf[0] = WEN|RW_R|(REG_DATA<<3)|CREAD_EN;
  
  adObj->ReadWrite(WriteBuf, ReadBuf, 1);
}*/

/*连续读取*/
/*static uint32_t AD7192ContinuousRead(Ad7192ObjectType *adObj)
{
  uint8_t WriteBuf[4];
  uint8_t ReadBuf[4];
  uint32_t DataBuffer;

  WriteBuf[0] = NOP;
  WriteBuf[1] = NOP;	
  WriteBuf[2] = NOP;
  WriteBuf[3] = NOP;

  adObj->ChipSelected(AD719xCS_Enable);
  while(adObj->GetReadyInput()== 0){;}			
  while(adObj->GetReadyInput()== 1){;}			//等待第一个RDY下降沿;

  if ((adObj->Registers[REG_MODE] & DAT_STA_EN) == DAT_STA_EN)   //带状态信息
  {
    adObj->ReadWrite(WriteBuf, ReadBuf, 4);	  
    DataBuffer = ReadBuf[0];
    DataBuffer = (DataBuffer<<8) + ReadBuf[1];
    DataBuffer = (DataBuffer<<8) + ReadBuf[2];
    DataBuffer = (DataBuffer<<8) + ReadBuf[3];
  }
  else                                                          //不带状态信息
  {
    adObj->ReadWrite(WriteBuf, ReadBuf, 3); 
    DataBuffer = ReadBuf[0];
    DataBuffer = (DataBuffer<<8) + ReadBuf[1];
    DataBuffer = (DataBuffer<<8) + ReadBuf[2];
  }
  
  return DataBuffer;
}*/

/*读取转换数据*/
static uint32_t AD7192ReadConvertingData(Ad7192ObjectType *adObj)
{
  uint8_t WriteBuf[4];
  uint8_t ReadBuf[4];
  uint32_t DataBuffer;

  adObj->ChipSelect(AD7192CS_Enable);
  
  WriteBuf[0] = WEN|RW_R|(regAdd[AD7192_REG_DATA])|CREAD_DIS;	 

  adObj->ReadWrite(WriteBuf, ReadBuf, 1);

  WriteBuf[0] = NOP;
  WriteBuf[1] = NOP;	
  WriteBuf[2] = NOP;
  WriteBuf[3] = NOP;

  while(adObj->GetReadyInput()== 0){;}			
  while(adObj->GetReadyInput()== 1){;}			//等待第一个RDY下降沿

  if ((adObj->Registers[AD7192_REG_MODE] & DAT_STA_EN) == DAT_STA_EN)//带状态信息
  {
    adObj->ReadWrite(WriteBuf, ReadBuf, 4);	  
    DataBuffer = ReadBuf[0];
    DataBuffer = (DataBuffer<<8) + ReadBuf[1];
    DataBuffer = (DataBuffer<<8) + ReadBuf[2];
    DataBuffer = (DataBuffer<<8) + ReadBuf[3];
  }
  else //不带状态信息
  {
    adObj->ReadWrite(WriteBuf, ReadBuf, 3);
    DataBuffer = ReadBuf[0];
    DataBuffer = (DataBuffer<<8) + ReadBuf[1];
    DataBuffer = (DataBuffer<<8) + ReadBuf[2];
  }

  return DataBuffer;
}

/*退出连续读*/
/*static void AD7192ExitContinuousRead(Ad7192ObjectType *adObj)
{
  uint8_t WriteBuf[1];
  uint8_t ReadBuf[1];

  while(adObj->GetReadyInput()== 0){;}			
  while(adObj->GetReadyInput()== 1){;}			//等待第一个RDY下降沿

  WriteBuf[0] = WEN|RW_R|(REG_DATA<<3)|CREAD_DIS;

  adObj->ReadWrite(WriteBuf, ReadBuf, 1);		

  adObj->ChipSelected(AD719xCS_Disable);
}*/

/*内部零点校准*/
static void AD7192InternalZeroScaleCalibration(Ad7192ObjectType *adObj,uint32_t Channels)
{
  //配置寄存器：斩波禁用，基准电压1，AI1-AI4单通道4路，禁用激励电流，禁用基准电压检测，禁用缓冲器
  adObj->Registers[AD7192_REG_CONF] = 0;
  adObj->Registers[AD7192_REG_CONF] = CHOP_DIS|REF_IN1|Channels|BURN_DIS|REFDET_DIS|BUF_DIS|adObj->polar|adObj->gain;
  WriteAD7192Register(adObj,AD7192_REG_CONF,1);

  //模式寄存器：内部零点校准，禁用状态同传，内部时钟输出，斩波4，使能奇偶校验，不分频，仅用单周期转换使能，禁用60Hz陷波
  adObj->Registers[AD7192_REG_MODE] = 0;
  adObj->Registers[AD7192_REG_MODE] = MODE_INZCL|DAT_STA_DIS|INCLK_MCLK2EN|SINC_4|ENPAR_EN|CLK_DIV_DIS|SINGLECYCLE_DIS|REJ60_DIS|0x080;
  WriteAD7192Register(adObj,AD7192_REG_MODE, 1);

  adObj->ChipSelect(AD7192CS_Enable);
  while(adObj->GetReadyInput()== 1){;}			//等待RDY为0;
  adObj->ChipSelect(AD7192CS_Disable);
}

/*内部量程校准*/
static void AD7192InternalFullScaleCalibration(Ad7192ObjectType *adObj,uint32_t Channels)
{
  
  //配置寄存器：斩波禁用，基准电压1，AI1-AI4单通道4路，禁用激励电流，禁用基准电压检测，禁用缓冲器
  adObj->Registers[AD7192_REG_CONF] = 0;
  adObj->Registers[AD7192_REG_CONF] = CHOP_DIS|REF_IN1|Channels|BURN_DIS|REFDET_DIS|BUF_DIS|adObj->polar|adObj->gain;
  WriteAD7192Register(adObj,AD7192_REG_CONF,1);
  
  //模式寄存器：内部量程校准，禁用状态同传，内部时钟输出，斩波4，使能奇偶校验，不分频，禁用单周期转换使能，禁用60Hz陷波
  adObj->Registers[AD7192_REG_MODE] = 0;
  adObj->Registers[AD7192_REG_MODE] = MODE_INFCL|DAT_STA_DIS|INCLK_MCLK2EN|SINC_4|ENPAR_EN|CLK_DIV_2|SINGLECYCLE_DIS|REJ60_DIS|adObj->fs;		
  WriteAD7192Register(adObj,AD7192_REG_MODE,1);

  adObj->ChipSelect(AD7192CS_Enable);
  while(adObj->GetReadyInput()== 1){;}			//等待RDY为0;
  adObj->ChipSelect(AD7192CS_Disable);
}

/*外部零点校准*/
static void AD7192ExternalZeroScaleCalibration(Ad7192ObjectType *adObj,uint32_t Channels)
{
	//设置配置寄存器
  adObj->Registers[AD7192_REG_CONF] = 0;
  adObj->Registers[AD7192_REG_CONF] = CHOP_DIS|REF_IN2|Channels|BURN_DIS|REFDET_DIS|BUF_DIS|adObj->polar|adObj->gain;
  WriteAD7192Register(adObj,AD7192_REG_CONF,1);

  //将系统零点输入连接到通道输入引脚并通过配置寄存器的CH7到CH0位进行配置
  adObj->Registers[AD7192_REG_MODE] = 0;
  adObj->Registers[AD7192_REG_MODE] = MODE_SYSZCL|DAT_STA_EN|INCLK_MCLK2EN|SINC_4|ENPAR_EN|CLK_DIV_DIS|SINGLECYCLE_DIS|REJ60_DIS|adObj->fs;		
  WriteAD7192Register(adObj,AD7192_REG_MODE,1);

  adObj->ChipSelect(AD7192CS_Enable);
  while(adObj->GetReadyInput()== 1){;}			//等待RDY为0;
  adObj->ChipSelect(AD7192CS_Disable);
}

/*外部量程校准*/
static void AD7192ExternalFullScaleCalibration(Ad7192ObjectType *adObj,uint32_t Channels)
{
	//设置配置寄存器
  adObj->Registers[AD7192_REG_CONF] = 0;
  adObj->Registers[AD7192_REG_CONF] = CHOP_DIS|REF_IN2|Channels|BURN_DIS|REFDET_DIS|BUF_DIS|adObj->polar|adObj->gain;
  WriteAD7192Register(adObj,AD7192_REG_CONF, 1);

  //将系统量程输入连接到通道输入引脚并通过配置寄存器的CH7到CH0位进行配置
  adObj->Registers[AD7192_REG_MODE] = 0;
  adObj->Registers[AD7192_REG_MODE] = MODE_SYSFCL|DAT_STA_EN|INCLK_MCLK2EN|SINC_4|ENPAR_EN|CLK_DIV_2|SINGLECYCLE_DIS|REJ60_DIS|adObj->fs;		
  WriteAD7192Register(adObj,AD7192_REG_MODE, 1);

  adObj->ChipSelect(AD7192CS_Enable);
  while(adObj->GetReadyInput()== 1){;}			//等待RDY为0;
  adObj->ChipSelect(AD7192CS_Disable);
}

/*启动单次转换*/
static void AD7192StartSingleConvertion(Ad7192ObjectType *adObj,uint32_t Channels)
{
  //配置寄存器：斩波禁用，基准电压1，设置通道，禁用激励电流，禁用基准电压检测，禁用缓冲器
  adObj->Registers[AD7192_REG_CONF] = 0;
  adObj->Registers[AD7192_REG_CONF] = CHOP_DIS|REF_IN1|Channels|BURN_DIS|REFDET_DIS|BUF_DIS|adObj->polar|adObj->gain;
  WriteAD7192Register(adObj,AD7192_REG_CONF, 1);
  //模式寄存器：单次转换模式，禁用状态同传，内部时钟输出，斩波4，使能奇偶校验，不分频，禁用单周期转换使能，禁用60Hz陷波
  adObj->Registers[AD7192_REG_MODE] = 0;
  adObj->Registers[AD7192_REG_MODE] = MODE_SING|DAT_STA_DIS|INCLK_MCLK2EN|SINC_4|ENPAR_EN|CLK_DIV_DIS|SINGLECYCLE_DIS|REJ60_DIS|adObj->fs;
  WriteAD7192Register(adObj,AD7192_REG_MODE, 1);
}

/*启动连续转换*/
static void AD7192StartContinuousConvertion(Ad7192ObjectType *adObj,uint32_t Channels)
{
  //模式寄存器：连续转换模式，使能状态同传，内部时钟输出，斩波4，使能奇偶校验，不分频，禁用单周期转换使能，禁用60Hz陷波
  adObj->Registers[AD7192_REG_MODE] = 0;
  adObj->Registers[AD7192_REG_MODE] = MODE_CONT|DAT_STA_EN|INCLK_MCLK2EN|SINC_4|ENPAR_EN|CLK_DIV_DIS|SINGLECYCLE_DIS|REJ60_DIS|adObj->fs;
  //配置寄存器：斩波禁用，基准电压1，设置通道，禁用激励电流，禁用基准电压检测，禁用缓冲器
  adObj->Registers[AD7192_REG_CONF] = 0;
  adObj->Registers[AD7192_REG_CONF] = CHOP_DIS|REF_IN1|Channels|BURN_DIS|REFDET_DIS|BUF_DIS|adObj->polar|adObj->gain;	
  WriteAD7192Register(adObj,AD7192_REG_MODE, 2);
}

/*读取内部温度数据，返回摄氏度温度*/
float GetTemperatureValue(Ad7192ObjectType *adObj)
{
  uint32_t temperatureCode=0;
  float temp = 0.0;
  //模式寄存器：单次转换模式，禁用状态同传，内部时钟输出，斩波4，使能奇偶校验，不分频，禁用单周期转换使能，禁用60Hz陷波
  adObj->Registers[AD7192_REG_MODE] = 0;
  adObj->Registers[AD7192_REG_MODE] = MODE_SING|DAT_STA_DIS|INCLK_MCLK2EN|SINC_4|ENPAR_EN|CLK_DIV_DIS|SINGLECYCLE_DIS|REJ60_DIS|adObj->fs;
  WriteAD7192Register(adObj,AD7192_REG_MODE, 1);
  //配置寄存器：斩波禁用，基准电压1，内部温度，禁用激励电流，禁用基准电压检测，禁用缓冲器
  adObj->Registers[AD7192_REG_CONF] = 0;  
  adObj->Registers[AD7192_REG_CONF] = CHOP_DIS|REF_IN1|TEMP|BURN_DIS|REFDET_DIS|BUF_DIS|UB_BI|GAIN_1;
  WriteAD7192Register(adObj,AD7192_REG_CONF, 1);

  temperatureCode = AD7192ReadConvertingData(adObj);
  temp = (temperatureCode-0x800000)/2815.0-273;
  return temp;
}

/* 默认的片选信号处理函数 */
static void AD719xChipSelect(AD7192CSType cs)
{
  return;
}

/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
