/******************************************************************************/
/** ģ�����ƣ���չ��������                                                   **/
/** �ļ����ƣ�esp8266function.c                                              **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʵ��ESP8266����ͨѶ�Ĳ�����ESP8266EX֧��TCP/IPЭ�飬��ȫ   **/
/**           ��ѭ802.11 b/g/n/e/i WLAN MAC Э��� Wi-Fi Direct ��׼������   **/
/**           USART�ӿڣ�����ͨ��ATָ��ʵ�����ݽ�����                        **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2017-07-17          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include <stdio.h>
#include "string.h"
#include "esp8266function.h"

/*����WIFIģʽ����ָ��*/
char cwModeCmd[3][17]={"AT+CWMODE_CUR=1\r\n",
"AT+CWMODE_CUR=2\r\n",
"AT+CWMODE_CUR=3\r\n"};


/*�������ʱ�Ƿ�������*/
static Esp8266RxStatusType ChecRecieveFinished(Esp8266ObjectObject *esp);
/*������ջ�����*/
static void ClearReciveBuffer(Esp8266ObjectObject *esp);
/*ESP8266��������*/
static Esp8266TxStatusType Esp8266SendCommmand(Esp8266ObjectObject *esp,char *cmd,char *ack,uint16_t timeOut);
/*ESP8266��������*/
//static void Esp8266SendData(Esp8266ObjectObject *esp,uint8_t *sData,uint16_t sSize);
/*ESP8266ģ�����͸��ģʽ*/
static Esp8266TxStatusType Esp8266EnterTrans(Esp8266ObjectObject *esp);
/*ESP8266ģ�����͸��ģʽ*/
static Esp8266TxStatusType Esp8266EnterTrans(Esp8266ObjectObject *esp);

/*ESP8266�����ʼ��*/
void Esp8266Initialization(Esp8266ObjectObject *esp,    //ESP8266����
                           Esp8266CWModeType cwMode,    //WIFIģʽ
                           Esp8266CIPModeType cipMode,  //����ģʽ��������͸��
                           char *wifiName,              //WIFI����
                           char *wifiPassword,          //WIFI����
                           ESP8266SendDataType send,    //���ͺ���ָ��
                           ESP8266DelaymsType delayms   //������ʱ����
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
    
    //���ù���ģʽ 1��stationģʽ   2��APģʽ  3������ AP+stationģʽ
    if(Esp8266SendCommmand(esp,cwModeCmd[esp->cwMode],"OK",50)==Esp8266_TxFial)
    {
        return;
    }
    
    //��Wifiģ������������
    if(Esp8266SendCommmand(esp,"AT+RST","OK",20)==Esp8266_TxFial)
    {
        return;
    }
    
    esp->Delayms(3000);         //��ʱ3S�ȴ������ɹ�
    
    if(esp->cwMode==Esp8266_StationMode)
    {
        sprintf(cwjap,"AT+CWJAP_CUR=\"%s\",\"%s\"\r\n",wifiName,wifiPassword);
        
        //��ģ���������Լ���·��
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
            //=0����·����ģʽ     =1����·����ģʽ
            if(Esp8266SendCommmand(esp,"AT+CIPMUX=0\r\n","OK",20)==Esp8266_TxFial)
            {
                return;
            }
        }
    }
    else if(esp->cwMode==Esp8266_SoftAPMode)
    {
        sprintf(cwsap,"AT+CWSAP_CUR=\"%s\",\"%s\"\r\n",wifiName,wifiPassword);
        
        //����ģ���WIFI��������
        if(Esp8266SendCommmand(esp,cwsap,"OK",600)==Esp8266_TxFial)
        {
            return;
        }
    }
    else if(esp->cwMode==Esp8266_MixedMode)
    {
        //��δʹ�ã��д����
    }
}

