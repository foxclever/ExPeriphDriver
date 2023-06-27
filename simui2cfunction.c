/******************************************************************************/
/** ģ�����ƣ��Զ���ͨ�ù���                                                 **/
/** �ļ����ƣ�simui2cfunction.c                                              **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʹ��GPIOģ��I2CӲ���ӿ�ͨѶ                                **/
/**           ����C���Ա�д��������ͨ��MCU                                   **/
/**           SCL��������Ϊ�������                                          **/
/**           SDA��������Ϊ��©������ڶ�ȡʱ����Ϊ�ߵ�ƽ                    **/
/**           �ٶȿ����ø���0K������500K��������Ĭ��ֵΪ100K                 **/
/**--------------------------------------------------------------------------**/
/**ʹ��˵����                                                                **/
/**    1������GPIOģ��I2C����ʵ�壨�磺SimuI2CTypeDef si2c���������ú���     **/
/**       SimuI2CInitialization��ʼ������ʵ�塣��ʼ��ʱ����������Ƶ��1-500K  **/
/**       ��kHzΪ��λ����Сʼ������Ϊ2΢�룬���Ϊ500k΢�롣                 **/
/**    2��ʵ������void f(SimuI2CPinValue op)�ĺ����������丳ֵ               **/
/**       ��SimuI2CTypeDef����ʵ���SetSCLPin����ָ�롣���Բ���SCL���š�     **/
/**    3��ʵ������void f(SimuI2CPinValue op)�ĺ����������丳ֵ               **/
/**       ��SimuI2CTypeDef����ʵ���SetSDAPin����ָ�롣���Բ���SDA���š�     **/
/**    4��ʵ������uint8_t f(void)�ĺ����������丳ֵ��SimuI2CTypeDef����ʵ��  **/
/**       ��ReadSDAPin����ָ�롣���Զ�ȡSDA���š�                            **/
/**    5��ʵ������void f(volatile uint32_t period)�ĺ����������丳ֵ         **/
/**       ��SimuI2CTypeDef����ʵ���Delayus����ָ�롣����������ʱ����λΪ΢��**/
/**    6��������Ӧ�ĺ���ʵ����Ҫ�Ĳ�����                                     **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2017-05-15          ľ��              �����ļ�               **/
/**     V1.0.1  2018-08-28          ľ��              �޸�Ϊ�������         **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "simui2cfunction.h"

/* ģ��I2CͨѶ��ʼ���� */
static void SimuI2CStart(SimuI2CObjectType *simuI2CInstance);

/* ģ��I2CͨѶ��ʼ���� */
static void SimuI2CStop(SimuI2CObjectType *simuI2CInstance);

/* ģ��I2C�ȴ�ȷ�� */
static SimuI2CStatus SimuI2CWaitAck(SimuI2CObjectType *simuI2CInstance,uint16_t timeOut);

/* ģ��I2C����һ��Ӧ��*/
static void IIC_Ack(SimuI2CObjectType *simuI2CInstance);

/* ģ��I2C����һ����Ӧ�� */
static void IIC_NAck(SimuI2CObjectType *simuI2CInstance);

/* ͨ��ģ��I2C����һ���ֽ� */
static void SendByteBySimuI2C(SimuI2CObjectType *simuI2CInstance,uint8_t byte);

/* ͨ��ģ��I2C����һ���ֽ� */
static uint8_t RecieveByteBySimuI2C(SimuI2CObjectType *simuI2CInstance);

/* GPIOģ��I2CͨѶ��ʼ�� */
void SimuI2CInitialization(SimuI2CObjectType *simuI2CInstance,
                           uint32_t speed,
                           SimuI2CSetPin setSCL,
                           SimuI2CSetPin setSDA,
                           SimuI2CReadSDAPin readSDA,
                           SimuI2CDelayus delayus)
{
    if((simuI2CInstance==NULL)||(setSCL==NULL)||(setSDA==NULL)||(readSDA==NULL)||(delayus==NULL))
    {
        return;
    }
    
    simuI2CInstance->SetSCLPin=setSCL;
    simuI2CInstance->SetSDAPin=setSDA;
    simuI2CInstance->ReadSDAPin=readSDA;
    simuI2CInstance->Delayus=delayus;
    
    /*��ʼ���ٶ�,Ĭ��100K*/
    if((speed>0)&&(speed<=400))
    {
        simuI2CInstance->period=500/speed;
    }
    else
    {
        simuI2CInstance->period=5;
    }
    
    /*�������ߣ�ʹ���ڿ���״̬*/
    simuI2CInstance->SetSDAPin(Set);
    simuI2CInstance->SetSCLPin(Set);
}

