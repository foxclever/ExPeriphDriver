/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：bme280function.c                                               **/
/** 版    本：V1.1.0                                                         **/
/** 简    介：用于实现BME280压力、温度及湿度传感器的操作                     **/
/**           BME280有55个可操作寄存器（包括26个校准数据）地址如下：         **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |寄存器名  |地址|bit7|bit6|bit5|bit4|bit3|bit2|bit1|bit0|复位状态|读写| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |hum_lsb   |0xFE|               hum_lsb<7:0>            |  0x00  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |hum_msb   |0xFD|               hum_msb<7:0>            |  0x80  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |temp_xlsb |0xFC|    temp_xlsb<7:4> | 0  | 0  | 0  | 0  |  0x00  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |temp_lsb  |0xFB|              temp_lsb<7:0>            |  0x00  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |temp_msb  |0xFA|              temp_msb<7:0>            |  0x80  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |press_xlsb|0xF9|   press_xlsb<7:4> | 0  | 0  | 0  | 0  |  0x00  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |press_lsb |0xF8|             press_lsb<7:0>            |  0x00  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |press_msb |0xF7|             press_msb<7:0>            |  0x80  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |config    |0xF5|   t_sb[2:0]  |  filter[2:0] |    |e[0]|  0x00  |读写| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |ctrl_meas |0xF4|  osrs_t[2:0] |  osrs_p[2:0] |mode[1:0]|  0x00  |读写| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  | status   |0xF3|                   |m[0]|         |u[0]|  0x00  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |ctrl_hum  |0xF2|                        |  osrs_h[2:0] |  0x00  |读写| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |calib41   |0xF0|        calibration data[共41]         |出厂设定|只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |calib26   |0xE1|        calibration data[共41]         |出厂设定|只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  | reset    |0xE0|             reset[7:0]                |  0x00  |只写| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |   id     |0xD0|             chip_id[7:0]              |  0x60  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |calib25   |0xA1|        calibration data[共41]         |出厂设定|只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |calib00   |0x88|        calibration data[共41]         |出厂设定|只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  控制测量寄存器压力测量和温度测量的配置：                                **/
/**    +-----------+----+------------+    +-----------+----+-------------+   **/
/**    |osrs_p[2:0]|倍数|    精度    |    |osrs_t[2:0]|倍数|     精度    |   **/
/**    +-----------+----+------------+    +-----------+----+-------------+   **/
/**    |   000     |  - |      -     |    |   000     |  - |     -       |   **/
/**    +-----------+----+------------+    +-----------+----+-------------+   **/
/**    |   001     | x1 |16bit/2.62Pa|    |   001     | x1 |16bit/0.0050C|   **/
/**    +-----------+----+------------+    +-----------+----+-------------+   **/
/**    |   010     | x2 |17bit/1.31Pa|    |   010     | x2 |17bit/0.0025C|   **/
/**    +-----------+----+------------+    +-----------+----+-------------+   **/
/**    |   011     | x4 |18bit/0.66Pa|    |   011     | x4 |18bit/0.0012C|   **/
/**    +-----------+----+------------+    +-----------+----+-------------+   **/
/**    |   100     | x8 |19bit/0.33Pa|    |   100     | x8 |19bit/0.0006C|   **/
/**    +-----------+----+------------+    +-----------+----+-------------+   **/
/**    |101,110,111| x16|20bit/0.16Pa|    |101,110,111| x16|20bit/0.0003C|   **/
/**    +-----------+----+------------+    +-----------+----+-------------+   **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    1、声明BME280ObjectType对象实体，并调用函数BME280Initialization初始化 **/
/**       对象实体。初始化时，需要传递如下参数：                             **/
/**       bmpAddress,采用I2C接口必须的设备地址，0xEC或0xEE。SPI接口时不需要  **/
/**       port,通讯接口类型，I2C或者SPI                                      **/
/**       t_sb,间隔周期，配置寄存器配置参数                                  **/
/**       filter,过滤器滤波设置，配置寄存器配置参数                          **/
/**       spi3W_en,是否适用3线SPI控制，配置寄存器配置参数                    **/
/**       osrs_t,温度采样精度设置，测量控制寄存器配置参数                    **/
/**       osrs_p,压力采样精度，测量控制寄存器配置参数                        **/
/**       mode,电源模式，测量控制寄存器配置参数                              **/
/**       osrs_h,湿度采样精度设置，湿度测量控制寄存器配置参数                **/
/**       Read,读数据操作指针，其函数类型见后续描述                          **/
/**       Write,写数据操作指针，其函数类型见后续描述                         **/
/**       Delayms,延时操作指针，其函数类型见后续描述                         **/
/**       ChipSelect,片选操作指针，SPI接口必需。I2C接口不需要，可置NULL      **/
/**    2、实现形如void f(struct BME280Object *bmp,uint8_t regAddress,        **/
/**       uint8_t *rData,uint16_t rSize)的函数，并将其赋值给BME280ObjectType **/
/**       对象实体的Read函数指针。                                           **/
/**    3、实现形如void f(struct BME280Object *bmp,uint8_t regAddress,        **/
/**       uint8_t command)的函数，并将其赋值给BME280ObjectType对象实体       **/
/**       的Write函数指针。                                                  **/
/**    4、实现形如void f(volatile uint32_t nTime)的函数，并将其赋值给        **/
/**       BME280ObjectType对象实体的Delayms函数指针。实现毫秒延时。          **/
/**    5、实现形如void f(bool en)的函数，并将其赋值BME280ObjectType对象实体  **/
/**       的ChipSelcet函数指针。                                             **/
/**    6、调用GetBME280Measure函数实现数据的读取。                           **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-07-17          木南              创建文件               **/
/**     V1.0.1  2018-05-20          木南              将设备定义为结构对象   **/
/**     V1.1.0  2018-06-01          木南              修改类型定义及数据返回 **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "bme280function.h"

/*寄存器地址定义*/
#define REG_HUMI_LSB	0xFE	/*湿度值寄存器低字节*/
#define REG_HUMI_MSB	0xFD	/*湿度值寄存器高字节*/
#define REG_TEMP_XLSB	0xFC	/*温度值寄存器最低字节*/
#define REG_TEMP_LSB	0xFB	/*温度值寄存器低字节*/
#define REG_TEMP_MSB	0xFA	/*温度值寄存器高字节*/
#define REG_PRESS_XLSB	0xF9	/*压力值寄存器最低字节*/
#define REG_PRESS_LSB	0xF8	/*压力值寄存器低字节*/
#define REG_PRESS_MSB	0xF7	/*压力值寄存器高字节*/
#define REG_CONFIG	0xF5	/*配置寄存器*/
#define REG_CTRL_MEAS	0xF4	/*测量控制寄存器*/
#define REG_STATUS	0xF3	/*状态寄存器*/
#define REG_CTRL_HUM	0xF2	/*湿度测量控制寄存器*/
#define REG_RESET	0xE0	/*复位寄存器*/
#define REG_BME280_ID   0xD0	/*设备ID寄存器*/

