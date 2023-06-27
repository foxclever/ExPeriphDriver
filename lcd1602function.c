/******************************************************************************/
/** ģ�����ƣ���չ��������                                                   **/
/** �ļ����ƣ�lcd1602function.c                                              **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺Ӧ����LCD1602Һ�����Ĳ���                                      **/
/**           ����HD4470������оƬ����������ʱ��                           **/
/**           +------+-------------------------------+------------+          **/
/**           | ���� |             ����              |    ���    |          **/
/**           +------+-------------------------------+------------+          **/
/**           |��״̬|RS=L,RW=H,E=H                  |D0-D7=״̬��|          **/
/**           +------+-------------------------------+------------+          **/
/**           |дָ��|RS=L,RW=L,D0-D7=ָ����,E=������|��          |          **/
/**           +------+-------------------------------+------------+          **/
/**           |������|RS=H,RW=H,E=H                  |D0-D7=����  |          **/
/**           +------+-------------------------------+------------+          **/
/**           |д����|RS=H,RW=L,D0-D7=����,E=������  |��          |          **/
/**           +------+-------------------------------+------------+          **/
/**           ��ʾģʽ���ã�                                                 **/
/**           +---------------+----------------------------------+           **/
/**           |    ָ����     |              ����                |           **/
/**           +-+-+-+-+-+-+-+-+----------------------------------+           **/
/**           |0|0|1|1|1|0|0|0|����16x2��ʾ��5x7����8λ���ݽӿ�|           **/
/**           +-+-+-+-+-+-+-+-+----------------------------------+           **/
/**           ��ʾ��/�ؼ�������ã�                                          **/
/**           +---------------+----------------------------------+           **/
/**           |    ָ����     |              ����                |           **/
/**           +-+-+-+-+-+-+-+-+----------------------------------+           **/
/**           |0|0|0|0|1|D|C|B|D=1 ����ʾ��D=0����ʾ             |           **/
/**           | | | | | | | | |C=1 ��ʾ��ꣻC=0 ����ʾ���      |           **/
/**           | | | | | | | | |B=1 �����˸��C=0 ��겻��˸      |           **/
/**           +-+-+-+-+-+-+-+-+----------------------------------+           **/
/**           |0|0|0|0|0|1|N|S|N=1 ��д��ָ��͹���1           |           **/
/**           | | | | | | | | |N=0 ��д��ָ��͹���1           |           **/
/**           | | | | | | | | |S=1 дһ���ַ��������ƶ���������N |           **/
/**           | | | | | | | | |S=0 дһ���ַ���������ʾ���ƶ�    |           **/
/**           +-+-+-+-+-+-+-+-+----------------------------------+           **/
/**           �������ã�                                                     **/
/**           +---------------+----------------------------------+           **/
/**           |    ָ����     |              ����                |           **/
/**           +-+-+-+-+-+-+-+-+----------------------------------+           **/
/**           |0|0|0|0|0|0|0|1|��ʾ������ָ�����ʾ������        |           **/
/**           +-+-+-+-+-+-+-+-+----------------------------------+           **/
/**           |0|0|0|0|0|0|1|0|��ʾ�س�������ָ������            |           **/
/**           +-+-+-+-+-+-+-+-+----------------------------------+           **/
/**           ����ָ�����ã���ַ��0-27H��40H-67H��                           **/
/**           +---------------+----------------------------------+           **/
/**           |    ָ����     |              ����                |           **/
/**           +---------------+----------------------------------+           **/
/**           |80H+��ַ��     |�������ݵ�ַָ��                  |           **/
/**           +---------------+----------------------------------+           **/
/**--------------------------------------------------------------------------**/
/**ʹ��˵����                                                                **/
/**    1������LCD1602ObjectType����ʵ�壬�����ú���LCD1602Initialization��   **/
/**       ʼ������ʵ�塣�����ú�Ӳ���ӿڣ����ò���ͨѶ��                     **/
/**    2��ʵ����������void f(uint8_t value)��������������������ֱ��RS��    **/
/**       RW��EN��������LCD1602PinSetType����3��Ԫ�صĺ���ָ�����飬RS�Ĳ�   **/
/**       ��������Ϊ��һ��Ԫ�أ�RW�Ĳ���������Ϊ�ڶ���Ԫ�أ�EN�Ĳ���������   **/
/**       Ϊ������Ԫ�ء���������ָ�������ָ�븳ֵ��LCD1602ObjectType����ʵ  **/
/**       ���PinHandle����ָ�롣ʵ�ֶ�RS��RW��EN�������ŵĲ�����            **/
/**    3��ʵ������uint8_t f(void)�ĺ����������丳ֵ��LCD1602ObjectType����ʵ **/
/**       ���GetByte����ָ�롣ʵ�ֻ�ȡһ���ֽڵ����ݡ�                      **/
/**    4��ʵ������void f(uint8_t)�ĺ����������丳ֵ��LCD1602ObjectType����ʵ **/
/**       ���SendByte����ָ�롣ʵ�ַ���һ���ֽڵ�����                       **/
/**    5��ʵ������void f(volatile uint32_t period)�ĺ����������丳ֵ��       **/
/**       LCD1602ObjectType����ʵ���Delayus����ָ�롣ʵ�ֲ�����ʱ����λ΢�� **/
/**    6��ʵ������void f(volatile uint32_t nTime)�ĺ����������丳ֵ��        **/
/**       LCD1602ObjectType����ʵ���Delayms����ָ�롣ʵ�ֲ�����ʱ����λ���� **/
/**    7��������Ӧ�ĺ���ʵ����Ҫ�Ĳ�����                                     **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2017-07-17          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "lcd1602function.h"
#include "stddef.h"

#define High 1
#define Low  0

/*��LCD1602��״̬*/
static uint8_t ReadStatusFromLCD1602(LCD1602ObjectType *lcd);
/*��LCD1602дָ��*/
static void WriteCommandToLCD1602(LCD1602ObjectType *lcd,uint8_t command);
/*��LCD1602������*/
static uint8_t ReadDataFromLCD1602(LCD1602ObjectType *lcd);
/*��LCD1602д����*/
static void WriteDatatoLCD1602(LCD1602ObjectType *lcd,uint8_t data);

