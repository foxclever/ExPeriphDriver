/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：esp8266function.c                                              **/
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

#include "stddef.h"
#include <stdio.h>
#include "string.h"
#include "esp8266function.h"

/*定义WIFI模式配置指令*/
char cwModeCmd[3][17]={"AT+CWMODE_CUR=1\r\n",
                       "AT+CWMODE_CUR=2\r\n",
                       "AT+CWMODE_CUR=3\r\n"};


/*检查数据时是否接受完成*/
static Esp8266RxStatusType ChecRecieveFinished(Esp8266ObjectObject *esp);
/*清除接收缓冲区*/
static void ClearReciveBuffer(Esp8266ObjectObject *esp);
/*ESP8266发送命令*/
static Esp8266TxStatusType Esp8266SendCommmand(Esp8266ObjectObject *esp,char *cmd,char *ack,uint16_t timeOut);
/*ESP8266发送数据*/
//static void Esp8266SendData(Esp8266ObjectObject *esp,uint8_t *sData,uint16_t sSize);
/*ESP8266模块进入透传模式*/
static Esp8266TxStatusType Esp8266EnterTrans(Esp8266ObjectObject *esp);
/*ESP8266模块进入透传模式*/
static Esp8266TxStatusType Esp8266EnterTrans(Esp8266ObjectObject *esp);

/*ESP8266对象初始化*/
void Esp8266Initialization(Esp8266ObjectObject *esp,    //ESP8266对象
                           Esp8266CWModeType cwMode,    //WIFI模式
                           Esp8266CIPModeType cipMode,  //传输模式，正常或透传
                           char *wifiName,              //WIFI名称
                           char *wifiPassword,          //WIFI密码
                           ESP8266SendDataType send,    //发送函数指针
                           ESP8266DelaymsType delayms   //毫秒延时函数
                           )
{
  char cwjap[50];
  char cwsap[50];
  
  if((esp==NULL)||(send==NULL)||(delayms==NULL))
  {
    return;
  }
  esp->SendData=send;
  esp->Delayms=delayms;
  
  esp->cwMode=cwMode;
  esp->cipMode=cipMode;
  
  esp->rxBuffer.lengthRecieved=0;
  ClearReciveBuffer(esp);
  
  //设置工作模式 1：station模式   2：AP模式  3：兼容 AP+station模式
  if(Esp8266SendCommmand(esp,cwModeCmd[esp->cwMode],"OK",50)==Esp8266_TxFial)
  {
    return;
  }
  
  //让Wifi模块重启的命令
  if(Esp8266SendCommmand(esp,"AT+RST","OK",20)==Esp8266_TxFial)
  {
    return;
  }
  
  esp->Delayms(3000);         //延时3S等待重启成功
  
  if(esp->cwMode==Esp8266_StationMode)
  {
    sprintf(cwjap,"AT+CWJAP_CUR=\"%s\",\"%s\"\r\n",wifiName,wifiPassword);
    
    //让模块连接上自己的路由
    if(Esp8266SendCommmand(esp,cwjap,"OK",600)==Esp8266_TxFial)
    {
      return;
    }
    
    if(esp->cipMode==Esp8266_TransMode)
    {
      if(Esp8266EnterTrans(esp)==Esp8266_TxFial)
      {
        return;
      }
    }
    else
    {
      //=0：单路连接模式     =1：多路连接模式
      if(Esp8266SendCommmand(esp,"AT+CIPMUX=0\r\n","OK",20)==Esp8266_TxFial)
      {
        return;
      }
    }
  }
  else if(esp->cwMode==Esp8266_SoftAPMode)
  {
    sprintf(cwsap,"AT+CWSAP_CUR=\"%s\",\"%s\"\r\n",wifiName,wifiPassword);
    
    //设置模块的WIFI名和密码
    if(Esp8266SendCommmand(esp,cwsap,"OK",600)==Esp8266_TxFial)
    {
      return;
    }
  }
  else if(esp->cwMode==Esp8266_MixedMode)
  {
    //尚未使用，有待添加
  }
}

/*ESP8266模块进入透传模式*/
static Esp8266TxStatusType Esp8266EnterTrans(Esp8266ObjectObject *esp)
{
  Esp8266TxStatusType status=Esp8266_RxSucceed;
  
  //=0：单路连接模式     =1：多路连接模式
  status=Esp8266SendCommmand(esp,"AT+CIPMUX=0\r\n","OK",20);
  if(status==Esp8266_TxFial)
  {
    return status;
  }
	
  //建立TCP连接  这四项分别代表了 要连接的ID号0~4   连接类型  远程服务器IP地址   远程服务器端口号
//  while(Esp8266SendCommmand(esp,"AT+CIPSTART=\"TCP\",\"xxx.xxx.xxx.xxx\",xxxx","CONNECT",200));
	
  //是否开启透传模式  0：表示关闭 1：表示开启透传
  status=Esp8266SendCommmand(esp,"AT+CIPMODE=1\r\n","OK",200);
  if(status==Esp8266_TxFial)
  {
    return status;
  }
	
  //透传模式下 开始发送数据的指令 这个指令之后就可以直接发数据了
  status=Esp8266SendCommmand(esp,"AT+CIPSEND\r\n","OK",50);
  
  return status;
}

