/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：esp8266function.h                                              **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现ESP8266无线通讯的操作。ESP8266EX支持TCP/IP协议，完全   **/
/**           遵循802.11 b/g/n/e/i WLAN MAC 协议和 Wi-Fi Direct 标准。采用   **/
/**           USART接口，可以通过AT指令实现数据交换。                        **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-07-17          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __ESP8266FUNCTION_H
#define __ESP8266FUNCTION_H

#include "stdint.h"

#define Esp8266RxBufferLength 50

/*定义接收状态枚举*/
typedef enum Esp8266RxStatus {
  Esp8266_RxNone,       //未开始接收
  Esp8266_RxWait,       //正在接收中
  Esp8266_RxFinish      //接收完成
}Esp8266RxStatusType;

/*定义命定发送结果枚举*/
typedef enum Esp8266TxStatus {
  Esp8266_TxFial,       //发送失败
  Esp8266_RxSucceed,    //发送成功
}Esp8266TxStatusType;

/*定义传输模式枚举*/
typedef enum Esp8266CIPMode {
  Esp8266_NormalMode,   //正常模式
  Esp8266_TransMode     //透传模式
}Esp8266CIPModeType;

/*定义ESP8266的WIFI模式*/
typedef enum Esp8266CWMode {
  Esp8266_StationMode,  //station 模式 
  Esp8266_SoftAPMode,   //softAP 模式 
  Esp8266_MixedMode     //softAP + station 模式 
}Esp8266CWModeType;

/*定义网络连接信息枚举*/
typedef enum Esp8266CIPStatus {
  Esp8266_Timeout=0,    //超时
  Esp8266_Error=1,        //错误
  Esp8266_GotIP=2,        //获得IP地址
  Esp8266_Connected=3,    //连接正常
  Esp8266_Disconnect=4,   //断开连接
  Esp8266_NoWifi=5        //未连接到 WiFi 
}Esp8266CIPStatusType;


/*定义ESP8266对象*/
typedef struct Esp8266Object {
  Esp8266CWModeType cwMode;     //WIFI模式
  Esp8266CIPModeType cipMode;   //传输模式，正常或透传
  struct EspRxBuffer{
    uint8_t queue[Esp8266RxBufferLength];	//数据存储队列
    uint8_t lengthRecieving;			//正在接收的数据长度
    uint8_t lengthRecieved;			//已经接收的数据长度
  }rxBuffer;
  void (*SendData)(uint8_t *sData,uint16_t sSize);//数据发送函数指针
  void (*Delayms)(volatile uint32_t nTime);     //延时操作指针
}Esp8266ObjectObject;

/*定义ESP8266数据发送指针类型*/
typedef void (*ESP8266SendDataType)(uint8_t *sData,uint16_t sSize);
/*延时操作指针*/
typedef void (*ESP8266DelaymsType)(volatile uint32_t nTime);     

/*ESP8266对象初始化*/
void Esp8266Initialization(Esp8266ObjectObject *esp,    //ESP8266对象
                           Esp8266CWModeType cwMode,    //WIFI模式
                           Esp8266CIPModeType cipMode,  //传输模式，正常或透传
                           char *wifiName,              //WIFI名称
                           char *wifiPassword,          //WIFI密码
                           ESP8266SendDataType send,    //发送函数指针
                           ESP8266DelaymsType delayms   //毫秒延时函数
                           );

/*ESP8266发送数据*/
void Esp8266SendData(Esp8266ObjectObject *esp,uint8_t *sData,uint16_t sSize);

/*检查模块的连接是否正常*/
Esp8266CIPStatusType CheckForConnection(Esp8266ObjectObject *esp,uint16_t timeOut);

#endif
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
