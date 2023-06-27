/******************************************************************************/
/** ģ�����ƣ��Զ���ͨ�ù���                                                 **/
/** �ļ����ƣ�simuspifunction.c                                              **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʹ��GPIOģ��SPIӲ���ӿ�ͨѶ������C����ʵ�֣���վģʽ       **/
/**           SCKΪʱ�����ţ���վΪ���ģʽ                                  **/
/**           MOSIΪ��վ����������ţ�����Ϊ���ģʽ                         **/
/**           MISOΪ��վ�����������ţ�����Ϊ����ģʽ                         **/
/**           SPI�ӿڸ���ʼ�տ���״̬������ʱ�̵Ĳ�ͬ����4�ֹ���ģʽ��       **/
/**           +-------+----+----+-------------+--------+                     **/
/**           |SPIģʽ|CPOL|CPHA|����ʱSCK��ƽ|����ʱ��|                     **/
/**           +-------+----+----+-------------+--------+                     **/
/**           |   0   |  0 |  0 |    �͵�ƽ   |��������|                     **/
/**           +-------+----+----+-------------+--------+                     **/
/**           |   1   |  0 |  1 |    �͵�ƽ   |ż������|                     **/
/**           +-------+----+----+-------------+--------+                     **/
/**           |   2   |  1 |  0 |    �ߵ�ƽ   |��������|                     **/
/**           +-------+----+----+-------------+--------+                     **/
/**           |   3   |  1 |  1 |    �ߵ�ƽ   |ż������|                     **/
/**           +-------+----+----+-------------+--------+                     **/
/**           �ٶȿ����ø���0K������400K��������Ĭ��ֵΪ100K                 **/
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
/**     �汾      ����          ����      ˵��                               **/
/**     V1.0.0  2020-10-06      ľ��      �����ļ�                           **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "simuspifunction.h"

/* ͨ��ģ��SPI����һ���ֽ� */
static void SendByteBySimuSPI(SimuSPIObjectType *simuSPIInstance,uint8_t byte);
/* ͨ��ģ��SPI����һ���ֽ� */
static uint8_t RecieveByteBySimuSPI(SimuSPIObjectType *simuSPIInstance);
/*ͨ��ģ��SPIͬʱ���Ͳ�����һ���ֽ�*/
static uint8_t SendRecieveByteBySimuSPI(SimuSPIObjectType *simuSPIInstance,uint8_t byte);

/* GPIOģ��SPIͨѶ��ʼ�� */
void SimuSPIInitialization(SimuSPIObjectType *simuSPIInstance,//��ʼ����ģ��SPI����
                           uint32_t speed,      //ʱ��Ƶ��
                           SimuSPICPOLType CPOL,        //ʱ�Ӽ���
                           SimuSPICPHAType CPHA,        //ʱ��Ƶ��
                           SimuSPIDataSizeType dataSize,//���ݳ���
                           SimuSPISetSCKPin setSCK,     //SCKʱ�Ӳ�������ָ��
                           SimuSPISetMOSIPin setMOSI,   //MOSI��������ָ��
                           SimuSPIReadMISOPin getMISO,  //MISO��������ָ��
                           SimuSPIDelayus delayus       //΢����ʱ��������ָ��
                               )
{
    if((simuSPIInstance==NULL)||(setSCK==NULL)||(setMOSI==NULL)||(getMISO==NULL)||(delayus==NULL))
    {
        return;
    }
    
    simuSPIInstance->SetSCKPin=setSCK;
    simuSPIInstance->SetMOSIPin=setMOSI;
    simuSPIInstance->ReadMISOPin=getMISO;
    simuSPIInstance->Delayus=delayus;
    
    /*��ʼ���ٶ�,Ĭ��100K*/
    if((speed>0)&&(speed<=500))
    {
        simuSPIInstance->period=500/speed;
    }
    else
    {
        simuSPIInstance->period=5;
    }
    
    simuSPIInstance->CPOL=CPOL;
    simuSPIInstance->CPHA=CPHA;
    simuSPIInstance->dataSize=dataSize;
    
    /*�������ߣ�ʹ���ڿ���״̬*/
    
    if(simuSPIInstance->CPOL==SimuSPI_POLARITY_LOW)
    {
        simuSPIInstance->SetSCKPin(SimuSPI_Reset);
    }
    else
    {
        simuSPIInstance->SetSCKPin(SimuSPI_Set);
    }
}

