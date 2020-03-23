/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：sht1xfunction.h                                                **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现SHT1X温湿度计的操作，采用类似I2C的通讯方式             **/
/**           包括SHT10,SHT11和SHT15以及SHT7x                                **/
/**           默认分辨率14bit(温度)和12bit(湿度)可以被降低为12和8bit         **/
/**           温度转换公式：T=d1+d2*SOT，其中SOT为数字输出，T为温度值        **/
/**           d1和d2的取值如下：                                             **/
/**           VDD               d1 (°C)         d1 (°F)                    **/
/**           5V                -40.1           -40.2                        **/
/**           4V                -39.8           -39.6                        **/
/**           3.5V              -39.7           -39.5                        **/
/**           3V                -39.6           -39.3                        **/
/**           2.5V              -39.4           -38.9                        **/
/**                                                                          **/
/**           SOT               d2 (°C)         d2 (°F)                    **/
/**           14bit             0.01            0.018                        **/
/**           12bit             0.04            0.072                        **/
/**           湿度数据的转换公式：RHlinear=c1+c2*SORH+c3*SORH*SORH，         **/
/**           其中SORH为数字输出，RHlinear为转换后的现对湿度                 **/
/**           SORH	  c1	          c2	          c3                 **/
/**           12bit	-2.0468         0.0367          -1.5955E-6           **/
/**           8bit	-2.0468         0.5872          -4.0845E-4           **/
/**           湿度温度补偿公式：RHtrue=(T-25)*(t1+t2*SORH)+RHlinear          **/
/**           其中RHtrue为温度补偿后的相对湿度                               **/
/**           SORH	  t1	  t2                                         **/
/**           12bit	0.01	0.00008                                      **/
/**           8bit	0.01	0.00128                                      **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    1、声明Sht1xObjectType对象实体，并调用函数SHT1xInitialization初始化   **/
/**       对象实体。给定设备的工作电源电压VDD以及温度单位（摄氏度或华氏度）  **/
/**    2、实现两个形如void f(BusPinValue value)的引脚输出操作函数，分别操    **/
/**       作SCK和SDA。并声明BusPinOperation类型的函数指针数组，SCK的操作函   **/
/**       数作为第一个元素，SDA的操作函数作为第二个元素。并将函数指针数组的  **/
/**       指针赋值给Sht1xObjectType对象实体的SetBusPin函数指针。实现对SCK引  **/
/**       脚和SDA引脚的操作。                                                **/
/**    3、实现形如uint8_t f(void)的函数，并将其赋值给Sht1xObjectType对象实体 **/
/**       的ReadSDABit函数指针。实现数据的读取。                             **/
/**    4、实现形如void f(IODirection direction);的函数，并将其赋值给         **/
/**       Sht1xObjectType对象实体的SetSDADirection函数指针。实现动态改变SDA引**/
/**       脚的输入输出方向                                                   **/
/**    5、实现形如void f(volatile uint32_t period)的函数，并将其赋值给       **/
/**       Sht1xObjectType对象实体的Delayus函数指针。实现操作延时，单位微秒   **/
/**    6、实现形如void f(volatile uint32_t nTime)的函数，并将其赋值给        **/
/**       Sht1xObjectType对象实体的Delayms函数指针。实现操作延时，单位毫秒   **/
/**    7、调用相应的函数实现需要的操作。                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-01-13          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "sht1xfunction.h"
#include "math.h"

/*定义SHT1x的操作命令*/
#define TEMP_MEAS_COMMAND       0x03    //温度测量命令
#define HUMI_MEAS_COMMAND       0x05    //湿度检测命令
#define READ_STATUS_REGISTER    0x07    //读状态寄存器
#define WRITE_STATUS_REGISTER   0x06    //写状态寄存器
#define SOFTWARE_RESET          0x1E    //软件复位命令

#define ONCHIPHEATERENABLE      0x04    //启用片内加热
#define ONCHIPHEATERDISABLE     0x00    //禁用片内加热
#define OTPENABLE               0x00    //能启动OTP加载
#define OTPDISABLE              0x02    //不能启动OTP加载
#define HIGH_RESOLUTION_DATA    0x00    //数据分辨率14bit(温度)和12bit(湿度)
#define LOW_RESOLUTION_DATA     0x01    //数据分辨率12bit(温度)和8bit(湿度)

