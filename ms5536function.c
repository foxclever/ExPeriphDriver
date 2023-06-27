/******************************************************************************/
/** ģ�����ƣ���չ��������                                                   **/
/** �ļ����ƣ�ms5536function.c                                               **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʵ��MS5536C��ѹ�������Ĳ���                                **/
/**           ÿ̨MS5536C��ѹ���������г���У׼��6������ϵ����               **/
/**           C1: Pressure sensitivity (13 Bit)                              **/
/**           C2: Pressure offset (13 Bit)                                   **/
/**           C3: Temperature coefficient of pressure sensitivity (9 Bit)    **/
/**           C4: Temperature coefficient of pressure offset (9 Bit)         **/
/**           C5: Reference Temperature (12 Bit)                             **/
/**           C6: Temperature coefficient of the temperature (8 Bit)         **/
/**           �ֱ�洢��4��16λ���У���ͨ���߼�����õ���                    **/
/**   Word 1��                                                               **/
/**   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+      **/
/**   |        C3/II (8-Bit)          |         C5/II(8-Bit)          |      **/
/**   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+      **/
/**   |DB7|DB6|DB5|DB4|DB3|DB2|DB1|DB0|DB7|DB6|DB5|DB4|DB3|DB2|DB1|DB0|      **/
/**   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+      **/
/**   Word 2��                                                               **/
/**   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+      **/
/**   |        C4/II (8-Bit)          |         C6(8-Bit)             |      **/
/**   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+      **/
/**   |DB7|DB6|DB5|DB4|DB3|DB2|DB1|DB0|DB7|DB6|DB5|DB4|DB3|DB2|DB1|DB0|      **/
/**   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+      **/
/**   Word 3��                                                               **/
/**   +-----------------+-------------------------------------------------+  **/
/**   |  C5/I (4-Bit)   |                 C1/II (12-Bit)                  |  **/
/**   +----+----+---+---+----+----+---+---+---+---+---+---+---+---+---+---+  **/
/**   |DB11|DB10|DB9|DB8|DB11|DB10|DB9|DB8|DB7|DB6|DB5|DB4|DB3|DB2|DB1|DB0|  **/
/**   +----+----+---+---+----+----+---+---+---+---+---+---+---+---+---+---+  **/
/**   Word 4��                                                               **/
/**   +----+----+----+------------------------------------------------------+**/
/**   |C1/I|C4/I|C3/I|                     C2 (13-Bit)                      |**/
/**   +----+----+----+----+----+----+---+---+---+---+---+---+---+---+---+---+**/
/**   |DB12|DB8 |DB8 |DB12|DB11|DB10|DB9|DB8|DB7|DB6|DB5|DB4|DB3|DB2|DB1|DB0|**/
/**   +----+----+----+----+----+----+---+---+---+---+---+---+---+---+---+---+**/
/**           �����ɣ�3λ��ʼλ+ָ��+3λֹͣλ���ɣ�                         **/
/**           1����ѹ����ֵ�������У�                                        **/
/**           +----+----+----+----+----+----+----+----+----+----+            **/
/**           |Bit0|Bit1|Bit2|Bit3|Bit4|Bit5|Bit6|Bit7|Bit8|Bit9|            **/
/**           +----+----+----+----+----+----+----+----+----+----+            **/
/**           |  1 |  1 |  1 |  1 |  0 |  1 |  0 |  0 |  0 |  0 |            **/
/**           +----+----+----+----+----+----+----+----+----+----+            **/
/**           |   Start-bit  |     Setup-bits    |    Stop-bit  |            **/
/**           +----+----+----+----+----+----+----+----+----+----+            **/
/**           2�����¶���ֵ�������У�                                        **/
/**           +----+----+----+----+----+----+----+----+----+----+            **/
/**           |Bit0|Bit1|Bit2|Bit3|Bit4|Bit5|Bit6|Bit7|Bit8|Bit9|            **/
/**           +----+----+----+----+----+----+----+----+----+----+            **/
/**           |  1 |  1 |  1 |  1 |  0 |  0 |  1 |  0 |  0 |  0 |            **/
/**           +----+----+----+----+----+----+----+----+----+----+            **/
/**           |   Start-bit  |     Setup-bits    |    Stop-bit  |            **/
/**           +--------------+-------------------+--------------+            **/
/**           3����У׼ϵ��Word1�������У�                                   **/
/**           +----+----+----+----+----+----+----+----+----+----+-----+-----+**/
/**           |Bit0|Bit1|Bit2|Bit3|Bit4|Bit5|Bit6|Bit7|Bit8|Bit9|Bit10|Bit11|**/
/**           +----+----+----+----+----+----+----+----+----+----+-----+-----+**/
/**           |  1 |  1 |  1 |  0 |  1 |  0 |  1 |  0 |  1 |  0 |  0  |  0  |**/
/**           +----+----+----+----+----+----+----+----+----+----+-----+-----+**/
/**           |   Start-bit  |           Setup-bits        |     Stop-bit   |**/
/**           +--------------+-----------------------------+----------------+**/
/**           4����У׼ϵ��Word3�������У�                                   **/
/**           +----+----+----+----+----+----+----+----+----+----+-----+-----+**/
/**           |Bit0|Bit1|Bit2|Bit3|Bit4|Bit5|Bit6|Bit7|Bit8|Bit9|Bit10|Bit11|**/
/**           +----+----+----+----+----+----+----+----+----+----+-----+-----+**/
/**           |  1 |  1 |  1 |  0 |  1 |  1 |  0 |  0 |  1 |  0 |  0  |  0  |**/
/**           +----+----+----+----+----+----+----+----+----+----+-----+-----+**/
/**           |   Start-bit  |           Setup-bits        |     Stop-bit   |**/
/**           +--------------+-----------------------------+----------------+**/
/**           5����У׼ϵ��Word2�������У�                                   **/
/**           +----+----+----+----+----+----+----+----+----+----+-----+-----+**/
/**           |Bit0|Bit1|Bit2|Bit3|Bit4|Bit5|Bit6|Bit7|Bit8|Bit9|Bit10|Bit11|**/
/**           +----+----+----+----+----+----+----+----+----+----+-----+-----+**/
/**           |  1 |  1 |  1 |  0 |  1 |  0 |  1 |  1 |  0 |  0 |  0  |  0  |**/
/**           +----+----+----+----+----+----+----+----+----+----+-----+-----+**/
/**           |   Start-bit  |           Setup-bits        |     Stop-bit   |**/
/**           +--------------+-----------------------------+----------------+**/
/**           6����У׼ϵ��Word4�������У�                                   **/
/**           +----+----+----+----+----+----+----+----+----+----+-----+-----+**/
/**           |Bit0|Bit1|Bit2|Bit3|Bit4|Bit5|Bit6|Bit7|Bit8|Bit9|Bit10|Bit11|**/
/**           +----+----+----+----+----+----+----+----+----+----+-----+-----+**/
/**           |  1 |  1 |  1 |  0 |  1 |  1 |  0 |  1 |  0 |  0 |  0  |  0  |**/
/**           +----+----+----+----+----+----+----+----+----+----+-----+-----+**/
/**           |   Start-bit  |           Setup-bits        |     Stop-bit   |**/
/**           +--------------+-----------------------------+----------------+**/
/**           7����λ�����ɣ�16λ��10���м�5λ�͵�ƽ��ɡ�                   **/
/**--------------------------------------------------------------------------**/
/**ʹ��˵����                                                                **/
/**    1������MS5536cObjectType����ʵ�壬�����ú���Ms5536cInitialization��ʼ **/
/**       ������ʵ�塣����ȡУ׼����                                         **/
/**    2��ʵ������uint8_t f(uint8_t *txData,uint8_t *rxData,uint16_t number) **/
/**       �ĺ��������丳ֵ��MS5536cObjectType����ʵ���ReadWriteMS����ָ�롣 **/
/**       ʵ�����ݵ��շ���                                                   **/
/**    3��ʵ������void f(bool en)�ĺ����������丳ֵ��MS5536cObjectType����ʵ **/
/**       ���SetPhase����ָ�롣ʵ��SCLK����λ�л�����Ϊ�ڸ�MS5536C��������ʱ**/
/**       ʹ��ʱ�ӵĵ�1�������أ�ģʽ0�������ڽ���MS5536C������ʱ��ʹ��ʱ�ӵ�**/
/**       ��2�������أ�ģʽ1����                                             **/
/**    4��ʵ������void f(volatile uint32_t nTime)�ĺ����������丳ֵ��        **/
/**       MS5536cObjectType����ʵ���Delayms����ָ�롣ʵ�ֲ�����ʱ����λ���� **/
/**    5��������Ӧ�ĺ���ʵ����Ҫ�Ĳ�����                                     **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2018-07-13          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "ms5536function.h"

/* �����ȡ����ĺ� */
#define READ_D1         0xF400
#define READ_D2         0xF200
#define READ_WORD1      0x0EA8
#define READ_WORD3      0x0EC8
#define READ_WORD2      0x0EB0
#define READ_WORD4      0x0ED0