/*定义校准数据寄存器地址*/
#define REG_DIG_T1 0x88         /*温度校准系数1，无符号16位整数*/
#define REG_DIG_T2 0x8A         /*温度校准系数2，有符号16位整数*/
#define REG_DIG_T3 0x8C         /*温度校准系数3，有符号16位整数*/
#define REG_DIG_P1 0x8E         /*压力校准系数1，无符号16位整数*/
#define REG_DIG_P2 0x90         /*压力校准系数2，有符号16位整数*/
#define REG_DIG_P3 0x92         /*压力校准系数3，有符号16位整数*/
#define REG_DIG_P4 0x94         /*压力校准系数4，有符号16位整数*/
#define REG_DIG_P5 0x95         /*压力校准系数5，有符号16位整数*/
#define REG_DIG_P6 0x98         /*压力校准系数6，有符号16位整数*/
#define REG_DIG_P7 0x9A         /*压力校准系数7，有符号16位整数*/
#define REG_DIG_P8 0x9C         /*压力校准系数8，有符号16位整数*/
#define REG_DIG_P9 0x9E         /*压力校准系数9，有符号16位整数*/
#define REG_DIG_H1 0xA1         /*湿度校准系数1，无符号8位整数*/
#define REG_DIG_H2 0xE1         /*湿度校准系数2，有符号16位整数*/
#define REG_DIG_H3 0xE3         /*湿度校准系数3，无符号8位整数*/
#define REG_DIG_H4 0xE4         /*湿度校准系数4，有符号16位整数*/
#define REG_DIG_H5 0xE5         /*湿度校准系数5，有符号16位整数*/
#define REG_DIG_H6 0xE7         /*湿度校准系数6，有符号8位整数*/

