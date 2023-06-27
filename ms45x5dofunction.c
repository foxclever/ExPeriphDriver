/******************************************************************************/
/** ģ�����ƣ���չ��������                                                   **/
/** �ļ����ƣ�ms45x5dofunction.h                                             **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʵ��ѹ����������MS45x5DO������ز���                       **/
/**           MS45x5DOѹ��������ʹ��I2C�ӿ�ͨѶ��ʽ���������ֲ������      **/
/**           ��1�������������Read_MR����ʽ���£�                         **/
/**           +-+--------------+-+-+-+                                       **/
/**           |S|Device Address|W|A|P|                                       **/
/**           +-+--------------+-+-+-+                                       **/
/**           ��2����ȡ2�ֽ����Read_DF2����ʽ���£�                       **/
/**           +-+--------------+-+-+----------+-+----+-+-+                   **/
/**           |S|Device Address|R|A|S1S0|P13-8|A|P7-0|N|P|                   **/
/**           +-+--------------+-+-+----------+-+----+-+-+                   **/
/**           ��3����ȡ2�ֽ����Read_DF3����ʽ���£�                       **/
/**      +-+--------------+-+-+-----------+-+-----+-+-----+-+-+              **/
/**      |S|Device Address|R|A|S1S0| P13-8|A|P7-0 |A|T10-3|N|P|              **/
/**      +-+--------------+-+-+-----------+-+-----+-+-----+-+-+              **/
/**           ��4����ȡ2�ֽ����Read_DF4����ʽ���£�                       **/
/**      +-+--------------+-+-+-----------+-+-----+-+-----+-+---------+-+-+  **/
/**      |S|Device Address|R|A|S1S0| P13-8|A|P7-0 |A|T10-3|A|T2-0|x4-0|N|P|  **/
/**      +-+--------------+-+-+-----------+-+-----+-+-----+-+---------+-+-+  **/
/**           S=Start Condition   W=Write   A=Acknowledge                    **/
/**           P=Stop Condition    R=Read    N=Not Acknowledge                **/
/**           ����Device AddressΪ7λ��CommandΪ8λ                          **/
/**--------------------------------------------------------------------------**/
/**ʹ��˵����                                                                **/
/**    1������MS45x5DOObjectType����ʵ�壬�����ú���MS45x5DOInitialization�� **/
/**       ʼ������ʵ�塣�����豸��ַ������ȡУ׼����                         **/
/**    2��ʵ������uint8_t f(struct MS45x5DOObject *ms,uint8_t *wData,        **/
/**       uint16_t wSize)�ĺ����������丳ֵ��MS45x5DOObjectType����ʵ���    **/
/**       Write����ָ�롣ʵ�ֶ�MS45x5DOд���ݡ�                              **/
/**    3��ʵ������void f(struct MS45x5DOObject *ms,uint8_t *rData,           **/
/**       uint16_t rSize)�ĺ����������丳ֵ��MS45x5DOObjectType����ʵ���    **/
/**       Read����ָ�롣ʵ�ִ�MS45x5DO������                                 **/
/**    4��ʵ������void f(volatile uint32_t nTime)�ĺ����������丳ֵ��        **/
/**       MS45x5DOObjectType����ʵ���Delayms����ָ�롣ʵ�ֲ�����ʱ����λ����**/
/**    5��������Ӧ�ĺ���ʵ����Ҫ�Ĳ�����                                     **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2020-11-26          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "ms45x5dofunction.h"

/*��ȡת��ֵ�������¶Ⱥ�ѹ��*/
void GetMS45x5DOConversionValue(MS45x5DOObjectType *ms)
{
    uint8_t rData[4]={0,0,0,0};
    float maxCount=16383;
    float minCount=0;
    
    if(ms->type==MS45x5DO_TypeA)
    {
        maxCount=13106;
        minCount=1638;
    }
    else
    {
        maxCount=14746;
        minCount=819;
    }
    
    ms->Read(ms,rData,4);
    
    ms->msData.rData[0]=rData[1];
    ms->msData.rData[1]=rData[0];
    ms->msData.rData[2]=rData[3];
    ms->msData.rData[3]=rData[2];
    
    if(ms->msData.pData.status!=MS45x5DO_Fault)
    {
        ms->fPressure=(((float)ms->msData.pData.pressure-minCount)/maxCount)*(ms->pUpperRange-ms->pLowerRange)+ms->pLowerRange;
        ms->fTemperature=((float)ms->msData.pData.temperature/2047.0)*200.0-50.0;
    }
}

