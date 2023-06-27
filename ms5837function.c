/******************************************************************************/
/** ģ�����ƣ���չ��������                                                   **/
/** �ļ����ƣ�ms5837function.c                                               **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʵ��ѹ����������MS5837������ز���                         **/
/**           MS5837ѹ��������ʹ��I2C�ӿ�ͨѶ��ʽ                            **/
/**           MS5837ͨѶ�������ʽ���£�                                     **/
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
/**--------------------------------------------------------------------------**/
/**ʹ��˵����                                                                **/
/**    1������MS5837ObjectType����ʵ�壬�����ú���MS5837Initialization��ʼ�� **/
/**       ����ʵ�塣�����豸��ַ������ȡУ׼����                             **/
/**    2��ʵ������uint8_t f(struct MS5837Object *ms,uint8_t command)�ĺ����� **/
/**       �����丳ֵ��MS5837ObjectType����ʵ���SendCommand����ָ�롣ʵ����  **/
/**       ��ķ��͡�                                                         **/
/**    3��ʵ������void f(struct MS5837Object *ms,uint8_t *rData,             **/
/**       uint16_t rSize)�ĺ����������丳ֵ��MS5837ObjectType����ʵ���      **/
/**       GetDatas����ָ�롣ʵ�����ݵĶ�ȡ                                   **/
/**    4��ʵ������void f(volatile uint32_t nTime)�ĺ����������丳ֵ��        **/
/**       MS5837ObjectType����ʵ���Delayms����ָ�롣ʵ�ֲ�����ʱ����λ����  **/
/**    5��������Ӧ�ĺ���ʵ����Ҫ�Ĳ�����                                     **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2017-01-16          ľ��              �����ļ�               **/
/**     V1.0.0  2019-05-16          ľ��              ��Ϊ�������           **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "ms5837function.h"

#define COMMAND_RESET 0x1E
#define COMMAND_CONVERTD1OSR256 0x40
#define COMMAND_CONVERTD1OSR512 0x42
#define COMMAND_CONVERTD1OSR1024 0x44
#define COMMAND_CONVERTD1OSR2048 0x46
#define COMMAND_CONVERTD1OSR4096 0x48
#define COMMAND_CONVERTD1OSR8192 0x4A
#define COMMAND_CONVERTD2OSR256 0x50
#define COMMAND_CONVERTD2OSR512 0x52
#define COMMAND_CONVERTD2OSR1024 0x54
#define COMMAND_CONVERTD2OSR2048 0x56
#define COMMAND_CONVERTD2OSR4096 0x58
#define COMMAND_CONVERTD2OSR8192 0x5A
#define COMMAND_ADC_READ 0x00
#define COMMAND_PROM_READ_CRC 0xA0
#define COMMAND_PROM_READ_C1 0xA2
#define COMMAND_PROM_READ_C2 0xA4
#define COMMAND_PROM_READ_C3 0xA6
#define COMMAND_PROM_READ_C4 0xA8
#define COMMAND_PROM_READ_C5 0xAA
#define COMMAND_PROM_READ_C6 0xAC

/*��MS5837��PROM�ж�ȡУ׼����*/
static void GetCalibrationData(MS5837ObjectType *ms);
/*��ȡMS5837�ڴ�Ĵ���������*/
uint16_t ReadPromFromMs5837(MS5837ObjectType *ms,uint8_t command);
/*��ȡMS5837ADC��ת��ֵ*/
uint32_t ReadConversionFromMs5837(MS5837ObjectType *ms,uint8_t command);

