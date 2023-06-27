/******************************************************************************/
/** ģ�����ƣ���չ��������                                                   **/
/** �ļ����ƣ�sht2xfunction.c                                                **/
/** ��    ����V1.0.1                                                         **/
/** ��    �飺����ʵ��SHT2X��ʪ�ȼƵĲ���                                    **/
/**           Ĭ�Ϸֱ���14bit(�¶�)��12bit(ʪ��)����ͨ���û��Ĵ����޸�       **/
/**           ���ʪ�ȵļ��㹫ʽ��RH=-6+125*SRH/Pow(2,16)���������Һ̬ˮ��  **/
/**           ���ʪ�ȣ������ڱ������ʪ�ȿ��Լ���õ���                     **/
/**           RHi=RHw*exp��17.62*t/(243.12+t))/exp(22.46*t/(272.62+t));      **/
/**           �¶ȵļ��㹫ʽ��T=-46.85+175.72*ST/Pow(2,16);                  **/
/**           ��ͬ�ֱ�����ADCת��ʱ�䣺                                      **/
/**           +------+--------+--------+-------+-------+----+                **/
/**           |�ֱ���|RH����ֵ|RH���ֵ|T����ֵ|T���ֵ|��λ|                **/
/**           +------+--------+--------+-------+-------+----+                **/
/**           | 14bit| ����   | ����   |  66   |  85   | ms |                **/
/**           +------+--------+--------+-------+-------+----+                **/
/**           | 13bit| ����   | ����   |  33   |  43   | ms |                **/
/**           +------+--------+--------+-------+-------+----+                **/
/**           | 12bit|   22   |   29   |  17   |  22   | ms |                **/
/**           +------+--------+--------+-------+-------+----+                **/
/**           | 11bit|   12   |   15   |   9   |  11   | ms |                **/
/**           +------+--------+--------+-------+-------+----+                **/
/**           | 10bit|   7    |    9   | ����  | ����  | ms |                **/
/**           +------+--------+--------+-------+-------+----+                **/
/**           | 8bit |   3    |    4   | ����  | ����  | ms |                **/
/**           +------+--------+--------+-------+-------+----+                **/
/**           SHT2x������룺                                                **/
/**           +------------+----------+----------+                           **/
/**           |    ����    |   ����   | ������� |                           **/
/**           +------------+----------+----------+                           **/
/**           | ����T����  | �������� | 11100011 |                           **/
/**           +------------+----------+----------+                           **/
/**           | ����RH���� | �������� | 11100101 |                           **/
/**           +------------+----------+----------+                           **/
/**           | ����T����  |�Ǳ�������| 11110011 |                           **/
/**           +------------+----------+----------+                           **/
/**           | ����RH���� |�Ǳ�������| 11110101 |                           **/
/**           +------------+----------+----------+                           **/
/**           |д�û��Ĵ���|          | 11100110 |                           **/
/**           +------------+----------+----------+                           **/
/**           |���û��Ĵ���|          | 11100111 |                           **/
/**           +------------+----------+----------+                           **/
/**           |   ��λ   |          | 11111110 |                           **/
/**           +------------+----------+----------+                           **/
/**--------------------------------------------------------------------------**/
/**ʹ��˵����                                                                **/
/**    1������SHT2xDevice����ʵ�壬�����ú���SHT2xInitialization��ʼ������   **/
/**       ʵ�塣SHT2x�豸��ַ�̶�Ϊ0x80                                      **/
/**    2��ʵ������void f(uint8_t devAddress,uint8_t *rData,uint16_t rSize)�� **/
/**       �����������丳ֵ��SHT2xDevice����ʵ���Receive����ָ�롣ʵ�����ݽ� **/
/**       �ա�                                                               **/
/**    3��ʵ������void f(uint8_t devAddress,uint8_t *tData,uint16_t tSize)�� **/
/**       �����������丳ֵ��SHT2xDevice����ʵ���Transmit����ָ�롣ʵ������  **/
/**       ���͡�                                                             **/
/**    4��ʵ������void f(volatile uint32_t nTime)�ĺ����������丳ֵ��        **/
/**       SHT2xDevice����ʵ���Delay����ָ�롣ʵ�ֲ�����ʱ����λ����         **/
/**    5��������Ӧ�ĺ���ʵ����Ҫ�Ĳ�����                                     **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2017-07-17          ľ��              �����ļ�               **/
/**     V1.0.1  2018-04-17          ľ��              ���SHT2x�����岢��  **/
/**                                                   �ĸ������β�           **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "sht2xfunction.h"

#define SHT2X_ADDRESS   0X80//SHT2X�ĵ�ַ

/* �������кŴ洢��ַ�� */
#define FIRST_MEM_ADDRESS       0xFA    //���кŵ�һ�δ洢��ַ
#define FIRST_MEM_COMMAND       0x0F    //���кŵ�һ�δ洢��ַ
#define SECOND_MEM_ADDRESS      0xFC    //���кŵڶ��δ洢��ַ
#define SECOND_MEM_COMMAND      0xC9    //���кŵڶ��δ洢��ַ