/*��LCD1602��״̬,��ȡ״̬�����£�
+----+----+----+----+----+----+----+----+
| D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 |
+----+----+----+----+----+----+----+----+
|STA7|STA6|STA5|STA4|STA3|STA2|STA1|STA0|
+----+----+----+----+----+----+----+----+
STA6-0����ǰ���ݵ�ַָ�����ֵ
STA7:��д����ʹ�ܣ�1��ֹ��0����          */
static uint8_t ReadStatusFromLCD1602(LCD1602ObjectType *lcd)
{
    uint8_t status;
    lcd->PinHandle[LCD1602_RS](Low);
    lcd->PinHandle[LCD1602_RW](High);
    lcd->PinHandle[LCD1602_EN](High);
    
    lcd->Delayus(20);
    status=lcd->GetByte();
    lcd->PinHandle[LCD1602_EN](Low);
    lcd->Delayus(5);
    return status;
}

/*��LCD1602дָ��*/
static void WriteCommandToLCD1602(LCD1602ObjectType *lcd,uint8_t command)
{
    lcd->PinHandle[LCD1602_RS](Low);
    lcd->PinHandle[LCD1602_RW](Low);
    
    lcd->SendByte(command);
    
    lcd->PinHandle[LCD1602_EN](High);
    lcd->Delayus(20);
    lcd->PinHandle[LCD1602_EN](Low);
    lcd->Delayus(5);
}

/*��LCD1602������*/
static uint8_t ReadDataFromLCD1602(LCD1602ObjectType *lcd)
{
    uint8_t data;
    lcd->PinHandle[LCD1602_RS](High);
    lcd->PinHandle[LCD1602_RW](High);
    lcd->PinHandle[LCD1602_EN](High);
    
    lcd->Delayus(20);
    data=lcd->GetByte();
    lcd->PinHandle[LCD1602_EN](Low);
    lcd->Delayus(5);
    return data;
}

