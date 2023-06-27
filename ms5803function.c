/******************************************************************************/
/** ģ�����ƣ���չ��������                                                   **/
/** �ļ����ƣ�ms5803function.c                                               **/
/** ��    ����V1.1.0                                                         **/
/** ��    �飺����ʵ��ѹ����������MS5803������ز���                         **/
/**           MS5803ѹ��������ʹ��I2C��SPI�ӿ�ͨѶ��ʽ����Ӳ�����߾���       **/
/**           MS5803����SPIͨѶʱ��֧��Mode0��Mode3                          **/
/**           MS5803����I2CͨѶ�������ʽ���£�                              **/
/**           +-+--------------+-+-+--------------+-+-+                      **/
/**           |S|Device Address|W|A|    Command   |A|P|                      **/
/**           +-+--------------+-+-+--------------+-+-+                      **/
/**           �����ڴ�Ĵ������ݵı��ĸ�ʽ���£�                             **/
/**           +-+--------------+-+-+--------------+-+--------------+-+-+     **/
/**           |S|Device Address|R|A|Menory bit15-8|A|Menory bit15-8|N|P|     **/
/**           +-+--------------+-+-+--------------+-+--------------+-+-+     **/
/**           ADC���ݽ��յı��ĸ�ʽ���£�                                    **/
/**      +-+--------------+-+-+------------+-+-----------+-+----------+-+-+  **/
/**      |S|Device Address|R|A| Data 23-16 |A| Data 15-8 |A| Data 7-0 |N|P|  **/
/**      +-+--------------+-+-+------------+-+-----------+-+----------+-+-+  **/
/**           S=Start Condition   W=Write   A=Acknowledge                    **/
/**           P=Stop Condition    R=Read    N=Not Acknowledge                **/
/**           ����Device AddressΪ7λ��CommandΪ8λ                          **/
/**           ʹ��I2CͨѶʱ������2̨����ַΪ0xEC��0xEE                     **/
/**--------------------------------------------------------------------------**/
/**ʹ��˵����                                                                **/
/**    1������MS5803ObjectType����ʵ�壬�����ú���MS5803Initialization��ʼ�� **/
/**       ����ʵ�塣��Ҫ�������ͣ�������enum MS5803Model���塣����ͨѶ�ӿڣ� **/
/**       ͨѶ�ӿ���enum MS5803Port���塣���ͨѶ�ӿ�ΪI2C����Ҫ�����豸��   **/
/**       ַ����ΪSPI�ӿ���Ӧ��ֵChipSelcetָ�롣����ȡУ׼����              **/
/**    2��ʵ������uint8_t f(struct MS5803Object *ms,uint8_t command)�ĺ����� **/
/**       �����丳ֵ��MS5803ObjectType����ʵ���SendCommand����ָ�롣ʵ����  **/
/**       ��ķ��͡�                                                         **/
/**    3��ʵ������void f(struct MS5803Object *ms,uint8_t *rData,             **/
/**       uint16_t rSize)�ĺ����������丳ֵ��MS5803ObjectType����ʵ���      **/
/**       GetDatas����ָ�롣ʵ�����ݵĶ�ȡ                                   **/
/**    4����ʹ��SPI�˿ڣ���ʵ������void f(bool en)�ĺ����������丳ֵ��       **/
/**       MS5803ObjectType����ʵ���ChipSelcet����ָ�롣ʵ��Ƭѡ�źš�����   **/
/**       ��I2C�ӿڣ���ChipSelcet����ָ����Զ���ֵΪNULL��                  **/
/**    5��ʵ������void f(volatile uint32_t nTime)�ĺ����������丳ֵ��        **/
/**       MS5837ObjectType����ʵ���Delayms����ָ�롣ʵ�ֲ�����ʱ����λ����  **/
/**    6��������Ӧ�ĺ���ʵ����Ҫ�Ĳ�����                                     **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2017-01-16          ľ��              �����ļ�               **/
/**     V1.1.0  2019-05-26          ľ��              �޸�Ϊ�������         **/
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

