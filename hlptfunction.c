/******************************************************************************/
/** ģ�����ƣ���չ��������                                                   **/
/** �ļ����ƣ�hlptfunction.c                                                 **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʵ��TSEV01CL55�����¶ȴ�������ͨѶ                         **/
/**           ����I2CͨѶͨѶ�ӿ�                                            **/
/**--------------------------------------------------------------------------**/
/**ʹ��˵����                                                                **/
/**    1������HLPTObjectType����ʵ�壬�����ú���HLPTInitialization��ʼ��     **/
/**       ����ʵ�塣��ʼ��ʱ����������ĵ�ַ��                               **/
/**    2��ʵ������void f(struct HLPTObject *hlpt,uint8_t *rData,             **/
/**       uint16_t rSize)�ĺ����������丳ֵ��HLPTObjectType����ʵ���Receive **/
/**       ����ָ�롣ʵ�ֶ����ݵĲ�����                                       **/
/**    3��ʵ������void f(struct HLPTObject *hlpt,uint8_t wData)�ĺ���������  **/
/**       �丳ֵ��HLPTObjectType����ʵ���Transmit����ָ�롣ʵ�ַ���������� **/
/**    4��ʵ������oid f(volatile uint32_t nTime)�ĺ����������丳ֵ��         **/
/**       HLPTObjectType����ʵ���Delay����ָ�롣ʵ����ʱ��������λΪ���롣  **/
/**    5��������Ӧ�ĺ���ʵ����Ҫ�Ĳ�����                                     **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2019-02-26          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "hlptfunction.h"

#define HLPTSlaveAddress        0x54

#define ReadObjectTemperature   0xB6
#define ReadAmbientTemperature  0xB5

/* ��ȡ���� */
static uint16_t ReadDataFromHLPT(HLPTObjectType *hlpt,uint8_t cmd);

//��ȡ�¶�ֵ
void GetHLPTemperature(HLPTObjectType *hlpt)
{
    hlpt->tempObject=ReadDataFromHLPT(hlpt,ReadObjectTemperature)/100.0;
    
    hlpt->tempAmbient=ReadDataFromHLPT(hlpt,ReadAmbientTemperature)/100.0;
}

/* �����¶ȴ����������ʼ�� */
void HLPTInitialization(HLPTObjectType *hlpt,   //��ʼ���Ķ������
                        uint8_t address,        //�豸I2C��ַ
                        HLPTReceive receive,    //���պ���ָ��
                        HLPTTransmit transmit,  //���ͺ���ָ��
                        HLPTDelayms delayms     //������ʱ����ָ��
                            )
{
    if((hlpt==NULL)||(receive==NULL)||(transmit==NULL)||(delayms==NULL))
    {
        return;
    }
    hlpt->Receive=receive;
    hlpt->Transmit=transmit;
    hlpt->Delayms=delayms;
    
    hlpt->tempObject=0.0;
    hlpt->tempAmbient=0.0;
    
    if(address>0x00)
    {
        hlpt->devAddress=address;
    }
    else
    {
        hlpt->devAddress=HLPTSlaveAddress;
    }
}

/* ��ȡ���� */
static uint16_t ReadDataFromHLPT(HLPTObjectType *hlpt,uint8_t cmd)
{
    uint8_t data[2];
    uint16_t tempCode;
    
    hlpt->Transmit(hlpt,&cmd,1);
    
    hlpt->Delayms(1);
    
    hlpt->Receive(hlpt,data,2);
    
    tempCode=(data[0]<<8)+data[1];
    
    return tempCode;
}

/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