/* ͨ��ģ��SPI����һ���ֽ� */
static void SendByteBySimuSPI(SimuSPIObjectType *simuSPIInstance,uint8_t byte)
{
//    uint8_t length[2]={8,16};
    
    if(simuSPIInstance->CPOL==SimuSPI_POLARITY_LOW)
    {
        /*����SCL����׼�����ݴ���*/
        simuSPIInstance->SetSCKPin(SimuSPI_Reset);
        
        if(simuSPIInstance->CPHA==SimuSPI_PHASE_1EDGE)  //ģʽ0
        {
            for(uint8_t count = 0; count < 8; count++)
            {
                if(byte & 0x80)     //ÿ�η������λ
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Set);
                }
                else
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Reset);
                }
                byte <<= 1;         //����һλ������һλ
                
                simuSPIInstance->Delayus(simuSPIInstance->period);
                simuSPIInstance->SetSCKPin(SimuSPI_Set);
                simuSPIInstance->Delayus(simuSPIInstance->period);
                simuSPIInstance->SetSCKPin(SimuSPI_Reset);
            }
        }
        else    //ģʽ1
        {
            for(uint8_t count = 0; count < 8; count++)
            {
                if(byte & 0x80)     //ÿ�η������λ
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Set);
                }
                else
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Reset);
                }
                byte <<= 1;         //����һλ������һλ
                
                simuSPIInstance->SetSCKPin(SimuSPI_Set);
                simuSPIInstance->Delayus(simuSPIInstance->period);
                simuSPIInstance->SetSCKPin(SimuSPI_Reset);
                simuSPIInstance->Delayus(simuSPIInstance->period);
            }
        }
    }
    else
    {
        /*����SCL����׼�����ݴ���*/
        simuSPIInstance->SetSCKPin(SimuSPI_Set);
        
        if(simuSPIInstance->CPHA==SimuSPI_PHASE_1EDGE)  //ģʽ2
        {
            for(uint8_t count = 0; count < 8; count++)
            {
                if(byte & 0x80)     //ÿ�η������λ
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Set);
                }
                else
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Reset);
                }
                byte <<= 1;         //����һλ������һλ
                
                simuSPIInstance->Delayus(simuSPIInstance->period);
                simuSPIInstance->SetSCKPin(SimuSPI_Reset);
                simuSPIInstance->Delayus(simuSPIInstance->period);
                simuSPIInstance->SetSCKPin(SimuSPI_Set);
            }
        }
        else    //ģʽ3
        {
            for(uint8_t count = 0; count < 8; count++)
            {
                if(byte & 0x80)     //ÿ�η������λ
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Set);
                }
                else
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Reset);
                }
                byte <<= 1;         //����һλ������һλ
                
                simuSPIInstance->Delayus(simuSPIInstance->period);
                simuSPIInstance->SetSCKPin(SimuSPI_Reset);
                simuSPIInstance->Delayus(simuSPIInstance->period);
                simuSPIInstance->SetSCKPin(SimuSPI_Set);
            }
        }
    }
}