#define noAck   0       //非确认
#define Ack     1       //确认

const uint16_t SHT1xPOLYNOMIAL = 0x31;      //定义校验多项式P(x) = x^8 + x^5 + x^4 + 1 = 100110001

SHT1xSetBusPin setBusPin[2];

/*SHT1X启动时序操作*/
static void StartSHT1XOperation(Sht1xObjectType *sht);
/*计算获取电压温度系数d1*/
static float CalcVoltageTempCoefficient(Sht1xObjectType *sht);
/*向SHT1X写一个字节*/
static uint8_t WriteByteToSht1x(Sht1xObjectType *sht,uint8_t value);
/*从SHT1X读取一个字节*/
static uint8_t ReadByteFromSht1x(Sht1xObjectType *sht,uint8_t ack);
/*温度数据转换*/
static float ConvertTemperatureData(Sht1xObjectType *sht,uint16_t sot);
/*湿度数据转换*/
static float ConvertHumidityData(Sht1xObjectType *sht,uint16_t sorh);
/*根据指定的多项式计算CRC校验*/
static bool CheckCRC8ForSHT1x(uint8_t *data, uint8_t numOfBytes, uint8_t checksum);
/*计算露点温度*/
static float CalcDewPointTemperature(Sht1xObjectType *sht);
/* 缺省IO方向控制函数 */
static void DefaultSDADirection(SHT1xIOModeType direction);
/*读状态寄存器*/
static uint8_t ReadStatusRegister(Sht1xObjectType *sht);
/*写状态寄存器*/
static uint8_t WriteStatusRegister(Sht1xObjectType *sht,uint8_t *pValue);
	
	
/* 获取SHT1x测量数据 */
void GetSht1xMeasureValue(Sht1xObjectType *sht)
{
  sht->temperature=GetSht1xTemperatureValue(sht);
  sht->humidity=GetSht1xHumidityValue(sht);
  sht->dewPoint=CalcDewPointTemperature(sht);
}

/*获取SHT1X的温度值*/
float GetSht1xTemperatureValue(Sht1xObjectType *sht)
{
  float tempValue=0.0;
  uint16_t sot=0;
  uint8_t err=0;
  uint8_t tempCode[2]={0,0};
  uint8_t checkSum=0;
  StartSHT1XOperation(sht);
  WriteByteToSht1x(sht,TEMP_MEAS_COMMAND);
  sht->SDADirection(SHT1xIn);
	
  if((sht->statusReg&0x01)==0x01)
  {
    sht->Delayms(80);
  }
  else
  {
    sht->Delayms(320);
  }
	
  if(sht->ReadSDABit() == 1)
  {
    err += 1;
  }
  tempCode[0]=ReadByteFromSht1x(sht,Ack);
  tempCode[1]=ReadByteFromSht1x(sht,Ack);
  checkSum=ReadByteFromSht1x(sht,noAck);

  if(CheckCRC8ForSHT1x(tempCode,2,checkSum))
  {
		sot=(uint16_t)tempCode[0];
		sot=(sot*256)+(uint16_t)tempCode[1];
  }
  else
  {
    err += 1;
  }
		
  if(err != 0)
  {
    ResetSHT1XCommunication(sht);
  }
  else
  {
    tempValue=ConvertTemperatureData(sht,sot);
  }
  
  return tempValue;
}

