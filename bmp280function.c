/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：bmp280function.c                                               **/
/** 版    本：V1.1.0                                                         **/
/** 简    介：用于实现BMP280压力及温度传感器的操作                           **/
/**           BMP280有37个可操作寄存器（包括26个校准数据）地址如下：         **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |寄存器名  |地址|bit7|bit6|bit5|bit4|bit3|bit2|bit1|bit0|复位状态|读写| **/
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
/**  | reset    |0xE0|             reset[7:0]                |  0x00  |只写| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |   id     |0xD0|             chip_id[7:]               |  0x58  |只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |calib25   |0xA1|        calibration data[共26]         |出厂设定|只读| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |calib00   |0x88|        calibration data[共26]         |出厂设定|只读| **/
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
/**使用说明：                                                                **/
/**    1、声明BMP280ObjectType对象实体，并调用函数BMP280Initialization初始化 **/
/**       对象实体。初始化时，需要传递如下参数：                             **/
/**       bmpAddress,采用I2C接口必须的设备地址，0xEC或0xEE。SPI接口时不需要  **/
/**       port,通讯接口类型，I2C或者SPI                                      **/
/**       t_sb,间隔周期，配置寄存器配置参数                                  **/
/**       filter,过滤器滤波设置，配置寄存器配置参数                          **/
/**       spi3W_en,是否适用3线SPI控制，配置寄存器配置参数                    **/
/**       osrs_t,温度采样精度设置，测量控制寄存器配置参数                    **/
/**       osrs_p,压力采样精度，测量控制寄存器配置参数                        **/
/**       mode,电源模式，测量控制寄存器配置参数                              **/
/**       Read,读数据操作指针，其函数类型见后续描述                          **/
/**       Write,写数据操作指针，其函数类型见后续描述                         **/
/**       Delayms,延时操作指针，其函数类型见后续描述                         **/
/**       ChipSelect,片选操作指针，SPI接口必需。I2C接口不需要，可置NULL      **/
/**    2、实现形如void f(struct BMP280Object *bmp,uint8_t regAddress,        **/
/**       uint8_t *rData,uint16_t rSize)的函数，并将其赋值给BMP280ObjectType **/
/**       对象实体的Read函数指针。                                           **/
/**    3、实现形如void f(struct BMP280Object *bmp,uint8_t regAddress,        **/
/**       uint8_t command)的函数，并将其赋值给BMP280ObjectType对象实体       **/
/**       的Write函数指针。                                                  **/
/**    4、实现形如void f(volatile uint32_t nTime)的函数，并将其赋值给        **/
/**       BMP280ObjectType对象实体的Delayms函数指针。实现毫秒延时。          **/
/**    5、实现形如void f(bool en)的函数，并将其赋值BMP280ObjectType对象实体  **/
/**       的ChipSelcet函数指针。                                             **/
/**    6、调用相应的函数实现需要的操作。                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-07-17          木南              创建文件               **/
/**     V1.0.1  2018-05-20          木南              将设备定义为结构对象   **/
/**     V1.1.0  2019-05-30          木南              修改并增加类型定义     **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "bmp280function.h"

/*寄存器地址定义*/
#define REG_TEMP_XLSB	0xFC	/*温度值寄存器最低字节*/
#define REG_TEMP_LSB	0xFB	/*温度值寄存器低字节*/
#define REG_TEMP_MSB	0xFA	/*温度值寄存器高字节*/
#define REG_PRESS_XLSB	0xF9	/*压力值寄存器最低字节*/
#define REG_PRESS_LSB	0xF8	/*压力值寄存器低字节*/
#define REG_PRESS_MSB	0xF7	/*压力值寄存器高字节*/
#define REG_CONFIG	0xF5	/*配置寄存器*/
#define REG_CTRL_MEAS	0xF4	/*测量控制寄存器*/
#define REG_STATUS	0xF3	/*状态寄存器*/
#define REG_RESET	0xE0	/*复位寄存器*/
#define REG_BMP280_ID   0xD0	/*设备ID寄存器*/

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