/*��LCD1602д����*/
static void WriteDatatoLCD1602(LCD1602ObjectType *lcd,uint8_t data)
{
    lcd->PinHandle[LCD1602_RS](High);
    lcd->PinHandle[LCD1602_RW](Low);
    
    lcd->SendByte(data);
    
    lcd->PinHandle[LCD1602_EN](High);
    lcd->Delayus(20);
    lcd->PinHandle[LCD1602_EN](Low);
    lcd->Delayus(5);
}

/*����ʾ������ʼ������*/
void LCD1602Initialization(LCD1602ObjectType *lcd,              //LCD1602����ָ��
                           LCD1602PinSetType *PinHandle,         //�������Ų�������ָ������
                           LCD1602SendByteType sendByte,        //����һ���ֽں���ָ��
                           LCD1602GetByteType getByte,          //��ȡһ���ֽں���ָ��
                           LCD1602DelayType delayus,            //΢����ʱ����ָ��
                           LCD1602DelayType delayms             //������ʱ����ָ��
                               )
{
    if((lcd==NULL)||(PinHandle==NULL)||(sendByte==NULL)||(getByte==NULL)||(delayus==NULL)||(delayms==NULL))
    {
        return;
    }
    
    lcd->PinHandle=PinHandle;
    lcd->SendByte=sendByte;
    lcd->GetByte=getByte;
    lcd->Delayus=delayus;
    lcd->Delayms=delayms;
    
    lcd->Delayus(15);
    WriteCommandToLCD1602(lcd,0x38);
    lcd->Delayms(5);
    WriteCommandToLCD1602(lcd,0x38);
    lcd->Delayms(5);
    WriteCommandToLCD1602(lcd,0x38);
    
    /*������Ҫ���BUSY���ȴ�10Mms*/
    lcd->Delayms(10);
    WriteCommandToLCD1602(lcd,0x38);//��ʾģʽ����
    lcd->Delayms(10);
    WriteCommandToLCD1602(lcd,0x08);//��ʾ�ر�
    lcd->Delayms(10);
    WriteCommandToLCD1602(lcd,0x01);//��ʾ����
    lcd->Delayms(10);
    WriteCommandToLCD1602(lcd,0x06);//��ʾ����ƶ�λ��
    lcd->Delayms(10);
    WriteCommandToLCD1602(lcd,0x0C);//��ʾ�����������
    
    lcd->PinHandle[LCD1602_EN](Low);
    
    lcd->status=ReadStatusFromLCD1602(lcd);
}

/*��LCD1602�趨��ʾ����*/
void Lcd1602ContentDisplay(LCD1602ObjectType *lcd,uint8_t pos,char *fmt,...)
{
    unsigned char LcdPrintfBuf[33];
    uint8_t count = 0;
    uint8_t remain = 0;
    va_list ap;
    unsigned char *pStr = LcdPrintfBuf;
    
    va_start(ap,fmt);
    vsnprintf((char *)LcdPrintfBuf, sizeof(LcdPrintfBuf), fmt, ap);
    va_end(ap);
    
    remain = 0x8F - pos;
    
    lcd->Delayms(10);
    WriteCommandToLCD1602(lcd,pos);
    
    while(*pStr !=0)
    {
        WriteDatatoLCD1602(lcd,*pStr++);
        if(++count > remain && pos <= 0x8F)
        {
            count = 0;
            lcd->Delayms(10);
            WriteCommandToLCD1602(lcd,0xC0);
            lcd->Delayms(1);
        }
    }
}

/*���LCD1602����ʾ*/
void LCD1602DisplayClear(LCD1602ObjectType *lcd,LCD1602DisplayLineType dLine)
{
    uint8_t pos[3]={0x80,0xC0,0x01};
    if(dLine==LCD1602_AllLine)
    {
        lcd->Delayms(10);
        WriteCommandToLCD1602(lcd,pos[2]);
    }
    else
    {
        Lcd1602ContentDisplay(lcd,pos[dLine],"                ");
    }
}
/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