/* ͨ��ģ��SPI����һ���ֽ� */
static uint8_t RecieveByteBySimuSPI(SimuSPIObjectType *simuSPIInstance)
{
    uint8_t receive = 0;
    
    if(simuSPIInstance->CPOL==SimuSPI_POLARITY_LOW)
    {
        /*����SCL����׼�����ݴ���*/
        simuSPIInstance->SetSCKPin(SimuSPI_Reset);
        
        if(simuSPIInstance->CPHA==SimuSPI_PHASE_1EDGE)  //ģʽ0
        {
            for(uint8_t count = 0; count < 8; count++ )
            {
                simuSPIInstance->SetSCKPin(SimuSPI_Set);
                simuSPIInstance->Delayus(simuSPIInstance->period);
                
                receive <<= 1;
                
                if(simuSPIInstance->ReadMISOPin())
                {
                    receive++;
                }
                simuSPIInstance->SetSCKPin(SimuSPI_Reset);
                simuSPIInstance->Delayus(simuSPIInstance->period);
            }
        }
        else    //ģʽ1
        {
            simuSPIInstance->SetSCKPin(SimuSPI_Set);
            simuSPIInstance->Delayus(simuSPIInstance->period);
            for(uint8_t count = 0; count < 8; count++ )
            {
                simuSPIInstance->SetSCKPin(SimuSPI_Reset);
                simuSPIInstance->Delayus(simuSPIInstance->period);
                
                receive <<= 1;
                
                if(simuSPIInstance->ReadMISOPin())
                {
                    receive++;
                }
                simuSPIInstance->SetSCKPin(SimuSPI_Set);
                simuSPIInstance->Delayus(simuSPIInstance->period);
            }
            simuSPIInstance->SetSCKPin(SimuSPI_Reset);
        }
    }
    else
    {
        /*����SCL����׼�����ݴ���*/
        simuSPIInstance->SetSCKPin(SimuSPI_Set);
        
        if(simuSPIInstance->CPHA==SimuSPI_PHASE_1EDGE)  //ģʽ2
        {
            for(uint8_t count = 0; count < 8; count++ )
            {
                simuSPIInstance->SetSCKPin(SimuSPI_Reset);
                simuSPIInstance->Delayus(simuSPIInstance->period);
                
                receive <<= 1;
                
                if(simuSPIInstance->ReadMISOPin())
                {
                    receive++;
                }
                simuSPIInstance->SetSCKPin(SimuSPI_Set);
                simuSPIInstance->Delayus(simuSPIInstance->period);
            }
        }
        else    //ģʽ3
        {
            simuSPIInstance->SetSCKPin(SimuSPI_Reset);
            simuSPIInstance->Delayus(simuSPIInstance->period);
            for(uint8_t count = 0; count < 8; count++ )
            {
                simuSPIInstance->SetSCKPin(SimuSPI_Set);
                simuSPIInstance->Delayus(simuSPIInstance->period);
                
                receive <<= 1;
                
                if(simuSPIInstance->ReadMISOPin())
                {
                    receive++;
                }
                simuSPIInstance->SetSCKPin(SimuSPI_Reset);
                simuSPIInstance->Delayus(simuSPIInstance->period);
            }
            simuSPIInstance->SetSCKPin(SimuSPI_Set);
        }
    }
    
    return receive;
}

