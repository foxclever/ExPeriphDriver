/******************************************************************************/
/** ģ�����ƣ���չ��������                                                   **/
/** �ļ����ƣ�ndirfunction.c                                                 **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺ʵ����ķǷֹ����������ģ��ͨѶ��صĺ����Ͳ���             **/
/**           ���Schutz S-MODULE�Ƿֹ��������ģ��ͨѶ��                    **/
/**           �� Baud rate	������:		2400		                         **/
/**           �� Data bits	����λ:		7		                             **/
/**           �� Stop bits	ֹͣλ:		1		                             **/
/**           �� Parity		��ż��:		Even/ż                              **/
/**           �� Timeout	��ʱʱ��:	1000ms		                         **/
/**           �� Retries	�ظ�����:	3	                                 **/
/**           ���Schutz S-MODULE�Ƿֹ��������ģ��Modbus������              **/
/**           0x00C0		Modbus_address	ģ�鵱ǰModbus��ַ  �ɶ�д       **/
/**           0x0080��0x0083	DeviceType      �豸���ͺ�          �ɶ�д   **/
/**           0x0084��0x0085	SoftwareVersion ����汾            ֻ��     **/
/**           0x0086��0x0089	Internal device number  �豸���к�  ֻ��     **/
/**           0x0005		MOD	����Ũ������ֵ	            ֻ��             **/
/**           0x000A		Concentration    ����Ũ��           ֻ��         **/
/**           0x0003		T_module (0.1x��C)   �������ڲ��¶� ֻ��         **/
/**           0x0045		Alarm_Level     ��������ֵ          �ɶ�д       **/
/**           0x0044		Warn_Level      Ԥ������ֵ          �ɶ�д       **/
/**           0x0047		IR_4tagneu      ����ǿ�Ȳ���      �ɶ�д       **/
/**           0x0009		Statusflags     ״̬��Ϣ            ֻ��         **/
/**                 Flag 0 Testflag ֵ��1����ʾ�������ڲ���״̬              **/
/**                 Flag 1 Warmup ֵ��1����ʾ�������Լ10��Ԥ��              **/
/**                 Flag 2 Syserr ֵ��1����ʾϵͳ����                        **/
/**                 Flag 3 Alarm ֵ��1����ʾ������������ʼ����               **/
/**                 Flag 4 Warn ֵ��1����ʾ����Ԥ������ʼ����                **/
/**                 Flag 5 Startup ֵ��1����ʾ���������׶Σ�����90�룩       **/
/**                 Flag 6 Korr ֵ��1����ʾS-MODULE�ṩ�¶Ȳ�������          **/
/**                 Flag 7 mw_ok ֵ��1����ʾ������趨                       **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2015-07-31          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/  

#include "stddef.h"
#include "ndirfunction.h"

/*�����������ݽ��д���ת��Ϊ16��������,������*/
static uint16_t ParsingChartoHex(uint8_t *rxData,uint16_t length,uint8_t *retData);
/*�������յ��Ĳ���������*/
static NdirParameterType ParsingParameterType(uint8_t typeBytes[]);
/*�������ݸ���ķǷֹ����������ģ�飨��д���ݣ�*/
static void NDIRSendData(NdirObjectType *ndir,uint8_t *txData,uint16_t length);
/*�����У���*/
static uint8_t CheckSumCalc(uint8_t *txData,uint16_t length);
/*��16������ת��Ϊ�ַ�ASCII��*/
static uint8_t ConvertHexToChar(uint8_t hexNumber);
/*��2��ASCII���ַ���ת��Ϊһ��16��������*/
static uint8_t ConvertCharToHex(uint8_t charNumber);
/*��16��������ת��ΪASCII���ַ�����*/
static uint16_t ConvertHexArrayToASCIICharArray(uint8_t *hexArray,uint16_t length,uint8_t *asciiCharArray);
/*��ASCII�ַ�����ת��Ϊ16�����ֽ�����*/
static uint16_t ConvertAsciiCharArrayToHexArray(uint8_t *asciiCharArray,uint16_t cLength,uint8_t *hexArray);

/*������汾*/
uint32_t ReadNDIRSoftVersion(NdirObjectType *ndir,uint8_t *receiveDataBuffer)
{
    uint8_t txData[6];
    txData[0]=ndir->moduleAddress;
    txData[1]=ReadRegisterFC;
    txData[2]=0x00;//��ʼ��ַ��λ
    txData[3]=0x84;//��ʼ��ַ��λ
    txData[4]=0x00;//�Ĵ���������λ
    txData[5]=0x02;//�Ĵ���������λ
    
    NDIRSendData(ndir,txData,6);
    ndir->Delayms(100);                 //��ʱ100����ȴ�������Ӧ
    NdirParseReceiveData(ndir,receiveDataBuffer);
    
    return ndir->softVersion;
}

