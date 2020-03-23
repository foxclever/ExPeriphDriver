/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ms5536function.c                                               **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现MS5536C表压传感器的操作                                **/
/**           每台MS5536C表压传感器都有出厂校准的6个修正系数：               **/
/**           C1: Pressure sensitivity (13 Bit)                              **/
/**           C2: Pressure offset (13 Bit)                                   **/
/**           C3: Temperature coefficient of pressure sensitivity (9 Bit)    **/
/**           C4: Temperature coefficient of pressure offset (9 Bit)         **/
/**           C5: Reference Temperature (12 Bit)                             **/
/**           C6: Temperature coefficient of the temperature (8 Bit)         **/
/**           分别存储于4个16位字中，可通过逻辑运算得到：                    **/
/**   Word 1：                                                               **/
/**   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+      **/
/**   |        C3/II (8-Bit)          |         C5/II(8-Bit)          |      **/
/**   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+      **/
/**   |DB7|DB6|DB5|DB4|DB3|DB2|DB1|DB0|DB7|DB6|DB5|DB4|DB3|DB2|DB1|DB0|      **/
/**   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+      **/
/**   Word 2：                                                               **/
/**   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+      **/
/**   |        C4/II (8-Bit)          |         C6(8-Bit)             |      **/
/**   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+      **/
/**   |DB7|DB6|DB5|DB4|DB3|DB2|DB1|DB0|DB7|DB6|DB5|DB4|DB3|DB2|DB1|DB0|      **/
/**   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+      **/
/**   Word 3：                                                               **/
/**   +-----------------+-------------------------------------------------+  **/
/**   |  C5/I (4-Bit)   |                 C1/II (12-Bit)                  |  **/
/**   +----+----+---+---+----+----+---+---+---+---+---+---+---+---+---+---+  **/
/**   |DB11|DB10|DB9|DB8|DB11|DB10|DB9|DB8|DB7|DB6|DB5|DB4|DB3|DB2|DB1|DB0|  **/
/**   +----+----+---+---+----+----+---+---+---+---+---+---+---+---+---+---+  **/
/**   Word 4：                                                               **/
/**   +----+----+----+------------------------------------------------------+**/
/**   |C1/I|C4/I|C3/I|                     C2 (13-Bit)                      |**/
/**   +----+----+----+----+----+----+---+---+---+---+---+---+---+---+---+---+**/
/**   |DB12|DB8 |DB8 |DB12|DB11|DB10|DB9|DB8|DB7|DB6|DB5|DB4|DB3|DB2|DB1|DB0|**/
/**   +----+----+----+----+----+----+---+---+---+---+---+---+---+---+---+---+**/
/**           命令由：3位起始位+指令+3位停止位构成：                         **/
/**           1、读压力数值命令序列：                                        **/
/**           +----+----+----+----+----+----+----+----+----+----+            **/
/**           |Bit0|Bit1|Bit2|Bit3|Bit4|Bit5|Bit6|Bit7|Bit8|Bit9|            **/
/**           +----+----+----+----+----+----+----+----+----+----+            **/
/**           |  1 |  1 |  1 |  1 |  0 |  1 |  0 |  0 |  0 |  0 |            **/
/**           +----+----+----+----+----+----+----+----+----+----+            **/
/**           |   Start-bit  |     Setup-bits    |    Stop-bit  |            **/
/**           +----+----+----+----+----+----+----+----+----+----+            **/
/**           2、读温度数值命令序列：                                        **/
/**           +----+----+----+----+----+----+----+----+----+----+            **/
/**           |Bit0|Bit1|Bit2|Bit3|Bit4|Bit5|Bit6|Bit7|Bit8|Bit9|            **/
/**           +----+----+----+----+----+----+----+----+----+----+            **/
/**           |  1 |  1 |  1 |  1 |  0 |  0 |  1 |  0 |  0 |  0 |            **/
/**           +----+----+----+----+----+----+----+----+----+----+            **/
/**           |   Start-bit  |     Setup-bits    |    Stop-bit  |            **/
/**           +--------------+-------------------+--------------+            **/
/**           3、读校准系数Word1命令序列：                                   **/
/**           +----+----+----+----+----+----+----+----+----+----+-----+-----+**/
/**           |Bit0|Bit1|Bit2|Bit3|Bit4|Bit5|Bit6|Bit7|Bit8|Bit9|Bit10|Bit11|**/
/**           +----+----+----+----+----+----+----+----+----+----+-----+-----+**/
/**           |  1 |  1 |  1 |  0 |  1 |  0 |  1 |  0 |  1 |  0 |  0  |  0  |**/
/**           +----+----+----+----+----+----+----+----+----+----+-----+-----+**/
/**           |   Start-bit  |           Setup-bits        |     Stop-bit   |**/
/**           +--------------+-----------------------------+----------------+**/
/**           4、读校准系数Word3命令序列：                                   **/
/**           +----+----+----+----+----+----+----+----+----+----+-----+-----+**/
/**           |Bit0|Bit1|Bit2|Bit3|Bit4|Bit5|Bit6|Bit7|Bit8|Bit9|Bit10|Bit11|**/
/**           +----+----+----+----+----+----+----+----+----+----+-----+-----+**/
/**           |  1 |  1 |  1 |  0 |  1 |  1 |  0 |  0 |  1 |  0 |  0  |  0  |**/
/**           +----+----+----+----+----+----+----+----+----+----+-----+-----+**/
/**           |   Start-bit  |           Setup-bits        |     Stop-bit   |**/
/**           +--------------+-----------------------------+----------------+**/
/**           5、读校准系数Word2命令序列：                                   **/
/**           +----+----+----+----+----+----+----+----+----+----+-----+-----+**/
/**           |Bit0|Bit1|Bit2|Bit3|Bit4|Bit5|Bit6|Bit7|Bit8|Bit9|Bit10|Bit11|**/
/**           +----+----+----+----+----+----+----+----+----+----+-----+-----+**/
/**           |  1 |  1 |  1 |  0 |  1 |  0 |  1 |  1 |  0 |  0 |  0  |  0  |**/
/**           +----+----+----+----+----+----+----+----+----+----+-----+-----+**/
/**           |   Start-bit  |           Setup-bits        |     Stop-bit   |**/
/**           +--------------+-----------------------------+----------------+**/
/**           6、读校准系数Word4命令序列：                                   **/
/**           +----+----+----+----+----+----+----+----+----+----+-----+-----+**/
/**           |Bit0|Bit1|Bit2|Bit3|Bit4|Bit5|Bit6|Bit7|Bit8|Bit9|Bit10|Bit11|**/
/**           +----+----+----+----+----+----+----+----+----+----+-----+-----+**/
/**           |  1 |  1 |  1 |  0 |  1 |  1 |  0 |  1 |  0 |  0 |  0  |  0  |**/
/**           +----+----+----+----+----+----+----+----+----+----+-----+-----+**/
/**           |   Start-bit  |           Setup-bits        |     Stop-bit   |**/
/**           +--------------+-----------------------------+----------------+**/
/**           7、复位命令由：16位的10序列及5位低电平组成。                   **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    1、声明MS5536cObjectType对象实体，并调用函数Ms5536cInitialization初始 **/
/**       化对象实体。并读取校准数据                                         **/
/**    2、实现形如uint8_t f(uint8_t *txData,uint8_t *rxData,uint16_t number) **/
/**       的函数，将其赋值给MS5536cObjectType对象实体的ReadWriteMS函数指针。 **/
/**       实现数据的收发。                                                   **/
/**    3、实现形如void f(bool en)的函数，并将其赋值给MS5536cObjectType对象实 **/
/**       体的SetPhase函数指针。实现SCLK的相位切换，因为在给MS5536C发送命令时**/
/**       使用时钟的第1个跳变沿（模式0），而在接收MS5536C的数据时，使用时钟的**/
/**       第2个跳变沿（模式1）。                                             **/
/**    4、实现形如void f(volatile uint32_t nTime)的函数，并将其赋值给        **/
/**       MS5536cObjectType对象实体的Delayms函数指针。实现操作延时，单位毫秒 **/
/**    5、调用相应的函数实现需要的操作。                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2018-07-13          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "ms5536function.h"

/* 定义读取命令的宏 */
#define READ_D1         0xF400
#define READ_D2         0xF200
#define READ_WORD1      0x0EA8
#define READ_WORD3      0x0EC8
#define READ_WORD2      0x0EB0
#define READ_WORD4      0x0ED0