/* ��ʼ��MS45x5DO���� */
void MS45x5DOInitialization(MS45x5DOObjectType *ms, //MS5837����
                            uint8_t devAddress,     //�豸��ַ
                            MS45x5DOType type,      //MS4515DO������
                            float pMax,             //ѹ����������
                            float pMin,             //ѹ����������
                            MS45x5DOWrite write,    //��MS45x5DOд���ݺ���ָ��
                            MS45x5DORead read,      //��MS45x5DO�����ݺ���ָ��
                            MS45x5DODelayms delayms //������ʱ����ָ��
                                )
{
    if((ms==NULL)||(write==NULL)||(read==NULL)||(delayms==NULL))
    {
        return;	
    }
    
    ms->Write=write;
    ms->Read=read;
    ms->Delayms=delayms;
    
    if((devAddress==0x28)||(devAddress==0x36)||(devAddress==0x46)||((0x48<=devAddress)&&(devAddress<=0x51)))
    {
        ms->devAddress=(devAddress<<1);
    }
    else if((devAddress==0x50)||(devAddress==0x6C)||(devAddress==0x8C)||((0x48<=(devAddress/2))&&((devAddress/2)<=0x51)))
    {
        ms->devAddress=devAddress;
    }
    else
    {
        ms->devAddress=0x00;
    }
    
    ms->type=type;
    
    ms->fPressure=0.0;
    ms->fTemperature=0.0;
    ms->msData.rData[0]=0;
    ms->msData.rData[1]=0;
    ms->msData.rData[2]=0;
    ms->msData.rData[3]=0;
    
    if((fabs(pMax)<=0.0000001)&&(fabs(pMin)<=0.0000001))
    {
        ms->pUpperRange=100.0;
        ms->pLowerRange=0.0;
    }
    else
    {
        ms->pUpperRange=pMax;
        ms->pLowerRange=pMin;
    }
}

/*�޸�MS45x5DO���豸��ַ*/
void ModifyMS45x5DODecAddress(MS45x5DOObjectType *ms,uint8_t newAddress)
{
    uint8_t eepromByte[3];
    uint16_t eepromTemp=0x00;
    
    //��1������������ģʽ
    eepromByte[0]=0xA0;
    eepromByte[1]=0x00;
    eepromByte[2]=0x00;
    
    ms->Write(ms,eepromByte,3);
    
    //��2�������Ͷ�EEPROM����
    eepromByte[0]=0x02;
    eepromByte[1]=0x00;
    eepromByte[2]=0x00;
    
    ms->Write(ms,eepromByte,3);
    
    //��3������ȡEEPROM��ֵ
    ms->Read(ms,eepromByte,3);
    
    //��4�����޸�Ϊ�µ�ַ
    if(eepromByte[0]==0x5A)
    {
        eepromTemp=(eepromByte[1]<<8)+eepromByte[2];
        eepromTemp=(eepromTemp&0xE007)+0xC00+(newAddress<<3);
        
        eepromByte[1]=(uint8_t)((eepromTemp&0xFF00)>>8);
        eepromByte[1]=(uint8_t)(eepromTemp&0x00FF);
    }
    else
    {
        return;
    }
    
    //��5�������µ�ַд��EEPROM
    eepromByte[0]=0x02;
    
    ms->Write(ms,eepromByte,3);
    
    //��6�����˳�����ģʽ
    eepromByte[0]=0x80;
    eepromByte[1]=0x00;
    eepromByte[2]=0x00;
    
    ms->Write(ms,eepromByte,3);
}

/*********** (C) COPYRIGHT 1999-2020 Moonan Technology *********END OF FILE****/