/*获取SHT1X的湿度值*/
float GetSht1xHumidityValue(Sht1xObjectType *sht)
{
  float humiValue=0.0;
  uint16_t sorh=0;
  uint8_t err=0;
  uint8_t humiCode[2]={0,0};
  uint8_t checkSum=0;
  
  StartSHT1XOperation(sht);
  WriteByteToSht1x(sht,HUMI_MEAS_COMMAND);
  sht->SDADirection(SHT1xIn);
	
  if((sht->statusReg&0x01)==0x01)
  {
    sht->Delayms(20);
  }
  else
  {
    sht->Delayms(80);
  }
	  
  if(sht->ReadSDABit() == 1)
  {
    err += 1;
  }
  humiCode[0]=ReadByteFromSht1x(sht,Ack);
  humiCode[1]=ReadByteFromSht1x(sht,Ack);
  checkSum=ReadByteFromSht1x(sht,noAck);
  if(CheckCRC8ForSHT1x(humiCode,2,checkSum))
  {
    sorh=(humiCode[0]<<8)|humiCode[1];
  }
  else
  {
    err += 1;
  }

  if(err != 0)
  {
    ResetSHT1XCommunication(sht);
  }
  else
  {
    humiValue=ConvertHumidityData(sht,sorh);
  }
  
  return humiValue;
}

/*计算露点温度*/
static float CalcDewPointTemperature(Sht1xObjectType *sht)
{
  float dewPoint=0.0;
  float logEx=0.0;
  logEx=0.66077+7.5*sht->temperature/(237.3+ sht->temperature) + (float)(log10(sht->humidity) - 2);
  dewPoint=((0.66077 - logEx) * 237.3) / (logEx - 8.16077);
  return dewPoint;
}
                               
/*SHT1X启动时序操作*/
static void StartSHT1XOperation(Sht1xObjectType *sht)
{
  /*将data线设置为输出模式*/
  sht->SDADirection(SHT1xOut);
  
  sht->SetBusPin[DataPin](SHT1xSet);
  sht->SetBusPin[SckPin](SHT1xReset);
  sht->Delayus(sht->period);
  
  sht->SetBusPin[SckPin](SHT1xSet);
  sht->Delayus(sht->period);
  sht->SetBusPin[DataPin](SHT1xReset);
  sht->Delayus(sht->period);
  sht->SetBusPin[SckPin](SHT1xReset);
  sht->Delayus(sht->period);
  sht->SetBusPin[SckPin](SHT1xSet);
  sht->Delayus(sht->period);
  sht->SetBusPin[DataPin](SHT1xSet);
  sht->Delayus(sht->period);
  sht->SetBusPin[SckPin](SHT1xReset);
}

/*SHT1X通讯复位*/
void ResetSHT1XCommunication(Sht1xObjectType *sht)
{
  /*将data线设置为输出模式*/
  sht->SDADirection(SHT1xOut);
  sht->Delayms(1);
  
  sht->SetBusPin[DataPin](SHT1xSet);
  sht->SetBusPin[SckPin](SHT1xReset);
  
  for(int i=0;i<9;i++)
  {
    sht->SetBusPin[SckPin](SHT1xSet);
    sht->Delayus(sht->period);
    sht->SetBusPin[SckPin](SHT1xReset);
    sht->Delayus(sht->period);
  }
  
  StartSHT1XOperation(sht);
}

/*向SHT1X写一个字节*/
static uint8_t WriteByteToSht1x(Sht1xObjectType *sht,uint8_t value)
{
  uint8_t err = 0;
  
  /*将data线设置为输出模式*/
  sht->SDADirection(SHT1xOut);
  sht->Delayms(1);
  
  for(int i=0x80;i>0;i/=2)
  {
    if(i&value)
    {
      sht->SetBusPin[DataPin](SHT1xSet);
    }
    else
    {
      sht->SetBusPin[DataPin](SHT1xReset);
    }
    sht->Delayus(sht->period);
    sht->SetBusPin[SckPin](SHT1xSet);
    sht->Delayus(sht->period);
    sht->SetBusPin[SckPin](SHT1xReset);
    sht->Delayus(sht->period);
  }
  sht->SDADirection(SHT1xIn);
  sht->Delayms(1);
  sht->SetBusPin[SckPin](SHT1xSet);
  
  /*读取SHT1X的应答位*/
  err=sht->ReadSDABit();
  sht->SetBusPin[SckPin](SHT1xReset);
  return err;
}

