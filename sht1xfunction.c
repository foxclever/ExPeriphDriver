/******************************************************************************/
/** ģ�����ƣ���չ��������                                                   **/
/** �ļ����ƣ�sht1xfunction.h                                                **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʵ��SHT1X��ʪ�ȼƵĲ�������������I2C��ͨѶ��ʽ             **/
/**           ����SHT10,SHT11��SHT15�Լ�SHT7x                                **/
/**           Ĭ�Ϸֱ���14bit(�¶�)��12bit(ʪ��)���Ա�����Ϊ12��8bit         **/
/**           �¶�ת����ʽ��T=d1+d2*SOT������SOTΪ���������TΪ�¶�ֵ        **/
/**           d1��d2��ȡֵ���£�                                             **/
/**           VDD               d1 (��C)         d1 (��F)                    **/
/**           5V                -40.1           -40.2                        **/
/**           4V                -39.8           -39.6                        **/
/**           3.5V              -39.7           -39.5                        **/
/**           3V                -39.6           -39.3                        **/
/**           2.5V              -39.4           -38.9                        **/
/**                                                                          **/
/**           SOT               d2 (��C)         d2 (��F)                    **/
/**           14bit             0.01            0.018                        **/
/**           12bit             0.04            0.072                        **/
/**           ʪ�����ݵ�ת����ʽ��RHlinear=c1+c2*SORH+c3*SORH*SORH��         **/
/**           ����SORHΪ���������RHlinearΪת������ֶ�ʪ��                 **/
/**           SORH	  c1	          c2	          c3                     **/
/**           12bit	-2.0468         0.0367          -1.5955E-6               **/
/**           8bit	-2.0468         0.5872          -4.0845E-4               **/
/**           ʪ���¶Ȳ�����ʽ��RHtrue=(T-25)*(t1+t2*SORH)+RHlinear          **/
/**           ����RHtrueΪ�¶Ȳ���������ʪ��                               **/
/**           SORH	  t1	  t2                                             **/
/**           12bit	0.01	0.00008                                          **/
/**           8bit	0.01	0.00128                                          **/
/**--------------------------------------------------------------------------**/
/**ʹ��˵����                                                                **/
/**    1������Sht1xObjectType����ʵ�壬�����ú���SHT1xInitialization��ʼ��   **/
/**       ����ʵ�塣�����豸�Ĺ�����Դ��ѹVDD�Լ��¶ȵ�λ�����϶Ȼ��϶ȣ�  **/
/**    2��ʵ����������void f(BusPinValue value)��������������������ֱ��    **/
/**       ��SCK��SDA��������BusPinOperation���͵ĺ���ָ�����飬SCK�Ĳ�����   **/
/**       ����Ϊ��һ��Ԫ�أ�SDA�Ĳ���������Ϊ�ڶ���Ԫ�ء���������ָ�������  **/
/**       ָ�븳ֵ��Sht1xObjectType����ʵ���SetBusPin����ָ�롣ʵ�ֶ�SCK��  **/
/**       �ź�SDA���ŵĲ�����                                                **/
/**    3��ʵ������uint8_t f(void)�ĺ����������丳ֵ��Sht1xObjectType����ʵ�� **/
/**       ��ReadSDABit����ָ�롣ʵ�����ݵĶ�ȡ��                             **/
/**    4��ʵ������void f(IODirection direction);�ĺ����������丳ֵ��         **/
/**       Sht1xObjectType����ʵ���SetSDADirection����ָ�롣ʵ�ֶ�̬�ı�SDA��**/
/**       �ŵ������������                                                   **/
/**    5��ʵ������void f(volatile uint32_t period)�ĺ����������丳ֵ��       **/
/**       Sht1xObjectType����ʵ���Delayus����ָ�롣ʵ�ֲ�����ʱ����λ΢��   **/
/**    6��ʵ������void f(volatile uint32_t nTime)�ĺ����������丳ֵ��        **/
/**       Sht1xObjectType����ʵ���Delayms����ָ�롣ʵ�ֲ�����ʱ����λ����   **/
/**    7��������Ӧ�ĺ���ʵ����Ҫ�Ĳ�����                                     **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2017-01-13          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "sht1xfunction.h"
#include "math.h"

/*����SHT1x�Ĳ�������*/
#define TEMP_MEAS_COMMAND       0x03    //�¶Ȳ�������
#define HUMI_MEAS_COMMAND       0x05    //ʪ�ȼ������
#define READ_STATUS_REGISTER    0x07    //��״̬�Ĵ���
#define WRITE_STATUS_REGISTER   0x06    //д״̬�Ĵ���
#define SOFTWARE_RESET          0x1E    //�����λ����

#define ONCHIPHEATERENABLE      0x04    //����Ƭ�ڼ���
#define ONCHIPHEATERDISABLE     0x00    //����Ƭ�ڼ���
#define OTPENABLE               0x00    //������OTP����
#define OTPDISABLE              0x02    //��������OTP����
#define HIGH_RESOLUTION_DATA    0x00    //���ݷֱ���14bit(�¶�)��12bit(ʪ��)
#define LOW_RESOLUTION_DATA     0x01    //���ݷֱ���12bit(�¶�)��8bit(ʪ��)

#define noAck   0       //��ȷ��
#define Ack     1       //ȷ��

const uint16_t SHT1xPOLYNOMIAL = 0x31;      //����У�����ʽP(x) = x^8 + x^5 + x^4 + 1 = 100110001

SHT1xSetBusPin setBusPin[2];