/* ģ��I2CͨѶ��ʼ���� */
static void SimuI2CStart(SimuI2CObjectType *simuI2CInstance)
{
    /*�������ߣ�ʹ���ڿ���״̬*/
    simuI2CInstance->SetSDAPin(Set);
    simuI2CInstance->SetSCLPin(Set);
    simuI2CInstance->Delayus(simuI2CInstance->period);
    
    /*��SCLΪ��ʱ��SDAһ���½�����ʼ*/
    simuI2CInstance->SetSDAPin(Reset);
    simuI2CInstance->Delayus(simuI2CInstance->period);
    
    /*ǶסSCL�Ա㷢������*/
    simuI2CInstance->SetSCLPin(Reset);
    simuI2CInstance->Delayus(simuI2CInstance->period);
}

/* ģ��I2CͨѶֹͣ���� */
static void SimuI2CStop(SimuI2CObjectType *simuI2CInstance)
{
    simuI2CInstance->SetSCLPin(Reset);
    simuI2CInstance->Delayus(simuI2CInstance->period);
    
    simuI2CInstance->SetSDAPin(Reset);
    simuI2CInstance->Delayus(simuI2CInstance->period);
    
    simuI2CInstance->SetSCLPin(Set);
    simuI2CInstance->Delayus(simuI2CInstance->period);
    
    simuI2CInstance->SetSDAPin(Set);
    simuI2CInstance->Delayus(simuI2CInstance->period);
}

/* ģ��I2C�ȴ�ȷ�� */
static SimuI2CStatus SimuI2CWaitAck(SimuI2CObjectType *simuI2CInstance,uint16_t timeOut)
{
    simuI2CInstance->SetSDAPin(Set);//����SDA
    simuI2CInstance->Delayus(simuI2CInstance->period);
    simuI2CInstance->SetSCLPin(Set);//����SCL
    simuI2CInstance->Delayus(simuI2CInstance->period);
    
    while(simuI2CInstance->ReadSDAPin())           //��ȡ��������
    {
        if(--timeOut)
        {
            SimuI2CStop(simuI2CInstance);
            return I2C_ERROR;
        }
        simuI2CInstance->Delayus(simuI2CInstance->period);
    }
    simuI2CInstance->SetSCLPin(Reset);
    simuI2CInstance->Delayus(simuI2CInstance->period);
    return I2C_OK;
}

/* ģ��I2C����һ��Ӧ��*/
static void IIC_Ack(SimuI2CObjectType *simuI2CInstance)
{
    simuI2CInstance->SetSDAPin(Reset);
    simuI2CInstance->Delayus(simuI2CInstance->period);
    
    simuI2CInstance->SetSCLPin(Set);
    simuI2CInstance->Delayus(simuI2CInstance->period);
    
    simuI2CInstance->SetSCLPin(Reset);
    simuI2CInstance->Delayus(simuI2CInstance->period);
    
    simuI2CInstance->SetSDAPin(Set);
}

/* ģ��I2C����һ����Ӧ�� */
static void IIC_NAck(SimuI2CObjectType *simuI2CInstance)
{
    simuI2CInstance->SetSCLPin(Reset);
    simuI2CInstance->SetSDAPin(Set);
    simuI2CInstance->Delayus(simuI2CInstance->period);
    
    simuI2CInstance->SetSCLPin(Set);
    simuI2CInstance->Delayus(simuI2CInstance->period);
    simuI2CInstance->SetSCLPin(Reset);
    simuI2CInstance->Delayus(simuI2CInstance->period);
}

/* ͨ��ģ��I2C����һ���ֽ� */
static void SendByteBySimuI2C(SimuI2CObjectType *simuI2CInstance,uint8_t byte)
{
    /*����SCL����׼�����ݴ���*/
    simuI2CInstance->SetSCLPin(Reset);
    
    for(uint8_t count = 0; count < 8; count++)
    {
        if(byte & 0x80)     //ÿ�η������λ
        {
            simuI2CInstance->SetSDAPin(Set);
        }
        else
        {
            simuI2CInstance->SetSDAPin(Reset);
        }
        byte <<= 1;         //����һλ������һλ
        
        simuI2CInstance->Delayus(simuI2CInstance->period);
        simuI2CInstance->SetSCLPin(Set);
        simuI2CInstance->Delayus(simuI2CInstance->period);
        simuI2CInstance->SetSCLPin(Reset);
    }
    
    //  simuI2CInstance->Delayus(simuI2CInstance->period);
    //  simuI2CInstance->SetSDAPin(Set);
}