uint16_t cWord[4]={0,0,0,0};

/* 读取寄存器值 */
static uint16_t ReadRegister(MS5536cObjectType *ms,uint16_t command);
/* 计算校准系数 */
static void CalcCoefficientForMs5536c(MS5536cObjectType *ms,uint16_t *cWord);
/* 读取测量数据 */
static uint16_t ReadMeasureData(MS5536cObjectType *ms,uint16_t command);
/* 获取MS5336C校准系数 */
static void GetCoefficientForMs5536c(MS5536cObjectType *ms);

/* 获取压力和温度数据 */
void GetMeasureForMs5536c(MS5536cObjectType *ms)
{
  uint16_t D1,D2;
  int64_t uT1=0; 
  int64_t dT=0;
  int32_t temp=0;
  int64_t off=0;
  int64_t sens=0;
  int64_t x=0;
  int32_t pres=0;
  
  //读取压力数据
  D1=ReadMeasureData(ms,READ_D1);
  //读取温度数据
  D2=ReadMeasureData(ms,READ_D2);
  
  uT1=(ms->coef5-200)*4+15136;

  dT=(D2-uT1);
  
  temp =(int32_t)(2000 + dT*(ms->coef6+262)/512);
  
  off= ms->coef2-2000 +10381+ ((ms->coef4-243)*dT)/4096;
  
  sens = ms->coef1-2000 + 10179+((ms->coef3+222)*dT)/2048;
  
  x = (sens * (off-D1))/4096;
  
  pres=(int32_t)(x*1365/512);
  
  ms->temperature=temp;
  ms->fTemperature=temp*0.01;
  
  ms->pressure=pres;
  ms->fPressure=pres*0.01;
}