/*SHT1X����ʱ�����*/
static void StartSHT1XOperation(Sht1xObjectType *sht);
/*�����ȡ��ѹ�¶�ϵ��d1*/
static float CalcVoltageTempCoefficient(Sht1xObjectType *sht);
/*��SHT1Xдһ���ֽ�*/
static uint8_t WriteByteToSht1x(Sht1xObjectType *sht,uint8_t value);
/*��SHT1X��ȡһ���ֽ�*/
static uint8_t ReadByteFromSht1x(Sht1xObjectType *sht,uint8_t ack);
/*�¶�����ת��*/
static float ConvertTemperatureData(Sht1xObjectType *sht,uint16_t sot);
/*ʪ������ת��*/
static float ConvertHumidityData(Sht1xObjectType *sht,uint16_t sorh);
/*����ָ���Ķ���ʽ����CRCУ��*/
static bool CheckCRC8ForSHT1x(uint8_t *data, uint8_t numOfBytes, uint8_t checksum);
/*����¶���¶�*/
static float CalcDewPointTemperature(Sht1xObjectType *sht);
/* ȱʡIO������ƺ��� */
static void DefaultSDADirection(SHT1xIOModeType direction);
/*��״̬�Ĵ���*/
static uint8_t ReadStatusRegister(Sht1xObjectType *sht);
/*д״̬�Ĵ���*/
static uint8_t WriteStatusRegister(Sht1xObjectType *sht,uint8_t *pValue);


/* ��ȡSHT1x�������� */
void GetSht1xMeasureValue(Sht1xObjectType *sht)
{
    sht->temperature=GetSht1xTemperatureValue(sht);
    sht->humidity=GetSht1xHumidityValue(sht);
    sht->dewPoint=CalcDewPointTemperature(sht);
}

/*��ȡSHT1X���¶�ֵ*/
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

/*��ȡSHT1X��ʪ��ֵ*/
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

/*����¶���¶�*/
static float CalcDewPointTemperature(Sht1xObjectType *sht)
{
    float dewPoint=0.0;
    float logEx=0.0;
    logEx=0.66077+7.5*sht->temperature/(237.3+ sht->temperature) + (float)(log10(sht->humidity) - 2);
    dewPoint=((0.66077 - logEx) * 237.3) / (logEx - 8.16077);
    return dewPoint;
}

/*SHT1X����ʱ�����*/
static void StartSHT1XOperation(Sht1xObjectType *sht)
{
    /*��data������Ϊ���ģʽ*/
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

/*SHT1XͨѶ��λ*/
void ResetSHT1XCommunication(Sht1xObjectType *sht)
{
    /*��data������Ϊ���ģʽ*/
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

/*��SHT1Xдһ���ֽ�*/
static uint8_t WriteByteToSht1x(Sht1xObjectType *sht,uint8_t value)
{
    uint8_t err = 0;
    
    /*��data������Ϊ���ģʽ*/
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
    
    /*��ȡSHT1X��Ӧ��λ*/
    err=sht->ReadSDABit();
    sht->SetBusPin[SckPin](SHT1xReset);
    return err;
}

/*��SHT1X��ȡһ���ֽ�*/
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

/*��SHT1Xʵ�������λ*/
uint8_t Sht1xSoftwareReset(Sht1xObjectType *sht)
{
    uint8_t err = 0;
    
    ResetSHT1XCommunication(sht);
    err=err+WriteByteToSht1x(sht,SOFTWARE_RESET);
    
    sht->Delayms(20);
    
    return err;
}

/*��״̬�Ĵ���*/
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

/*д״̬�Ĵ���*/
static uint8_t WriteStatusRegister(Sht1xObjectType *sht,uint8_t *pValue)
{
    uint8_t err=0;
    
    StartSHT1XOperation(sht);
    err +=WriteByteToSht1x(sht,WRITE_STATUS_REGISTER);
    err +=WriteByteToSht1x(sht,*pValue);
    
    err +=ReadStatusRegister(sht);
    return err;
}

/* ��ʼ��SHT1x���� */
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
    uint8_t heaterSet[]={ONCHIPHEATERDISABLE,ONCHIPHEATERENABLE};	//�Ƿ�����Ƭ�ڼ������ü�
    uint8_t otpSet[]={OTPENABLE,OTPDISABLE};											//�Ƿ����OTP���ü�
    uint8_t dpiSet[]={HIGH_RESOLUTION_DATA,LOW_RESOLUTION_DATA};	//���ݷֱ������ü�
    
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
    
    /*��ʼ���ٶ�,Ĭ��100K*/
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

/*�����ȡ��ѹ�¶�ϵ��d1*/
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

/*�¶�����ת��,sot:�������¶ȣ�vdd:��Դ��ѹ*/
static float ConvertTemperatureData(Sht1xObjectType *sht,uint16_t sot)
{
    float temp=0.0;
    uint8_t row=0;
    
    /*����ת����ʽ�е�ϵ��*/
    float d1=0.0;
    float d2[2][2]={{0.01,0.018},{0.04,0.072}};
    
    row=sht->statusReg&0x01;
    
    /*�����¶�ʱd1��ȡֵ*/
    d1=CalcVoltageTempCoefficient(sht);
    
    temp=d1+d2[row][sht->tempUnit]*sot;
    return temp;
}

/*ʪ������ת��,sorh:������ʪ�ȣ�temp:�����¶�*/
static float ConvertHumidityData(Sht1xObjectType *sht,uint16_t sorh)
{
    float rhTrue=0.0;
    float rhLinear=0.0;
    uint8_t index=0;
    
    /*����ת����ʽ�е�ϵ��*/
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

/*����ָ���Ķ���ʽ����CRCУ��*/
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

/* ȱʡIO������ƺ��� */
static void DefaultSDADirection(SHT1xIOModeType direction)
{
    //�����Ų��ÿ�©���ʱ��ʹ�ô�Ĭ�Ϻ���
}
/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
