/******************************************************************************/
/** ģ�����ƣ���չ��������                                                   **/
/** �ļ����ƣ�oledfunction.c                                                 **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺Ӧ����ʵ��OLED��ʾ���Ĳ���                                     **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2017-07-17          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "stdarg.h"
#include "stdio.h"
#include "oledfontlibrary.h"
#include "oledfunction.h"

/*������������*/
#define EntireDisplayFollowsRAM 0xA4
#define EntireDisplayIgnoresRAM 0xA5
#define SetNormalDisplay 0xA6
#define SetInverse Display 0xA7
#define SetDisplayOFF 0xAE
#define SetDisplayON 0xAF

/*��OLED��������*/
static void SendToOled(OledObjectType *oled,uint8_t sData,OledDCType type);
/* Ĭ�ϵ�Ƭѡ�źŴ����� */
static void OledChipSelect(OledCSType cs);
/*����OLED������ʾ����,segˮƽ���꣨0��127��,com��ʾOLED��ҳ��0��7�� */
static void SetOledCoordinate(OledObjectType *oled,uint8_t com, uint8_t seg);
/*���OLED��ʾ*/
static void OledClearScreen(OledObjectType *oled);

/*��ʾASCII�ַ���*/
void OledShowString(OledObjectType *oled,OledFontSizeType fontSize,uint8_t page,uint8_t seg,char *fmt,...)
{
    uint8_t showString[256];
    uint8_t *pStr;
    va_list args;
    
    va_start(args,fmt);
    vsprintf((char *)showString,fmt,args);
    va_end(args);
    
    pStr=showString;
    
    while(*pStr != '\0')
    {
        char ch=*pStr - 32;
        
        if(fontSize==OLED_FONT_6x8)
        {
            if(seg>120)
            {
                seg=0;
                page+=1;
            }
            
            OledShow6x8Char(oled,page,seg,oledFont6x8[ch]);
        }
        else
        {
            if(seg>120)
            {
                seg=0;
                page+=2;
            }
            
            OledShow8x16Char(oled,page,seg,oledFont8X16[ch]);
        }
        
        seg+=8;
        pStr++;
    }
}

/*��ʾ6x8��8��6�У����ַ�����ASCII���ַ�*/
void OledShow6x8Char(OledObjectType *oled,uint8_t page,uint8_t seg,uint8_t *sData)
{
    SetOledCoordinate(oled,page,seg);
    
    for(int j=0;j<6;j++)
    {
        SendToOled(oled,*sData++,OLEDDC_Data);
    }
}

/*��ʾ8x16��16��8�У����ַ�����ASCII���ַ���*/
void OledShow8x16Char(OledObjectType *oled,uint8_t page,uint8_t seg,uint8_t *sData)
{
    for(int i=0;i<2;i++)
    {
        SetOledCoordinate(oled,page+i,seg);
        
        for(int j=0;j<8;j++)
        {
            SendToOled(oled,*sData++,OLEDDC_Data);
        }
    }
}

/*��ʾ16x16��16��16�У����ַ����纺��*/
void OledShow16x16Char(OledObjectType *oled,uint8_t page,uint8_t seg,uint8_t *sData)
{
    for(int i=0;i<2;i++)
    {
        SetOledCoordinate(oled,page+i,seg);
        
        for(int j=0;j<16;j++)
        {
            SendToOled(oled,*sData++,OLEDDC_Data);
        }
    }
}

/*��ʾ32x32��32��32�У����ַ����纺��*/
void OledShow32x32Char(OledObjectType *oled,uint8_t page,uint8_t seg,uint8_t *sData)
{
    for(int i=0;i<4;i++)
    {
        SetOledCoordinate(oled,page+i,seg);
        
        for(int j=0;j<32;j++)
        {
            SendToOled(oled,*sData++,OLEDDC_Data);
        }
    }
}

/*��ʾһ��128x64��ͼƬ*/
void OledShowPicture(OledObjectType *oled,uint8_t *sData)
{
    for(int page=0;page<8;page++)
    {
        SetOledCoordinate(oled,page,0);
        
        for(int seg=0;seg<128;seg++)
        {
            SendToOled(oled,*sData++,OLEDDC_Data);
        }
    }
}