uint16_t cWord[4]={0,0,0,0};

/* ��ȡ�Ĵ���ֵ */
static uint16_t ReadRegister(MS5536cObjectType *ms,uint16_t command);
/* ����У׼ϵ�� */
static void CalcCoefficientForMs5536c(MS5536cObjectType *ms,uint16_t *cWord);
/* ��ȡ�������� */
static uint16_t ReadMeasureData(MS5536cObjectType *ms,uint16_t command);
/* ��ȡMS5336CУ׼ϵ�� */
static void GetCoefficientForMs5536c(MS5536cObjectType *ms);

/* ��ȡѹ�����¶����� */
void GetMeasureForMs5536c(MS5536cObjectType *ms)
{
    uint16_t D1,D2;
    int64_t uT1=0; 
    int64_t dT=0;
    int32_t temp=0;
    int64_t off=0;
    int64_t sens=0;
    int64_t x=0;
    int32_t pres=0;
    
    //��ȡѹ������
    D1=ReadMeasureData(ms,READ_D1);
    //��ȡ�¶�����
    D2=ReadMeasureData(ms,READ_D2);
    
    uT1=(ms->coef5-200)*4+15136;
    
    dT=(D2-uT1);
    
    temp =(int32_t)(2000 + dT*(ms->coef6+262)/512);
    
    off= ms->coef2-2000 +10381+ ((ms->coef4-243)*dT)/4096;
    
    sens = ms->coef1-2000 + 10179+((ms->coef3+222)*dT)/2048;
    
    x = (sens * (off-D1))/4096;
    
    pres=(int32_t)(x*1365/512);
    
    ms->temperature=temp;
    ms->fTemperature=temp*0.01;
    
    ms->pressure=pres;
    ms->fPressure=pres*0.01;
}