/*����ķǷֹ����������ģ���ȡŨ��ֵ*/
float ReadConcentrationData(NdirObjectType *ndir,uint8_t * receiveDataBuffer)
{
    uint8_t txData[6];
    txData[0]=ndir->moduleAddress;
    txData[1]=ReadRegisterFC;
    txData[2]=0x00;//��ʼ��ַ��λ
    txData[3]=0x0A;//��ʼ��ַ��λ
    txData[4]=0x00;//�Ĵ���������λ
    txData[5]=0x01;//�Ĵ���������λ
    
    NDIRSendData(ndir,txData,6);
    ndir->Delayms(100);                 //��ʱ100����ȴ�������Ӧ
    NdirParseReceiveData(ndir,receiveDataBuffer);
    
    return ndir->concentration;
}

/*����ķǷֹ����������ģ���ȡ�ڲ��¶�ֵ*/
float ReadTemperatureData(NdirObjectType *ndir,uint8_t * receiveDataBuffer)
{
    uint8_t txData[6];
    txData[0]=ndir->moduleAddress;
    txData[1]=ReadRegisterFC;
    txData[2]=0x00;//��ʼ��ַ��λ
    txData[3]=0x03;//��ʼ��ַ��λ
    txData[4]=0x00;//�Ĵ���������λ
    txData[5]=0x01;//�Ĵ���������λ
    
    NDIRSendData(ndir,txData,6);
    ndir->Delayms(100);                 //��ʱ100����ȴ�������Ӧ
    NdirParseReceiveData(ndir,receiveDataBuffer);
    
    return ndir->temperature;
}

/*����ķǷֹ����������ģ���ȡ״̬��־*/
uint16_t ReadNDIRStatusflags(NdirObjectType *ndir,uint8_t * receiveDataBuffer)
{
    uint8_t txData[6];
    txData[0]=ndir->moduleAddress;
    txData[1]=ReadRegisterFC;
    txData[2]=0x00;//��ʼ��ַ��λ
    txData[3]=0x09;//��ʼ��ַ��λ
    txData[4]=0x00;//�Ĵ���������λ
    txData[5]=0x01;//�Ĵ���������λ
    
    NDIRSendData(ndir,txData,6);
    ndir->Delayms(100);                 //��ʱ100����ȴ�������Ӧ
    NdirParseReceiveData(ndir,receiveDataBuffer);
    
    return ndir->status;
}

/*NDIR��ʼ�����ú���*/
void NdirInitialization(NdirObjectType *ndir,   //NDIR����
                        uint8_t moduleAddress,  //ģ���ַ
                        NdirSendByteType send,  //�������ݲ�������
                        NdirDelaymsType delayms //������ʱ��������
                            )
{
    if((ndir==NULL)||(send==NULL)||(delayms==NULL))
    {
        return;
    }
    ndir->SendByte=send;
    ndir->Delayms=delayms;
    
    ndir->moduleAddress=moduleAddress;
    
    ndir->concentration=0.0;
    ndir->temperature=0.0;
    
    ndir->status=0;
    ndir->softVersion=0;
}

/*���ݽ��������Խ������յ���������Ϣ*/
void NdirParseReceiveData(NdirObjectType *ndir,uint8_t * receiveDataBuffer)
{
    uint16_t retLength=19;
    uint8_t returnData[19];
    for(int i=0;i<retLength;i++)
    {
        returnData[i]=receiveDataBuffer[i+17];
    }
    
    do
    {
        retLength--;
    }while((retLength>0)&&((returnData[retLength]!=0x0A)||(returnData[retLength-1]!=0x0D)));
    
    if(retLength>0)
    {
        retLength--;
    }
    
    uint8_t hexData[19]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    uint16_t hLength;
    if(returnData[0]==':')
    {
        hLength=ParsingChartoHex(returnData,retLength,hexData);
    }
    
    uint8_t typeBytes[4]={receiveDataBuffer[5],receiveDataBuffer[6],receiveDataBuffer[7],receiveDataBuffer[8]};
    NdirParameterType pType=ParsingParameterType(typeBytes);
    
    if((pType==SoftVersion)&&(hLength>=9))
    {
        /*�汾��Ϣ����*/
        ndir->softVersion=(hexData[4]<<24)+(hexData[5]<<16)+(hexData[6]<<8)+hexData[7];
    }
    else if((pType==Concentration)&&(hLength>=7))
    {
        /*Ũ��ֵ���ݡ��¶�ֵ���ݡ�״̬��Ϣ����*/
        ndir->concentration=((float)((hexData[4]<<8)+hexData[5]))*0.01;
    }
    else if((pType==Temperature)&&(hLength>=7))
    {
        /*Ũ��ֵ���ݡ��¶�ֵ���ݡ�״̬��Ϣ����*/
        ndir->temperature=((float)((hexData[4]<<8)+hexData[5]))*0.1;
    }
    else if((pType==StatusFlags)&&(hLength>=7))
    {
        /*Ũ��ֵ���ݡ��¶�ֵ���ݡ�״̬��Ϣ����*/
        ndir->status=(hexData[4]<<8)+hexData[5];
    }
}

