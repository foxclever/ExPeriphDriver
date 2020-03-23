/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：lcd1602function.c                                              **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：应用于LCD1602液晶屏的操作                                      **/
/**           采用HD4470及兼容芯片，基本操作时序：                           **/
/**           +------+-------------------------------+------------+          **/
/**           | 操作 |             输入              |    输出    |          **/
/**           +------+-------------------------------+------------+          **/
/**           |读状态|RS=L,RW=H,E=H                  |D0-D7=状态字|          **/
/**           +------+-------------------------------+------------+          **/
/**           |写指令|RS=L,RW=L,D0-D7=指令码,E=高脉冲|无          |          **/
/**           +------+-------------------------------+------------+          **/
/**           |读数据|RS=H,RW=H,E=H                  |D0-D7=数据  |          **/
/**           +------+-------------------------------+------------+          **/
/**           |写数据|RS=H,RW=L,D0-D7=数据,E=高脉冲  |无          |          **/
/**           +------+-------------------------------+------------+          **/
/**           显示模式设置：                                                 **/
/**           +---------------+----------------------------------+           **/
/**           |    指令码     |              功能                |           **/
/**           +-+-+-+-+-+-+-+-+----------------------------------+           **/
/**           |0|0|1|1|1|0|0|0|设置16x2显示，5x7点阵，8位数据接口|           **/
/**           +-+-+-+-+-+-+-+-+----------------------------------+           **/
/**           显示开/关及光标设置：                                          **/
/**           +---------------+----------------------------------+           **/
/**           |    指令码     |              功能                |           **/
/**           +-+-+-+-+-+-+-+-+----------------------------------+           **/
/**           |0|0|0|0|1|D|C|B|D=1 开显示；D=0关显示             |           **/
/**           | | | | | | | | |C=1 显示光标；C=0 不显示光标      |           **/
/**           | | | | | | | | |B=1 光标闪烁；C=0 光标不闪烁      |           **/
/**           +-+-+-+-+-+-+-+-+----------------------------------+           **/
/**           |0|0|0|0|0|1|N|S|N=1 读写后指针和贯标加1           |           **/
/**           | | | | | | | | |N=0 读写后指针和贯标减1           |           **/
/**           | | | | | | | | |S=1 写一个字符后整屏移动，方向随N |           **/
/**           | | | | | | | | |S=0 写一个字符后整屏显示不移动    |           **/
/**           +-+-+-+-+-+-+-+-+----------------------------------+           **/
/**           其他设置：                                                     **/
/**           +---------------+----------------------------------+           **/
/**           |    指令码     |              功能                |           **/
/**           +-+-+-+-+-+-+-+-+----------------------------------+           **/
/**           |0|0|0|0|0|0|0|1|显示清屏，指针和显示均清零        |           **/
/**           +-+-+-+-+-+-+-+-+----------------------------------+           **/
/**           |0|0|0|0|0|0|1|0|显示回车，数据指针清零            |           **/
/**           +-+-+-+-+-+-+-+-+----------------------------------+           **/
/**           数据指针设置，地址码0-27H，40H-67H：                           **/
/**           +---------------+----------------------------------+           **/
/**           |    指令码     |              功能                |           **/
/**           +---------------+----------------------------------+           **/
/**           |80H+地址码     |设置数据地址指针                  |           **/
/**           +---------------+----------------------------------+           **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    1、声明LCD1602ObjectType对象实体，并调用函数LCD1602Initialization初   **/
/**       始化对象实体。并配置好硬件接口，采用并行通讯。                     **/
/**    2、实现三个形如void f(uint8_t value)的引脚输出操作函数，分别操RS、    **/
/**       RW、EN。并声明LCD1602PinSetType类型3个元素的函数指针数组，RS的操   **/
/**       作函数作为第一个元素，RW的操作函数作为第二个元素，EN的操作函数作   **/
/**       为第三个元素。并将函数指针数组的指针赋值给LCD1602ObjectType对象实  **/
/**       体的PinHandle函数指针。实现对RS、RW、EN三个引脚的操作。            **/
/**    3、实现形如uint8_t f(void)的函数，并将其赋值给LCD1602ObjectType对象实 **/
/**       体的GetByte函数指针。实现获取一个字节的数据。                      **/
/**    4、实现形如void f(uint8_t)的函数，并将其赋值给LCD1602ObjectType对象实 **/
/**       体的SendByte函数指针。实现发送一个字节的数据                       **/
/**    5、实现形如void f(volatile uint32_t period)的函数，并将其赋值给       **/
/**       LCD1602ObjectType对象实体的Delayus函数指针。实现操作延时，单位微秒 **/
/**    6、实现形如void f(volatile uint32_t nTime)的函数，并将其赋值给        **/
/**       LCD1602ObjectType对象实体的Delayms函数指针。实现操作延时，单位毫秒 **/
/**    7、调用相应的函数实现需要的操作。                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-07-17          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "lcd1602function.h"
#include "stddef.h"

#define High 1
#define Low  0

/*从LCD1602读状态*/
static uint8_t ReadStatusFromLCD1602(LCD1602ObjectType *lcd);
/*向LCD1602写指令*/
static void WriteCommandToLCD1602(LCD1602ObjectType *lcd,uint8_t command);
/*从LCD1602读数据*/
static uint8_t ReadDataFromLCD1602(LCD1602ObjectType *lcd);
/*向LCD1602写数据*/
static void WriteDatatoLCD1602(LCD1602ObjectType *lcd,uint8_t data);

/*从LCD1602读状态,读取状态字如下：
+----+----+----+----+----+----+----+----+
| D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 |
+----+----+----+----+----+----+----+----+
|STA7|STA6|STA5|STA4|STA3|STA2|STA1|STA0|
+----+----+----+----+----+----+----+----+
STA6-0：当前数据地址指针的数值
STA7:读写操作使能，1禁止，0允许          */
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

/*向LCD1602写指令*/
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

/*从LCD1602读数据*/
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

/*向LCD1602写数据*/
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

/*对显示屏作初始化配置*/
void LCD1602Initialization(LCD1602ObjectType *lcd,              //LCD1602对象指针
                           LCD1602PinSetType *PinHandle,         //控制引脚操作函数指针数组
                           LCD1602SendByteType sendByte,        //发送一个字节函数指针
                           LCD1602GetByteType getByte,          //读取一个字节函数指针
                           LCD1602DelayType delayus,            //微秒延时函数指针
                           LCD1602DelayType delayms             //毫秒延时函数指针
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
  
  /*后续需要检测BUSY，等待10Mms*/
  lcd->Delayms(10);
  WriteCommandToLCD1602(lcd,0x38);//显示模式设置
  lcd->Delayms(10);
  WriteCommandToLCD1602(lcd,0x08);//显示关闭
  lcd->Delayms(10);
  WriteCommandToLCD1602(lcd,0x01);//显示清屏
  lcd->Delayms(10);
  WriteCommandToLCD1602(lcd,0x06);//显示光标移动位置
  lcd->Delayms(10);
  WriteCommandToLCD1602(lcd,0x0C);//显示开及光标设置

  lcd->PinHandle[LCD1602_EN](Low);
  
  lcd->status=ReadStatusFromLCD1602(lcd);
}

/*向LCD1602设定显示内容*/
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

/*清除LCD1602的显示*/
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