/*判断对象是否有效*/
static bool ObjectIsValid(BMP280ObjectType *bmp);
/*计算实际温度值,精度0.01GegC*/
static double CompensateTemperature(BMP280ObjectType *bmp,uint32_t adcCode);
/*计算实际的压力值，修正后为Q24.8格式*/
static double CompensatePressure(BMP280ObjectType *bmp,uint32_t adcCode);
/*读取校准数据*/
static void GetBMP280CalibrationData(BMP280ObjectType *bmp);
/*读取BMP280寄存器*/
static uint8_t ReadBMP280Register(BMP280ObjectType *bmp,uint8_t regAddress);
/* 向BMP280寄存器写一个字节 */
static void WriteBMP280Register(BMP280ObjectType *bmp,uint8_t regAddress,uint8_t command);
/*BMP280软件复位*/
static void BMP280SoftReset(BMP280ObjectType *bmp);

/* 读取压力和温度数据 */
void GetBMP280Measure(BMP280ObjectType *bmp)
{
    uint8_t regAddress[]={REG_TEMP_XLSB,REG_TEMP_LSB,REG_TEMP_MSB,
    REG_PRESS_XLSB,REG_PRESS_LSB,REG_PRESS_MSB};
    uint8_t mData[6];
    uint32_t adcTemp=0;
    uint32_t adcPres=0;
    uint16_t timeOut=0;
    
    if(!ObjectIsValid(bmp))
    {
        return;
    }
    
    /*读取状态检测AD转换是否完成*/
    uint8_t status=0xFF;
    status=ReadBMP280Register(bmp,REG_STATUS);
    while((status&0x08)&&(timeOut<100))
    {
        timeOut++;
        status=ReadBMP280Register(bmp,REG_STATUS);
    }
    
    if(timeOut>=100)
    {
        return;
    }
    
    /*读取寄存器值*/
    for(int i=0;i<6;i++)
    {
        mData[i]=ReadBMP280Register(bmp,regAddress[i]);
    }
    
    /*计算压力和温度的ADC值*/
    for(int i=0;i<3;i++)
    {
        adcTemp=(adcTemp<<8)+mData[2-i];
        adcPres=(adcPres<<8)+mData[5-i];
    }
    
    adcTemp=(adcTemp>>4);
    adcPres=(adcPres>>4);
    
    /*计算压力和温度的物理量值*/
    bmp->temperature=CompensateTemperature(bmp,adcTemp);
    
    bmp->pressure=CompensatePressure(bmp,adcPres);
}

/* 实现BMP280初始化配置 */
void BMP280Initialization(BMP280ObjectType *bmp,                //BMP280对象
                          uint8_t bmpAddress,                   //I2C接口是设备地址
                          BMP280PortType port,                  //接口选择
                          BMP280TimeStandbyType t_sb,           //间隔周期
                          BMP280IIRFilterCoeffType filter,      //过滤器
                          BMP280UseSPI3wType spi3W_en,          //3线SPI控制
                          BMP280TemperatureSampleType osrs_t,   //温度精度
                          BMP280PressureSampleType osrs_p,      //压力精度
                          BMP280PowerModeType mode,             //电源模式
                          BMP280Read Read,                      //读数据操作指针
                          BMP280Write Write,                    //写数据操作指针
                          BMP280Delayms Delayms,                //延时操作指针
                          BMP280ChipSelect ChipSelect           //片选操作指针
                              )
{
    uint8_t try_count = 5;
    uint8_t regAddress=0;
    uint8_t command=0; 
    
    if((bmp==NULL)||(Read==NULL)||(Write==NULL)||(Delayms==NULL))
    {
        return;
    }
    bmp->Read=Read;
    bmp->Write=Write;
    bmp->Delayms=Delayms;
    
    bmp->chipID=0x00;
    bmp->pressure=0.0;
    bmp->temperature=0.0;
    bmp->bmpAddress=0x00;
    bmp->port=port;
    if(bmp->port==BMP280_I2C)
    {
        if((bmpAddress==0xEC)||(bmpAddress==0xEE))
        {
            bmp->bmpAddress=bmpAddress;
        }
        bmp->ChipSelect=NULL;
    }
    else
    {
        if(ChipSelect!=NULL)
        {
            bmp->ChipSelect=ChipSelect;
        }
        else
        {
            
        }
    }
    
    bmp->caliPara.t_fine=0;
    
    if(!ObjectIsValid(bmp))
    {
        return;
    }
    
    while(try_count--)
    {
        bmp->chipID=ReadBMP280Register(bmp,REG_BMP280_ID);
        if(0x58==bmp->chipID)
        {
            BMP280SoftReset(bmp);
            
            break;
        }
    }
    
    if(try_count)
    {
        /*配置配置寄存器:间隔周期0.5ms、IIR滤波系数16、不使用SPI3线通讯*/
        regAddress=REG_CONFIG;
        command=(uint8_t)t_sb|(uint8_t)filter|(uint8_t)spi3W_en;
        WriteBMP280Register(bmp,regAddress,command);
        
        /*配置测量控制寄存器:温度20位，压力20位，电源正常模式*/
        regAddress=REG_CTRL_MEAS;
        command=(uint8_t)osrs_t|(uint8_t)osrs_p|(uint8_t)mode;
        WriteBMP280Register(bmp,regAddress,command);
        
        bmp->Delayms(10);
        bmp->config=ReadBMP280Register(bmp,REG_CONFIG);
        bmp->Delayms(10);
        bmp->ctrlMeas=ReadBMP280Register(bmp,REG_CTRL_MEAS);
        bmp->Delayms(10);
        /*读取校准值*/
        GetBMP280CalibrationData(bmp);
    }
}

