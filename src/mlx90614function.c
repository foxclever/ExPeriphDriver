/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：mlx90614function.c                                             **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现MLX90614红外温度传感器的通讯                           **/
/**           采用SMBus通讯通讯接口                                          **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    SMBus通讯通讯接口的定义与低速I2C接口定义相同，可以采用硬件I2C接口和   **/
/**    软件模拟I2C接口实现通讯。                                             **/
/**    1、声明MLX90614对象实体，并调用函数MLXInitialization初始化对象实体。  **/
/**       初始化时，给定对象的地址。                                         **/
/**    2、实现形如void f(struct MLXObject *mlx,uint8_t cmd,uint8_t *rData,   **/
/**       uint16_t rSize)的函数，并将其赋值给MLX90614对象实体的Read函数指针。**/
/**       实现数据的读取操作。                                               **/
/**    3、实现形如void f(struct MLXObject *mlx,uint8_t cmd,uint8_t *wData,   **/
/**       uint16_t wSize)的函数，并将其赋值给MLX90614对象实体的Write函数指   **/
/**       针。实现数据的写操作。                                             **/
/**    4、实现形如oid f(volatile uint32_t nTime)的函数，并将其赋值给MLX90614t**/
/**       对象实体的Delay函数指针。实现延时操作，时间单位为毫秒。            **/
/**    5、调用相应的函数实现需要的操作。                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2019-05-05          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "mlx90614function.h"

#define MLXSlaveAddress         0xB4

/* 定义操作命令码 */
#define RAMAccess               0x00
#define EEPROMAccess            0x20
#define ReadFlags               0xF0
#define EnterSLEEPMode          0xFF

/* 定义EEPROM内存量地址 */
#define Tomax                   0x00
#define Tomin                   0x01
#define PWMCTRL                 0x02
#define Ta_Range                0x03
#define CorrectCoefficient      0x04
#define ConfigRegister1         0x05
#define SMBus_Address           0x0E
#define ID_Number1              0x1C
#define ID_Number2              0x1D
#define ID_Number3              0x1E
#define ID_Number4              0x1F

/* 定义RAM内存量地址 */
#define RawDataIRChannel1       0x04
#define RawDataIRChannel2       0x05
#define TA                      0x06
#define TOBJ1                   0x07
#define TOBJ2                   0x08

const uint16_t MLXPOLYNOMIAL = 0x07;      //定义校验多项式P(x) = x^8 + x^2 + x^1 + 1 = 100000111

/* 获取MLX90614器件的ID码 */
static void GetIDFromMLX90614(MLXObjectType *mlx);
/* 校验码计算*/
static uint8_t PECCalculation(uint8_t *ptr, uint8_t len);
/*读数据操作*/
static uint16_t ReadDataFromMLX(MLXObjectType *mlx,uint8_t cmd);
/* 写数据操作 */
static void WriteDataToMLX(MLXObjectType *mlx,uint8_t cmd,uint16_t data);

/* 读取温度值 */
void GetMLXTemperature(MLXObjectType *mlx)
{
 
  mlx->tempAmbient=ReadDataFromMLX(mlx,RAMAccess|TA)*0.02-273.15;
  
  mlx->tempObject1=ReadDataFromMLX(mlx,RAMAccess|TOBJ1)*0.02-273.15;
  
  mlx->tempObject2=ReadDataFromMLX(mlx,RAMAccess|TOBJ2)*0.02-273.15;
  
}

/* 读状态标志 */
uint16_t ReadFlagFromMLX(MLXObjectType *mlx)
{
  uint16_t tempCode;
  
  tempCode=ReadDataFromMLX(mlx,ReadFlags);
  mlx->flags=(uint8_t)tempCode;
	
  return tempCode;
}

/* 使设备进入休眠模式 */
void EnterSleepModeForMLX(MLXObjectType *mlx)
{
  uint8_t cmd;
  static uint8_t pec;
  uint8_t data[2];
  
  cmd=EnterSLEEPMode;
  
  data[0]=mlx->devAddress;
  data[1]=cmd;
  
  pec=PECCalculation(data,2);
   
  mlx->Write(mlx,cmd,&pec,1);
}

/*使设备退出休眠模式*/
void ExitSleepModeForMLX(void)
{
	
}

/* 配置PWM控制 */
void SetPWMControll(MLXObjectType *mlx,uint16_t data)
{
  WriteDataToMLX(mlx,EEPROMAccess|PWMCTRL,data);
  
  mlx->pwmctrl=ReadDataFromMLX(mlx,EEPROMAccess|PWMCTRL);
}

