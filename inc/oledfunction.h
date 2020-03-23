/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：oledfunction.h                                                 **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：应用于实现OLED显示屏的操作                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-07-17          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __OLEDFUNCTION_H
#define __OLEDFUNCTION_H

#include "stdint.h"

/*定义显示的字体大小*/
typedef enum OledFontSize{
  OLED_FONT_6x8,
  OLED_FONT_8x16
}OledFontSizeType;

/*OLED的接口类型*/
typedef enum OledPort{
  OLED_SPI,
  OLED_I2C
}OledPortType;

/* 定义OLED片选信号枚举 */
typedef enum OledCS {
  OLEDCS_Enable,
  OLEDCS_Disable
}OledCSType;

/* 定义OLEDDC信号枚举 */
typedef enum OledDC {
  OLEDDC_Command,
  OLEDDC_Data
}OledDCType;

/* 定义RST信号枚举 */
typedef enum OledRST {
  OLED_RESET,
  OLED_WORK
}OledRSTType;

/*定义OLED对象类型*/
typedef struct OledObject {
  uint8_t devAddress;
  OledPortType port;
  void (*Write)(struct OledObject *oled,uint8_t *wData,uint16_t wSize);
  void (*ChipSelcet)(OledCSType en);
  void (*DCSelcet)(OledDCType dc);
  void (*ChipReset)(OledRSTType rst);
  void (*Delayms)(volatile uint32_t nTime);
}OledObjectType;

/*向MS5803下发指令，指令格式均为1个字节*/
typedef void (*OledWrite)(OledObjectType *oled,uint8_t *wData,uint16_t wSize);
/*复位信号操作函数指针*/
typedef void (*OledChipReset)(OledRSTType rst);
/*数据命令，用于SPI接口*/
typedef void (*OledDCSelcet)(OledDCType dc);
/*片选信号,用于SPI接口*/
typedef void (*OledChipSelcet)(OledCSType en);     
/*毫秒秒延时函数*/
typedef void (*OledDelayms)(volatile uint32_t nTime);   


/*OLED显示屏对象初始化*/
void OledInitialization(OledObjectType *oled,           //OLED对象
                        OledPortType port,              //通讯端口
                        uint8_t address,                //I2C设备地址
                        OledWrite write,                //写数据函数
                        OledChipReset rst,              //复位信号操作函数指针
                        OledDCSelcet dc,                //DC信号控制函数指针
                        OledChipSelcet cs,              //SPI片选信号函数指针
                        OledDelayms delayms             //毫秒延时函数指针
                       );

/*显示一副128x64的图片*/
void OledShowPicture(OledObjectType *oled,uint8_t *sData);

/*显示32x32（32行32列）的字符，如汉字*/
void OledShow32x32Char(OledObjectType *oled,uint8_t page,uint8_t seg,uint8_t *sData);

/*显示16x16（16行16列）的字符，如汉字*/
void OledShow16x16Char(OledObjectType *oled,uint8_t page,uint8_t seg,uint8_t *sData);

/*显示8x16（16行8列）的字符，如ASCII码字符等*/
void OledShow8x16Char(OledObjectType *oled,uint8_t page,uint8_t seg,uint8_t *sData);

/*显示6x8（8行6列）的字符，如ASCII码字符*/
void OledShow6x8Char(OledObjectType *oled,uint8_t page,uint8_t seg,uint8_t *sData);

/*显示字符串*/
void OledShowString(OledObjectType *oled,OledFontSizeType fontSize,uint8_t page,uint8_t seg,char *fmt,...);

#endif
/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
