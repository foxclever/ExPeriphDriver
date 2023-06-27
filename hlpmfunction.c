/******************************************************************************/
/** ģ�����ƣ���չ��������                                                   **/
/** �ļ����ƣ�hlpmfunction.c                                                 **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺ʵ��PM25���⴫����HLPM025K3�����ݶ�ȡ                          **/
/**           (1)�����ʣ�9600bit/S;����λ:8λ;ֹͣλ:1λ;У��λ:��           **/
/**           (2)���ݷ��ͼ��ʱ��Ϊ0.8��1.2S���룩                           **/
/**           (3)���ݸ�ʽ��7���ֽ�                                           **/
/**             +------+--------+--------+-------+-------+------+------+     **/
/**             |��ʼλ|PM2.5(H)|PM2.5(L)|PM10(H)|PM10(L)|У��λ|����λ|     **/
/**             +------+--------+--------+-------+-------+------+------+     **/
/**             | 0xAA | 1�ֽ�  | 1�ֽ�  | 1�ֽ� | 1�ֽ� | 1�ֽ�| 0xFF |     **/
/**             +------+--------+--------+-------+-------+------+------+     **/
/**               ����У��λ=PM2.5(H)+PM2.5(L)+PM10(H)+PM10(L)               **/
/**           (4)���ݴ������յ������ݰ���ʽ�����õ�PM2.5��PM10��ֵ��     **/
/**               PM2.5=(PM2.5(H)��256+PM2.5(L))��0.1                        **/
/**               PM10=(PM2.5(H)��256+PM2.5(L))��0.1                         **/
/**--------------------------------------------------------------------------**/
/**ʹ��˵����                                                                **/
/**    1�����ô���ͨѶ��������յ���Ϊ7���ֽڳ������ݣ��򴫸�ParsingPMData�� **/
/**       ��������ͬʱ�����ս���ı�������Ϊ2�������������飬�Բ�����ʽ����  **/
/**       ParsingPMData��������һ��Ԫ��ΪPM2.5����,�ڶ���Ԫ��ΪPM10���ݡ�    **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2013-07-28          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "hlpmfunction.h"

/*У�������Ƿ���Ч*/
static bool CheckDataIsValid(uint8_t *receivedData);
/*��������PM��ֵ*/
static float SynthesisPMValue(uint8_t *rawData);
/*�����յ������ݼ��뻺��*/
static void BufferDataEnQueue(HlpmObjectType *hlpm,uint8_t rData);
/*�ӻ����ж�ȡ����*/
static uint16_t BufferDataDeQueue(HlpmObjectType *hlpm,uint8_t *rxBuf);

/*����PM2.5��PM10������*/
bool ParsingPMData(HlpmObjectType *hlpm)
{
    uint16_t length=0;
    uint8_t receivedData[7]={0};
    bool isValid=false;
    
    length=hlpm->rxBuffer.DeQueue(hlpm,receivedData);
    
    if(length>0)
    {
        isValid=CheckDataIsValid(receivedData);
        if(isValid)
        {
            hlpm->pm25=SynthesisPMValue(receivedData+1);
            hlpm->pm100=SynthesisPMValue(receivedData+3);
        }
    }
    return isValid;
}

/*HLPM�����ʼ������*/
void HlpmInitialization(HlpmObjectType *hlpm)
{
    if(hlpm==NULL)
    {
        return;
    }
    
    hlpm->pm25=0.0;
    hlpm->pm100=0.0;
    
    hlpm->rxBuffer.pRead=0;
    hlpm->rxBuffer.pWrite=0;
    hlpm->rxBuffer.EnQueue=BufferDataEnQueue;
    hlpm->rxBuffer.DeQueue=BufferDataDeQueue;
}

/*У�������Ƿ���Ч*/
static bool CheckDataIsValid(uint8_t *receivedData)
{
    bool isValid=false;
    uint8_t checkSum=receivedData[1]+receivedData[2]+receivedData[3]+receivedData[4];
    if((receivedData[0]==0xAA)&&(receivedData[6]==0xFF)&&(checkSum==receivedData[5]))
    {
        isValid=true;
    }
    
    return isValid;
}

/*��������PM��ֵ*/
static float SynthesisPMValue(uint8_t *rawData)
{
    float result=0.0;
    result=(float)((uint16_t)rawData[0]*256+(uint16_t)rawData[1])*0.1;
    return result;
}

/*�ӻ����ж�ȡ����*/
static uint16_t BufferDataDeQueue(HlpmObjectType *hlpm,uint8_t *rxBuf)
{
    uint16_t length=0;
    uint8_t pTemp;
    
    if(hlpm->rxBuffer.pRead==hlpm->rxBuffer.pWrite)
    {
        length=0;
    }
    else
    {
        while(hlpm->rxBuffer.queue[hlpm->rxBuffer.pRead]!=0xAA)
        {
            if(++(hlpm->rxBuffer.pRead)>=(uint8_t)HLPMRxBufferLength)
            {
                hlpm->rxBuffer.pRead=0;
            }
            
            if(hlpm->rxBuffer.pRead==hlpm->rxBuffer.pWrite)
            {
                length=0;
                return length;
            }
        }
        
        pTemp=hlpm->rxBuffer.pRead;
        
        do
        {
            rxBuf[length++]=hlpm->rxBuffer.queue[hlpm->rxBuffer.pRead];
            
            if(++(hlpm->rxBuffer.pRead)>=(uint8_t)HLPMRxBufferLength)
            {
                hlpm->rxBuffer.pRead=0;
            }
            
            if(hlpm->rxBuffer.pRead==hlpm->rxBuffer.pWrite)
            {
                length=0;
                hlpm->rxBuffer.pRead=pTemp;
                return length;
            }
        }while(hlpm->rxBuffer.queue[hlpm->rxBuffer.pRead]!=0xFF);
        
        rxBuf[length++]=hlpm->rxBuffer.queue[hlpm->rxBuffer.pRead];
    }
    return length;
}

/*�����յ������ݼ��뻺��*/
static void BufferDataEnQueue(HlpmObjectType *hlpm,uint8_t rData)
{
    hlpm->rxBuffer.queue[hlpm->rxBuffer.pWrite]=rData;
    
    if(++(hlpm->rxBuffer.pWrite)>=(uint8_t)HLPMRxBufferLength)
    {
        hlpm->rxBuffer.pWrite=0;
    }
}
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
