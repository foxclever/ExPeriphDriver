/******************************************************************************/
/** ģ�����ƣ���չ��������                                                   **/
/** �ļ����ƣ�ds18b20function.c                                              **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺ʵ���¶ȼ��ģ��DS18B20�����ݶ�ȡ                              **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2012-07-28          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "ds18b20function.h"

#define RESOLUTION_BITS9        0x1F
#define RESOLUTION_BITS10       0x3F
#define RESOLUTION_BITS11       0x5F
#define RESOLUTION_BITS12       0x7F

#define Search_Rom      0xF0
#define Read_Rom        0x33
#define Match_Rom       0x55
#define Skip_Rom        0xCC
#define Alarm_Search    0xEC

#define Convert_T       0x44
#define Write_Scratchpad        0x4E
#define Read_Scratchpad		0xBE
#define Copy_Scratchpad		0x48
#define Recall_EE               0xB8
#define Read_Power_Supply	0xB4

//CRC = X8 + X5 + X4 + 1

/*�������ӻ����͸�λ����*/
static void ResetDs18b20(Ds18b20ObjectType *ds18b20);
/*���ӻ����������صĴ������� 0���ɹ�;1��ʧ��*/
static uint8_t PresenceDs18b20(Ds18b20ObjectType *ds18b20);
/*��DS18B20��ȡһ��λ,����ֵ��1/0*/
static uint8_t ReadBitFromDs18b20(Ds18b20ObjectType *ds18b20);
/*��DS18B20дһ���ֽ�*/
static void WriteByteToDs1820(Ds18b20ObjectType *ds18b20,uint8_t commond);
/*��DS18B20��ȡһ���ֽ�*/
static uint8_t ReadByteFromDs18b20(Ds18b20ObjectType *ds18b20);
/*���õ������������ģʽ��ȱʡ����*/
static void SetPinModeDefault(Ds18b20IOModeType mode);
/*��ȡDS18B20��������к�*/
static void GetDs18b20SerialNumber(Ds18b20ObjectType *ds18b20);

/*�¶����ݻ�ȡ��ת������*/
float GetDS18b20TemperatureValue(Ds18b20ObjectType *ds18b20)
{
    uint8_t temph,temp1;
    uint16_t itemp;
    float tFactor=0.0625;
    
    ResetDs18b20(ds18b20);        //��λDS18B20
    PresenceDs18b20(ds18b20);
    WriteByteToDs1820(ds18b20,Skip_Rom);  //������ROMƥ������
    WriteByteToDs1820(ds18b20,Convert_T); //���¶�ת������
    
    ResetDs18b20(ds18b20);        //��λDS18B20
    PresenceDs18b20(ds18b20);
    WriteByteToDs1820(ds18b20,Skip_Rom);  //������ROMƥ������
    WriteByteToDs1820(ds18b20,Read_Scratchpad);   //�������¶�����
    
    temp1=ReadByteFromDs18b20(ds18b20);   //���������¶����ݵ�λ�ֽ�
    temph=ReadByteFromDs18b20(ds18b20);   //���������¶����ݸ�λ�ֽ�
    itemp=(temph<<8) | temp1;
    
    if(itemp<=0x7FF)
    {
        ds18b20->temperature=(float)itemp*tFactor;
    }
    else
    {
        itemp=~itemp+1;
        ds18b20->temperature=0.0-(float)itemp*tFactor;
    }
    
    return ds18b20->temperature;
}

/*�������ӻ����͸�λ����*/
static void ResetDs18b20(Ds18b20ObjectType *ds18b20)
{
    /* ��������Ϊ�������*/
    ds18b20->SetPinMode(DS18B20_Out);
    /* �������ٲ���480us �ĵ͵�ƽ��λ�ź�*/
    ds18b20->SetBit(DS18B20_Reset);
    ds18b20->Delayus(550);
    /* �����ڲ�����λ�źź��轫��������*/
    ds18b20->SetBit(DS18B20_Set);
    /*�ӻ����յ������ĸ�λ�źź󣬻���15~60us ���������һ����������*/
    ds18b20->Delayus(15);
}

