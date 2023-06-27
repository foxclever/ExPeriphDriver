/******************************************************************************/
/** ģ�����ƣ���չ��������                                                   **/
/** �ļ����ƣ�ltc2400function.c                                              **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʵ��LTC2400 ADC�����ݻ�ȡ                                  **/
/**           LTC2400����SPI�����ӿ�                                         **/
/**           ������ݵĽṹ���£�                                           **/
/**           Bit31 Bit30 Bit29 Bit28 Bit27 �� Bit4 Bit3-0                    **/
/**           EOC   DMY   SIG   EXR   MSB      LSB  SUB LSBs                 **/
/**           0     0     0/1   0/1   0/1      0/1  X                        **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2017-01-18          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "ltc2400function.h"

#define LTC2400_OUTPUT_SIGNBIT 0x20000000
#define LTC2400_OUTPUT_EXRNBIT 0x10000000
#define LTC2400_OUTPUT_DATABITS 0xFFFFFF0
#define LTC2400_DIGITAL_RANGE 0xFFFFFF
#define LTC2400_DIGITAL_ZERO 0x0

/*LTC2400���ݵĺϳ�*/
static float CompoundLTC2400Data(Ltc2400ObjectType *ltc,uint8_t *dataCode);
/* Ĭ��Ƭѡ�������� */
static void DefaultChipSelect(LTC2400CSType en);

/* ��ȡLTC2400ת������,�����������ݵı���ֵ */
float GetLtc2400Data(Ltc2400ObjectType *ltc)
{
    uint8_t rData[4];
    
    ltc->ChipSelect(LTC2400CS_Enable);
    ltc->Delayms(1);
    
    ltc->Receive(rData);
    
    ltc->Delayms(1);
    ltc->ChipSelect(LTC2400CS_Disable);
    
    return CompoundLTC2400Data(ltc,rData);
}

/*LTC2400���ݵĺϳ�*/
static float CompoundLTC2400Data(Ltc2400ObjectType *ltc,uint8_t *dataCode)
{
    uint32_t temp=0;
    float result=0.0;
    
    temp=(dataCode[0]<<24)+(dataCode[1]<<16)+(dataCode[2]<<8)+dataCode[3];
    
    ltc->dataCode=temp;
    
    result=(float)(((temp&LTC2400_OUTPUT_DATABITS)>>4)-LTC2400_DIGITAL_ZERO)/(float)(LTC2400_DIGITAL_RANGE-LTC2400_DIGITAL_ZERO);
    
    //�ж��Ƿ�Ϊ����
    if((temp&LTC2400_OUTPUT_SIGNBIT)!=LTC2400_OUTPUT_SIGNBIT)
    {
        
        result=result-1.0;
        return result;
    }
    
    //�ж������Ƿ�����
    if((temp&LTC2400_OUTPUT_EXRNBIT)==LTC2400_OUTPUT_EXRNBIT)
    {
        result=result+1.0;
    }
    
    return result;
}

/* LTC2400�����ʼ������ */
void LTC2400Initialization(Ltc2400ObjectType *ltc,
                           LTC2400ClockType clock,
                           LTC2400Receive receive,
                           LTC2400ChipSelect cs,
                           LTC2400Delay msDelay)
{
    if((ltc==NULL)||(receive==NULL)||(msDelay==NULL))
    {
        return;
    }
    
    ltc->dataCode=0;
    ltc->clock=clock;
    
    if(cs==NULL)	//Ӳ����·ʵ��Ƭѡ
    {
        ltc->ChipSelect=DefaultChipSelect;
    }
    else
    {
        ltc->ChipSelect=cs;
    }
    
    ltc->Receive=receive;
    ltc->Delayms=msDelay;
}

/* Ĭ��Ƭѡ�������� */
static void DefaultChipSelect(LTC2400CSType en)
{
    return;
}

/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