/*读取校准数据*/
static void GetBMP280CalibrationData(BMP280ObjectType *bmp)
{
    uint8_t pData[2];
    
    if(!ObjectIsValid(bmp))
    {
        return;
    }
    
    if(bmp->port==BMP280_SPI)
    {
        bmp->ChipSelect(BMP280CS_Enable);
        bmp->Delayms(1);
    }
    
    bmp->Read(bmp,REG_DIG_T1,pData,2);
    bmp->caliPara.dig_t1=(uint16_t)(((uint16_t)pData[1]<<8)+((uint16_t)pData[0]));
    
    bmp->Read(bmp,REG_DIG_T2,pData,2);
    bmp->caliPara.dig_t2=(int16_t)(((int16_t)pData[1]<<8)+((int16_t)pData[0]));
    
    bmp->Read(bmp,REG_DIG_T3,pData,2);
    bmp->caliPara.dig_t3=(int16_t)(((int16_t)pData[1]<<8)+((int16_t)pData[0]));
    
    bmp->Read(bmp,REG_DIG_P1,pData,2);
    bmp->caliPara.dig_p1=(uint16_t)(((uint16_t)pData[1]<<8)+((uint16_t)pData[0]));
    
    bmp->Read(bmp,REG_DIG_P2,pData,2);
    bmp->caliPara.dig_p2=(int16_t)(((int16_t)pData[1]<<8)+((int16_t)pData[0]));
    
    bmp->Read(bmp,REG_DIG_P3,pData,2);
    bmp->caliPara.dig_p3=(int16_t)(((int16_t)pData[1]<<8)+((int16_t)pData[0]));
    
    bmp->Read(bmp,REG_DIG_P4,pData,2);
    bmp->caliPara.dig_p4=(int16_t)(((int16_t)pData[1]<<8)+((int16_t)pData[0]));
    
    bmp->Read(bmp,REG_DIG_P5,pData,2);
    bmp->caliPara.dig_p5=(int16_t)(((int16_t)pData[1]<<8)+((int16_t)pData[0]));
    
    bmp->Read(bmp,REG_DIG_P6,pData,2);
    bmp->caliPara.dig_p6=(int16_t)(((int16_t)pData[1]<<8)+((int16_t)pData[0]));
    
    bmp->Read(bmp,REG_DIG_P7,pData,2);
    bmp->caliPara.dig_p7=(int16_t)(((int16_t)pData[1]<<8)+((int16_t)pData[0]));
    
    bmp->Read(bmp,REG_DIG_P8,pData,2);
    bmp->caliPara.dig_p8=(int16_t)(((int16_t)pData[1]<<8)+((int16_t)pData[0]));
    
    bmp->Read(bmp,REG_DIG_P9,pData,2);
    bmp->caliPara.dig_p9=(int16_t)(((int16_t)pData[1]<<8)+((int16_t)pData[0]));
    
    if(bmp->port==BMP280_SPI)
    {
        bmp->Delayms(1);
        bmp->ChipSelect(BMP280CS_Disable);
    }
}