/*从SHT1X读取一个字节*/
static uint8_t ReadByteFromSht1x(Sht1xObjectType *sht,uint8_t ack)
{
  uint8_t val=0;
  sht->SDADirection(SHT1xIn);
  
  for(int i=0x80;i>0;i/=2)
  {
    sht->Delayus(sht->period);
    sht->SetBusPin[SckPin](SHT1xSet);
    sht->Delayus(sht->period);
    if(sht->ReadSDABit())
    {
      val=(val|i);
    }
    sht->SetBusPin[SckPin](SHT1xReset);
  }
  sht->SDADirection(SHT1xOut);
  if(ack)
  {
    sht->SetBusPin[DataPin](SHT1xReset);
  }
  else
  {
    sht->SetBusPin[DataPin](SHT1xSet);
  }
  sht->Delayus(sht->period);
  sht->SetBusPin[SckPin](SHT1xSet);
  sht->Delayus(sht->period);
  sht->SetBusPin[SckPin](SHT1xReset);
  sht->Delayus(sht->period);
  
  return val;
}

/*对SHT1X实现软件复位*/
uint8_t Sht1xSoftwareReset(Sht1xObjectType *sht)
{
  uint8_t err = 0;
  
  ResetSHT1XCommunication(sht);
  err=err+WriteByteToSht1x(sht,SOFTWARE_RESET);
  
  sht->Delayms(20);
  
  return err;
 }

/*读状态寄存器*/
static uint8_t ReadStatusRegister(Sht1xObjectType *sht)
{
  uint8_t err=0;
  uint8_t status;
  uint8_t checkSum;
	
  StartSHT1XOperation(sht);
  err=WriteByteToSht1x(sht,READ_STATUS_REGISTER);
  status=ReadByteFromSht1x(sht,Ack);
  checkSum=ReadByteFromSht1x(sht,noAck);
  
  if(CheckCRC8ForSHT1x(&status,1,checkSum))
  {
    sht->statusReg=status;
  }
  else
  {
    err+=1;
  }
  return err;
}

/*写状态寄存器*/
static uint8_t WriteStatusRegister(Sht1xObjectType *sht,uint8_t *pValue)
{
  uint8_t err=0;
  
  StartSHT1XOperation(sht);
  err +=WriteByteToSht1x(sht,WRITE_STATUS_REGISTER);
  err +=WriteByteToSht1x(sht,*pValue);
  
  err +=ReadStatusRegister(sht);
  return err;
}

/* 初始化SHT1x对象 */
void SHT1xInitialization(Sht1xObjectType *sht,
                         uint32_t sck,
                         float vdd,
                         SHT1xTempUnitType uint,
                         SHT1xHeaterType heater,
                         SHT1xOTPType otp,
                         SHT1xResolutionType resolution,
                         SHT1xSetBusPin setSckPin,
                         SHT1xSetBusPin setDataPin,
                         SHT1xReadSDABit readSDA,
                         SHT1xSDADirection direction,
                         SHT1xDelay delayus,
                         SHT1xDelay delayms)
{
  uint8_t regSetup=0x00;
  uint8_t heaterSet[]={ONCHIPHEATERDISABLE,ONCHIPHEATERENABLE};	//是否启用片内加热配置集
  uint8_t otpSet[]={OTPENABLE,OTPDISABLE};											//是否加载OTP配置集
  uint8_t dpiSet[]={HIGH_RESOLUTION_DATA,LOW_RESOLUTION_DATA};	//数据分辨率配置集
	
  if((sht==NULL)||(setSckPin==NULL)||(setDataPin==NULL)||(readSDA==NULL)||(delayus==NULL)||(delayms==NULL))
  {
    return;
  }
	
  setBusPin[0]=setSckPin;
  setBusPin[1]=setDataPin;
  sht->SetBusPin=setBusPin;
  sht->ReadSDABit=readSDA;
  sht->Delayus=delayus;
  sht->Delayms=delayms;
	
  if(direction!=NULL)
  {
    sht->SDADirection=direction;
  }
  else
  {
    sht->SDADirection=DefaultSDADirection;
  }
	
  /*初始化速度,默认100K*/
  if((sck>0)&&(sck<=500))
  {
    sht->period=500/sck;
  }
  else
  {
    sht->period=5;
  }
	
  sht->temperature=0.0;
  sht->humidity=0.0;
  sht->dewPoint=0.0;
  sht->vdd=vdd;
  sht->tempUnit=uint;
  
  regSetup=regSetup|heaterSet[heater]|otpSet[otp]|dpiSet[resolution];
  
  WriteStatusRegister(sht,&regSetup);
  
  sht->Delayms(10);
  
  ReadStatusRegister(sht);
}