/*OLED��ʾ�������ʼ��*/
void OledInitialization(OledObjectType *oled,           //OLED����
                        OledPortType port,              //ͨѶ�˿�
                        uint8_t address,                //I2C�豸��ַ
                        OledWrite write,                //д���ݺ���
                        OledChipReset rst,              //��λ�źŲ�������ָ��
                        OledDCSelcet dc,                //DC�źſ��ƺ���ָ��
                        OledChipSelcet cs,              //SPIƬѡ�źź���ָ��
                        OledDelayms delayms             //������ʱ����ָ��
                            )
{
    if((oled==NULL)||(write==NULL)||(rst==NULL)||(delayms==NULL))
    {
        return;
    }
    oled->Write=write;
    oled->ChipReset=rst;
    oled->Delayms=delayms;
    
    oled->port=port;
    
    if(port==OLED_I2C)
    {
        if((address==0x3C)||(address==0x3D))
        {
            oled->devAddress=(address<<1);
        }
        else if((address==0x78)||(address==0x7A))
        {
            oled->devAddress=address;
        }
        else
        {
            oled->devAddress=0x00;
        }
        
        if(dc==NULL)
        {
            return;
        }
        oled->DCSelcet=dc;
        oled->ChipSelcet=cs;
    }
    else
    {
        oled->devAddress=0xFF;
        
        if(cs==NULL)
        {
            oled->ChipSelcet=OledChipSelect;
        }
        else
        {
            oled->ChipSelcet=cs;
        }
        
        oled->DCSelcet=dc;
    }
    
    oled->ChipReset(OLED_WORK);
    oled->Delayms(100);
    oled->ChipReset(OLED_RESET);
    oled->Delayms(100);
    oled->ChipReset(OLED_WORK);
    
//    SendToOled(oled,0xAE,OLEDDC_Command); //�ر���ʾ
//    SendToOled(oled,0x20,OLEDDC_Command); //Set Memory Addressing Mode	
//    SendToOled(oled,0x10,OLEDDC_Command); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
//    SendToOled(oled,0xB0,OLEDDC_Command); //Set Page Start Address for Page Addressing Mode,0-7
//    
//    SendToOled(oled,0xA1,OLEDDC_Command); //0xA0��X��������ʾ��0xA1��X�᾵����ʾ
//    SendToOled(oled,0xC8,OLEDDC_Command); //0xC0��Y��������ʾ��0xC8��Y�᾵����ʾ
//    
//    SendToOled(oled,0x00,OLEDDC_Command); //�����е�ַ��4λ
//    SendToOled(oled,0x10,OLEDDC_Command); //�����е�ַ��4λ
//    SendToOled(oled,0x40,OLEDDC_Command); //������ʼ�ߵ�ַ
//    
//    SendToOled(oled,0x81,OLEDDC_Command); //���öԱȶ�ֵ
//    SendToOled(oled,0x7F,OLEDDC_Command); //------
//    
//    SendToOled(oled,0xA6,OLEDDC_Command); //0xA6,������ʾģʽ;0xA7����ɫ��ʾ
//    
//    SendToOled(oled,0xA8,OLEDDC_Command); //���ö�·������(1 to 64)
//    SendToOled(oled,0x3F,OLEDDC_Command); //------
//    
//    SendToOled(oled,0xA4,OLEDDC_Command); //0xa4,��ʾ����RAM�ĸı���ı�;0xa5,��ʾ���ݺ���RAM������
//    
//    SendToOled(oled,0xD3,OLEDDC_Command); //������ʾƫ��
//    SendToOled(oled,0x00,OLEDDC_Command); //------
//    
//    SendToOled(oled,0xD5,OLEDDC_Command); //�����ڲ���ʾʱ��Ƶ��
//    SendToOled(oled,0xF0,OLEDDC_Command); //------
//    
//    SendToOled(oled,0xD9,OLEDDC_Command); //--set pre-charge period
//    SendToOled(oled,0x22,OLEDDC_Command); //------
//    
//    SendToOled(oled,0xDA,OLEDDC_Command); //--set com pins hardware configuration
//    SendToOled(oled,0x12,OLEDDC_Command); //------
//    
//    SendToOled(oled,0xDB,OLEDDC_Command); //--set vcomh
//    SendToOled(oled,0x20,OLEDDC_Command); //------
//    
//    SendToOled(oled,0x8D,OLEDDC_Command); //--set DC-DC enable
//    SendToOled(oled,0x14,OLEDDC_Command); //------
//    
//    SendToOled(oled,0xAF,OLEDDC_Command); //����ʾ
//    
    SendToOled(oled,0xAE,OLEDDC_Command); //�ر���ʾ
    
    SendToOled(oled,0x00,OLEDDC_Command); //�����е�ַ��4λ
    SendToOled(oled,0x10,OLEDDC_Command); //�����е�ַ��4λ
    
    SendToOled(oled,0x40,OLEDDC_Command); //������ʼ�ߵ�ַ
    
    SendToOled(oled,0x81,OLEDDC_Command); //���öԱȶ�ֵ
    SendToOled(oled,0xCF,OLEDDC_Command); //------
    
    SendToOled(oled,0xA1,OLEDDC_Command); //0xA0��X��������ʾ��0xA1��X�᾵����ʾ
    SendToOled(oled,0xC8,OLEDDC_Command); //0xC0��Y��������ʾ��0xC8��Y�᾵����ʾ
    
    SendToOled(oled,0xA6,OLEDDC_Command); //0xA6,������ʾģʽ;0xA7����ɫ��ʾ
    
    SendToOled(oled,0xA8,OLEDDC_Command); //���ö�·������(1 to 64)
    SendToOled(oled,0x3F,OLEDDC_Command); //------
    
    SendToOled(oled,0xD3,OLEDDC_Command); //������ʾƫ��
    SendToOled(oled,0x00,OLEDDC_Command); //------
    
    SendToOled(oled,0xD5,OLEDDC_Command); //�����ڲ���ʾʱ��Ƶ��
    SendToOled(oled,0x80,OLEDDC_Command); //------
    
    SendToOled(oled,0xD9,OLEDDC_Command); //--set pre-charge period
    SendToOled(oled,0xF1,OLEDDC_Command); //------
    
    SendToOled(oled,0xDA,OLEDDC_Command); //--set com pins hardware configuration
    SendToOled(oled,0x12,OLEDDC_Command); //------
    
    SendToOled(oled,0xDB,OLEDDC_Command); //--set vcomh
    SendToOled(oled,0x20,OLEDDC_Command); //------
    
    SendToOled(oled,0x20,OLEDDC_Command); //Set Memory Addressing Mode	
    SendToOled(oled,0x02,OLEDDC_Command); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    
    SendToOled(oled,0x8D,OLEDDC_Command); //--set DC-DC enable
    SendToOled(oled,0x14,OLEDDC_Command); //------
    
    SendToOled(oled,0xA4,OLEDDC_Command); //0xa4,��ʾ����RAM�ĸı���ı�;0xa5,��ʾ���ݺ���RAM������
    SendToOled(oled,0xA6,OLEDDC_Command); //0xA6,������ʾģʽ;0xA7����ɫ��ʾ
    SendToOled(oled,0xAF,OLEDDC_Command); //����ʾ
    
    OledClearScreen(oled);
}