/*����MS5803-01BA���ʹ��������յ�������ֵ*/
static void Calc01BAActualValue(MS5803ObjectType *ms,uint32_t digitalPressure,uint32_t digitalTemperature);
/*����MS5803-02BA���ʹ��������յ�������ֵ*/
static void Calc02BAActualValue(MS5803ObjectType *ms,uint32_t digitalPressure,uint32_t digitalTemperature);
/*����MS5803-05BA���ʹ��������յ�������ֵ*/
static void Calc05BAActualValue(MS5803ObjectType *ms,uint32_t digitalPressure,uint32_t digitalTemperature);
/*����MS5803-14BA���ʹ��������յ�������ֵ*/
static void Calc14BAActualValue(MS5803ObjectType *ms,uint32_t digitalPressure,uint32_t digitalTemperature);
/*����MS5803-30BA���ʹ��������յ�������ֵ*/
static void Calc30BAActualValue(MS5803ObjectType *ms,uint32_t digitalPressure,uint32_t digitalTemperature);
/*��ȡMS5803�ڴ�Ĵ���������*/
static uint16_t ReadPromFromMS5803(MS5803ObjectType *ms,uint8_t command);
/*��ȡMS5803ADC��ת��ֵ*/
static uint32_t ReadConversionFromMS5803(MS5803ObjectType *ms,uint8_t command);
/*��MS5803��PROM�ж�ȡУ׼����*/
static void GetMS5803CalibrationData(MS5803ObjectType *ms);
/* Ĭ�ϵ�Ƭѡ�źŴ����� */
static void MS5803ChipSelect(MS5803CSType cs);

/*����У�����㺯������ָ��*/
void (*CalculateActualValue[])(MS5803ObjectType *ms,uint32_t digitalPressure,uint32_t digitalTemperature)
={Calc01BAActualValue,Calc02BAActualValue,
Calc05BAActualValue,Calc14BAActualValue,
Calc30BAActualValue};

/*��ȡת��ֵ�������¶Ⱥ�ѹ��*/
void GetMS5803ConversionValue(MS5803ObjectType *ms,MS5803OSRType osrT,MS5803OSRType osrP)
{
    uint32_t digitalPressureValue=0;
    uint32_t digitalTemperatureValue=0;
    
    uint8_t presOSR[5]={COMMAND_CONVERTD1OSR256,COMMAND_CONVERTD1OSR512,COMMAND_CONVERTD1OSR1024,COMMAND_CONVERTD1OSR2048,COMMAND_CONVERTD1OSR4096};
    uint8_t tempOSR[5]={COMMAND_CONVERTD2OSR256,COMMAND_CONVERTD2OSR512,COMMAND_CONVERTD2OSR1024,COMMAND_CONVERTD2OSR2048,COMMAND_CONVERTD2OSR4096};
    
    /*��ȡѹ������*/
    digitalPressureValue=ReadConversionFromMS5803(ms,presOSR[osrP]);
    
    ms->Delayms(10);
    
    /*��ȡ�¶�����*/
    digitalTemperatureValue=ReadConversionFromMS5803(ms,tempOSR[osrT]);
    
    /*�����������빫ʽ�������յ�������ֵ*/
    CalculateActualValue[ms->model](ms,digitalPressureValue,digitalTemperatureValue);
}

/*��λMS5803����*/
void ResetForMS5803(MS5803ObjectType *ms)
{
    uint8_t command=COMMAND_RESET;
    /*�·���λ����*/
    ms->Write(ms,command);
    
    ms->Delayms(3);
}