/*判断对象是否有效*/
static bool ObjectIsValid(BME280ObjectType *bme);
/*计算实际温度值,精度0.01GegC*/
static double CompensateTemperature(BME280ObjectType *bme,uint32_t adcCode);
/*计算实际的压力值，修正后为Q24.8格式*/
static double CompensatePressure(BME280ObjectType *bme,uint32_t adcCode);
/*计算实际的湿度值，修正后为Q22.10格式*/
static double CompensateHumidity(BME280ObjectType *bme,uint32_t adcCode);
/*向BME280寄存器写一个字节*/
static void WriteBME280Register(BME280ObjectType *bme,uint8_t regAddress,uint8_t command);
/*从BME280寄存器读取一个字节*/
static uint8_t ReadBME280Register(BME280ObjectType *bme,uint8_t regAddress);
/*BME280软件复位*/
static void BME280SoftReset(BME280ObjectType *bme);
/*读取校准数据*/
static void GetBME280CalibrationData(BME280ObjectType *bme);
/*缺省片选处理函数*/
static void BME280ChipSelectDefault(BME280CSType cs);

/*读取压力和温度数据*/
void GetBME280Measure(BME280ObjectType *bme)
{
  uint8_t regAddress[]={REG_TEMP_XLSB,REG_TEMP_LSB,REG_TEMP_MSB,
                        REG_PRESS_XLSB,REG_PRESS_LSB,REG_PRESS_MSB,
                        REG_HUMI_LSB,REG_HUMI_MSB};
  uint8_t mData[8];
  uint32_t adcTemp=0;
  uint32_t adcPres=0;
  uint32_t adcHumi=0;
  
  if(!ObjectIsValid(bme))
  {
    return;
  }
  
  /*读取状态检测AD转换是否完成*/
  uint8_t status=0xFF;
  status=ReadBME280Register(bme,REG_STATUS);
  while(status&0x08)
  {
    status=ReadBME280Register(bme,REG_STATUS);
  }
  
  /*读取寄存器值*/
  for(int i=0;i<8;i++)
  {
    mData[i]=ReadBME280Register(bme,regAddress[i]);
  }

  /*计算压力和温度的ADC值*/
  for(int i=0;i<3;i++)
  {
    adcTemp=(adcTemp<<8)+mData[2-i];
    adcPres=(adcPres<<8)+mData[5-i];
  }
  adcHumi=((uint32_t)mData[6]<<8)+(uint32_t)mData[7];
  
  
  adcTemp=(adcTemp>>4);
  adcPres=(adcPres>>4);
  
  /*计算压力和温度的物理量值*/
  bme->temperature=(float)CompensateTemperature(bme,adcTemp);
  
  bme->pressure=(float)CompensatePressure(bme,adcPres);
  
  bme->humidity=(float)CompensateHumidity(bme,adcHumi);
}


