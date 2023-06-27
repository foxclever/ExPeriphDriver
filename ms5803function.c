/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ms5803function.c                                               **/
/** 版    本：V1.1.0                                                         **/
/** 简    介：用于实现压力变送器（MS5803）的相关操作                         **/
/**           MS5803压力变送器使用I2C或SPI接口通讯方式，由硬件接线决定       **/
/**           MS5803采用SPI通讯时，支持Mode0和Mode3                          **/
/**           MS5803采用I2C通讯的命令格式如下：                              **/
/**           +-+--------------+-+-+--------------+-+-+                      **/
/**           |S|Device Address|W|A|    Command   |A|P|                      **/
/**           +-+--------------+-+-+--------------+-+-+                      **/
/**           接收内存寄存器数据的报文格式如下：                             **/
/**           +-+--------------+-+-+--------------+-+--------------+-+-+     **/
/**           |S|Device Address|R|A|Menory bit15-8|A|Menory bit15-8|N|P|     **/
/**           +-+--------------+-+-+--------------+-+--------------+-+-+     **/
/**           ADC数据接收的报文格式如下：                                    **/
/**      +-+--------------+-+-+------------+-+-----------+-+----------+-+-+  **/
/**      |S|Device Address|R|A| Data 23-16 |A| Data 15-8 |A| Data 7-0 |N|P|  **/
/**      +-+--------------+-+-+------------+-+-----------+-+----------+-+-+  **/
/**           S=Start Condition   W=Write   A=Acknowledge                    **/
/**           P=Stop Condition    R=Read    N=Not Acknowledge                **/
/**           其中Device Address为7位，Command为8位                          **/
/**           使用I2C通讯时，最多带2台。地址为0xEC和0xEE                     **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    1、声明MS5803ObjectType对象实体，并调用函数MS5803Initialization初始化 **/
/**       对象实体。需要给定类型，类型由enum MS5803Model定义。给定通讯接口， **/
/**       通讯接口由enum MS5803Port定义。如果通讯接口为I2C则需要给定设备地   **/
/**       址，若为SPI接口则应赋值ChipSelcet指针。并读取校准数据              **/
/**    2、实现形如uint8_t f(struct MS5803Object *ms,uint8_t command)的函数， **/
/**       并将其赋值给MS5803ObjectType对象实体的SendCommand函数指针。实现命  **/
/**       令的发送。                                                         **/
/**    3、实现形如void f(struct MS5803Object *ms,uint8_t *rData,             **/
/**       uint16_t rSize)的函数，并将其赋值给MS5803ObjectType对象实体的      **/
/**       GetDatas函数指针。实现数据的读取                                   **/
/**    4、若使用SPI端口，则实现形如void f(bool en)的函数，并将其赋值给       **/
/**       MS5803ObjectType对象实体的ChipSelcet函数指针。实现片选信号。若采   **/
/**       用I2C接口，则ChipSelcet函数指针会自动赋值为NULL。                  **/
/**    5、实现形如void f(volatile uint32_t nTime)的函数，并将其赋值给        **/
/**       MS5837ObjectType对象实体的Delayms函数指针。实现操作延时，单位毫秒  **/
/**    6、调用相应的函数实现需要的操作。                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-01-16          木南              创建文件               **/
/**     V1.1.0  2019-05-26          木南              修改为对象操作         **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "MS5803function.h"

#define COMMAND_RESET 0x1E
#define COMMAND_CONVERTD1OSR256 0x40
#define COMMAND_CONVERTD1OSR512 0x42
#define COMMAND_CONVERTD1OSR1024 0x44
#define COMMAND_CONVERTD1OSR2048 0x46
#define COMMAND_CONVERTD1OSR4096 0x48
#define COMMAND_CONVERTD2OSR256 0x50
#define COMMAND_CONVERTD2OSR512 0x52
#define COMMAND_CONVERTD2OSR1024 0x54
#define COMMAND_CONVERTD2OSR2048 0x56
#define COMMAND_CONVERTD2OSR4096 0x58
#define COMMAND_ADC_READ 0x00
#define COMMAND_PROM_READ_CRC 0xA0
#define COMMAND_PROM_READ_C1 0xA2
#define COMMAND_PROM_READ_C2 0xA4
#define COMMAND_PROM_READ_C3 0xA6
#define COMMAND_PROM_READ_C4 0xA8
#define COMMAND_PROM_READ_C5 0xAA
#define COMMAND_PROM_READ_C6 0xAC

/*计算MS5803-01BA类型传感器最终的物理量值*/
static void Calc01BAActualValue(MS5803ObjectType *ms,uint32_t digitalPressure,uint32_t digitalTemperature);
/*计算MS5803-02BA类型传感器最终的物理量值*/
static void Calc02BAActualValue(MS5803ObjectType *ms,uint32_t digitalPressure,uint32_t digitalTemperature);
/*计算MS5803-05BA类型传感器最终的物理量值*/
static void Calc05BAActualValue(MS5803ObjectType *ms,uint32_t digitalPressure,uint32_t digitalTemperature);
/*计算MS5803-14BA类型传感器最终的物理量值*/
static void Calc14BAActualValue(MS5803ObjectType *ms,uint32_t digitalPressure,uint32_t digitalTemperature);
/*计算MS5803-30BA类型传感器最终的物理量值*/
static void Calc30BAActualValue(MS5803ObjectType *ms,uint32_t digitalPressure,uint32_t digitalTemperature);
/*读取MS5803内存寄存器的数据*/
static uint16_t ReadPromFromMS5803(MS5803ObjectType *ms,uint8_t command);
/*读取MS5803ADC的转换值*/
static uint32_t ReadConversionFromMS5803(MS5803ObjectType *ms,uint8_t command);
/*从MS5803的PROM中读取校准数据*/
static void GetMS5803CalibrationData(MS5803ObjectType *ms);
/* 默认的片选信号处理函数 */
static void MS5803ChipSelect(MS5803CSType cs);

