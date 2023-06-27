/******************************************************************************/
/** ģ�����ƣ���չ��������                                                   **/
/** �ļ����ƣ�mlx90614function.c                                             **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʵ��MLX90614�����¶ȴ�������ͨѶ                           **/
/**           ����SMBusͨѶͨѶ�ӿ�                                          **/
/**--------------------------------------------------------------------------**/
/**ʹ��˵����                                                                **/
/**    SMBusͨѶͨѶ�ӿڵĶ��������I2C�ӿڶ�����ͬ�����Բ���Ӳ��I2C�ӿں�   **/
/**    ���ģ��I2C�ӿ�ʵ��ͨѶ��                                             **/
/**    1������MLX90614����ʵ�壬�����ú���MLXInitialization��ʼ������ʵ�塣  **/
/**       ��ʼ��ʱ����������ĵ�ַ��                                         **/
/**    2��ʵ������void f(struct MLXObject *mlx,uint8_t cmd,uint8_t *rData,   **/
/**       uint16_t rSize)�ĺ����������丳ֵ��MLX90614����ʵ���Read����ָ�롣**/
/**       ʵ�����ݵĶ�ȡ������                                               **/
/**    3��ʵ������void f(struct MLXObject *mlx,uint8_t cmd,uint8_t *wData,   **/
/**       uint16_t wSize)�ĺ����������丳ֵ��MLX90614����ʵ���Write����ָ   **/
/**       �롣ʵ�����ݵ�д������                                             **/
/**    4��ʵ������oid f(volatile uint32_t nTime)�ĺ����������丳ֵ��MLX90614t**/
/**       ����ʵ���Delay����ָ�롣ʵ����ʱ������ʱ�䵥λΪ���롣            **/
/**    5��������Ӧ�ĺ���ʵ����Ҫ�Ĳ�����                                     **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2019-05-05          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "mlx90614function.h"

#define MLXSlaveAddress         0xB4

/* ������������� */
#define RAMAccess               0x00
#define EEPROMAccess            0x20
#define ReadFlags               0xF0
#define EnterSLEEPMode          0xFF

/* ����EEPROM�ڴ�����ַ */
#define Tomax                   0x00
#define Tomin                   0x01
#define PWMCTRL                 0x02
#define Ta_Range                0x03
#define CorrectCoefficient      0x04
#define ConfigRegister1         0x05
#define SMBus_Address           0x0E
#define ID_Number1              0x1C
#define ID_Number2              0x1D
#define ID_Number3              0x1E
#define ID_Number4              0x1F

/* ����RAM�ڴ�����ַ */
#define RawDataIRChannel1       0x04
#define RawDataIRChannel2       0x05
#define TA                      0x06
#define TOBJ1                   0x07
#define TOBJ2                   0x08

const uint16_t MLXPOLYNOMIAL = 0x07;      //����У�����ʽP(x) = x^8 + x^2 + x^1 + 1 = 100000111

/* ��ȡMLX90614������ID�� */
static void GetIDFromMLX90614(MLXObjectType *mlx);
/* У�������*/
static uint8_t PECCalculation(uint8_t *ptr, uint8_t len);
/*�����ݲ���*/
static uint16_t ReadDataFromMLX(MLXObjectType *mlx,uint8_t cmd);
/* д���ݲ��� */
static void WriteDataToMLX(MLXObjectType *mlx,uint8_t cmd,uint16_t data);

/* ��ȡ�¶�ֵ */
void GetMLXTemperature(MLXObjectType *mlx)
{
    
    mlx->tempAmbient=ReadDataFromMLX(mlx,RAMAccess|TA)*0.02-273.15;
    
    mlx->tempObject1=ReadDataFromMLX(mlx,RAMAccess|TOBJ1)*0.02-273.15;
    
    mlx->tempObject2=ReadDataFromMLX(mlx,RAMAccess|TOBJ2)*0.02-273.15;
    
}

/* ��״̬��־ */
uint16_t ReadFlagFromMLX(MLXObjectType *mlx)
{
    uint16_t tempCode;
    
    tempCode=ReadDataFromMLX(mlx,ReadFlags);
    mlx->flags=(uint8_t)tempCode;
    
    return tempCode;
}

/* ʹ�豸��������ģʽ */
void EnterSleepModeForMLX(MLXObjectType *mlx)
{
    uint8_t cmd;
    static uint8_t pec;
    uint8_t data[2];
    
    cmd=EnterSLEEPMode;
    
    data[0]=mlx->devAddress;
    data[1]=cmd;
    
    pec=PECCalculation(data,2);
    
    mlx->Write(mlx,cmd,&pec,1);
}

/*ʹ�豸�˳�����ģʽ*/
void ExitSleepModeForMLX(void)
{
    
}

/* ����PWM���� */
void SetPWMControll(MLXObjectType *mlx,uint16_t data)
{
    WriteDataToMLX(mlx,EEPROMAccess|PWMCTRL,data);
    
    mlx->pwmctrl=ReadDataFromMLX(mlx,EEPROMAccess|PWMCTRL);
}