/* MS5803�����ʼ�� */
void MS5803Initialization(MS5803ObjectType *ms,		//MS5803����
                          MS5803ModelType model,	//����
                          MS5803PortType port,		//ͨѶ�˿�
                          uint8_t address,				//I2C�豸��ַ
                          MS5803Write write,			//д���ݺ���
                          MS5803Read read,				//�����ݺ���
                          MS5803ChipSelcet cs,		//SPIƬѡ�ź�
                          MS5803Delayms delayms		//������ʱ
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

/*��MS5803��PROM�ж�ȡУ׼����*/
static void GetMS5803CalibrationData(MS5803ObjectType *ms)
{
    /*C1ѹ��������*/
    ms->caliData[0]=ReadPromFromMS5803(ms,COMMAND_PROM_READ_C1);
    /*C2ѹ������ֵ*/
    ms->caliData[1]=ReadPromFromMS5803(ms,COMMAND_PROM_READ_C2);
    /*C3ѹ���������¶�ϵ��*/
    ms->caliData[2]=ReadPromFromMS5803(ms,COMMAND_PROM_READ_C3);
    /*C4ѹ�������¶�ϵ��*/
    ms->caliData[3]=ReadPromFromMS5803(ms,COMMAND_PROM_READ_C4);
    /*C5�ο��¶�*/
    ms->caliData[4]=ReadPromFromMS5803(ms,COMMAND_PROM_READ_C5);
    /*C6�¶ȴ������¶�ϵ��*/
    ms->caliData[5]=ReadPromFromMS5803(ms,COMMAND_PROM_READ_C6);
}

/*����MS5803-01BA���ʹ��������յ�������ֵ*/
static void Calc01BAActualValue(MS5803ObjectType *ms,uint32_t digitalPressure,uint32_t digitalTemperature)
{
    uint16_t senst1=(int64_t)ms->caliData[0];        /*C1ѹ��������*/
    uint16_t offt1=(int64_t)ms->caliData[1];         /*C2ѹ������ֵ*/
    uint16_t tcs=(int64_t)ms->caliData[2];           /*C3ѹ���������¶�ϵ��*/
    uint16_t tco=(int64_t)ms->caliData[3];           /*C4ѹ�������¶�ϵ��*/
    uint16_t tref=(int64_t)ms->caliData[4];          /*C5�ο��¶�*/
    uint16_t tempsens=(int64_t)ms->caliData[5];      /*C6�¶ȴ������¶�ϵ��*/
    
    /*���¶Ƚ���һ������*/
    int64_t dT=0;
    int32_t temp=0;
    dT=(int64_t)(digitalTemperature-tref*pow(2,8));
    temp=(int32_t)(2000+dT*tempsens/pow(2,23));
    
    /*��ѹ������һ������*/
    int64_t off=0;
    int64_t sens=0;
    int32_t pres=0;
    off=(int64_t)(offt1*pow(2,16)+(tco*dT)/pow(2,7));
    sens=(int64_t)(senst1*pow(2,15)+(tcs*dT)/pow(2,8));
    
    /*���¶Ⱥ�ѹ�����ж�������*/
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

/*����MS5803-02BA���ʹ��������յ�������ֵ*/
static void Calc02BAActualValue(MS5803ObjectType *ms,uint32_t digitalPressure,uint32_t digitalTemperature)
{
    int64_t senst1=(int64_t)ms->caliData[0];        /*C1ѹ��������*/
    int64_t offt1=(int64_t)ms->caliData[1];         /*C2ѹ������ֵ*/
    int64_t tcs=(int64_t)ms->caliData[2];           /*C3ѹ���������¶�ϵ��*/
    int64_t tco=(int64_t)ms->caliData[3];           /*C4ѹ�������¶�ϵ��*/
    int64_t tref=(int64_t)ms->caliData[4];          /*C5�ο��¶�*/
    int64_t tempsens=(int64_t)ms->caliData[5];      /*C6�¶ȴ������¶�ϵ��*/
    
    /*���¶Ƚ���һ������*/
    int64_t dT=0;
    int32_t temp=0;
    dT=(int64_t)(digitalTemperature-tref*pow(2,8));
    temp=(int32_t)(2000+dT*tempsens/pow(2,23));
    
    /*��ѹ������һ������*/
    int64_t off=0;
    int64_t sens=0;
    int32_t pres=0;
    off=(int64_t)(offt1*pow(2,17)+(tco*dT)/pow(2,6));
    sens=(int64_t)(senst1*pow(2,16)+(tcs*dT)/pow(2,7));
    
    /*���¶Ⱥ�ѹ�����ж�������*/
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

/*����MS5803-05BA���ʹ��������յ�������ֵ*/
static void Calc05BAActualValue(MS5803ObjectType *ms,uint32_t digitalPressure,uint32_t digitalTemperature)
{
    int64_t senst1=(int64_t)ms->caliData[0];        /*C1ѹ��������*/
    int64_t offt1=(int64_t)ms->caliData[1];         /*C2ѹ������ֵ*/
    int64_t tcs=(int64_t)ms->caliData[2];           /*C3ѹ���������¶�ϵ��*/
    int64_t tco=(int64_t)ms->caliData[3];           /*C4ѹ�������¶�ϵ��*/
    int64_t tref=(int64_t)ms->caliData[4];          /*C5�ο��¶�*/
    int64_t tempsens=(int64_t)ms->caliData[5];      /*C6�¶ȴ������¶�ϵ��*/
    
    /*���¶Ƚ���һ������*/
    int64_t dT=0;
    int32_t temp=0;
    dT=(int64_t)(digitalTemperature-tref*pow(2,8));
    temp=(int32_t)(2000+dT*tempsens/pow(2,23));
    
    /*��ѹ������һ������*/
    int64_t off=0;
    int64_t sens=0;
    int32_t pres=0;
    off=(int64_t)(offt1*pow(2,18)+(tco*dT)/pow(2,5));
    sens=(int64_t)(senst1*pow(2,17)+(tcs*dT)/pow(2,7));
    
    /*���¶Ⱥ�ѹ�����ж�������*/
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

/*����MS5803-14BA���ʹ��������յ�������ֵ*/
static void Calc14BAActualValue(MS5803ObjectType *ms,uint32_t digitalPressure,uint32_t digitalTemperature)
{
    int64_t senst1=(int64_t)ms->caliData[0];        /*C1ѹ��������*/
    int64_t offt1=(int64_t)ms->caliData[1];         /*C2ѹ������ֵ*/
    int64_t tcs=(int64_t)ms->caliData[2];           /*C3ѹ���������¶�ϵ��*/
    int64_t tco=(int64_t)ms->caliData[3];           /*C4ѹ�������¶�ϵ��*/
    int64_t tref=(int64_t)ms->caliData[4];          /*C5�ο��¶�*/
    int64_t tempsens=(int64_t)ms->caliData[5];      /*C6�¶ȴ������¶�ϵ��*/
    
    /*���¶Ƚ���һ������*/
    int64_t dT=0;
    int32_t temp=0;
    dT=(int64_t)(digitalTemperature-tref*pow(2,8));
    temp=(int32_t)(2000+dT*tempsens/pow(2,23));
    
    /*��ѹ������һ������*/
    int64_t off=0;
    int64_t sens=0;
    int32_t pres=0;
    off=(int64_t)(offt1*pow(2,16)+(tco*dT)/pow(2,7));
    sens=(int64_t)(senst1*pow(2,15)+(tcs*dT)/pow(2,8));
    
    /*���¶Ⱥ�ѹ�����ж�������*/
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

/*����MS5803-30BA���ʹ��������յ�������ֵ*/
static void Calc30BAActualValue(MS5803ObjectType *ms,uint32_t digitalPressure,uint32_t digitalTemperature)
{
    int64_t senst1=(int64_t)ms->caliData[0];        /*C1ѹ��������*/
    int64_t offt1=(int64_t)ms->caliData[1];         /*C2ѹ������ֵ*/
    int64_t tcs=(int64_t)ms->caliData[2];           /*C3ѹ���������¶�ϵ��*/
    int64_t tco=(int64_t)ms->caliData[3];           /*C4ѹ�������¶�ϵ��*/
    int64_t tref=(int64_t)ms->caliData[4];          /*C5�ο��¶�*/
    int64_t tempsens=(int64_t)ms->caliData[5];      /*C6�¶ȴ������¶�ϵ��*/
    
    /*���¶Ƚ���һ������*/
    int64_t dT=0;
    int32_t temp=0;
    dT=(int64_t)(digitalTemperature-tref*pow(2,8));
    temp=(int32_t)(2000+dT*tempsens/pow(2,23));
    
    /*��ѹ������һ������*/
    int64_t off=0;
    int64_t sens=0;
    int32_t pres=0;
    off=(int64_t)(offt1*pow(2,16)+(tco*dT)/pow(2,7));
    sens=(int64_t)(senst1*pow(2,15)+(tcs*dT)/pow(2,8));
    
    /*���¶Ⱥ�ѹ�����ж�������*/
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

/*��ȡMS5803�ڴ�Ĵ���������*/
static uint16_t ReadPromFromMS5803(MS5803ObjectType *ms,uint8_t command)
{
    /*�·���ȡָ���ڴ浥Ԫ������*/
    ms->Write(ms,command);
    
    /*���ն�ȡ��ָ���ڴ浥Ԫ��ֵ*/
    uint8_t promValue[2];
    ms->Read(ms,promValue,2);
    
    uint16_t result=(uint16_t)promValue[0];
    result=(result<<8)+(uint16_t)promValue[1];
    
    return result;
}

/*��ȡMS5803ADC��ת��ֵ*/
static uint32_t ReadConversionFromMS5803(MS5803ObjectType *ms,uint8_t command)
{
    /*�·�ת�����󼰾�����������*/
    ms->Write(ms,command);
    
    ms->Delayms(10);
    
    /*�·���ȡADC������*/
    ms->Write(ms,COMMAND_ADC_READ);
    
    if(ms->port==I2C)
    {  
        ms->Delayms(10);
    } 
    
    /*���ն�ȡ��ADCת�����*/
    uint8_t adcValue[3];
    ms->Read(ms,adcValue,3);
    
    uint32_t result=(uint32_t)adcValue[0];
    result=(result<<8)+(uint32_t)adcValue[1];
    result=(result<<8)+(uint32_t)adcValue[2];
    
    return result;
}

/* Ĭ�ϵ�Ƭѡ�źŴ����� */
static void MS5803ChipSelect(MS5803CSType cs)
{
    return;
}
/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