/*���ӻ����������صĴ������� 0���ɹ�;1��ʧ��*/
static uint8_t PresenceDs18b20(Ds18b20ObjectType *ds18b20)
{
    uint8_t pulse_time = 0;
    /* ��������Ϊ��������*/
    ds18b20->SetPinMode(DS18B20_In);
    
    /* �ȴ���������ĵ�������������Ϊһ��60~240us �ĵ͵�ƽ�ź�*/
    /*�����������û����������ʱ�����ӻ����յ������ĸ�λ�źź󣬻���15~60us ���������һ����������*/
    while( ds18b20->GetBit() && pulse_time<100 )
    {
        pulse_time++;
        ds18b20->Delayus(1);
    }
    /* ����100us �󣬴������嶼��û�е���*/
    if( pulse_time >=100 )
        return 1;
    else
        pulse_time = 0;
    /* �������嵽�����Ҵ��ڵ�ʱ�䲻�ܳ���240us*/
    while(!ds18b20->GetBit() && (pulse_time<240))
    {
        pulse_time++;
        ds18b20->Delayus(1);
    }
    if( pulse_time >=240 )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*��DS18B20��ȡһ��λ,����ֵ��1/0*/
static uint8_t ReadBitFromDs18b20(Ds18b20ObjectType *ds18b20) 
{
    uint8_t data;
    
    ds18b20->SetPinMode(DS18B20_Out);
    ds18b20->SetBit(DS18B20_Reset);
    ds18b20->Delayus(2);
    ds18b20->SetBit(DS18B20_Set);
    ds18b20->SetPinMode(DS18B20_In);
    ds18b20->Delayus(12);
    data=ds18b20->GetBit();
    ds18b20->Delayus(50);
    return data;
}

/*��DS18B20дһ���ֽ�*/
static void WriteByteToDs1820(Ds18b20ObjectType *ds18b20,uint8_t commond)
{
    uint8_t i, testb;
    
    ds18b20->SetPinMode(DS18B20_Out);
    
    for(i=0; i<8; i++)
    {
        testb = commond&0x01;
        commond = commond>>1;
        // д0��д1��ʱ������Ҫ����60us
        if (testb)
        {
            ds18b20->SetBit(DS18B20_Reset);
            // 1us < �����ʱ < 15us
            ds18b20->Delayus(10);
            ds18b20->SetBit(DS18B20_Set);
            ds18b20->Delayus(45);
        }
        else
        {
            ds18b20->SetBit(DS18B20_Reset);
            // 60us < Tx 0 < 120us
            ds18b20->Delayus(60);
            ds18b20->SetBit(DS18B20_Set);
            // 1us < Trec(�ָ�ʱ��) < �����
        }
        ds18b20->Delayus(2);
    }
}

/*��DS18B20��ȡһ���ֽ�*/
static uint8_t ReadByteFromDs18b20(Ds18b20ObjectType *ds18b20)
{
    uint8_t i, j, data = 0;
    for(i=0; i<8; i++)
    {
        j = ReadBitFromDs18b20(ds18b20);
        data = (data) | (j<<i);
    }
    return data;
}

/*��ȡDS18B20��������к�*/
static void GetDs18b20SerialNumber(Ds18b20ObjectType *ds18b20)
{
    uint8_t data[8];
    
    WriteByteToDs1820(ds18b20,Read_Rom);
    
    for(int i=0;i<8;i++)
    {
        data[i]=ReadByteFromDs18b20(ds18b20);
    }
    
    for(int i=0;i<6;i++)
    {
        ds18b20->sn[i]=data[i+1];
    }
}

/*��DS18B20�������г�ʼ��*/
Ds18b20StatusType Ds18b20Initialization(Ds18b20ObjectType *ds18b20,
                                        Ds18b20SetBitType setBit,
                                        Ds18b20GetBitType getBit,
                                        Ds18b20SetPinModeType pinDirection,
                                        Ds18b20DelayType delayus)
{
    if((ds18b20==NULL)||(setBit==NULL)||(getBit==NULL)||(delayus==NULL))
    {
        return DS18B20_InitialError;
    }
    
    ds18b20->SetBit=setBit;
    ds18b20->GetBit=getBit;
    ds18b20->Delayus=delayus;
    
    if(pinDirection==NULL)
    {
        ds18b20->SetPinMode=SetPinModeDefault;
    }
    else
    {
        ds18b20->SetPinMode=pinDirection;
    }
    
    ds18b20->temperature=0.0;
    
    ResetDs18b20(ds18b20);
    if(PresenceDs18b20(ds18b20))
    {
        return DS18B20_NoResponse;
    }
    
    GetDs18b20SerialNumber(ds18b20);
    
    return DS18B20_OK;
}

/*���õ������������ģʽ��ȱʡ����*/
static void SetPinModeDefault(Ds18b20IOModeType mode)
{
    //�����ſ�������Ϊ��¥���ģʽʱ�������޸��������ģʽ��ʹ�ô�ȱʡ����
}
/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
