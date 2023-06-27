/******************************************************************************/
/** ģ�����ƣ���չ��������                                                   **/
/** �ļ����ƣ�ds17887function.h                                              **/
/** ��    ����V1.1.0                                                         **/
/** ��    �飺ʵ��ʵʱʱ��ģ��DS17887�Ĳ���                                  **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2012-07-28          ľ��              �����ļ�               **/
/**     V1.1.0  2019-08-07          ľ��              �޸�Ϊ��������Ĳ���   **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "ds17887function.h"

/*��DS17887������*/
static uint8_t ReadDataFromDS17887(Ds17887ObjectType *ds17887,uint16_t address);
/*��DS17887д����*/
static void WriteDataToDS17887(Ds17887ObjectType *ds17887,uint16_t address,uint16_t data);

/*��ʵʱʱ��ģ���ȡʱ��*/
void GetDateTimeFromDs17887(Ds17887ObjectType *ds17887)
{
    /*��ȡϵͳʱ��ֵ*/
    ds17887->dateTime[0]=ReadDataFromDS17887(ds17887,DS17887_Year);//ϵͳʱ����
    ds17887->Delayus(5);
    ds17887->dateTime[1]=ReadDataFromDS17887(ds17887,DS17887_Month);//ϵͳʱ����
    ds17887->Delayus(5);
    ds17887->dateTime[2]=ReadDataFromDS17887(ds17887,DS17887_Date);//ϵͳʱ����
    ds17887->Delayus(5);
    ds17887->dateTime[3]=ReadDataFromDS17887(ds17887,DS17887_Hour);//ϵͳʱ��ʱ
    ds17887->Delayus(5);
    ds17887->dateTime[4]=ReadDataFromDS17887(ds17887,DS17887_Minute);//ϵͳʱ���
    ds17887->Delayus(5);
    ds17887->dateTime[5]=ReadDataFromDS17887(ds17887,DS17887_Second);//ϵͳʱ����
    ds17887->Delayus(5);
    
    /*��ȡ���ƼĴ���*/
    ds17887->ctlReg[Reg_A]=ReadDataFromDS17887(ds17887,DS17887_Reg_A);
    ds17887->Delayus(5);
    ds17887->ctlReg[Reg_B]=ReadDataFromDS17887(ds17887,DS17887_Reg_B);
    ds17887->Delayus(5);
    ds17887->ctlReg[Reg_C]=ReadDataFromDS17887(ds17887,DS17887_Reg_C);
    ds17887->Delayus(5);
    ds17887->ctlReg[Reg_D]=ReadDataFromDS17887(ds17887,DS17887_Reg_D);
    ds17887->Delayus(5);
}

/*У׼DS17887��ʱ��*/
void CalibrationDs17887DateTime(Ds17887ObjectType *ds17887,uint16_t * dateTime)
{
    /*��ALE��RD��WR��λ*/
    ds17887->SetCtlPin[DS17887_ALE](DS17887_Reset);
    ds17887->SetCtlPin[DS17887_WR](DS17887_Reset);
    ds17887->SetCtlPin[DS17887_RD](DS17887_Reset);
    
    /*��ʼ�����ƼĴ������Ա�У׼ʱ��*/
    WriteDataToDS17887(ds17887,DS17887_Reg_A,0x20);
    WriteDataToDS17887(ds17887,DS17887_Reg_B,0x06);
    WriteDataToDS17887(ds17887,DS17887_Reg_B,0x80);
    
    /*����ϵͳʱ��ֵ*/
    WriteDataToDS17887(ds17887,DS17887_Year,dateTime[0]);//ϵͳʱ����
    WriteDataToDS17887(ds17887,DS17887_Month,dateTime[1]);//ϵͳʱ����
    WriteDataToDS17887(ds17887,DS17887_Date,dateTime[2]);//ϵͳʱ����
    WriteDataToDS17887(ds17887,DS17887_Hour,dateTime[3]);//ϵͳʱ��ʱ
    WriteDataToDS17887(ds17887,DS17887_Minute,dateTime[4]);//ϵͳʱ���
    WriteDataToDS17887(ds17887,DS17887_Second,dateTime[5]);//ϵͳʱ����
    
    /*���üĴ���B��A��ֵ������DS17887*/
    WriteDataToDS17887(ds17887,DS17887_Reg_B,0x06);
    WriteDataToDS17887(ds17887,DS17887_Reg_A,0x20);
    
    //��ȡDS17887��ʱ��
    GetDateTimeFromDs17887(ds17887);
}