/*定义校正计算函数调用指针*/
void (*CalculateActualValue[])(MS5803ObjectType *ms,uint32_t digitalPressure,uint32_t digitalTemperature)
={Calc01BAActualValue,Calc02BAActualValue,
Calc05BAActualValue,Calc14BAActualValue,
Calc30BAActualValue};

/*获取转换值，包括温度和压力*/
void GetMS5803ConversionValue(MS5803ObjectType *ms,MS5803OSRType osrT,MS5803OSRType osrP)
{
    uint32_t digitalPressureValue=0;
    uint32_t digitalTemperatureValue=0;
    
    uint8_t presOSR[5]={COMMAND_CONVERTD1OSR256,COMMAND_CONVERTD1OSR512,COMMAND_CONVERTD1OSR1024,COMMAND_CONVERTD1OSR2048,COMMAND_CONVERTD1OSR4096};
    uint8_t tempOSR[5]={COMMAND_CONVERTD2OSR256,COMMAND_CONVERTD2OSR512,COMMAND_CONVERTD2OSR1024,COMMAND_CONVERTD2OSR2048,COMMAND_CONVERTD2OSR4096};
    
    /*读取压力数据*/
    digitalPressureValue=ReadConversionFromMS5803(ms,presOSR[osrP]);
    
    ms->Delayms(10);
    
    /*读取温度数据*/
    digitalTemperatureValue=ReadConversionFromMS5803(ms,tempOSR[osrT]);
    
    /*将数字量代入公式计算最终的物理量值*/
    CalculateActualValue[ms->model](ms,digitalPressureValue,digitalTemperatureValue);
}

/*复位MS5803操作*/
void ResetForMS5803(MS5803ObjectType *ms)
{
    uint8_t command=COMMAND_RESET;
    /*下发复位命令*/
    ms->Write(ms,command);
    
    ms->Delayms(3);
}