/*ESP8266退出透传模式   返回值:0,退出成功;1,退出失败*/
static Esp8266TxStatusType Esp8266QuitTrans(Esp8266ObjectObject *esp)
{
  Esp8266TxStatusType status=Esp8266_RxSucceed;
  
  status=Esp8266SendCommmand(esp,"+++","OK",20);
  
  if(status==Esp8266_TxFial)
  {
    return status;
  }
  
  esp->Delayms(1000);
  
  status=Esp8266SendCommmand(esp,"AT\r\n","OK",20);
  
  return status;
}


/*检查模块设备是否存在*/
void CheckForEquipment(Esp8266ObjectObject *esp)
{
  
}

/*检查模块的连接是否正常*/
Esp8266CIPStatusType CheckForConnection(Esp8266ObjectObject *esp,uint16_t timeOut)
{
  Esp8266CIPStatusType status = Esp8266_Error;

  if(esp->cipMode==Esp8266_TransMode)
  {
    Esp8266QuitTrans(esp);
  }

  ClearReciveBuffer(esp);
  esp->SendData((unsigned char *)"AT+CIPSTATUS\r\n",  14);	
	
  while(--timeOut)
  {
    if(ChecRecieveFinished(esp) == Esp8266_RxFinish)
    {
      if(strstr((const char *)esp->rxBuffer.queue, "STATUS:2"))       //获得IP
      {
        status = Esp8266_GotIP;
      }
      else if(strstr((const char *)esp->rxBuffer.queue, "STATUS:3"))  //建立连接
      {
        status = Esp8266_Connected;
      }
      else if(strstr((const char *)esp->rxBuffer.queue, "STATUS:4"))  //失去连接
      {
        status = Esp8266_Disconnect;
      }
      else if(strstr((const char *)esp->rxBuffer.queue, "STATUS:5"))  //物理掉线
      {
        status = Esp8266_NoWifi;
      }

      break;
    }
		
    esp->Delayms(10);
  }
	
  if(timeOut == 0)
  {
    status = Esp8266_Timeout;
  }
	
  return status;
}

/*ESP8266发送命令*/
static Esp8266TxStatusType Esp8266SendCommmand(Esp8266ObjectObject *esp,char *cmd,char *ack,uint16_t timeOut)
{
  esp->SendData((unsigned char *)cmd, strlen((const char *)cmd));	//写命令到网络设备
  
  if(ack&&timeOut)
  {
    while(timeOut--)      //等待超时
    {
      if(ChecRecieveFinished(esp) == Esp8266_RxFinish)    //如果数据接收完成
      {
        if(strstr((const char *)esp->rxBuffer.queue,ack) != NULL)	//如果检索到关键词
        {
          ClearReciveBuffer(esp);
				
          return Esp8266_RxSucceed;
        }
      }
		
      esp->Delayms(10);
    }
  }

  return Esp8266_TxFial;
}

/*ESP8266发送数据*/
void Esp8266SendData(Esp8266ObjectObject *esp,uint8_t *sData,uint16_t sSize)
{
  if(esp->cipMode==Esp8266_TransMode)
  {
    esp->SendData(sData,sSize);
  }
  else
  {
    char cmd[32];

    esp->Delayms(50);
    ClearReciveBuffer(esp);
    sprintf(cmd,"AT+CIPSEND=%d\r\n",sSize);
    
    if(Esp8266SendCommmand(esp,cmd, ">", 1)==Esp8266_RxSucceed)     //收到‘>’时可以发送数据
    {
      esp->SendData(sData,sSize);
    }
  }
}

/*检查数据时是否接受完成*/
static Esp8266RxStatusType ChecRecieveFinished(Esp8266ObjectObject *esp)
{
  //尚未开始接收数据
  if(esp->rxBuffer.lengthRecieving==0)
  {
    return Esp8266_RxNone;
  }
  
  //完成接收数据
  if(esp->rxBuffer.lengthRecieving==esp->rxBuffer.lengthRecieved)
  {
    esp->rxBuffer.lengthRecieving=0;
    
    return Esp8266_RxFinish;
  }
  
  //正在接收数据
  esp->rxBuffer.lengthRecieved=esp->rxBuffer.lengthRecieving;
  
  return Esp8266_RxWait;
}

/*清除接收缓冲区*/
static void ClearReciveBuffer(Esp8266ObjectObject *esp)
{
  esp->rxBuffer.lengthRecieving=0;
  
  memset(esp->rxBuffer.queue,0,sizeof(esp->rxBuffer.queue));
}

/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