/* �����û��Ĵ������ú� */
#define DPI_RH12_T14    0x00    //ʪ��12λ�¶�14λ����
#define DPI_RH8_T12     0x01    //ʪ��8λ�¶�13λ����
#define DPI_RH10_T13    0x80    //ʪ��10λ�¶�12λ����
#define DPI_RH11_T11    0x81    //ʪ��11λ�¶�11λ����
#define END_OF_BATTERY_H        0x00    //���״̬VDD>2.25
#define END_OF_BATTERY_L        0x40    //���״̬VDD<2.25
#define ONCHIPHEATERDISABLE     0x00    //����Ƭ�ڼ���
#define ONCHIPHEATERENABLE      0x04    //����Ƭ�ڼ���
#define OTPENABLE       0x00    //������OTP����
#define OTPDISABLE      0x02    //��������OTP����

const uint16_t SHT2xPOLYNOMIAL = 0x131;      ////����У�����ʽP(x) = x^8 + x^5 + x^4 + 1 = 100110001

/*����ָ���Ķ���ʽ����CRCУ��*/
static bool CheckCRC8ForSHT2x(uint8_t *data, uint8_t numOfBytes, uint8_t checksum);
/*�����¶ȵ�������ֵ*/
static float CalcSHT2xTemperature(uint16_t sTemp);
/*�������ʪ�ȵ�������ֵ*/
static float CalcSHT2xHumidity(uint16_t sHumi);
/*��ȡSHT2X����ʶ���루���кţ�*/
static void GetSHT2xSerialNumber(SHT2xObjectType *sht);
/*��ȡ�û��Ĵ���*/
static void GetSHT2xUserRegister(SHT2xObjectType *sht);
/*�����û��Ĵ���*/
static void SetSHT2xUserRegister(SHT2xObjectType *sht,uint8_t cmd);

/*��ȡSHT2x���¶�����*/
float GetSHT2xTemperatureValue(SHT2xObjectType *sht,uint8_t cmd)
{
    uint8_t data[3]={0,0,0};
    uint16_t result=0;
    
    /*����ת������*/
    sht->Transmit(sht,&cmd,1);
    
    sht->Delayms(85);
    
    /*��ȡ����*/
    sht->Receive(sht,data,3);
    
    if(CheckCRC8ForSHT2x(data,2,data[2]))
    {
        result=(uint16_t)(data[0]);
        result=(result<<8)+(uint16_t)(data[1]);
    }
    
    sht->tempCode=result;
    
    return CalcSHT2xTemperature(result);
}

/*��ȡSHT2x��ʪ������*/
float GetSHT2xHumidityValue(SHT2xObjectType *sht,uint8_t cmd)
{
    uint8_t data[3]={0,0,0};
    uint16_t result=0;
    
    /*����ת������*/
    sht->Transmit(sht,&cmd,1);
    
    sht->Delayms(29);
    
    /*��ȡ����*/
    sht->Receive(sht,data,3);
    
    if(CheckCRC8ForSHT2x(data,2,data[2]))
    {
        result=(uint16_t)(data[0]);
        result=(result<<8)+(uint16_t)(data[1]);
    }
    
    sht->humiCode=result;
    
    return CalcSHT2xHumidity(result);
}

/*�����λ*/
void SoftResetSHT2x(SHT2xObjectType *sht)
{
    uint8_t tData=SOFT_RESET;
    sht->Transmit(sht,&tData,1);
}