/*实现BME280初始化配置*/
void BME280Initialization(BME280ObjectType *bme,        //BMP280对象
                          uint8_t bmeAddress,           //I2C接口是设备地址
                          BME280PortType port,          //接口选择
                          BME280TimeSBType t_sb,        //间隔周期
                          BME280IIRFilterType filter,   //过滤器
                          BME280SPI3wUseType spi3W_en,  //3线SPI控制
                          BME280TempSampleType osrs_t,  //温度精度
                          BME280PresSampleType osrs_p,  //压力精度
                          BME280PowerModeType mode,     //电源模式
                          BME280HumiSampleType osrs_h,  //湿度精度
                          BME280Read Read,              //读数据操作指针
                          BME280Write Write,            //写数据操作指针
                          BME280Delayms Delayms,        //延时操作指针
                          BME280ChipSelect ChipSelect   //片选操作指针
                         )
{
  uint8_t try_count = 5;
  uint8_t regAddress=0;
  uint8_t command=0; 
  
  if((bme==NULL)||(Read==NULL)||(Write==NULL)||(Delayms==NULL))
  {
		return;
  }
  bme->Read=Read;
  bme->Write=Write;
  bme->Delayms=Delayms;
  
  bme->chipID=0x00;
  bme->pressure=0.0;
  bme->temperature=0.0;
  bme->humidity=0.0;
  bme->bmeAddress=0x00;
  bme->port=port;
  if(bme->port==BME280_I2C)
  {
    if((bmeAddress==0xEC)||(bmeAddress==0xEE))
    {
      bme->bmeAddress=bmeAddress;
    }
    bme->ChipSelect=NULL;
  }
  else
  {
    if(ChipSelect!=NULL)
    {
      bme->ChipSelect=ChipSelect;
    }
    else
    {
      bme->ChipSelect=BME280ChipSelectDefault;
    }
  }
	
  bme->caliPara.t_fine=0;
		
  if(!ObjectIsValid(bme))
  {
    return;
  }
  	
  while(try_count--)
  {
    bme->chipID=ReadBME280Register(bme,REG_BME280_ID);
    if(0x60==bme->chipID)
    {
      BME280SoftReset(bme);
      
      break;
    }
  }
  
  if(try_count)
  {
    /*配置配置寄存器:间隔周期0.5ms、IIR滤波系数16、不使用SPI3线通讯*/
    regAddress=REG_CONFIG;
    command=(uint8_t)t_sb|(uint8_t)filter|(uint8_t)spi3W_en;
    WriteBME280Register(bme,regAddress,command);
  
    /*配置测量控制寄存器:温度20位，压力20位，电源正常模式*/
    regAddress=(uint8_t)(REG_CTRL_MEAS);
    command=(uint8_t)osrs_t|(uint8_t)osrs_p|(uint8_t)mode;
    WriteBME280Register(bme,regAddress,command);
  
    /*配置湿度测量控制寄存器*/
    regAddress=REG_CTRL_HUM;
    command=osrs_h;
    WriteBME280Register(bme,regAddress,command);
    
    bme->Delayms(10);
    bme->config=ReadBME280Register(bme,REG_CONFIG);
    bme->Delayms(10);
    bme->ctrlMeas=ReadBME280Register(bme,REG_CTRL_MEAS);
    bme->Delayms(10);
    bme->ctrlHumi=ReadBME280Register(bme,REG_CTRL_HUM);
    bme->Delayms(10);
		
    /*读取校准值*/
    GetBME280CalibrationData(bme);
  }
}

