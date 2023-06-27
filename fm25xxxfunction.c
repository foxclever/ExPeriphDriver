/******************************************************************************/
/** ģ�����ƣ���չ��������                                                   **/
/** �ļ����ƣ�fm25xxxfunction.c                                              **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʵ��FM25ϵ�д���FRAM�Ĳ���������4K��16K��64K��128K��256K�� **/
/**           512K��1M�ȶ�����������ͬ��������õ��豸��ַλ���Ĵ�����ַλ�� **/
/**           �ڲ��죬�����������£�                                         **/
/**           +----------+---------------+-----------------+--------+        **/
/**           |   �ͺ�   |   �����ṹ    |    �Ĵ�����ַ   |ÿҳ�ֽ�|        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25L04B | 512x8(4K)     | 9λ(1���Ĵ���)  |  8�ֽ� |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25040B | 512x8(4K)     | 9λ(1���Ĵ���)  |  8�ֽ� |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25C160B| 2048x8(16K)   | 11λ(1���Ĵ���) | 32�ֽ� |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25L16B | 2048x8(16K)   | 11λ(1���Ĵ���) | 32�ֽ� |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25CL64B| 8192x8(64K)   | 13λ(2���Ĵ���) | 32�ֽ� |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25640B | 8192x8(64K)   | 13λ(2���Ĵ���) | 32�ֽ� |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25V01A | 16384x8(128K) | 14λ(2���Ĵ���) | 64�ֽ� |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25W256 | 32768x8(256K) | 15λ(2���Ĵ���) | 64�ֽ� |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25V02A | 32768x8(256K) | 15λ(2���Ĵ���) | 64�ֽ� |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25V05  | 65536x8(512K) | 16λ(2���Ĵ���) | 128�ֽ�|        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25V10  | 131072x8(1M)  | 17λ(3���Ĵ���) | 256�ֽ�|        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           FM25ϵ��FRAM����SPI�ӿڣ�SPI�ӿ�ģʽ֧��Mode0��Mode3��         **/
/**                   SPI Mode 0 (CPOL = 0, CPHA = 0)                        **/
/**                   SPI Mode 3 (CPOL = 1, CPHA = 1)                        **/
/**           ״̬�Ĵ���������ӳ״̬����������д������ת̨�Ĵ�����λ��       **/
/**           +-------+------+------+------+------+------+------+------+     **/
/**           |  Bit7 | Bit6 | Bit5 | Bit4 | Bit3 | Bit2 | Bit1 | Bit0 |     **/
/**           +-------+------+------+------+------+------+------+------+     **/
/**           |WPEN(0)| X(0) | X(0) | X(0) |BP1(0)|BP0(0)|WEL(0)| X(0) |     **/
/**           +-------+------+------+------+------+------+------+------+     **/
/**           ����WEL��дʹ��������������WPEN��BP1��BP0�����趨��            **/
/**           BP1��BP0��������������Ǳ�������Ĵ�С��                       **/
/**             +-----+-----+------------------+                             **/
/**             | BP1 | BP0 |  ����������Χ  |                             **/
/**             +-----+-----+------------------+                             **/
/**             |  0  |  0  | None             |                             **/
/**             +-----+-----+------------------+                             **/
/**             |  0  |  1  | 1800h-1FFFh(1/4) |                             **/
/**             +-----+-----+------------------+                             **/
/**             |  1  |  0  | 1000h-1FFFh(1/2) |                             **/
/**             +-----+-----+------------------+                             **/
/**             |  1  |  1  | 0000h-1FFFh(ȫ��)|                             **/
/**             +-----+-----+------------------+                             **/
/**           FM25Cxxϵ�д洢�����ö���д��������ϵ���£�                    **/
/**           +-----+------+----+----------+----------+----------+           **/
/**           | WEL | WPEN | WP |����������|����������|״̬�Ĵ���|           **/
/**           +-----+------+----+----------+----------+----------+           **/
/**           |  0  |  X   | X  |  ������  |  ������  |  ������  |           **/
/**           +-----+------+----+----------+----------+----------+           **/
/**           |  1  |  0   | X  |  ������  |  ������  |  ������  |           **/
/**           +-----+------+----+----------+----------+----------+           **/
/**           |  1  |  1   | 0  |  ������  |  ������  |  ������  |           **/
/**           +-----+------+----+----------+----------+----------+           **/
/**           |  1  |  1   | 1  |  ������  |  ������  |  ������  |           **/
/**           +-----+------+----+----------+----------+----------+           **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2017-07-17          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "fm25xxxfunction.h"

/*ȱʡƬѡ������*/
static void FM25ChipSelectDefault(FM25CSType cs);