/* ��ȡ�������� */
static uint16_t ReadMeasureData(MS5536cObjectType *ms,uint16_t command)
{
    uint8_t txData[2];
    uint8_t rxData[2];
    uint16_t result=0;
    
    txData[0]=(uint8_t)(command>>8);
    txData[1]=(uint8_t)command;
    ms->ReadWriteMS(txData,rxData,2);
    //   ms->WriteMS(txData,2);
    
    ms->Delayms(23);
    ms->SetPhase(MS5536_SCLK_FALL);
    ms->Delayms(10);
    
    txData[0]=0x00;
    txData[1]=0x00;
    ms->ReadWriteMS(txData,rxData,2);
    //  ms->ReadMS(rxData,2);
    
    result=(rxData[0]<<8)+rxData[1];
    
    ms->SetPhase(MS5536_SCLK_RISE);
    ms->Delayms(10);
    return result;
}

/* ��ȡMS5336CУ׼ϵ�� */
static void GetCoefficientForMs5536c(MS5536cObjectType *ms)
{
    //  uint16_t cWord[4]={0,0,0,0};
    
    //��word1
    cWord[0]=ReadRegister(ms,READ_WORD1);
    
    //��word2
    cWord[1]=ReadRegister(ms,READ_WORD2);
    
    //��word3
    cWord[2]=ReadRegister(ms,READ_WORD3);
    
    //��word4
    cWord[3]=ReadRegister(ms,READ_WORD4);
    
    CalcCoefficientForMs5536c(ms,cWord);
}

