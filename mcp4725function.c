/******************************************************************************/
/** ģ�����ƣ���չ��������                                                   **/
/** �ļ����ƣ�mcp4725function.c                                              **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺�����Ͷ���������MCP4725��ģת��оƬ�ĺ����Ͳ�����              **/
/**           ����I2C�ӿڽӿڣ�MCP4725��ģת��оƬд�������£�               **/
/**     +--+--+--+-----------------------------+----------------+            **/
/**     |C2|C1|C0|        Command Name         |    Function    |            **/
/**     +--+--+--+-----------------------------+----------------+            **/
/**     |0 |0 |X |          Fast Mode          |    ����ģʽ    |            **/
/**     +--+--+--+-----------------------------+----------------+            **/
/**     |0 |0 |X |          Fast Mode          |    ����ģʽ    |            **/
/**     +--+--+--+-----------------------------+----------------+            **/
/**     |0 |1 |0 |      Write DAC Register     |    д�Ĵ���    |            **/
/**     +--+--+--+-----------------------------+----------------+            **/
/**     |0 |1 |1 |Write DAC Register and EEPROM|д�Ĵ�����EEPROM|            **/
/**     +--+--+--+-----------------------------+----------------+            **/
/**     |1 |0 |0 |          Reserved           |      ����      |            **/
/**     +--+--+--+-----------------------------+----------------+            **/
/**     |1 |0 |1 |          Reserved           |      ����      |            **/
/**     +--+--+--+-----------------------------+----------------+            **/
/**     |1 |1 |0 |          Reserved           |      ����      |            **/
/**     +--+--+--+-----------------------------+----------------+            **/
/**     |1 |1 |1 |          Reserved           |      ����      |            **/
/**     +--+--+--+-----------------------------+----------------+            **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2021-02-15          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "mcp4725function.h"

#define Fast_Mode   0x00
#define Write_DAC_Register  0x40
#define Write_DAC_Register_EEPROM   0x60

/*MCP4725ͨ�ò���*/
static void Mcp4725GeneralCall(Mcp4725ObjectType *mcp,uint8_t cmd);

/*����MCP4725���*/
void Mcp4725SetDatas(Mcp4725ObjectType *mcp,Mcp4725CommandType cmd,uint16_t data)
{
    uint8_t wData[3];
    uint8_t pdMode=0;
    uint16_t wSize=0;
    uint8_t command[]={Fast_Mode,Write_DAC_Register,Write_DAC_Register_EEPROM};
    
    pdMode=(uint8_t)(mcp->pdMode);
    
    if(cmd==Mcp4725_Fast_Mode)  //����ģʽ
    {
        wData[1]=(uint8_t)data;
        wData[0]=(uint8_t)(data>>8);
        wData[0]=wData[0]|command[cmd];
        wData[0]=wData[0]|(pdMode<<4);
        wSize=2;
    }
    else    //��ͨģʽ
    {
        wData[0]=command[cmd];
        wData[0]=wData[0]|(pdMode<<1);
        wData[1]=(uint8_t)(data>>4);
        wData[2]=(uint8_t)(data<<4);
        wSize=3;
    }
    
    mcp->Write(mcp,wData,wSize);
}

/*��ȡMCP4725����*/
void Mcp4725GetDatas(Mcp4725ObjectType *mcp,uint8_t *rData)
{
    mcp->Read(mcp,rData,5);
}

/*MCP4725��ʼ������*/
void Mcp4725Initialization(Mcp4725ObjectType *mcp,  //MCP4725�������
                           uint8_t slaveAddress,    //��վ�豸�ĵ�ַ
                           Mcp4725PDModeType pdMode,//�������ģʽ
                           Mcp4725Write write,      //д���ݺ���ָ��
                           Mcp4725Read read         //�����ݺ���ָ��
                               )
{
    if((mcp==NULL)||(write==NULL)||(read==NULL))
    {
        return;
    }
    
    mcp->Write=write;
    mcp->Read=read;
    
    if((slaveAddress==0x60)||(slaveAddress==0x61))
    {
        mcp->devAddress=(slaveAddress<<1);
    }
    else if((slaveAddress==0xC0)||(slaveAddress==0xC2))
    {
        mcp->devAddress=slaveAddress;
    }
    else
    {
        mcp->devAddress=0x00;
    }
    
    mcp->pdMode=pdMode;
    
}

/*ִ��MCP4725��λ*/
void Mcp4725Reset(Mcp4725ObjectType *mcp)
{
    Mcp4725GeneralCall(mcp,0x06);
}

/*ִ��MCP4725����*/
void Mcp4725Wakeup(Mcp4725ObjectType *mcp)
{
    Mcp4725GeneralCall(mcp,0x09);
}

/*MCP4725ͨ�ò���*/
static void Mcp4725GeneralCall(Mcp4725ObjectType *mcp,uint8_t cmd)
{
    uint8_t wData=cmd;
    uint8_t devAdd=mcp->devAddress;
    
    mcp->devAddress=0x00;
    
    mcp->Write(mcp,&wData,1);
    
    mcp->devAddress=devAdd;
}

/*********** (C) COPYRIGHT 1999-2021 Moonan Technology *********END OF FILE****/