/* ��ʼ������SHT2x */
void SHT2xInitialization(SHT2xObjectType *sht,
                         SHT2xDPIType dpi,
                         SHT2xBatteryType endBat,
                         SHT2xHeaterType heater,
                         SHT2xOTPType otp,
                         SHT2xTransmit write,
                         SHT2xReceive read,
                         SHT2xDelayms delayms)
{
    uint8_t userReg=0;
    uint8_t dpiSet[4]={DPI_RH12_T14,DPI_RH8_T12,DPI_RH10_T13,DPI_RH11_T11};
    uint8_t batEnd[2]={END_OF_BATTERY_H,END_OF_BATTERY_L};
    uint8_t heaterSet[2]={ONCHIPHEATERDISABLE,ONCHIPHEATERENABLE};
    uint8_t otpSet[2]={OTPENABLE,OTPDISABLE};
    
    if((sht==NULL)||(write==NULL)||(read==NULL)||(delayms==NULL))
    {
        return;
    }
    
    sht->Transmit=write;
    sht->Receive=read;
    sht->Delayms=delayms;
    
    sht->devAddress=SHT2X_ADDRESS;
    sht->tempCode=0;
    sht->humiCode=0;
    
    SoftResetSHT2x(sht);
    
    userReg=dpiSet[dpi]|batEnd[endBat]|heaterSet[heater]|otpSet[otp];
    
    SetSHT2xUserRegister(sht,userReg);	//�����û��Ĵ���
    
    sht->Delayms(20);
    
    //GetSHT2xUserRegister(sht);			//��ȡ�û��Ĵ���
    
    GetSHT2xSerialNumber(sht);			//��ȡ����ʶ����
}

/*�����û��Ĵ���*/
static void SetSHT2xUserRegister(SHT2xObjectType *sht,uint8_t cmd)
{
    uint8_t status;
    uint8_t command;
    uint8_t pData[2];
    
    GetSHT2xUserRegister(sht);
    
    status=sht->userReg&0x38;
    
    command=cmd&0xC7;
    command=command|status;
    
    if(command!=sht->userReg)
    {
        pData[0]=WRITE_USER_REGISTER;
        pData[1]=command;
        
        sht->Transmit(sht,pData,2);
        
        sht->Delayms(10);
        
        GetSHT2xUserRegister(sht);
    }
}

/*��ȡ�û��Ĵ���*/
static void GetSHT2xUserRegister(SHT2xObjectType *sht)
{
    uint8_t pData;
    uint8_t rData=0;
    
    pData=READ_USER_REGISTER;
    sht->Transmit(sht,&pData,1);

    sht->Receive(sht,&rData,1);
    sht->userReg=rData;
}

/*�����¶ȵ�������ֵ*/
static float CalcSHT2xTemperature(uint16_t sTemp)
{
    float temperatureC;
    
    sTemp &= 0xFFFC;
    /*T= -46.85 + 175.72 * ST/2^16*/
    temperatureC = (float)-46.85 + (float)sTemp*((float)175.72 / 65536);
    
    return temperatureC;
}

/*�������ʪ�ȵ�������ֵ*/
static float CalcSHT2xHumidity(uint16_t sHumi)
{
    float humidityRH;
    
    sHumi &= 0xFFFC;
    /* RH= -6 + 125 * SRH/2^16*/
    humidityRH = (float)-6.0 + (float)sHumi*((float)125.0/65536); 
    
    return humidityRH;
}

/*����ָ���Ķ���ʽ����CRCУ��*/
static bool CheckCRC8ForSHT2x(uint8_t *data, uint8_t numOfBytes, uint8_t checksum)
{
    uint8_t crc = 0x00;
    uint8_t bit=0;
    bool error;
    
    for(uint8_t i = 0; i < numOfBytes; i++)
    {
        crc ^= (data[i]);
        for ( bit = 8; bit > 0; --bit)
        {
            if (crc & 0x80)
            {
                crc = (crc << 1) ^ SHT2xPOLYNOMIAL;
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

/*��ȡSHT2X����ʶ���루���кţ�*/
static void GetSHT2xSerialNumber(SHT2xObjectType *sht)
{
    uint8_t snbBuff[8];
    uint8_t snacBuff[6];
    uint8_t pData[2];
    
    /*���ʵ�һ���洢λ��*/
    pData[0]=FIRST_MEM_ADDRESS;
    pData[1]=FIRST_MEM_COMMAND;
    sht->Transmit(sht,pData,2);
    sht->Delayms(100);
    sht->Receive(sht,snbBuff,8);
    
    /*���ʵڶ����洢λ��*/
    pData[0]=SECOND_MEM_ADDRESS;
    pData[1]=SECOND_MEM_COMMAND;
    sht->Transmit(sht,pData,2);
    sht->Delayms(100);
    sht->Receive(sht,snacBuff,6);
    
    sht->sn[0]=snacBuff[4];
    sht->sn[1]=snacBuff[3];
    sht->sn[2]=snbBuff[6];
    sht->sn[3]=snbBuff[4];
    sht->sn[4]=snbBuff[2];
    sht->sn[5]=snbBuff[0];
    sht->sn[6]=snacBuff[1];
    sht->sn[7]=snacBuff[0];
}

/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