/*读取校准数据*/
static void GetBME280CalibrationData(BME280ObjectType *bme)
{
  uint8_t pData[2];
  
  if(!ObjectIsValid(bme))
  {
    return;
  }
	
  if(bme->port==BME280_SPI)
  {
    bme->ChipSelect(BME280CS_ENABLE);
    bme->Delayms(1);
  }
	
  bme->Read(bme,REG_DIG_T1,pData,2);
  bme->caliPara.dig_t1=(uint16_t)(((uint16_t)pData[1]<<8)+((uint16_t)pData[0]));
  
  bme->Read(bme,REG_DIG_T2,pData,2);
  bme->caliPara.dig_t2=(int16_t)(((int16_t)pData[1]<<8)+((int16_t)pData[0]));
  
  bme->Read(bme,REG_DIG_T3,pData,2);
  bme->caliPara.dig_t3=(int16_t)(((int16_t)pData[1]<<8)+((int16_t)pData[0]));
  
  bme->Read(bme,REG_DIG_P1,pData,2);
  bme->caliPara.dig_p1=(uint16_t)(((uint16_t)pData[1]<<8)+((uint16_t)pData[0]));
  
  bme->Read(bme,REG_DIG_P2,pData,2);
  bme->caliPara.dig_p2=(int16_t)(((int16_t)pData[1]<<8)+((int16_t)pData[0]));
  
  bme->Read(bme,REG_DIG_P3,pData,2);
  bme->caliPara.dig_p3=(int16_t)(((int16_t)pData[1]<<8)+((int16_t)pData[0]));
  
  bme->Read(bme,REG_DIG_P4,pData,2);
  bme->caliPara.dig_p4=(int16_t)(((int16_t)pData[1]<<8)+((int16_t)pData[0]));
  
  bme->Read(bme,REG_DIG_P5,pData,2);
  bme->caliPara.dig_p5=(int16_t)(((int16_t)pData[1]<<8)+((int16_t)pData[0]));
  
  bme->Read(bme,REG_DIG_P6,pData,2);
  bme->caliPara.dig_p6=(int16_t)(((int16_t)pData[1]<<8)+((int16_t)pData[0]));
  
  bme->Read(bme,REG_DIG_P7,pData,2);
  bme->caliPara.dig_p7=(int16_t)(((int16_t)pData[1]<<8)+((int16_t)pData[0]));
  
  bme->Read(bme,REG_DIG_P8,pData,2);
  bme->caliPara.dig_p8=(int16_t)(((int16_t)pData[1]<<8)+((int16_t)pData[0]));
  
  bme->Read(bme,REG_DIG_P9,pData,2);
  bme->caliPara.dig_p9=(int16_t)(((int16_t)pData[1]<<8)+((int16_t)pData[0]));
  
  bme->Read(bme,REG_DIG_H1,pData,1);
  bme->caliPara.dig_h1=pData[0];
  
  bme->Read(bme,REG_DIG_H2,pData,2);
  bme->caliPara.dig_h2=(int16_t)(((int16_t)pData[1]<<8)+((int16_t)pData[0]));
  
  bme->Read(bme,REG_DIG_H3,pData,1);
  bme->caliPara.dig_h3=pData[0];
  
  bme->Read(bme,REG_DIG_H4,pData,2);
  bme->caliPara.dig_h4=(int16_t)(((int16_t)pData[0]<<4)+(((int16_t)pData[1])&0x0F));
  
  bme->Read(bme,REG_DIG_H5,pData,2);
  bme->caliPara.dig_h5=(int16_t)(((int16_t)pData[1]<<4)+((int16_t)(pData[1]>>4)));
  
  bme->Read(bme,REG_DIG_H6,pData,1);
  bme->caliPara.dig_h6=pData[0];
	
  if(bme->port==BME280_SPI)
  {
    bme->Delayms(1);
    bme->ChipSelect(BME280CS_DISABLE);
  }
}

/*BME280软件复位*/
static void BME280SoftReset(BME280ObjectType *bme)
{
  uint8_t command=0xB6;
  
  if(ObjectIsValid(bme))
  {
    WriteBME280Register(bme,REG_RESET,command);
  }
}

/*从BMP280寄存器读取一个字节*/
static uint8_t ReadBME280Register(BME280ObjectType *bme,uint8_t regAddress)
{
  uint8_t regValue=0xFF;
  
  if(ObjectIsValid(bme))
  {
    if(bme->port==BME280_SPI)
    {
      regAddress |= 0x80;
      bme->ChipSelect(BME280CS_ENABLE);
      bme->Delayms(1);
      bme->Read(bme,regAddress,&regValue,1);
      bme->Delayms(1);
      bme->ChipSelect(BME280CS_DISABLE);
    }
    else
    {
      bme->Read(bme,regAddress,&regValue,1);
    }
  }

  return regValue;
}

/* 向BME280寄存器写一个字节 */
static void WriteBME280Register(BME280ObjectType *bme,uint8_t regAddress,uint8_t command)
{
  if(ObjectIsValid(bme))
  {
    if(bme->port==BME280_SPI)
    {
      regAddress&=0x7F;
      bme->ChipSelect(BME280CS_ENABLE);
      bme->Delayms(1);
      bme->Write(bme,regAddress,command);
      bme->Delayms(1);
      bme->ChipSelect(BME280CS_DISABLE);
    }
    else
    {
      bme->Write(bme,regAddress,command);
    }
  }
}