/* MS5803对象初始化 */
void MS5803Initialization(MS5803ObjectType *ms,		//MS5803对象
                          MS5803ModelType model,	//类型
                          MS5803PortType port,		//通讯端口
                          uint8_t address,				//I2C设备地址
                          MS5803Write write,			//写数据函数
                          MS5803Read read,				//读数据函数
                          MS5803ChipSelcet cs,		//SPI片选信号
                          MS5803Delayms delayms		//毫秒延时
                              )
{
    if((ms==NULL)||(write==NULL)||(read==NULL)||(delayms==NULL))
    {
        return;
    }
    ms->Write=write;
    ms->Read=read;
    ms->Delayms=delayms;
    
    ms->model=model;
    ms->port=port;
    
    if(port==I2C)
    {
        if((address==0x76)||(address==0x77))
        {
            ms->devAddress=(address<<1);
        }
        else if((address==0xEC)||(address==0xEE))
        {
            ms->devAddress=address;
        }
        else
        {
            ms->devAddress=0x00;
        }
        
        ms->ChipSelcet=cs;
    }
    else
    {
        ms->devAddress=0xFF;
        
        if(cs==NULL)
        {
            ms->ChipSelcet=MS5803ChipSelect;
        }
        else
        {
            ms->ChipSelcet=cs;
        }
    }
    
    ms->pressure=0.0;
    ms->temperature=0.0;
    
    ResetForMS5803(ms);
    
    GetMS5803CalibrationData(ms);
}

/*从MS5803的PROM中读取校准数据*/
static void GetMS5803CalibrationData(MS5803ObjectType *ms)
{
    /*C1压力灵敏度*/
    ms->caliData[0]=ReadPromFromMS5803(ms,COMMAND_PROM_READ_C1);
    /*C2压力补偿值*/
    ms->caliData[1]=ReadPromFromMS5803(ms,COMMAND_PROM_READ_C2);
    /*C3压力灵敏度温度系数*/
    ms->caliData[2]=ReadPromFromMS5803(ms,COMMAND_PROM_READ_C3);
    /*C4压力补偿温度系数*/
    ms->caliData[3]=ReadPromFromMS5803(ms,COMMAND_PROM_READ_C4);
    /*C5参考温度*/
    ms->caliData[4]=ReadPromFromMS5803(ms,COMMAND_PROM_READ_C5);
    /*C6温度传感器温度系数*/
    ms->caliData[5]=ReadPromFromMS5803(ms,COMMAND_PROM_READ_C6);
}

/*计算MS5803-01BA类型传感器最终的物理量值*/
static void Calc01BAActualValue(MS5803ObjectType *ms,uint32_t digitalPressure,uint32_t digitalTemperature)
{
    uint16_t senst1=(int64_t)ms->caliData[0];        /*C1压力灵敏度*/
    uint16_t offt1=(int64_t)ms->caliData[1];         /*C2压力补偿值*/
    uint16_t tcs=(int64_t)ms->caliData[2];           /*C3压力灵敏度温度系数*/
    uint16_t tco=(int64_t)ms->caliData[3];           /*C4压力补偿温度系数*/
    uint16_t tref=(int64_t)ms->caliData[4];          /*C5参考温度*/
    uint16_t tempsens=(int64_t)ms->caliData[5];      /*C6温度传感器温度系数*/
    
    /*对温度进行一阶修正*/
    int64_t dT=0;
    int32_t temp=0;
    dT=(int64_t)(digitalTemperature-tref*pow(2,8));
    temp=(int32_t)(2000+dT*tempsens/pow(2,23));
    
    /*对压力进行一阶修正*/
    int64_t off=0;
    int64_t sens=0;
    int32_t pres=0;
    off=(int64_t)(offt1*pow(2,16)+(tco*dT)/pow(2,7));
    sens=(int64_t)(senst1*pow(2,15)+(tcs*dT)/pow(2,8));
    
    /*对温度和压力进行二阶修正*/
    int64_t t2=0;
    int64_t off2=0;
    int64_t sens2=0;  
    
    if(temp<2000)
    {
        t2=(int64_t)(dT*dT/pow(2,31));
        off2=(int64_t)(3*(temp-2000)*(temp-2000));
        sens2=(int64_t)(7*(temp-2000)*(temp-2000)/8);
        
        if(temp<-1500)
        {
            sens2=sens2+2*(temp+1500)*(temp+1500);
        }
    }
    else
    {
        if(temp>4500)
        {
            sens2=sens2-(temp-4500)*(temp-4500)/8;
        }
    }
    temp=temp-(int32_t)t2;
    off=off-off2;
    sens=sens-sens2;
    pres=(int32_t)(((int64_t)digitalPressure*sens/pow(2,21)-off)/pow(2,15));
    
    if((-4000<=temp)&&(temp<=12500))
    {
        ms->temperature=(float)temp/100.0;
    }
    if((10000<=pres)&&(pres<=190000))
    {
        ms->pressure=(float)pres/100.0;
    }
}

