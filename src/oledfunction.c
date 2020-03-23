/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：oledfunction.c                                                 **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：应用于实现OLED显示屏的操作                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-07-17          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "stdarg.h"
#include "stdio.h"
#include "oledfontlibrary.h"
#include "oledfunction.h"

/*基本控制命令*/
#define EntireDisplayFollowsRAM 0xA4
#define EntireDisplayIgnoresRAM 0xA5
#define SetNormalDisplay 0xA6
#define SetInverse Display 0xA7
#define SetDisplayOFF 0xAE
#define SetDisplayON 0xAF

/*向OLED发送数据*/
static void SendToOled(OledObjectType *oled,uint8_t sData,OledDCType type);
/* 默认的片选信号处理函数 */
static void OledChipSelect(OledCSType cs);
/*设置OLED屏的显示坐标,seg水平坐标（0—127）,com表示OLED的页（0—7） */
static void SetOledCoordinate(OledObjectType *oled,uint8_t com, uint8_t seg);
/*清除OLED显示*/
static void OledClearScreen(OledObjectType *oled);



/*显示ASCII字符串*/
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

/*显示6x8（8行6列）的字符，如ASCII码字符*/
void OledShow6x8Char(OledObjectType *oled,uint8_t page,uint8_t seg,uint8_t *sData)
{
  SetOledCoordinate(oled,page,seg);
    
  for(int j=0;j<8;j++)
  {
    SendToOled(oled,*sData++,OLEDDC_Data);
  }
}

/*显示8x16（16行8列）的字符，如ASCII码字符等*/
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

/*显示16x16（16行16列）的字符，如汉字*/
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

/*显示32x32（32行32列）的字符，如汉字*/
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

/*显示一副128x64的图片*/
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

/*OLED显示屏对象初始化*/
void OledInitialization(OledObjectType *oled,           //OLED对象
                        OledPortType port,              //通讯端口
                        uint8_t address,                //I2C设备地址
                        OledWrite write,                //写数据函数
                        OledChipReset rst,              //复位信号操作函数指针
                        OledDCSelcet dc,                //DC信号控制函数指针
                        OledChipSelcet cs,              //SPI片选信号函数指针
                        OledDelayms delayms             //毫秒延时函数指针
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
  
  SendToOled(oled,0xAE,OLEDDC_Command); //关闭显示
  SendToOled(oled,0x20,OLEDDC_Command); //Set Memory Addressing Mode	
  SendToOled(oled,0x10,OLEDDC_Command); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
  SendToOled(oled,0xB0,OLEDDC_Command); //Set Page Start Address for Page Addressing Mode,0-7
  SendToOled(oled,0xA1,OLEDDC_Command); //0xa0，X轴正常显示；0xa1，X轴镜像显示
  SendToOled(oled,0xC8,OLEDDC_Command); //0xc0，Y轴正常显示；0xc8，Y轴镜像显示
  SendToOled(oled,0x00,OLEDDC_Command); //设置列地址低4位
  SendToOled(oled,0x10,OLEDDC_Command); //设置列地址高4位
  SendToOled(oled,0x40,OLEDDC_Command); //设置起始线地址
  
  SendToOled(oled,0x81,OLEDDC_Command); //设置对比度值
  SendToOled(oled,0x7F,OLEDDC_Command); //------
  
  SendToOled(oled,0xA6,OLEDDC_Command); //0xa6,正常显示模式;0xa7，
  
  SendToOled(oled,0xA8,OLEDDC_Command); //--set multiplex ratio(1 to 64)
  SendToOled(oled,0x3F,OLEDDC_Command); //------
  
  SendToOled(oled,0xA4,OLEDDC_Command); //0xa4,显示跟随RAM的改变而改变;0xa5,显示内容忽略RAM的内容
  
  SendToOled(oled,0xD3,OLEDDC_Command); //设置显示偏移
  SendToOled(oled,0x00,OLEDDC_Command); //------
  
  SendToOled(oled,0xD5,OLEDDC_Command); //设置内部显示时钟频率
  SendToOled(oled,0xF0,OLEDDC_Command); //------
  
  SendToOled(oled,0xD9,OLEDDC_Command); //--set pre-charge period
  SendToOled(oled,0x22,OLEDDC_Command); //------
  
  SendToOled(oled,0xDA,OLEDDC_Command); //--set com pins hardware configuration
  SendToOled(oled,0x12,OLEDDC_Command); //------
  
  SendToOled(oled,0xDB,OLEDDC_Command); //--set vcomh
  SendToOled(oled,0x20,OLEDDC_Command); //------
  
  SendToOled(oled,0x8D,OLEDDC_Command); //--set DC-DC enable
  SendToOled(oled,0x14,OLEDDC_Command); //------
  
  SendToOled(oled,0xAF,OLEDDC_Command); //打开显示
  
  OledClearScreen(oled);
}

/*清除OLED显示*/
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

/*向OLED发送数据*/
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

/*设置OLED屏的显示坐标,seg水平坐标（0—127）,com表示OLED的页（0—7） */
static void SetOledCoordinate(OledObjectType *oled,uint8_t com, uint8_t seg)
{
    SendToOled(oled,(0xB0+(com & 0x0F)),OLEDDC_Command);               //设置OLED显示的页    
    SendToOled(oled,(((seg & 0xF0)>>4) | 0x10), OLEDDC_Command);        //设置OLED水平坐标的高4位
    SendToOled(oled,(seg & 0x0F), OLEDDC_Command);                 //设置OLED水平坐标的低4位
}


/* 默认的片选信号处理函数 */
static void OledChipSelect(OledCSType cs)
{
  return;
}

/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