/* ��ȡ�Ĵ���ֵ */
static uint16_t ReadRegister(MS5536cObjectType *ms,uint16_t command)
{
    uint8_t txData[2];
    uint8_t rxData[2];
    uint16_t result=0;
    
    txData[0]=(uint8_t)(command>>8);
    txData[1]=(uint8_t)command;
    ms->ReadWriteMS(txData,rxData,2);
    //  ms->WriteMS(txData,2);
    
    ms->SetPhase(MS5536_SCLK_FALL);
    
    txData[0]=0x00;
    txData[1]=0x00;
    ms->ReadWriteMS(txData,rxData,2);
    //  ms->ReadMS(rxData,2);
    result=(rxData[0]<<8)+rxData[1];
    
    ms->SetPhase(MS5536_SCLK_RISE);
    ms->Delayms(1);
    return result;
}

/* ����У׼ϵ�� */
static void CalcCoefficientForMs5536c(MS5536cObjectType *ms,uint16_t *cWord)
{
    ms->coef3=(cWord[0]>>8);
    ms->coef5=(cWord[0]&0x00FF);
    ms->coef4=(cWord[1]>>8);
    ms->coef6=(cWord[1]&0x00FF);
    ms->coef1=(cWord[2]&0x0FFF);
    ms->coef2=(cWord[3]&0x1FFF);
    
    ms->coef5=ms->coef5+((cWord[2]&0xF000)>>4);
    
    ms->coef1=ms->coef1+((cWord[3]&0x8000)>>3);
    ms->coef4=ms->coef4+((cWord[3]&0x4000)>>6);
    ms->coef3=ms->coef3+((cWord[3]&0x2000)>>5);
}

/*��MS5336C���������λ*/
void Ms5336cSoftwareReset(MS5536cObjectType *ms)
{
    //����Ϊ21λ��10101010 10101010 00000
    uint8_t command[3]={170,170,0};
    uint8_t rxDate[3];
    
    ms->ReadWriteMS(command,rxDate,3);
    //  ms->WriteMS(command,3);
}

/* ��MS5536C������г�ʼ�� */
void Ms5536cInitialization(MS5536cObjectType *ms,
                           MS5536cReadWriteMS readwrite,
                           MS5536cSetPhase setPhase,
                           MS5536cDelayms delayms
                               )
{
    if((ms==NULL)||(readwrite==NULL)||(setPhase==NULL)||(delayms==NULL))
    {
        return;
    }
    ms->ReadWriteMS=readwrite;
    ms->SetPhase=setPhase;
    ms->Delayms=delayms;
    
    ms->coef1=0;
    ms->coef2=0;
    ms->coef3=0;
    ms->coef4=0;
    ms->coef5=0;
    ms->coef6=0;
    
    ms->pressure=0;
    ms->fPressure=0.0;
    
    ms->temperature=0;
    ms->fTemperature=0.0;
    
    //��λ
    Ms5336cSoftwareReset(ms);
    
    ms->Delayms(10);
    
    //��ȡ����ϵ��
    GetCoefficientForMs5536c(ms);
}

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/