/*ͨ��ģ��SPIͬʱ���Ͳ�����һ���ֽ�*/
static uint8_t SendRecieveByteBySimuSPI(SimuSPIObjectType *simuSPIInstance,uint8_t byte)
{
    uint8_t receive = 0;
    
    if(simuSPIInstance->CPOL==SimuSPI_POLARITY_LOW)
    {
        /*����SCL����׼�����ݴ���*/
        simuSPIInstance->SetSCKPin(SimuSPI_Reset);
        
        if(simuSPIInstance->CPHA==SimuSPI_PHASE_1EDGE)  //ģʽ0
        {
            for(uint8_t count = 0; count < 8; count++)
            {
                if(byte & 0x80)     //ÿ�η������λ
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Set);
                }
                else
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Reset);
                }
                byte <<= 1;         //����һλ������һλ
                
                simuSPIInstance->Delayus(simuSPIInstance->period);
                simuSPIInstance->SetSCKPin(SimuSPI_Set);
                simuSPIInstance->Delayus(simuSPIInstance->period);
                
                receive <<= 1;
                if(simuSPIInstance->ReadMISOPin())
                {
                    receive++;
                }
                simuSPIInstance->SetSCKPin(SimuSPI_Reset);
            }
        }
        else    //ģʽ1
        {
            simuSPIInstance->SetSCKPin(SimuSPI_Set);
            for(uint8_t count = 0; count < 8; count++)
            {
                if(byte & 0x80)     //ÿ�η������λ
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Set);
                }
                else
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Reset);
                }
                byte <<= 1;         //����һλ������һλ
                
                simuSPIInstance->Delayus(simuSPIInstance->period);
                simuSPIInstance->SetSCKPin(SimuSPI_Reset);
                simuSPIInstance->Delayus(simuSPIInstance->period);
                
                receive <<= 1;
                if(simuSPIInstance->ReadMISOPin())
                {
                    receive++;
                }
                simuSPIInstance->SetSCKPin(SimuSPI_Set);
            }
            simuSPIInstance->SetSCKPin(SimuSPI_Reset);
        }
    }
    else
    {
        /*����SCL����׼�����ݴ���*/
        simuSPIInstance->SetSCKPin(SimuSPI_Set);
        
        if(simuSPIInstance->CPHA==SimuSPI_PHASE_1EDGE)  //ģʽ2
        {
            for(uint8_t count = 0; count < 8; count++)
            {
                if(byte & 0x80)     //ÿ�η������λ
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Set);
                }
                else
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Reset);
                }
                byte <<= 1;         //����һλ������һλ
                
                simuSPIInstance->Delayus(simuSPIInstance->period);
                simuSPIInstance->SetSCKPin(SimuSPI_Reset);
                simuSPIInstance->Delayus(simuSPIInstance->period);
                
                receive <<= 1;
                if(simuSPIInstance->ReadMISOPin())
                {
                    receive++;
                }
                simuSPIInstance->SetSCKPin(SimuSPI_Set);
            }
        }
        else    //ģʽ3
        {
            simuSPIInstance->SetSCKPin(SimuSPI_Reset);
            for(uint8_t count = 0; count < 8; count++)
            {
                if(byte & 0x80)     //ÿ�η������λ
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Set);
                }
                else
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Reset);
                }
                byte <<= 1;         //����һλ������һλ
                
                simuSPIInstance->Delayus(simuSPIInstance->period);
                simuSPIInstance->SetSCKPin(SimuSPI_Set);
                simuSPIInstance->Delayus(simuSPIInstance->period);
                
                receive <<= 1;
                if(simuSPIInstance->ReadMISOPin())
                {
                    receive++;
                }
                simuSPIInstance->SetSCKPin(SimuSPI_Reset);
            }
        }
    }
    
    return receive;
}

/* ͨ��ģ��SPI���վд���� */
SimuSPIStatusType WriteDataBySimuSPI(SimuSPIObjectType *simuSPIInstance,uint8_t *wData,uint16_t wSize,uint16_t timeOut)
{
    SimuSPIStatusType ret=SimuSPI_OK;
        
    for(int i=0;i<wSize;i++)
    {
        SendByteBySimuSPI(simuSPIInstance,wData[i]);
    }
    
    return ret;
}

/* ͨ��ģ��SPI�Դ�վ������ */
SimuSPIStatusType ReadDataBySimuSPI(SimuSPIObjectType *simuSPIInstance,uint8_t *rData, uint16_t rSize,uint16_t timeOut)
{
    SimuSPIStatusType ret=SimuSPI_OK;
    
    for(int i=0;i<rSize;i++)
    {
        rData[i]=RecieveByteBySimuSPI(simuSPIInstance);
    }
    
    return ret;
}

/* ͨ��ģ��SPIʵ�ֶԴ�վ��д���ݽ��Ӷ�������ϲ��� */
SimuSPIStatusType WriteReadDataBySimuSPI(SimuSPIObjectType *simuSPIInstance, uint8_t *wData,uint16_t wSize,uint8_t *rData, uint16_t rSize,uint16_t timeOut)
{
    SimuSPIStatusType ret=SimuSPI_OK;
    
    for(int i=0;i<wSize;i++)
    {
        SendByteBySimuSPI(simuSPIInstance,wData[i]);
    }
    
    for(int i=0;i<rSize;i++)
    {
        rData[i]=RecieveByteBySimuSPI(simuSPIInstance);
    }
    
    return ret;
}

/* ͨ��ģ��SPIʵ�ֶԴ�վͬʱд�Ͷ�������ϲ���*/
SimuSPIStatusType WriteWhileReadDataBySimuSPI(SimuSPIObjectType *simuSPIInstance, uint8_t *wData,uint8_t *rData,uint16_t pSize,uint16_t timeOut)
{
    SimuSPIStatusType ret=SimuSPI_OK;
        
    for(int i=0;i<pSize;i++)
    {
        rData[i]= SendRecieveByteBySimuSPI(simuSPIInstance,wData[i]);
    }
    
    return ret;
}

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/