/*��FM25XXX��ȡ�����ֽ�,�������ַ��ȡ*/
uint8_t ReadByteFromFM25xxx(FM25ObjectType *fram,uint32_t regAddress)
{
    uint8_t rData;
    
    ReadBytesFromFM25xxx(fram,regAddress,&rData,1);
    
    return rData;
}

/*��FM25XXXд�뵥���ֽ�*/
void WriteByteToFM25xxx(FM25ObjectType *fram,uint32_t regAddress,uint8_t data)
{
    WriteBytesToFM25xxx(fram,regAddress,&data,1);
}

/* ����дʹ��������*/
void SetWriteEnableLatchForFM25xxx(FM25ObjectType *fram)
{
    uint8_t opCode=FM25_WREN;
    
    fram->ChipSelect(FM25CS_Enable);
    fram->Write(&opCode,1);
    fram->ChipSelect(FM25CS_Enable);
    
    ReadStatusForFM25xxx(fram);
}

/* ��λдʹ��������*/
void ResetWriteEnableLatchForFM25xxx(FM25ObjectType *fram)
{
    uint8_t opCode=FM25_WRDI;
    
    fram->ChipSelect(FM25CS_Enable);
    fram->Write(&opCode,1);
    fram->ChipSelect(FM25CS_Enable);
    
    ReadStatusForFM25xxx(fram);
}

/*��FM25xxx״̬�Ĵ���*/
void ReadStatusForFM25xxx(FM25ObjectType *fram)
{
    uint8_t opCode=FM25_RDSR;
    uint8_t status;
    
    fram->ChipSelect(FM25CS_Enable);
    fram->Write(&opCode,1);
    fram->Delayms(1);
    fram->Read(&status,1);
    fram->ChipSelect(FM25CS_Enable);
    
    fram->status=status;
}

/*дFM25xxx״̬�Ĵ���*/
void WriteStatusForFM25xx(FM25ObjectType *fram,uint8_t cmd)
{
    uint8_t data[2];
    
    data[0]=FM25_WRSR;
    data[1]=cmd;
    
    if(((fram->status)&0x02)!=0x02)
    {
        SetWriteEnableLatchForFM25xxx(fram);
    }
    
    if((((fram->status)&FM25_WPEN)!=FM25_WPEN)&&(fram->WP!=NULL))
    {
        fram->WP(FM25WP_Disable);
    }
    
    fram->ChipSelect(FM25CS_Enable);
    fram->Write(data,2);
    fram->ChipSelect(FM25CS_Disable);
    
    ReadStatusForFM25xxx(fram);
    
    if(fram->WP!=NULL)
    {
        fram->WP(FM25WP_Enable);
    }
}

/*��FM25xxx��ȡ����*/
void ReadBytesFromFM25xxx(FM25ObjectType *fram,uint32_t regAddress,uint8_t *rData,uint16_t rSize)
{
    uint8_t data[4];
    uint16_t index=0;
    uint8_t temp;
    
    data[index++]=FM25_READ;
    
    if(fram->memAddLength==FM258BitMemAdd)
    {
        data[index++]=(uint8_t)regAddress;
        
        if((fram->mode==FM25L04B)||(fram->mode==FM25040B))
        {
            temp=(uint8_t)(regAddress>>8);
            data[0]|=((temp&0x01)<<3);
        }
    }
    else if(fram->memAddLength==FM2516BitMemAdd)
    {
        data[index++]=(uint8_t)(regAddress>>8);
        data[index++]=(uint8_t)regAddress;
    }
    else
    {
        data[index++]=(uint8_t)(regAddress>>16);
        data[index++]=(uint8_t)(regAddress>>8);
        data[index++]=(uint8_t)regAddress;
    }
    
    fram->ChipSelect(FM25CS_Enable);
    fram->Write(data,index);
    fram->Delayms(1);
    fram->Read(rData,rSize);
    fram->ChipSelect(FM25CS_Disable);
}