/*计算MS5803-02BA类型传感器最终的物理量值*/
static void Calc02BAActualValue(MS5803ObjectType *ms,uint32_t digitalPressure,uint32_t digitalTemperature)
{
    int64_t senst1=(int64_t)ms->caliData[0];        /*C1压力灵敏度*/
    int64_t offt1=(int64_t)ms->caliData[1];         /*C2压力补偿值*/
    int64_t tcs=(int64_t)ms->caliData[2];           /*C3压力灵敏度温度系数*/
    int64_t tco=(int64_t)ms->caliData[3];           /*C4压力补偿温度系数*/
    int64_t tref=(int64_t)ms->caliData[4];          /*C5参考温度*/
    int64_t tempsens=(int64_t)ms->caliData[5];      /*C6温度传感器温度系数*/
    
    /*对温度进行一阶修正*/
    int64_t dT=0;
    int32_t temp=0;
    dT=(int64_t)(digitalTemperature-tref*pow(2,8));
    temp=(int32_t)(2000+dT*tempsens/pow(2,23));
    
    /*对压力进行一阶修正*/
    int64_t off=0;
    int64_t sens=0;
    int32_t pres=0;
    off=(int64_t)(offt1*pow(2,17)+(tco*dT)/pow(2,6));
    sens=(int64_t)(senst1*pow(2,16)+(tcs*dT)/pow(2,7));
    
    /*对温度和压力进行二阶修正*/
    int64_t t2=0;
    int64_t off2=0;
    int64_t sens2=0;  
    
    if(temp<2000)
    {
        t2=(int64_t)(dT*dT/pow(2,31));
        off2=(int64_t)(61*(temp-2000)*(temp-2000)/16);
        sens2=(int64_t)(2*(temp-2000)*(temp-2000));
        
        if(temp<-1500)
        {
            off2=off2+20*(temp+1500)*(temp+1500);
            sens2=sens2+12*(temp+1500)*(temp+1500);
        }
    }
    temp=temp-(int32_t)t2;
    off=off-off2;
    sens=sens-sens2;
    pres=(int32_t)(((int64_t)digitalPressure*sens/pow(2,21)-off)/pow(2,15));
    
    if((-4000<=temp)&&(temp<=12500))
    {
        ms->temperature=(float)temp/100.0;
    }
    if((10000<=pres)&&(pres<=190000))
    {
        ms->pressure=(float)pres/100.0;
    }
}

