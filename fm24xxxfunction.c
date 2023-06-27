/******************************************************************************/
/** ģ�����ƣ���չ��������                                                   **/
/** �ļ����ƣ�fm24xxxfunction.c                                              **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʵ�����ƴ���FRAM�Ĳ�����FM24XXXϵ��FRAM����4K��16K��64K��  **/
/**           128K��256K��512K��1M�ȶ�����������ͬ��������õ��豸��ַλ���� **/
/**           ������ַλ���ڲ���ÿһҳ���������ֽ���Ҳ��һ���������������£� **/
/**  +----------+-------------+--------------------+---------------+--------+**/
/**  |   �ͺ�   |  �����ṹ   |     �豸��ַλ     |   �Ĵ�����ַ  |ÿҳ�ֽ�|**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |FM24C04B  |512x8(4K)    |1|0|1|0|A2|A1|P0|R/W|9λ(1���Ĵ���) | 16�ֽ� |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |FM24C16B  |2048x8(16K)  |1|0|1|0|P2|P1|P0|R/W|11λ(1���Ĵ���)| 16�ֽ� |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |FM24C64B  |8192x8(64K)  |1|0|1|0|A2|A1|A0|R/W|13λ(2���Ĵ���)| 32�ֽ� |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |FM24CL04B |512x8(4K)    |1|0|1|0|A2|A1|P0|R/W|9λ(1���Ĵ���) | 16�ֽ� |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |FM24CL16B |2048x8(16K)  |1|0|1|0|P2|P1|P0|R/W|11λ(1���Ĵ���)| 16�ֽ� |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |FM24CL64B |8192x8(64K)  |1|0|1|0|A2|A1|A0|R/W|13λ(2���Ĵ���)| 32�ֽ� |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |FM24V01A  |16384x8(128K)|1|0|1|0|A2|A1|A0|R/W|14λ(2���Ĵ���)| 64�ֽ� |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |FM24V02A  |32768x8(256K)|1|0|1|0|A2|A1|A0|R/W|15λ(2���Ĵ���)| 64�ֽ� |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |FM24V05   |65536x8(512K)|1|0|1|0|A2|A1|A0|R/W|16λ(2���Ĵ���)| 128�ֽ�|**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |FM24V10   |131072x8(1M) |1|0|1|0|A2|A1|P0|R/W|17λ(2���Ĵ���)| 256�ֽ�|**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |FM24W256  |32768x8(256K)|1|0|1|0|A2|A1|A0|R/W|15λ(2���Ĵ���)| 64�ֽ� |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**           ��Щ�������豸��ַ����Щλ���ڼĴ�����ַ��P��Ѱַ���豸��ַ�Ŀ�**/
/**           �趨λ����A��������ͬһ�����������ص�����������                **/
/**               3λ�����豸��ַ��0λ�����ڴ�Ѱַ���ɹ���8����              **/
/**               2λ�����豸��ַ��1λ�����ڴ�Ѱַ���ɹ���4����              **/
/**               1λ�����豸��ַ��2λ�����ڴ�Ѱַ���ɹ���2����              **/
/**               0λ�����豸��ַ��3λ�����ڴ�Ѱַ���ɹ���1����              **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2019-07-17          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "fm24xxxfunction.h"

static uint16_t devAddMask[FM24Number]={0xFC,0xF0,0xFE,0xFC,0xF0,0xFE,0xFE,0xFE,0xFE,0xFC,0xFE};

/*��FM24XXX��ȡ�����ֽ�,�������ַ��ȡ*/
uint8_t ReadByteFromFM24xxx(FM24ObjectType *fram,uint32_t regAddress)
{
    uint8_t wData[2];
    uint16_t wSize=0;
    uint8_t rData;
    uint8_t temp;
    
    if(fram->memAddLength==FM248BitMemAdd)
    {
        wData[wSize++]=(uint8_t)regAddress;
        temp=(uint8_t)(regAddress>>8);
    }
    else
    {
        wData[wSize++]=(uint8_t)(regAddress>>8);
        wData[wSize++]=(uint8_t)regAddress;
        temp=(uint8_t)(regAddress>>16);
    }
    temp=(temp&(~(devAddMask[fram->mode]>>1)))<<1;
    fram->devAddress=(fram->devAddress & devAddMask[fram->mode])|temp;
    
    fram->Read(fram,wData,wSize,&rData,1);
    
    return rData;
}

/*��FM24XXXд�뵥���ֽ�*/
void WriteByteToFM24xxx(FM24ObjectType *fram,uint32_t regAddress,uint8_t data)
{
    uint8_t temp;
    uint8_t tData[3];
    uint16_t tSize=0;
    
    if(fram->memAddLength==FM248BitMemAdd)
    {
        tData[tSize++]=(uint8_t)(regAddress&0xFF);
        temp=(uint8_t)(regAddress>>8);
    }
    else
    {
        tData[tSize++]=(uint8_t)(regAddress>>8);
        tData[tSize++]=(uint8_t)(regAddress&0xFF);
        temp=(uint8_t)(regAddress>>16);
    }
    temp=(temp&(~(devAddMask[fram->mode]>>1)))<<1;
    fram->devAddress=(fram->devAddress & devAddMask[fram->mode])|temp;
    
    tData[tSize++]=data;
    
    fram->WP(FM24WP_Disable);
    fram->Write(fram,tData,tSize);
    fram->WP(FM24WP_Enable);
}