/* �������üĴ��� */
void SetConfigRegister(MLXObjectType *mlx,uint16_t data)
{
    WriteDataToMLX(mlx,EEPROMAccess|ConfigRegister1,data);
    
    mlx->ConfigRegister=ReadDataFromMLX(mlx,EEPROMAccess|ConfigRegister1);
}

/* �޸��豸��ַ */
void ModifyDeviceAddress(MLXObjectType *mlx,uint16_t address)
{
    WriteDataToMLX(mlx,EEPROMAccess|SMBus_Address,address);
    
    uint8_t temp=mlx->devAddress;
    mlx->devAddress=(uint8_t)address;
    
    if(ReadDataFromMLX(mlx,EEPROMAccess|SMBus_Address)!=address)
    {
        mlx->devAddress=temp;
    }
}

/* �����¶ȴ����������ʼ�� */
void MLXInitialization(MLXObjectType *mlx,	//MLX90614����
                       uint8_t address,		//�豸��ַ
                       MLXRead read,			//�����ݺ���ָ��
                       MLXWrite write,			//д���ݺ���ָ��
                       MLXDelayus delayus		//΢����ʱ����ָ��
                           )
{
    if((mlx==NULL)||(read==NULL)||(write==NULL)||(delayus==NULL))
    {
        return;
    }
    mlx->Read=read;
    mlx->Write=write;
    mlx->Delayus=delayus;
    
    mlx->tempAmbient=0.0;
    mlx->tempObject1=0.0;
    mlx->tempObject2=0.0;
    
    if(address>0x00)
    {
        mlx->devAddress=address;
    }
    else
    {
        mlx->devAddress=MLXSlaveAddress;
    }
    
    mlx->Delayus(200);
    
    GetIDFromMLX90614(mlx);
    
    ReadFlagFromMLX(mlx);
    
    mlx->pwmctrl=ReadDataFromMLX(mlx,EEPROMAccess|PWMCTRL);
    
    mlx->ConfigRegister=ReadDataFromMLX(mlx,EEPROMAccess|ConfigRegister1);
}

/*�����ݲ���*/
static uint16_t ReadDataFromMLX(MLXObjectType *mlx,uint8_t cmd)
{
    uint8_t data[3];
    uint16_t tempCode=0;
    uint8_t pec[6];
    
    mlx->Read(mlx,cmd,data,3);
    
    
    pec[0]=mlx->devAddress;
    pec[1]=cmd;
    pec[2]=mlx->devAddress+1;
    pec[3]=data[0];
    pec[4]=data[1];
    pec[5]=data[2];
    
    if(PECCalculation(pec,6)==0x00)
    {
        tempCode=(data[1]<<8)+data[0];
        return tempCode;
    }
    
    //������Ŀ2021.04.27
    data[1]=data[1]&0x3F;
    pec[4]=data[1];
    
    if(PECCalculation(pec,6)==0x00)
    {
        tempCode=(data[1]<<8)+data[0];
        return tempCode;
    }
    
    data[2]=data[2]&0x3F;
    pec[5]=data[2];
    
    if(PECCalculation(pec,6)==0x00)
    {
        tempCode=(data[1]<<8)+data[0];
        return tempCode;
    }
    
    return 0x0000;
}

/* д���ݲ��� */
static void WriteDataToMLX(MLXObjectType *mlx,uint8_t cmd,uint16_t data)
{
    uint8_t wData[3];
    uint8_t pec[4];
    
    pec[0]=mlx->devAddress;
    pec[1]=cmd;
    pec[2]=(uint8_t)data;
    pec[3]=(uint8_t)(data>>8);
    
    wData[0]=(uint8_t)data;
    wData[1]=(uint8_t)(data>>8);
    wData[2]=PECCalculation(pec,4);
    
    mlx->Write(mlx,cmd,wData,3);
}

/* ��ȡMLX90614������ID�� */
static void GetIDFromMLX90614(MLXObjectType *mlx)
{
    mlx->ID[0]=ReadDataFromMLX(mlx,EEPROMAccess|ID_Number1);
    
    mlx->ID[1]=ReadDataFromMLX(mlx,EEPROMAccess|ID_Number2);
    
    mlx->ID[2]=ReadDataFromMLX(mlx,EEPROMAccess|ID_Number3);
    
    mlx->ID[3]=ReadDataFromMLX(mlx,EEPROMAccess|ID_Number4);
}

/* У�������*/
static uint8_t PECCalculation(uint8_t *ptr, uint8_t len)
{
    uint8_t i; 
    uint8_t crc=0x00;           //����ĳ�ʼcrcֵ
    
    while(len--)
    {
        crc ^= *ptr++;          //ÿ��������Ҫ������������,������ָ����һ����
        for (i=8; i>0; --i)
        { 
            if (crc & 0x80)
            {
                crc = (crc << 1) ^ MLXPOLYNOMIAL;
            }
            else
            {
                crc = (crc << 1);
            }
        }
    }
    
    return (crc); 
}


/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