/*��FM25xxxд������*/
void WriteBytesToFM25xxx(FM25ObjectType *fram,uint32_t regAddress,uint8_t *wData,uint16_t wSize)
{
    uint8_t data[128];
    uint8_t temp;
    uint16_t index=0;
    
    data[index++]=FM25_WRITE;
    
    if(fram->memAddLength==FM258BitMemAdd)
    {
        data[index++]=(uint8_t)regAddress;
        
        if((fram->mode==FM25L04B)||(fram->mode==FM25040B))
        {
            temp=(uint8_t)(regAddress>>8);
            data[0]|=((temp&0x01)<<3);
        }
    }
    else if(fram->memAddLength==FM2516BitMemAdd)
    {
        data[index++]=(uint8_t)(regAddress>>8);
        data[index++]=(uint8_t)regAddress;
    }
    else
    {
        data[index++]=(uint8_t)(regAddress>>16);
        data[index++]=(uint8_t)(regAddress>>8);
        data[index++]=(uint8_t)regAddress;
    }
    
    for(int i=0;i<wSize;i++)
    {
        data[index++]=wData[i];
    }
    
    if(((fram->status)&0x02)!=0x02)
    {
        SetWriteEnableLatchForFM25xxx(fram);
    }
    
    if(((fram->status)&0x0C)!=0x00)
    {
        WriteStatusForFM25xx(fram,fram->status|FM25_BPNONE);
    }
    
    fram->ChipSelect(FM25CS_Enable);
    fram->Write(data,index);
    fram->ChipSelect(FM25CS_Disable);
    
    WriteStatusForFM25xx(fram,fram->status|FM25_BPALL);
}

/*FM25�����ʼ��*/
void Fm25cxxInitialization(FM25ObjectType *fram,        //FM25xxx����ʵ��
                           FM25ModeType mode,          //�豸����
                           Fm25Read read,              //��FM25xxx�������ָ��
                           Fm25Write write,            //дFM25xxx�������ָ��
                           Fm25Delayms delayms,        //��ʱ����ָ��
                           Fm25WP wp,                  //д������������ָ��
                           Fm25ChipSelect cs,          //Ƭѡ�źź���ָ��
                           Fm25Hold hold               //�����źŲ�������ָ��
                               )
{
    if((fram==NULL)||(read==NULL)||(write==NULL)||(delayms==NULL))
    {
        return;
    }
    fram->Read=read;
    fram->Write=write;
    fram->Delayms=delayms;
    
    if(cs!=NULL)
    {
        fram->ChipSelect=cs;
    }
    else
    {
        fram->ChipSelect=FM25ChipSelectDefault;
    }
    
    if(mode>=FM25Number)
    {
        return;
    }
    fram->mode=mode;
    
    if(mode<FM25C160B)
    {
        fram->memAddLength=FM258BitMemAdd;
    }
    else if(mode<FM25V10)
    {
        fram->memAddLength=FM2516BitMemAdd;
    }
    else
    {
        fram->memAddLength=FM2524BitMemAdd;
    }
    
    ReadStatusForFM25xxx(fram);
    
    //д����
    SetWriteEnableLatchForFM25xxx(fram);
    
    uint8_t cmd;
    //ʹ��д����������ȫ������
    cmd=fram->status|FM25_WPEN|FM25_BPALL;
    WriteStatusForFM25xx(fram,cmd);
    
    ReadStatusForFM25xxx(fram);
}

/*ȱʡƬѡ������*/
static void FM25ChipSelectDefault(FM25CSType cs)
{
    //������SPIͨѶʱ��Ƭѡ�ź�Ӳ����·ѡ�е����
    return;
}
/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
