/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名   dwinlcdfunction.h                                               **/
/** 版  本   V1.1.0                                                          **/
/** 简  介   实现迪文LCD显示屏读写操作                                       **/
/**          帧格式：帧头+数据长度+指令+数据+CRC校验                         **/
/**          其中：帧头2个字节，有配置文件决定帧头的内容                     **/
/**                数据长度1个字节，包括指令、数据和校验                     **/
/**                指令有5个：0x80,0x81,0x82,0x83,0x84,对应不同操作          **/
/**                     0x80写寄存器                                         **/
/**                     0x81读寄存器                                         **/
/**                     0x82写变量存储器                                     **/
/**                     0x83度变量存储器                                     **/
/**                     0x84写曲线显示                                       **/
/**                数据的长度因操作而定                                      **/
/**                CRC校验是否启用，在配置文件中设定                         **/
/**          寄存器的地址为1个字节，变量的地址为2个字节                      **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2014-01-18          木南              创建文件               **/
/**     V1.1.0  2019-08-01          木南              修改为基于对象的操作   **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __dwinlcdfunction_h
#define __dwinlcdfunction_h

#include "stdint.h"

#ifndef RxBufferLength
#define RxBufferLength 260
#endif

/*定义迪文屏的开发类型*/
typedef enum DwinSort {
  DGUS,
  DGUSII
}DwinSortType;

/* 定义通讯校验码使用与否枚举 */
typedef enum DwinCheckCode {
  DwinNone,
  DwinCRC16
}DwinCheckCodeType;

/*功能码定义*/
typedef enum {
  FC_REG_Write=((uint8_t)0x80),         /*写寄存器的功能码*/
  FC_REG_Read=((uint8_t)0x81),          /*读寄存器的功能码*/
  FC_VAR_Write=((uint8_t)0x82),         /*写变量的功能码*/
  FC_VAR_Read=((uint8_t)0x83),          /*读变量的功能码*/
  FC_Curve_Write=((uint8_t)0x84)        /*写曲线缓冲区的功能码*/
}DwinFunctionCode;

/* 定义迪文串口屏对象类型 */
typedef struct DwinObject {
  DwinSortType sort;							//屏的类别
  DwinCheckCodeType checkMode;		//校验方式
  struct DwinRxBuffer{
    uint8_t queue[RxBufferLength];	//键值存储队列
    uint8_t pRead;									//读队列指针
    uint8_t pWrite;									//写队列指针
    uint16_t (*DeQueue)(struct DwinObject *dwin,uint8_t *rxBuf);	//出队操作
    void (*EnQueue)(struct DwinObject *dwin,uint8_t rData);				//入队操作
  }rxBuffer;					//定义接收缓存队列
  void (*SendData)(uint8_t *txData,uint16_t length);	//发送数据
  void (*GetRegister)(struct DwinObject *dwin,uint8_t regAddress,uint8_t readByteLength);
  void (*SetRegister)(struct DwinObject *dwin,uint8_t regAddress,uint8_t *txData,uint16_t length);
}DwinObjectType;

typedef void (*SendDataForDwinType)(uint8_t *txData,uint16_t length);

/*写数据变量存储器，一次最多允许写47个字，即length<=94*/
void WriteFlashDataToDwinLCD(DwinObjectType *dwin,uint16_t startAddress,uint8_t *txData,uint16_t length);
/*读变量存储器数据*/
void ReadFlashDataFromDwinLCD(DwinObjectType *dwin,uint16_t startAddress,uint8_t readWordLength);
/*写曲线缓冲区，一次最多允许写8个字，即length<=16*/
void WriteCurveToDwinLCD(DwinObjectType *dwin,uint8_t *txData,uint16_t length,uint8_t channelMode);
/*读取LCD系统时间*/
void GetDateTimeFromDwinLCD(DwinObjectType *dwin);
/*校准LCD系统时间*/
void CalibrationDateTimeForDwinLCD(DwinObjectType *dwin,uint8_t *dateTime);
/*音乐播放控制playNum为0时表示停止播放*/
void HandleDwinLCDToPlayMusic(DwinObjectType *dwin,uint16_t playStart,uint16_t playNum,uint16_t volume);
/*设置屏显示画面*/
void SetDwinLCDDisplay(DwinObjectType *dwin,uint16_t picID);

/* 初始化迪文串口屏对象 */
void DwinInitialization(DwinObjectType *dwin,
                        DwinCheckCodeType checkMode,
                        DwinSortType sort,
                        SendDataForDwinType SendData
                       );

#endif
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
