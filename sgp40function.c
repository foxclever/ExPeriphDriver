/******************************************************************************/
/** ģ�����ƣ���չ��������                                                   **/
/** �ļ����ƣ�sgp40function.c                                                **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺Ӧ����ʵ�ֶ�SGP40�������Ĳ���                                  **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2021-10-13          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "sgp40function.h"

/*����SGP40�����*/
#define SGP40_MEASURE_RAW_SIGNAL    0x260F
#define SGP40_EXECUTE_SELF_TEST     0x280E
#define SGP4x_TURN_HEATER_OFF       0x3615
#define SGP4x_GET_SERIAL_NUMBER     0x3682

/*����CRCУ����*/
static uint8_t CalcCrc(uint8_t data[2]);
/*�����λ*/
static void Sgp40SoftReset(Sgp40ObjectType *sgp);
/*��ȡ���к�*/
static void Sgp40GetSerialNumber(Sgp40ObjectType *sgp);
/*ִ�����Ҳ���*/
static void Sgp40ExecuteSelfTest(Sgp40ObjectType *sgp);

/*��ȡԭʼ����*/
void Sgp40MeasureRawSignal(Sgp40ObjectType *sgp,float humi,float temp)
{
    uint8_t wData[8]={0x26,0x0F,0x80,0x00,0xA2,0x66,0x66,0x93};
    uint8_t rData[3];
    uint8_t crc=0xFF;
    uint8_t voc=0;
    
    uint16_t ticks=0;
    uint8_t code[2];
    
    if(humi!=50)
    {
        ticks=(uint16_t)(humi*65535/100);
        
        wData[2]=(ticks>>8);    
        wData[3]=ticks;
        wData[4]==CalcCrc(&wData[2]);
    }
    
    if(temp!=25)
    {
        ticks=(uint16_t)((temp+45)*65535/175);
        
        wData[5]=(ticks>>8);    
        wData[6]=ticks;
        wData[7]==CalcCrc(&wData[5]);
    }
    
    sgp->Write(sgp,wData,8);
    
    sgp->Delayms(30);
    
    sgp->Read(sgp,rData,3);
    
    crc=CalcCrc(rData);
    if(crc==rData[2])
    {
        voc=rData[0];
        voc=(voc<<8)+rData[1];
    }
    
    sgp->rawSignal=voc;
}

/*SGP40�����ʼ��*/
void Sgp40Initialization(Sgp40ObjectType *sgp,      //SGP40����
                         Sgp40WriteType write,      //д��������ָ��
                         Sgp40ReadType read,        //����������ָ��
                         Sgp40Delayms delayms       //��ʱ��������ָ��
                         )
{
    if((sgp==NULL)||(write==NULL)||(read==NULL)||(delayms==NULL))
    {
        return;
    }
    
    sgp->Write=write;
    sgp->Read=read;
    sgp->Delayms=delayms;
    
    sgp->devAddress=0xB2;
    
    /*�����λ*/
    Sgp40SoftReset(sgp);
    
    /*��ȡ���к�*/
    Sgp40GetSerialNumber(sgp);
    
    /*ִ�����Ҳ���*/
    Sgp40ExecuteSelfTest(sgp);
}

/*ִ�����Ҳ���*/
static void Sgp40ExecuteSelfTest(Sgp40ObjectType *sgp)
{
    uint8_t wData[2]={0x28,0x0E};
    uint8_t rData[3];
    uint8_t crc=0xFF;
    
    sgp->Write(sgp,wData,2);
    
    sgp->Delayms(320);
    
    sgp->Read(sgp,rData,3);
    
    crc=CalcCrc(rData);
    if(crc==rData[2])
    {
        sgp->status=rData[0];
    }
}

/*�رռ��ȣ��������ģʽ*/
void Sgp40TurnHeaterOff(Sgp40ObjectType *sgp)
{
    uint8_t wData[2]={0x36,0x15};
    
    sgp->Write(sgp,wData,2);
}

/*��ȡ���к�*/
static void Sgp40GetSerialNumber(Sgp40ObjectType *sgp)
{
    uint8_t wData[2]={0x36,0x82};
    uint8_t rData[9];
    uint8_t crc=0xFF;
    
    sgp->Write(sgp,wData,2);
    
    sgp->Delayms(1);
    
    sgp->Read(sgp,rData,9);
    
    crc=CalcCrc(rData);
    if(crc==rData[2])
    {
        sgp->serialNumber[0]=rData[0];
        sgp->serialNumber[1]=rData[1];
    }
    
    crc=CalcCrc(&rData[3]);
    if(crc==rData[5])
    {
        sgp->serialNumber[2]=rData[3];
        sgp->serialNumber[3]=rData[4];
    }
    
    crc=CalcCrc(&rData[6]);
    if(crc==rData[8])
    {
        sgp->serialNumber[4]=rData[6];
        sgp->serialNumber[5]=rData[7;
    }
}

/*�����λ*/
static void Sgp40SoftReset(Sgp40ObjectType *sgp)
{
    uint8_t wData[2]={0x00,0x06};
    
    sgp->Write(sgp,wData,2);
}

/*����CRCУ����*/
static uint8_t CalcCrc(uint8_t data[2])
{ 
    uint8_t crc = 0xFF;
    
    for(int i = 0; i < 2; i++)
    { 
        crc ^= data[i]; 
        for(uint8_t bit = 8; bit > 0; --bit)
        { 
            if(crc & 0x80)
            { 
                crc = (crc << 1) ^ 0x31u; 
            }
            else
            { 
                crc = (crc << 1); 
            } 
        } 
    }
    
    return crc; 
} 

/*********** (C) COPYRIGHT 1999-2021 Moonan Technology *********END OF FILE****/