/*��ȡת��ֵ�������¶Ⱥ�ѹ��*/
void GetMS5837ConversionValue(MS5837ObjectType *ms,MS5837OSRType pOSR,MS5837OSRType tOSR)
{
    uint8_t presOSR[6]={COMMAND_CONVERTD1OSR256,COMMAND_CONVERTD1OSR512,
    COMMAND_CONVERTD1OSR1024,COMMAND_CONVERTD1OSR2048,
    COMMAND_CONVERTD1OSR4096,COMMAND_CONVERTD1OSR8192};
    uint8_t tempOSR[6]={COMMAND_CONVERTD2OSR256,COMMAND_CONVERTD2OSR512,
    COMMAND_CONVERTD2OSR1024,COMMAND_CONVERTD2OSR2048,
    COMMAND_CONVERTD2OSR4096,COMMAND_CONVERTD2OSR8192};
    
    uint16_t senst1=ms->caliData[0];        //C1ѹ��������
    uint16_t offt1=ms->caliData[1];         //C2ѹ������ֵ
    uint16_t tcs=ms->caliData[2];           //C3ѹ���������¶�ϵ��
    uint16_t tco=ms->caliData[3];           //C4ѹ�������¶�ϵ��
    uint16_t tref=ms->caliData[4];          //C5�ο��¶�
    uint16_t tempsens=ms->caliData[5];      //C6�¶ȴ������¶�ϵ��
    
    uint32_t digitalPressureValue;
    uint32_t digitalTemperatureValue;
    
    /*��ȡѹ������*/
    digitalPressureValue=ReadConversionFromMs5837(ms,presOSR[pOSR]);
    
    ms->Delayms(20);
    
    /*��ȡ�¶�����*/
    digitalTemperatureValue=ReadConversionFromMs5837(ms,tempOSR[tOSR]);
    
    /*���¶Ƚ���һ������*/
    int32_t dT;
    int32_t temp;
    dT=digitalTemperatureValue-tref*256;
    temp=(int32_t)(2000+dT*tempsens/pow(2,23));
    
    /*��ѹ������һ������*/
    int64_t off;
    int64_t sens;
    int32_t pres;
    off=(int64_t)(offt1*pow(2,17)+(tco*dT)/pow(2,6));
    sens=(int64_t)(senst1*pow(2,16)+(tcs*dT)/pow(2,7));
    pres=(int32_t)((digitalPressureValue*sens/pow(2,21)-off)/pow(2,15));
    
    /*���¶Ⱥ�ѹ�����ж�������*/
    int64_t ti=0;
    int64_t offi=0;
    int64_t sensi=0;
    int64_t off2=0;
    int64_t sens2=0;  
    
    if(temp<2000)
    {
        ti=(int64_t)(11*dT*dT/pow(2,35));
        offi=(int64_t)(31*(temp-2000)*(temp-2000)/pow(2,3));
        sensi=(int64_t)(63*(temp-2000)*(temp-2000)/pow(2,5));
        
        off2=off-offi;
        sens2=sens-sensi;
        
        temp=temp-(int32_t)ti;
        pres=(int32_t)((digitalPressureValue*sens2/pow(2,21)-off2)/pow(2,15));
    }
    
    if((-4000<=temp)&&(temp<=8500))
    {
        ms->temperature=(float)temp/100.0f;
    }
    if((1000<=pres)&&(pres<=120000))
    {
        ms->pressure=(float)pres/100.0f;
    }
}

/*��λMS5837����*/
void ResetForMs5837(MS5837ObjectType *ms)
{
    uint8_t command=COMMAND_RESET;
    /*�·���λ����*/
    ms->Write(ms,command);
}

/* ��ʼ��MS5837���� */
void MS5837Initialization(MS5837ObjectType *ms,MS5837Write write,MS5837Read read,MS5837Delayms delayms)
{
    if((ms==NULL)||(write==NULL)||(read==NULL)||(delayms==NULL))
    {
        return;	
    }
    
    ms->Write=write;
    ms->Read=read;
    ms->Delayms=delayms;
    
    ms->devAddress=0xEC;
    ms->pressure=0.0;
    ms->temperature=0.0;
    
    ResetForMs5837(ms);
    
    GetCalibrationData(ms);
}

/*��MS5837��PROM�ж�ȡУ׼����*/
static void GetCalibrationData(MS5837ObjectType *ms)
{
    /*C1ѹ��������*/
    ms->caliData[0]=ReadPromFromMs5837(ms,COMMAND_PROM_READ_C1);
    /*C2ѹ������ֵ*/
    ms->caliData[1]=ReadPromFromMs5837(ms,COMMAND_PROM_READ_C2);
    /*C3ѹ���������¶�ϵ��*/
    ms->caliData[2]=ReadPromFromMs5837(ms,COMMAND_PROM_READ_C3);
    /*C4ѹ�������¶�ϵ��*/
    ms->caliData[3]=ReadPromFromMs5837(ms,COMMAND_PROM_READ_C4);
    /*C5�ο��¶�*/
    ms->caliData[4]=ReadPromFromMs5837(ms,COMMAND_PROM_READ_C5);
    /*C6�¶ȴ������¶�ϵ��*/
    ms->caliData[5]=ReadPromFromMs5837(ms,COMMAND_PROM_READ_C6);
}

/*��ȡMS5837�ڴ�Ĵ���������*/
static uint16_t ReadPromFromMs5837(MS5837ObjectType *ms,uint8_t command)
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

/*��ȡMS5837ADC��ת��ֵ*/
static uint32_t ReadConversionFromMs5837(MS5837ObjectType *ms,uint8_t command)
{
    /*�·�ת�����󼰾�����������*/
    ms->Write(ms,command);
    
    ms->Delayms(10);
    
    /*�·���ȡADC������*/
    ms->Write(ms,COMMAND_ADC_READ);
    
    ms->Delayms(10);
    
    /*���ն�ȡ��ADCת�����*/
    uint8_t adcValue[3];
    ms->Read(ms,adcValue,3);
    
    uint32_t result=(uint32_t)adcValue[0];
    result=(result<<8)+(uint32_t)adcValue[1];
    result=(result<<8)+(uint32_t)adcValue[2];
    
    return result;
}

/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