/*�����������ݽ��д���ת��Ϊ16��������,������*/
static uint16_t ParsingChartoHex(uint8_t *rxData,uint16_t length,uint8_t *retData)
{
    uint16_t hLength=0;
    retData[hLength++]=rxData[0];
    uint8_t asciiCharArray[30];
    for(int i=0;i<length-1;i++)
    {
        asciiCharArray[i]=rxData[i+1];
    }
    
    uint8_t hexArray[15];
    ConvertAsciiCharArrayToHexArray(asciiCharArray,length-1,hexArray);
    for(int i=0;i<(length-1)/2;i++)
    {
        retData[hLength++]=hexArray[i];
    }
    return hLength;
}

/*�������յ��Ĳ���������*/
static NdirParameterType ParsingParameterType(uint8_t typeBytes[])
{
    uint8_t hexArray[4];
    for(int i=0;i<4;i++)
    {
        hexArray[i]=ConvertCharToHex(typeBytes[i]);
    }
    uint16_t hiByte=(hexArray[0]<<4)+hexArray[1];
    uint16_t loByte=(hexArray[2]<<4)+hexArray[3];
    return (NdirParameterType)((hiByte<<8)+loByte);
}

/*�������ݸ���ķǷֹ����������ģ�飨��д���ݣ�*/
static void NDIRSendData(NdirObjectType *ndir,uint8_t *txData,uint16_t length)
{
    uint16_t sendDataAmount=0;
    uint8_t sendDataArray[17]; //��С��(length+1)*2+3
    uint8_t rawData[7];//��У����
    for(int i=0;i<length;i++)
    {
        rawData[i]=txData[i];
    }
    rawData[6]=CheckSumCalc(txData,length);
    uint8_t objData[14];
    uint16_t cLength=ConvertHexArrayToASCIICharArray(rawData,length+1,objData);
    
    sendDataArray[sendDataAmount++]=':';
    for(int i=0;i<cLength;i++)
    {
        sendDataArray[sendDataAmount++]=objData[i];
    }
    sendDataArray[sendDataAmount++]=0x0D;
    sendDataArray[sendDataAmount++]=0x0A;
    
    for(uint16_t sendDataIndex=0;sendDataIndex<sendDataAmount;sendDataIndex++)
    {
        /*����һ���ֽ�*/
        ndir->SendByte(sendDataArray[sendDataIndex]);
    }
}

/*�����У���*/
static uint8_t CheckSumCalc(uint8_t *txData,uint16_t length)
{
    uint8_t checkSum=0x00;
    uint8_t highChar,lowChar;
    for(int i=0;i<length;i++)
    {
        lowChar=txData[i] & 0x0F;
        highChar=txData[i]>>4;
        checkSum=checkSum+ConvertHexToChar(highChar);
        checkSum=checkSum+ConvertHexToChar(lowChar);
    }
    
    checkSum=0xFF-checkSum+0x01;
    return checkSum;
}

/*��16������ת��Ϊ�ַ�ASCII��*/
static uint8_t ConvertHexToChar(uint8_t hexNumber)
{
    uint8_t result=0;
    result=((0xA<=hexNumber)&&(hexNumber<=0xF))?(hexNumber+0x37):(hexNumber+0x30);
    /*  if((0xA<=hexNumber)&&(hexNumber<=0xF))
    {
    result=hexNumber+0x37;      //A-Fת��
}
  else
    {
    result=hexNumber+0x30;      //0-9ת��
}*/
    return result;
}

/*��2��ASCII���ַ���ת��Ϊһ��16��������*/
static uint8_t ConvertCharToHex(uint8_t charNumber)
{
    uint8_t hexNumber=0;
    //hexNumber=((0x30<=charNumber)&&(charNumber<=0x39))?(charNumber-0x30):(((0x41<=charNumber)&&(charNumber<=0x46))?(charNumber-0x37):(0xFF));
    if((0x30<=charNumber)&&(charNumber<=0x39))
    {
        hexNumber=charNumber-0x30;
    }
    else 
    {if((0x41<=charNumber)&&(charNumber<=0x46))
    {
        hexNumber=charNumber-0x37;
    }
    else
    {
        hexNumber=0xFF;
    }
    }
    return hexNumber;
}

/*��16��������ת��ΪASCII���ַ�����*/
static uint16_t ConvertHexArrayToASCIICharArray(uint8_t *hexArray,uint16_t length,uint8_t *asciiCharArray)
{
    uint8_t highChar,lowChar;
    uint16_t charLength=0;
    for(int i=0;i<length;i++)
    {
        lowChar=hexArray[i] & 0x0F;
        highChar=hexArray[i]>>4;
        asciiCharArray[charLength++]=ConvertHexToChar(highChar);
        asciiCharArray[charLength++]=ConvertHexToChar(lowChar);
    }
    
    return charLength;
}

/*��ASCII�ַ�����ת��Ϊ16�����ֽ�����*/
static uint16_t ConvertAsciiCharArrayToHexArray(uint8_t *asciiCharArray,uint16_t cLength,uint8_t *hexArray)
{
    uint16_t hLength=0;
    uint8_t highChar,lowChar;
    for(int i=0;i<cLength;i=i+2)
    {
        highChar=ConvertCharToHex(asciiCharArray[i]);
        lowChar=ConvertCharToHex(asciiCharArray[i+1]);
        hexArray[hLength++]=(highChar<<4)+lowChar;
    }
    return hLength;
}

/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