/*���OLED��ʾ*/
static void OledClearScreen(OledObjectType *oled)
{
    for(int page=0;page<8;page++)
    {
        SetOledCoordinate(oled,page,0);
        
        for(int i=0;i<128;i++)
        {
            SendToOled(oled,0x00,OLEDDC_Data);
        }
    }
    
    SetOledCoordinate(oled,0,0);
}

/*��OLED��������*/
static void SendToOled(OledObjectType *oled,uint8_t sData,OledDCType type)
{
    uint8_t wData[2];
    
    if(oled->port==OLED_SPI)
    {
        oled->ChipSelcet(OLEDCS_Enable);
        
        if(type==OLEDDC_Command)
        {
            oled->DCSelcet(OLEDDC_Command);
        }
        else
        {
            oled->DCSelcet(OLEDDC_Data);
        }
        
        oled->Write(oled,&sData,1);
        
        oled->ChipSelcet(OLEDCS_Disable);
    }
    else
    {
        if(type==OLEDDC_Command)
        {
            wData[0]=0x00;
        }
        else
        {
            wData[0]=0x40;
        }
        
        wData[1]=sData;
        
        oled->Write(oled,wData,2);
    }
}

/*����OLED������ʾ����,segˮƽ���꣨0��127��,com��ʾOLED��ҳ��0��7�� */
static void SetOledCoordinate(OledObjectType *oled,uint8_t com, uint8_t seg)
{
    SendToOled(oled,(0xB0+(com & 0x0F)),OLEDDC_Command);               //����OLED��ʾ��ҳ    
    SendToOled(oled,(((seg & 0xF0)>>4) | 0x10), OLEDDC_Command);        //����OLEDˮƽ����ĸ�4λ
    SendToOled(oled,(seg & 0x0F), OLEDDC_Command);                 //����OLEDˮƽ����ĵ�4λ
}


/* Ĭ�ϵ�Ƭѡ�źŴ����� */
static void OledChipSelect(OledCSType cs)
{
    return;
}

/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