/* ͨ��ģ��I2C����һ���ֽ� */
static uint8_t RecieveByteBySimuI2C(SimuI2CObjectType *simuI2CInstance)
{
    uint8_t receive = 0;
    
    simuI2CInstance->SetSDAPin(Set);//��������Ϊ��©���ʱ����Ҫ�ø߲��ܶ�ȡ����
    
    for(uint8_t count = 0; count < 8; count++ )
    {
        //    simuI2CInstance->SetSCLPin(Reset);
        //    simuI2CInstance->Delayus(simuI2CInstance->period);
        simuI2CInstance->SetSCLPin(Set);
        simuI2CInstance->Delayus(simuI2CInstance->period);
        
        receive <<= 1;
        
        if(simuI2CInstance->ReadSDAPin())
        {
            receive++;
        }
        simuI2CInstance->SetSCLPin(Reset);
        simuI2CInstance->Delayus(simuI2CInstance->period);
    }
    return receive;	
}

/* ͨ��ģ��I2C���վд���� */
SimuI2CStatus WriteDataBySimuI2C(SimuI2CObjectType *simuI2CInstance,uint8_t deviceAddress,uint8_t *wData,uint16_t wSize)
{
    //����ͨѶ
    SimuI2CStart(simuI2CInstance);
    //���ʹ�վ��ַ��д��
    SendByteBySimuI2C(simuI2CInstance,deviceAddress);
    if(SimuI2CWaitAck(simuI2CInstance,5000))
    {
        return I2C_ERROR;
    }
    
    while(wSize--)
    {
        SendByteBySimuI2C(simuI2CInstance,*wData);
        if(SimuI2CWaitAck(simuI2CInstance,5000))
        {
            return I2C_ERROR;
        }
        wData++;
        simuI2CInstance->Delayus(10);
    }
    
    SimuI2CStop(simuI2CInstance);
    return I2C_OK;
}

/* ͨ��ģ��I2C�Դ�վ������ */
SimuI2CStatus ReadDataBySimuI2C(SimuI2CObjectType *simuI2CInstance,uint8_t deviceAddress,uint8_t *rData, uint16_t rSize)
{
    //����ͨѶ
    SimuI2CStart(simuI2CInstance);
    
    //���ʹ�վ��ַ������
    SendByteBySimuI2C(simuI2CInstance,deviceAddress+1);
    if(SimuI2CWaitAck(simuI2CInstance,5000))
    {
        return I2C_ERROR;
    }
    
    simuI2CInstance->Delayus(1000);
    
    while(rSize--)
    {
        *rData=RecieveByteBySimuI2C(simuI2CInstance);
        rData++;
        if(rSize==0)
        {
            IIC_NAck(simuI2CInstance);
        }
        else
        {
            IIC_Ack(simuI2CInstance);
            simuI2CInstance->Delayus(1000);
        }
    }
    
    //����ͨѶ
    SimuI2CStop(simuI2CInstance);
    return I2C_OK;
}

/* ͨ��ģ��I2Cʵ�ֶԴ�վ��д���ݽ��Ӷ�������ϲ��� */
SimuI2CStatus WriteReadDataBySimuI2C(SimuI2CObjectType *simuI2CInstance,uint8_t deviceAddress, uint8_t *wData,uint16_t wSize,uint8_t *rData, uint16_t rSize)
{
    //����ͨѶ
    SimuI2CStart(simuI2CInstance);
    //���ʹ�վ��ַ��д��
    SendByteBySimuI2C(simuI2CInstance,deviceAddress);
    if(SimuI2CWaitAck(simuI2CInstance,5000))
    {
        return I2C_ERROR;
    }
    
    while(wSize--)
    {
        SendByteBySimuI2C(simuI2CInstance,*wData);
        if(SimuI2CWaitAck(simuI2CInstance,5000))
        {
            return I2C_ERROR;
        }
        wData++;
        simuI2CInstance->Delayus(10);
    }
    
    //������
    SimuI2CStart(simuI2CInstance);
    //���ʹ�վ��ַ������
    SendByteBySimuI2C(simuI2CInstance,deviceAddress+1);
    if(SimuI2CWaitAck(simuI2CInstance,5000))
    {
        return I2C_ERROR;
    }
    
    while(rSize--)
    {
        *rData=RecieveByteBySimuI2C(simuI2CInstance);
        rData++;
        if(rSize==0)
        {
            IIC_NAck(simuI2CInstance);
        }
        else
        {
            IIC_Ack(simuI2CInstance);
        }
    }
    //����ͨѶ
    SimuI2CStop(simuI2CInstance);
    return I2C_OK;
}
/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/