/* 读取测量数据 */
static uint16_t ReadMeasureData(MS5536cObjectType *ms,uint16_t command)
{
  uint8_t txData[2];
  uint8_t rxData[2];
  uint16_t result=0;
  
  txData[0]=(uint8_t)(command>>8);
  txData[1]=(uint8_t)command;
  ms->ReadWriteMS(txData,rxData,2);
//   ms->WriteMS(txData,2);
  
  ms->Delayms(23);
  ms->SetPhase(MS5536_SCLK_FALL);
  ms->Delayms(10);
  
  txData[0]=0x00;
  txData[1]=0x00;
  ms->ReadWriteMS(txData,rxData,2);
//  ms->ReadMS(rxData,2);
  
  result=(rxData[0]<<8)+rxData[1];

  ms->SetPhase(MS5536_SCLK_RISE);
  ms->Delayms(10);
  return result;
}

/* 获取MS5336C校准系数 */
static void GetCoefficientForMs5536c(MS5536cObjectType *ms)
{
//  uint16_t cWord[4]={0,0,0,0};
  
  //读word1
  cWord[0]=ReadRegister(ms,READ_WORD1);
  
  //读word2
  cWord[1]=ReadRegister(ms,READ_WORD2);
  
  //读word3
  cWord[2]=ReadRegister(ms,READ_WORD3);
  
  //读word4
  cWord[3]=ReadRegister(ms,READ_WORD4);
  
  CalcCoefficientForMs5536c(ms,cWord);
}

/* 读取寄存器值 */
static uint16_t ReadRegister(MS5536cObjectType *ms,uint16_t command)
{
  uint8_t txData[2];
  uint8_t rxData[2];
  uint16_t result=0;
  
  txData[0]=(uint8_t)(command>>8);
  txData[1]=(uint8_t)command;
  ms->ReadWriteMS(txData,rxData,2);
//  ms->WriteMS(txData,2);

  ms->SetPhase(MS5536_SCLK_FALL);

  txData[0]=0x00;
  txData[1]=0x00;
  ms->ReadWriteMS(txData,rxData,2);
//  ms->ReadMS(rxData,2);
  result=(rxData[0]<<8)+rxData[1];
  
  ms->SetPhase(MS5536_SCLK_RISE);
  ms->Delayms(1);
  return result;
}

/* 计算校准系数 */
static void CalcCoefficientForMs5536c(MS5536cObjectType *ms,uint16_t *cWord)
{
  ms->coef3=(cWord[0]>>8);
  ms->coef5=(cWord[0]&0x00FF);
  ms->coef4=(cWord[1]>>8);
  ms->coef6=(cWord[1]&0x00FF);
  ms->coef1=(cWord[2]&0x0FFF);
  ms->coef2=(cWord[3]&0x1FFF);
  
  ms->coef5=ms->coef5+((cWord[2]&0xF000)>>4);
  
  ms->coef1=ms->coef1+((cWord[3]&0x8000)>>3);
  ms->coef4=ms->coef4+((cWord[3]&0x4000)>>6);
  ms->coef3=ms->coef3+((cWord[3]&0x2000)>>5);
}

/*对MS5336C进行软件复位*/
void Ms5336cSoftwareReset(MS5536cObjectType *ms)
{
  //命令为21位：10101010 10101010 00000
  uint8_t command[3]={170,170,0};
  uint8_t rxDate[3];
  
  ms->ReadWriteMS(command,rxDate,3);
//  ms->WriteMS(command,3);
}

/* 对MS5536C对象进行初始化 */
void Ms5536cInitialization(MS5536cObjectType *ms,
                           MS5536cReadWriteMS readwrite,
                           MS5536cSetPhase setPhase,
                           MS5536cDelayms delayms
                          )
{
  if((ms==NULL)||(readwrite==NULL)||(setPhase==NULL)||(delayms==NULL))
  {
    return;
  }
  ms->ReadWriteMS=readwrite;
  ms->SetPhase=setPhase;
  ms->Delayms=delayms;
	
  ms->coef1=0;
  ms->coef2=0;
  ms->coef3=0;
  ms->coef4=0;
  ms->coef5=0;
  ms->coef6=0;
  
  ms->pressure=0;
  ms->fPressure=0.0;
  
  ms->temperature=0;
  ms->fTemperature=0.0;
  
  //复位
  Ms5336cSoftwareReset(ms);
  
  ms->Delayms(10);
  
  //获取修正系数
  GetCoefficientForMs5536c(ms);
}

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/