/*计算实际的压力值，修正后为Q24.8格式*/
static double CompensatePressure(BME280ObjectType *bme,uint32_t adcCode)
{
  double var1;
  double var2;
  double var3;
  double pressure;
  double pressure_min = 30000.0;
  double pressure_max = 110000.0;

  var1 = ((double)bme->caliPara.t_fine / 2.0) - 64000.0;
  var2 = var1 * var1 * ((double)bme->caliPara.dig_p6) / 32768.0;
  var2 = var2 + var1 * ((double)bme->caliPara.dig_p5) * 2.0;
  var2 = (var2 / 4.0) + (((double)bme->caliPara.dig_p4) * 65536.0);
  var3 = ((double)bme->caliPara.dig_p3) * var1 * var1 / 524288.0;
  var1 = (var3 + ((double)bme->caliPara.dig_p2) * var1) / 524288.0;
  var1 = (1.0 + var1 / 32768.0) * ((double)bme->caliPara.dig_p1);
  
  /*避免除零异常 */
  if (var1)
  {
    pressure = 1048576.0 - (double)adcCode;
    pressure = (pressure - (var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((double)bme->caliPara.dig_p9) * pressure * pressure / 2147483648.0;
    var2 = pressure * ((double)bme->caliPara.dig_p8) / 32768.0;
    pressure = pressure + (var1 + var2 + ((double)bme->caliPara.dig_p7)) / 16.0;

    if (pressure < pressure_min)
      pressure = pressure_min;
    else if (pressure > pressure_max)
      pressure = pressure_max;
  }
  else
  {
    pressure = pressure_min;
  }

  return pressure;
}

/*计算实际温度值,精度0.01GegC*/
static double CompensateTemperature(BME280ObjectType *bme,uint32_t adcCode)
{
  double var1;
  double var2;
  double temperature;
  double temperature_min = -40;
  double temperature_max = 85;

  var1 = ((double)adcCode) / 16384.0 - ((double)bme->caliPara.dig_t1) / 1024.0;
  var1 = var1 * ((double)bme->caliPara.dig_t2);
  var2 = (((double)adcCode) / 131072.0 - ((double)bme->caliPara.dig_t1) / 8192.0);
  var2 = (var2 * var2) * ((double)bme->caliPara.dig_t3);
  bme->caliPara.t_fine = (int32_t)(var1 + var2);
  temperature = (var1 + var2) / 5120.0;

  if (temperature < temperature_min)
    temperature = temperature_min;
  else if (temperature > temperature_max)
    temperature = temperature_max;

  return temperature;
}

/*计算实际的湿度值，修正后为Q22.10格式*/
static double CompensateHumidity(BME280ObjectType *bme,uint32_t adcCode)
{
  double humidity;
  double humidity_min = 0.0;
  double humidity_max = 100.0;
  double var1;
  double var2;
  double var3;
  double var4;
  double var5;
  double var6;

  var1 = ((double)bme->caliPara.t_fine) - 76800.0;
  var2 = (((double)bme->caliPara.dig_h4) * 64.0 + (((double)bme->caliPara.dig_h5) / 16384.0) * var1);
  var3 = adcCode - var2;
  var4 = ((double)bme->caliPara.dig_h2) / 65536.0;
  var5 = (1.0 + (((double)bme->caliPara.dig_h3) / 67108864.0) * var1);
  var6 = 1.0 + (((double)bme->caliPara.dig_h6) / 67108864.0) * var1 * var5;
  var6 = var3 * var4 * (var5 * var6);
  humidity = var6 * (1.0 - ((double)bme->caliPara.dig_h1) * var6 / 524288.0);

  if (humidity > humidity_max)
    humidity = humidity_max;
  else if (humidity < humidity_min)
    humidity = humidity_min;

  return humidity;
}

/*判断对象是否有效*/
static bool ObjectIsValid(BME280ObjectType *bme)
{
  bool valid=true;
  
  if((bme==NULL)||(bme->Delayms==NULL)||(bme->Read==NULL)||(bme->Write==NULL))
  {
    valid=false;
  }
  
  if((bme->port==BME280_SPI)&&(bme->ChipSelect==NULL))
  {
    valid=false;
  }
	
  return valid;
}

/*缺省片选处理函数*/
static void BME280ChipSelectDefault(BME280CSType cs)
{
  //用于在SPI通讯时，片选信号硬件电路选中的情况
  return;
}
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