/*计算MS5803-05BA类型传感器最终的物理量值*/
static void Calc05BAActualValue(MS5803ObjectType *ms,uint32_t digitalPressure,uint32_t digitalTemperature)
{
    int64_t senst1=(int64_t)ms->caliData[0];        /*C1压力灵敏度*/
    int64_t offt1=(int64_t)ms->caliData[1];         /*C2压力补偿值*/
    int64_t tcs=(int64_t)ms->caliData[2];           /*C3压力灵敏度温度系数*/
    int64_t tco=(int64_t)ms->caliData[3];           /*C4压力补偿温度系数*/
    int64_t tref=(int64_t)ms->caliData[4];          /*C5参考温度*/
    int64_t tempsens=(int64_t)ms->caliData[5];      /*C6温度传感器温度系数*/
    
    /*对温度进行一阶修正*/
    int64_t dT=0;
    int32_t temp=0;
    dT=(int64_t)(digitalTemperature-tref*pow(2,8));
    temp=(int32_t)(2000+dT*tempsens/pow(2,23));
    
    /*对压力进行一阶修正*/
    int64_t off=0;
    int64_t sens=0;
    int32_t pres=0;
    off=(int64_t)(offt1*pow(2,18)+(tco*dT)/pow(2,5));
    sens=(int64_t)(senst1*pow(2,17)+(tcs*dT)/pow(2,7));
    
    /*对温度和压力进行二阶修正*/
    int64_t t2=0;
    int64_t off2=0;
    int64_t sens2=0;  
    
    if(temp<2000)
    {
        t2=(int64_t)(3*dT*dT/pow(2,33));
        off2=(int64_t)(3*(temp-2000)*(temp-2000)/8);
        sens2=(int64_t)(7*(temp-2000)*(temp-2000)/8);
        
        if(temp<-1500)
        {
            sens2=sens2+3*(temp+1500)*(temp+1500);
        }
    }
    temp=temp-(int32_t)t2;
    off=off-off2;
    sens=sens-sens2;
    pres=(int32_t)(((int64_t)digitalPressure*sens/pow(2,21)-off)/pow(2,15));
    
    if((-4000<=temp)&&(temp<=12500))
    {
        ms->temperature=(float)temp/100.0;
    }
    if((0<=pres)&&(pres<=600000))
    {
        ms->pressure=(float)pres/100.0;
    }
}

/*计算MS5803-14BA类型传感器最终的物理量值*/
static void Calc14BAActualValue(MS5803ObjectType *ms,uint32_t digitalPressure,uint32_t digitalTemperature)
{
    int64_t senst1=(int64_t)ms->caliData[0];        /*C1压力灵敏度*/
    int64_t offt1=(int64_t)ms->caliData[1];         /*C2压力补偿值*/
    int64_t tcs=(int64_t)ms->caliData[2];           /*C3压力灵敏度温度系数*/
    int64_t tco=(int64_t)ms->caliData[3];           /*C4压力补偿温度系数*/
    int64_t tref=(int64_t)ms->caliData[4];          /*C5参考温度*/
    int64_t tempsens=(int64_t)ms->caliData[5];      /*C6温度传感器温度系数*/
    
    /*对温度进行一阶修正*/
    int64_t dT=0;
    int32_t temp=0;
    dT=(int64_t)(digitalTemperature-tref*pow(2,8));
    temp=(int32_t)(2000+dT*tempsens/pow(2,23));
    
    /*对压力进行一阶修正*/
    int64_t off=0;
    int64_t sens=0;
    int32_t pres=0;
    off=(int64_t)(offt1*pow(2,16)+(tco*dT)/pow(2,7));
    sens=(int64_t)(senst1*pow(2,15)+(tcs*dT)/pow(2,8));
    
    /*对温度和压力进行二阶修正*/
    int64_t t2=0;
    int64_t off2=0;
    int64_t sens2=0;  
    
    if(temp<2000)
    {
        t2=(int64_t)(3*dT*dT/pow(2,33));
        off2=(int64_t)(3*(temp-2000)*(temp-2000)/2);
        sens2=(int64_t)(5*(temp-2000)*(temp-2000)/8);
        
        if(temp<-1500)
        {
            off2=off2+7*(temp+1500)*(temp+1500);
            sens2=sens2+4*(temp+1500)*(temp+1500);
        }
    }
    else
    {
        t2=(int64_t)(7*dT*dT/pow(2,37));
        off2=(int64_t)((temp-2000)*(temp-2000)/16);
    }
    temp=temp-(int32_t)t2;
    off=off-off2;
    sens=sens-sens2;
    pres=(int32_t)(((int64_t)digitalPressure*sens/pow(2,21)-off)/pow(2,15));
    
    if((-4000<=temp)&&(temp<=12500))
    {
        ms->temperature=(float)temp/10.0;
    }
    if((0<=pres)&&(pres<=190000))
    {
        ms->pressure=(float)pres/10.0;
    }
}