/*计算获取电压温度系数d1*/
static float CalcVoltageTempCoefficient(Sht1xObjectType *sht)
{
  float d1=0.0;
  float coff[2][5]={{-40.1,-39.8,-39.7,-39.6,-39.4},{-40.2,-39.6,-39.5,-39.3,-38.9}};
  float x1,y1,x2,y2;
  
  if((4.0<sht->vdd)&&(sht->vdd<=5.0))
  {
    x1=5.0;
    y1=coff[sht->tempUnit][0];
    x2=5.0;
    y2=coff[sht->tempUnit][1];
  }
  if((3.5<sht->vdd)&&(sht->vdd<=4.0))
  {
    x1=4.0;
    y1=coff[sht->tempUnit][1];
    x2=3.5;
    y2=coff[sht->tempUnit][2];
  }
  if((3.0<sht->vdd)&&(sht->vdd<=3.5))
  {
    x1=3.5;
    y1=coff[sht->tempUnit][2];
    x2=3.0;
    y2=coff[sht->tempUnit][3];
  }
  if((2.5<=sht->vdd)&&(sht->vdd<=3.0))
  {
    x1=3.0;
    y1=coff[sht->tempUnit][3];
    x2=2.5;
    y2=coff[sht->tempUnit][4];
  }
  else
  {
    return 0.0;
  }
	
  d1=(y1-y2)*(sht->vdd-y2)/(x1-x2)+y2;
	
  return d1;
}

/*温度数据转换,sot:数字量温度；vdd:电源电压*/
static float ConvertTemperatureData(Sht1xObjectType *sht,uint16_t sot)
{
  float temp=0.0;
  uint8_t row=0;
	
  /*定义转换公式中的系数*/
  float d1=0.0;
  float d2[2][2]={{0.01,0.018},{0.04,0.072}};

  row=sht->statusReg&0x01;

  /*摄氏温度时d1的取值*/
  d1=CalcVoltageTempCoefficient(sht);
  
  temp=d1+d2[row][sht->tempUnit]*sot;
  return temp;
}

/*湿度数据转换,sorh:数字量湿度；temp:补偿温度*/
static float ConvertHumidityData(Sht1xObjectType *sht,uint16_t sorh)
{
  float rhTrue=0.0;
  float rhLinear=0.0;
  uint8_t index=0;
  
  /*定义转换公式中的系数*/
  float c1[2]={-2.0468,-2.0468};
  float c2[2]={0.0367,0.5872};
  float c3[2]={-0.0000015955,-0.00040845};
  float t1[2]={0.01,0.01};
  float t2[2]={0.00008,0.00128};
	
  index=sht->statusReg&0x01;

  rhLinear=c1[index]+c2[index]*sorh+c3[index]*sorh*sorh;
  rhTrue=(sht->temperature-25)*(t1[index]+t2[index]*sorh)+rhLinear;
  
  if(rhTrue>=100.0)
  {
    rhTrue=100.0;
  }
  
  if(rhTrue<=0.1)
  {
    rhTrue=0.1;
  }
  
  return rhTrue;
}

/*根据指定的多项式计算CRC校验*/
static bool CheckCRC8ForSHT1x(uint8_t *data, uint8_t numOfBytes, uint8_t checksum)
{
  uint8_t crc = 0x00;
  uint8_t bit;
  bool error;

  for(uint8_t i = 0; i < numOfBytes; i++)
  {
    crc ^= (data[i]);
    for ( bit = 8; bit > 0; --bit)
    {
      if (crc & 0x80)
      {
        crc = (crc << 1) ^ SHT1xPOLYNOMIAL;
      }
      else
      {
        crc = (crc << 1);
      }
    }
  }
  error=(crc != checksum)?false:true;
  return error;
}

/* 缺省IO方向控制函数 */
static void DefaultSDADirection(SHT1xIOModeType direction)
{
  //在引脚采用开漏输出时，使用此默认函数
}
/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