/*从BMP280寄存器读取一个字节*/
static uint8_t ReadBMP280Register(BMP280ObjectType *bmp,uint8_t regAddress)
{
    uint8_t regValue=0xFF;
    
    if(ObjectIsValid(bmp))
    {
        if(bmp->port==BMP280_SPI)
        {
            regAddress |= 0x80;
            bmp->ChipSelect(BMP280CS_Enable);
            bmp->Delayms(1);
            bmp->Read(bmp,regAddress,&regValue,1);
            bmp->Delayms(1);
            bmp->ChipSelect(BMP280CS_Disable);
        }
        else
        {
            bmp->Read(bmp,regAddress,&regValue,1);
        }
    }
    
    return regValue;
}

/* 向BMP280寄存器写一个字节 */
static void WriteBMP280Register(BMP280ObjectType *bmp,uint8_t regAddress,uint8_t command)
{
    if(ObjectIsValid(bmp))
    {
        if(bmp->port==BMP280_SPI)
        {
            regAddress&=0x7F;
            bmp->ChipSelect(BMP280CS_Enable);
            bmp->Delayms(1);
            bmp->Write(bmp,regAddress,command);
            bmp->Delayms(1);
            bmp->ChipSelect(BMP280CS_Disable);
        }
        else
        {
            bmp->Write(bmp,regAddress,command);
        }
    }
}

/*BMP280软件复位*/
static void BMP280SoftReset(BMP280ObjectType *bmp)
{
    uint8_t command=0xB6;
    
    if(ObjectIsValid(bmp))
    {
        WriteBMP280Register(bmp,REG_RESET,command);
    }
}

/*计算实际的压力值，修正后为Q24.8格式*/
static double CompensatePressure(BMP280ObjectType *bmp,uint32_t adcCode)
{
    double var1,var2,pres;
    
    pres=0.0;
    
    var1=((double)bmp->caliPara.t_fine/2.0)-64000.0;
    var2=var1*var1*((double)bmp->caliPara.dig_p6)/32768.0;
    var2=var2+var1*((double)bmp->caliPara.dig_p5)*2.0;
    var2=(var2/4.0)+(((double)bmp->caliPara.dig_p4)*65536.0);
    var1=(((double)bmp->caliPara.dig_p3)*var1*var1/524288.0+((double)bmp->caliPara.dig_p2)*var1)/524288.0;
    var1=(1.0+var1/32768.0)*((double)bmp->caliPara.dig_p1);
    pres=1048576.0-(double)adcCode;
    
    if(var1 != 0)
    {
        pres=(pres-(var2/4096.0))*6250/var1;
        var1=((double)bmp->caliPara.dig_p9)*pres*pres/2147483648.0;
        var2=pres*((double)bmp->caliPara.dig_p8)/32768.0;
        pres=pres+(var1+var2+((double)bmp->caliPara.dig_p7))/16.0;
    }
    return pres;
}

/*计算实际温度值,精度0.01GegC*/
static double CompensateTemperature(BMP280ObjectType *bmp,uint32_t adcCode)
{
    double var1,var2;
    double temp;
    
    var1=(((double)adcCode)/16384.0-((double)bmp->caliPara.dig_t1)/1024.0)*((double)bmp->caliPara.dig_t2);
    var2=((((double)adcCode)/131072.0-((double)bmp->caliPara.dig_t1)/8192.0)
          *(((double)adcCode)/131072.0-((double)bmp->caliPara.dig_t1)/8192.0))
        *((double)bmp->caliPara.dig_t3);
    bmp->caliPara.t_fine=(int32_t)((var1+var2));
    temp=(double)((var1+var2)/5120.0);
    
    return temp;
}

/*判断对象是否有效*/
static bool ObjectIsValid(BMP280ObjectType *bmp)
{
    bool valid=true;
    
    if((bmp==NULL)||(bmp->Delayms==NULL)||(bmp->Read==NULL)||(bmp->Write==NULL))
    {
        valid=false;
    }
    
    if((bmp->port==BMP280_SPI)&&(bmp->ChipSelect==NULL))
    {
        valid=false;
    }
    
    return valid;
}

/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