/*计算MS5803-30BA类型传感器最终的物理量值*/
static void Calc30BAActualValue(MS5803ObjectType *ms,uint32_t digitalPressure,uint32_t digitalTemperature)
{
    int64_t senst1=(int64_t)ms->caliData[0];        /*C1压力灵敏度*/
    int64_t offt1=(int64_t)ms->caliData[1];         /*C2压力补偿值*/
    int64_t tcs=(int64_t)ms->caliData[2];           /*C3压力灵敏度温度系数*/
    int64_t tco=(int64_t)ms->caliData[3];           /*C4压力补偿温度系数*/
    int64_t tref=(int64_t)ms->caliData[4];          /*C5参考温度*/
    int64_t tempsens=(int64_t)ms->caliData[5];      /*C6温度传感器温度系数*/
    
    /*对温度进行一阶修正*/
    int64_t dT=0;
    int32_t temp=0;
    dT=(int64_t)(digitalTemperature-tref*pow(2,8));
    temp=(int32_t)(2000+dT*tempsens/pow(2,23));
    
    /*对压力进行一阶修正*/
    int64_t off=0;
    int64_t sens=0;
    int32_t pres=0;
    off=(int64_t)(offt1*pow(2,16)+(tco*dT)/pow(2,7));
    sens=(int64_t)(senst1*pow(2,15)+(tcs*dT)/pow(2,8));
    
    /*对温度和压力进行二阶修正*/
    int64_t t2=0;
    int64_t off2=0;
    int64_t sens2=0;  
    
    if(temp<2000)
    {
        t2=(int64_t)(3*dT*dT/pow(2,33));
        off2=(int64_t)(3*(temp-2000)*(temp-2000)/2);
        sens2=(int64_t)(5*(temp-2000)*(temp-2000)/8);
        
        if(temp<-1500)
        {
            off2=off2+7*(temp+1500)*(temp+1500);
            sens2=sens2+4*(temp+1500)*(temp+1500);
        }
    }
    else
    {
        t2=(int64_t)(7*dT*dT/pow(2,37));
        off2=(int64_t)((temp-2000)*(temp-2000)/16);
    }
    temp=temp-(int32_t)t2;
    off=off-off2;
    sens=sens-sens2;
    pres=(int32_t)(((int64_t)digitalPressure*sens/pow(2,21)-off)/pow(2,15));
    
    if((-4000<=temp)&&(temp<=12500))
    {
        ms->temperature=(float)temp/10.0;
    }
    if((0<=pres)&&(pres<=300000))
    {
        ms->pressure=(float)pres/10.0;
    }
}

/*读取MS5803内存寄存器的数据*/
static uint16_t ReadPromFromMS5803(MS5803ObjectType *ms,uint8_t command)
{
    /*下发读取指定内存单元的命令*/
    ms->Write(ms,command);
    
    /*接收读取的指定内存单元的值*/
    uint8_t promValue[2];
    ms->Read(ms,promValue,2);
    
    uint16_t result=(uint16_t)promValue[0];
    result=(result<<8)+(uint16_t)promValue[1];
    
    return result;
}

/*读取MS5803ADC的转换值*/
static uint32_t ReadConversionFromMS5803(MS5803ObjectType *ms,uint8_t command)
{
    /*下发转化对象及精度配置命令*/
    ms->Write(ms,command);
    
    ms->Delayms(10);
    
    /*下发读取ADC的命令*/
    ms->Write(ms,COMMAND_ADC_READ);
    
    if(ms->port==I2C)
    {  
        ms->Delayms(10);
    } 
    
    /*接收读取的ADC转换结果*/
    uint8_t adcValue[3];
    ms->Read(ms,adcValue,3);
    
    uint32_t result=(uint32_t)adcValue[0];
    result=(result<<8)+(uint32_t)adcValue[1];
    result=(result<<8)+(uint32_t)adcValue[2];
    
    return result;
}

/* 默认的片选信号处理函数 */
static void MS5803ChipSelect(MS5803CSType cs)
{
    return;
}
/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