/*ESP8266ģ�����͸��ģʽ*/
static Esp8266TxStatusType Esp8266EnterTrans(Esp8266ObjectObject *esp)
{
    Esp8266TxStatusType status=Esp8266_RxSucceed;
    
    //=0����·����ģʽ     =1����·����ģʽ
    status=Esp8266SendCommmand(esp,"AT+CIPMUX=0\r\n","OK",20);
    if(status==Esp8266_TxFial)
    {
        return status;
    }
    
    //����TCP����  ������ֱ������ Ҫ���ӵ�ID��0~4   ��������  Զ�̷�����IP��ַ   Զ�̷������˿ں�
    //  while(Esp8266SendCommmand(esp,"AT+CIPSTART=\"TCP\",\"xxx.xxx.xxx.xxx\",xxxx","CONNECT",200));
    
    //�Ƿ���͸��ģʽ  0����ʾ�ر� 1����ʾ����͸��
    status=Esp8266SendCommmand(esp,"AT+CIPMODE=1\r\n","OK",200);
    if(status==Esp8266_TxFial)
    {
        return status;
    }
    
    //͸��ģʽ�� ��ʼ�������ݵ�ָ�� ���ָ��֮��Ϳ���ֱ�ӷ�������
    status=Esp8266SendCommmand(esp,"AT+CIPSEND\r\n","OK",50);
    
    return status;
}

/*ESP8266�˳�͸��ģʽ   ����ֵ:0,�˳��ɹ�;1,�˳�ʧ��*/
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


/*���ģ���豸�Ƿ����*/
void CheckForEquipment(Esp8266ObjectObject *esp)
{
    
}

/*���ģ��������Ƿ�����*/
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
            if(strstr((const char *)esp->rxBuffer.queue, "STATUS:2"))       //���IP
            {
                status = Esp8266_GotIP;
            }
            else if(strstr((const char *)esp->rxBuffer.queue, "STATUS:3"))  //��������
            {
                status = Esp8266_Connected;
            }
            else if(strstr((const char *)esp->rxBuffer.queue, "STATUS:4"))  //ʧȥ����
            {
                status = Esp8266_Disconnect;
            }
            else if(strstr((const char *)esp->rxBuffer.queue, "STATUS:5"))  //�������
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

/*ESP8266��������*/
static Esp8266TxStatusType Esp8266SendCommmand(Esp8266ObjectObject *esp,char *cmd,char *ack,uint16_t timeOut)
{
    esp->SendData((unsigned char *)cmd, strlen((const char *)cmd));	//д��������豸
    
    if(ack&&timeOut)
    {
        while(timeOut--)      //�ȴ���ʱ
        {
            if(ChecRecieveFinished(esp) == Esp8266_RxFinish)    //������ݽ������
            {
                if(strstr((const char *)esp->rxBuffer.queue,ack) != NULL)	//����������ؼ���
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

/*ESP8266��������*/
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
        
        if(Esp8266SendCommmand(esp,cmd, ">", 1)==Esp8266_RxSucceed)     //�յ���>��ʱ���Է�������
        {
            esp->SendData(sData,sSize);
        }
    }
}

/*�������ʱ�Ƿ�������*/
static Esp8266RxStatusType ChecRecieveFinished(Esp8266ObjectObject *esp)
{
    //��δ��ʼ��������
    if(esp->rxBuffer.lengthRecieving==0)
    {
        return Esp8266_RxNone;
    }
    
    //��ɽ�������
    if(esp->rxBuffer.lengthRecieving==esp->rxBuffer.lengthRecieved)
    {
        esp->rxBuffer.lengthRecieving=0;
        
        return Esp8266_RxFinish;
    }
    
    //���ڽ�������
    esp->rxBuffer.lengthRecieved=esp->rxBuffer.lengthRecieving;
    
    return Esp8266_RxWait;
}

/*������ջ�����*/
static void ClearReciveBuffer(Esp8266ObjectObject *esp)
{
    esp->rxBuffer.lengthRecieving=0;
    
    memset(esp->rxBuffer.queue,0,sizeof(esp->rxBuffer.queue));
}

/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