/* 设置配置寄存器 */
void SetConfigRegister(MLXObjectType *mlx,uint16_t data)
{
  WriteDataToMLX(mlx,EEPROMAccess|ConfigRegister1,data);
  
  mlx->ConfigRegister=ReadDataFromMLX(mlx,EEPROMAccess|ConfigRegister1);
}

/* 修改设备地址 */
void ModifyDeviceAddress(MLXObjectType *mlx,uint16_t address)
{
  WriteDataToMLX(mlx,EEPROMAccess|SMBus_Address,address);
  
  uint8_t temp=mlx->devAddress;
  mlx->devAddress=(uint8_t)address;
  
  if(ReadDataFromMLX(mlx,EEPROMAccess|SMBus_Address)!=address)
  {
    mlx->devAddress=temp;
  }
}

/* 红外温度传感器对象初始化 */
void MLXInitialization(MLXObjectType *mlx,	//MLX90614对象
                       uint8_t address,			//设备地址
                       MLXRead read,				//读数据函数指针
                       MLXWrite write,			//写数据函数指针
                       MLXDelayus delayus		//微秒延时函数指针
                      )
{
  if((mlx==NULL)||(read==NULL)||(write==NULL)||(delayus==NULL))
  {
    return;
  }
  mlx->Read=read;
  mlx->Write=write;
  mlx->Delayus=delayus;
		
  mlx->tempAmbient=0.0;
  mlx->tempObject1=0.0;
  mlx->tempObject2=0.0;
  
  if(address>0x00)
  {
    mlx->devAddress=address;
  }
  else
  {
    mlx->devAddress=MLXSlaveAddress;
  }
  
  mlx->Delayus(200);
  
  GetIDFromMLX90614(mlx);
  
  ReadFlagFromMLX(mlx);
  
  mlx->pwmctrl=ReadDataFromMLX(mlx,EEPROMAccess|PWMCTRL);
  
  mlx->ConfigRegister=ReadDataFromMLX(mlx,EEPROMAccess|ConfigRegister1);
}

/*读数据操作*/
static uint16_t ReadDataFromMLX(MLXObjectType *mlx,uint8_t cmd)
{
  uint8_t data[3];
  uint16_t tempCode=0;
  uint8_t pec[6];
  
  mlx->Read(mlx,cmd,data,3);
  
  pec[0]=mlx->devAddress;
  pec[1]=cmd;
  pec[2]=mlx->devAddress+1;
  pec[3]=data[0];
  pec[4]=data[1];
  pec[5]=data[2];
  
  if(PECCalculation(pec,6)==0x00)
  {
    tempCode=(data[1]<<8)+data[0];
  }
  
  return tempCode;
}

/* 写数据操作 */
static void WriteDataToMLX(MLXObjectType *mlx,uint8_t cmd,uint16_t data)
{
  uint8_t wData[3];
  uint8_t pec[4];
  
  pec[0]=mlx->devAddress;
  pec[1]=cmd;
  pec[2]=(uint8_t)data;
  pec[3]=(uint8_t)(data>>8);
  
  wData[0]=(uint8_t)data;
  wData[1]=(uint8_t)(data>>8);
  wData[2]=PECCalculation(pec,4);
  
  mlx->Write(mlx,cmd,wData,3);
}

/* 获取MLX90614器件的ID码 */
static void GetIDFromMLX90614(MLXObjectType *mlx)
{
  mlx->ID[0]=ReadDataFromMLX(mlx,EEPROMAccess|ID_Number1);
  
  mlx->ID[1]=ReadDataFromMLX(mlx,EEPROMAccess|ID_Number2);
  
  mlx->ID[2]=ReadDataFromMLX(mlx,EEPROMAccess|ID_Number3);
  
  mlx->ID[3]=ReadDataFromMLX(mlx,EEPROMAccess|ID_Number4);
}

/* 校验码计算*/
static uint8_t PECCalculation(uint8_t *ptr, uint8_t len)
{
  uint8_t i; 
  uint8_t crc=0x00;           //计算的初始crc值

  while(len--)
  {
    crc ^= *ptr++;          //每次先与需要计算的数据异或,计算完指向下一数据
    for (i=8; i>0; --i)
    { 
      if (crc & 0x80)
      {
        crc = (crc << 1) ^ MLXPOLYNOMIAL;
      }
      else
      {
        crc = (crc << 1);
      }
    }
  }

  return (crc); 
}


/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