/*��DS17887���г�ʼ������*/
void Ds17887Initialization(Ds17887ObjectType *ds17887,
                           DS17887CtlPinOperationType *SetCtlPin,
                           WriteByteToDs17887Type WriteByte,
                           ReadByteFromDs17887Type ReadByte,
                           Ds17887SetBusDirectionType SetBusDirection,
                           Ds17887DelayusType Delayus)
{
    if((ds17887==NULL)||(SetCtlPin==NULL)||(WriteByte==NULL)||(ReadByte==NULL)||(SetBusDirection==NULL)||(Delayus==NULL))
    {
        return;
    }
    
    ds17887->ctlReg[Reg_A]=0x00;
    ds17887->ctlReg[Reg_B]=0x00;
    ds17887->ctlReg[Reg_C]=0x00;
    ds17887->ctlReg[Reg_D]=0x00;
    
    for(int i=0;i<6;i++)
    {
        ds17887->dateTime[0]=0;
        ds17887->SetCtlPin[i]=SetCtlPin[i];
    }
    ds17887->WriteByte=WriteByte;
    ds17887->ReadByte=ReadByte;
    ds17887->SetBusDirection=SetBusDirection;
    ds17887->Delayus=Delayus;
    
    /*��ALE��RD��WR��λ*/
    SetCtlPin[DS17887_ALE](DS17887_Reset);
    SetCtlPin[DS17887_WR](DS17887_Reset);
    SetCtlPin[DS17887_RD](DS17887_Reset);
    
    /*���üĴ���B��A��ֵ������DS17887*/
    WriteDataToDS17887(ds17887,DS17887_Reg_B,0x06);
    WriteDataToDS17887(ds17887,DS17887_Reg_A,0x20);
    
    //��ȡDS17887��ʱ��
    GetDateTimeFromDs17887(ds17887);
}

/*��DS17887������*/
static uint8_t ReadDataFromDS17887(Ds17887ObjectType *ds17887,uint16_t address)
{
    /*��Ƭѡ�ź���λ��ʧ��Ƭѡ*/
    ds17887->SetCtlPin[DS17887_CS](DS17887_Set);
    /*��RD��WR��λ*/
    ds17887->SetCtlPin[DS17887_WR](DS17887_Set);
    ds17887->SetCtlPin[DS17887_RD](DS17887_Set);
    ds17887->Delayus(2);
    /*��λALE*/
    ds17887->SetCtlPin[DS17887_ALE](DS17887_Set);
    
    /*����ַ�������ߵ�ģʽ��Ϊ���*/
    ds17887->SetBusDirection(DS17887_Out);
    /*д�Ĵ�����ַ*/
    ds17887->WriteByte(address);
    /*��Ƭѡ�ź���λ��ʹ��Ƭѡ*/
    ds17887->SetCtlPin[DS17887_CS](DS17887_Reset);
    ds17887->Delayus(2);
    /*��λALE*/
    ds17887->SetCtlPin[DS17887_ALE](DS17887_Reset);
    ds17887->Delayus(2);
    /*��λRD*/
    ds17887->SetCtlPin[DS17887_RD](DS17887_Reset);
    ds17887->Delayus(10);
    
    /*����ַ�������ߵ�ģʽ��Ϊ����*/
    ds17887->SetBusDirection(DS17887_In);
    ds17887->Delayus(40);
    /*��ȡ����*/
    uint16_t readData=0;
    readData=ds17887->ReadByte();
    ds17887->Delayus(4);
    /* ��RD��λ������CS�ź���λ��ʧ��оƬ */
    ds17887->SetCtlPin[DS17887_RD](DS17887_Set);
    ds17887->SetCtlPin[DS17887_CS](DS17887_Set);
    ds17887->Delayus(4);
    /*��ALE��λ*/
    ds17887->SetCtlPin[DS17887_ALE](DS17887_Set);
    ds17887->Delayus(20);
    return readData;
}

/*��DS17887д����*/
static void WriteDataToDS17887(Ds17887ObjectType *ds17887,uint16_t address,uint16_t data)
{
    /*��DS17887��Ƭѡ�ź�ʧ��*/
    ds17887->SetCtlPin[DS17887_CS](DS17887_Set);
    /*��RD��WR��λ*/
    ds17887->SetCtlPin[DS17887_WR](DS17887_Set);
    ds17887->SetCtlPin[DS17887_RD](DS17887_Set);
    ds17887->Delayus(2);
    /*��ALE�ź��ø�*/
    ds17887->SetCtlPin[DS17887_ALE](DS17887_Set);
    
    /*����ַ�������ߵ�ģʽ��Ϊ���*/
    ds17887->SetBusDirection(DS17887_Out);
    /*д�Ĵ�����ַ*/
    ds17887->WriteByte(address);
    /*��Ƭѡ�ź���λ��ʹ��Ƭѡ*/
    ds17887->SetCtlPin[DS17887_CS](DS17887_Reset);
    ds17887->Delayus(4);
    /*��λALE�ź�*/
    ds17887->SetCtlPin[DS17887_ALE](DS17887_Reset);
    ds17887->Delayus(4);
    /*��λWR*/
    ds17887->SetCtlPin[DS17887_WR](DS17887_Reset);
    /*д����*/
    ds17887->WriteByte(data);
    ds17887->Delayus(4);
    
    /* ��WR��λ������CS�ź���λ��ʧ��оƬ */
    ds17887->SetCtlPin[DS17887_WR](DS17887_Set);
    ds17887->SetCtlPin[DS17887_CS](DS17887_Set);  
    ds17887->Delayus(4);
    /*��ALE��λ*/
    ds17887->SetCtlPin[DS17887_ALE](DS17887_Set);
    ds17887->Delayus(10);
}

/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