/*��FM24XXX��ȡ����ֽڣ���ָ����ַ��ൽ����ҳ�Ľ�β*/
void ReadBytesFromFM24xxx(FM24ObjectType *fram,uint32_t regAddress,uint8_t *rData,uint16_t rSize)
{
    uint8_t temp;
    uint8_t wData[2];
    uint16_t wSize=0;
    
    if(fram->memAddLength==FM248BitMemAdd)
    {
        wData[wSize++]=(uint8_t)regAddress;
        temp=(uint8_t)(regAddress>>8);
    }
    else
    {
        wData[wSize++]=(uint8_t)(regAddress>>8);
        wData[wSize++]=(uint8_t)regAddress;
        temp=(uint8_t)(regAddress>>16);
    }
    temp=(temp&(~(devAddMask[fram->mode]>>1)))<<1;
    fram->devAddress=(fram->devAddress & devAddMask[fram->mode])|temp;
    
    fram->Read(fram,wData,wSize,rData,rSize);
}

/*��FM24XXXд�����ֽڣ���ָ����ַ��ൽ����ҳ�Ľ�β*/
void WriteBytesToFM24xxx(FM24ObjectType *fram,uint32_t regAddress,uint8_t *wData,uint16_t wSize)
{
    uint8_t temp;
    uint8_t tData[128];
    uint16_t tSize=0;
    
    if(fram->memAddLength==FM248BitMemAdd)
    {
        tData[tSize++]=(uint8_t)(regAddress&0xFF);
        temp=(uint8_t)(regAddress>>8);
    }
    else
    {
        tData[tSize++]=(uint8_t)(regAddress>>8);
        tData[tSize++]=(uint8_t)(regAddress&0xFF);
        temp=(uint8_t)(regAddress>>16);
    }
    temp=(temp&(~(devAddMask[fram->mode]>>1)))<<1;
    fram->devAddress=(fram->devAddress & devAddMask[fram->mode])|temp;
    
    for(int i=0;i<wSize;i++)
    {
        tData[tSize++]=wData[i];
    }
    
    fram->WP(FM24WP_Disable);
    fram->Write(fram,tData,tSize);
    fram->WP(FM24WP_Enable);
}

/*FM24XXX�����������ģʽ*/
void FM24xxxEnterSleepMode(FM24ObjectType *fram)
{
    uint8_t wData[2];
    
    wData[0]=fram->devAddress;
    fram->devAddress=0xF8;
    wData[1]=0x86;
    
    fram->Write(fram,wData,2);
    
    fram->devAddress=wData[0];
}

/*��ȡFM24XXX����ID*/
uint32_t ReadFM24xxxDeviceID(FM24ObjectType *fram)
{
    uint8_t wData[2];
    uint8_t rData[3];
    uint32_t deviceID=0;
    wData[0]=fram->devAddress;
    fram->devAddress=0xF8;
    
    fram->Read(fram,wData,1,rData,3);
    
    fram->devAddress=wData[0];
    
    deviceID=rData[0];
    deviceID=(deviceID<<8)+rData[1];
    deviceID=(deviceID<<8)+rData[2];
    
    return deviceID;
}

/*FM24XXX�����ʼ��*/
void Fm24cxxInitialization(FM24ObjectType *fram,        //FM24xxx����ʵ��
                           uint8_t devAddress,          //FM24xxx�豸��ַ
                           FM24ModeType mode,           //FM24xxx�豸����
                           Fm24WP wp,                   //FM24xxxд����
                           Fm24Read read,               //��FM24xxx�������ָ��
                           Fm24Write write,             //дFM24xxx�������ָ��
                           Fm24Delayms delayms          //��ʱ����ָ��
                               )
{
    if((fram==NULL)||(read==NULL)||(write==NULL)||(delayms==NULL))
    {
        return;
    }
    fram->Read=read;
    fram->Write=write;
    fram->Delayms=delayms;
    
    if((0xA0<=devAddress)&&(devAddress<=0xAE))
    {
        fram->devAddress=devAddress;
    }
    else if((0x50<=devAddress)&&(devAddress<=0x57))
    {
        fram->devAddress=(devAddress<<1);
    }
    else
    {
        fram->devAddress=0x00;
    }
    
    if(mode>=FM24Number)
    {
        return;
    }
    fram->mode=mode;
    
    if((mode<FM24CL64B)&&(mode!=FM24C64B))
    {
        fram->memAddLength=FM248BitMemAdd;
    }
    else
    {
        fram->memAddLength=FM2416BitMemAdd;
    }
    
    fram->WP=wp;
    
}

/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
