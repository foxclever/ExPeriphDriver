/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ndirfunction.c                                                 **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：实现舒茨非分光红外气体检测模块通讯相关的函数和操作             **/
/**           舒茨Schutz S-MODULE非分光红外气体模块通讯：                    **/
/**           ★ Baud rate	传输率:		2400		             **/
/**           ★ Data bits	数据位:		7		             **/
/**           ★ Stop bits	停止位:		1		             **/
/**           ★ Parity		奇偶性:		Even/偶                      **/
/**           ★ Timeout	超时时间:	1000ms		             **/
/**           ★ Retries	重复次数:	3	                     **/
/**           舒茨Schutz S-MODULE非分光红外气体模块Modbus参数：              **/
/**           0x00C0		Modbus_address	模块当前Modbus地址  可读写   **/
/**           0x0080到0x0083	DeviceType      设备的型号          可读写   **/
/**           0x0084到0x0085	SoftwareVersion 软件版本            只读     **/
/**           0x0086到0x0089	Internal device number  设备序列号  只读     **/
/**           0x0005		MOD	气体浓度设置值	            只读     **/
/**           0x000A		Concentration    气体浓度           只读     **/
/**           0x0003		T_module (0.1x°C)   传感器内部温度 只读     **/
/**           0x0045		Alarm_Level     主报警限值          可读写   **/
/**           0x0044		Warn_Level      预报警限值          可读写   **/
/**           0x0047		IR_4tagneu      零点的强度测量      可读写   **/
/**           0x0009		Statusflags     状态信息            只读     **/
/**                 Flag 0 Testflag 值“1”表示仪器处于测试状态              **/
/**                 Flag 1 Warmup 值“1”表示启动后大约10秒预热              **/
/**                 Flag 2 Syserr 值“1”表示系统错误                        **/
/**                 Flag 3 Alarm 值“1”表示气体主报警开始工作               **/
/**                 Flag 4 Warn 值“1”表示气体预报警开始工作                **/
/**                 Flag 5 Startup 值“1”表示处于启动阶段（至少90秒）       **/
/**                 Flag 6 Korr 值“1”表示S-MODULE提供温度补偿功能          **/
/**                 Flag 7 mw_ok 值“1”表示零点已设定                       **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2015-07-31          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/  

#include "stddef.h"
#include "ndirfunction.h"

/*将读到的数据进行处理，转化为16进制数据,并返回*/
static uint16_t ParsingChartoHex(uint8_t *rxData,uint16_t length,uint8_t *retData);
/*解析接收到的参数的类型*/
static NdirParameterType ParsingParameterType(uint8_t typeBytes[]);
/*发送数据给舒茨非分光红外气体检测模块（读写数据）*/
static void NDIRSendData(NdirObjectType *ndir,uint8_t *txData,uint16_t length);
/*计算机校验和*/
static uint8_t CheckSumCalc(uint8_t *txData,uint16_t length);
/*将16进制数转化为字符ASCII码*/
static uint8_t ConvertHexToChar(uint8_t hexNumber);
/*将2个ASCII码字符数转化为一个16进制数字*/
static uint8_t ConvertCharToHex(uint8_t charNumber);
/*将16进制数组转化为ASCII码字符数组*/
static uint16_t ConvertHexArrayToASCIICharArray(uint8_t *hexArray,uint16_t length,uint8_t *asciiCharArray);
/*将ASCII字符数组转化为16进制字节数组*/
static uint16_t ConvertAsciiCharArrayToHexArray(uint8_t *asciiCharArray,uint16_t cLength,uint8_t *hexArray);

/*读软件版本*/
uint32_t ReadNDIRSoftVersion(NdirObjectType *ndir,uint8_t *receiveDataBuffer)
{
  uint8_t txData[6];
  txData[0]=ndir->moduleAddress;
  txData[1]=ReadRegisterFC;
  txData[2]=0x00;//起始地址高位
  txData[3]=0x84;//起始地址低位
  txData[4]=0x00;//寄存器数量高位
  txData[5]=0x02;//寄存器数量低位
  
  NDIRSendData(ndir,txData,6);
  ndir->Delayms(100);                 //延时100毫秒等待处理响应
  NdirParseReceiveData(ndir,receiveDataBuffer);
  
  return ndir->softVersion;
}

/*从舒茨非分光红外气体检测模块读取浓度值*/
float ReadConcentrationData(NdirObjectType *ndir,uint8_t * receiveDataBuffer)
{
  uint8_t txData[6];
  txData[0]=ndir->moduleAddress;
  txData[1]=ReadRegisterFC;
  txData[2]=0x00;//起始地址高位
  txData[3]=0x0A;//起始地址低位
  txData[4]=0x00;//寄存器数量高位
  txData[5]=0x01;//寄存器数量低位
  
  NDIRSendData(ndir,txData,6);
  ndir->Delayms(100);                 //延时100毫秒等待处理响应
  NdirParseReceiveData(ndir,receiveDataBuffer);
  
  return ndir->concentration;
}

/*从舒茨非分光红外气体检测模块读取内部温度值*/
float ReadTemperatureData(NdirObjectType *ndir,uint8_t * receiveDataBuffer)
{
  uint8_t txData[6];
  txData[0]=ndir->moduleAddress;
  txData[1]=ReadRegisterFC;
  txData[2]=0x00;//起始地址高位
  txData[3]=0x03;//起始地址低位
  txData[4]=0x00;//寄存器数量高位
  txData[5]=0x01;//寄存器数量低位
  
  NDIRSendData(ndir,txData,6);
  ndir->Delayms(100);                 //延时100毫秒等待处理响应
  NdirParseReceiveData(ndir,receiveDataBuffer);
  
  return ndir->temperature;
}

/*从舒茨非分光红外气体检测模块读取状态标志*/
uint16_t ReadNDIRStatusflags(NdirObjectType *ndir,uint8_t * receiveDataBuffer)
{
  uint8_t txData[6];
  txData[0]=ndir->moduleAddress;
  txData[1]=ReadRegisterFC;
  txData[2]=0x00;//起始地址高位
  txData[3]=0x09;//起始地址低位
  txData[4]=0x00;//寄存器数量高位
  txData[5]=0x01;//寄存器数量低位
  
  NDIRSendData(ndir,txData,6);
  ndir->Delayms(100);                 //延时100毫秒等待处理响应
  NdirParseReceiveData(ndir,receiveDataBuffer);

  return ndir->status;
}

/*NDIR初始化配置函数*/
void NdirInitialization(NdirObjectType *ndir,   //NDIR对象
                        uint8_t moduleAddress,  //模块地址
                        NdirSendByteType send,  //发送数据操作函数
                        NdirDelaymsType delayms //毫秒延时操作函数
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

/*数据解析，用以解析接收到的数据信息*/
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
    /*版本信息解析*/
    ndir->softVersion=(hexData[4]<<24)+(hexData[5]<<16)+(hexData[6]<<8)+hexData[7];
  }
  else if((pType==Concentration)&&(hLength>=7))
  {
    /*浓度值数据、温度值数据、状态信息解析*/
    ndir->concentration=((float)((hexData[4]<<8)+hexData[5]))*0.01;
  }
  else if((pType==Temperature)&&(hLength>=7))
  {
    /*浓度值数据、温度值数据、状态信息解析*/
    ndir->temperature=((float)((hexData[4]<<8)+hexData[5]))*0.1;
  }
  else if((pType==StatusFlags)&&(hLength>=7))
  {
    /*浓度值数据、温度值数据、状态信息解析*/
    ndir->status=(hexData[4]<<8)+hexData[5];
  }
}

/*将读到的数据进行处理，转化为16进制数据,并返回*/
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

/*解析接收到的参数的类型*/
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

/*发送数据给舒茨非分光红外气体检测模块（读写数据）*/
static void NDIRSendData(NdirObjectType *ndir,uint8_t *txData,uint16_t length)
{
  uint16_t sendDataAmount=0;
  uint8_t sendDataArray[17]; //不小于(length+1)*2+3
  uint8_t rawData[7];//含校验码
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
    /*发送一个字节*/
    ndir->SendByte(sendDataArray[sendDataIndex]);
  }
}

/*计算机校验和*/
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

/*将16进制数转化为字符ASCII码*/
static uint8_t ConvertHexToChar(uint8_t hexNumber)
{
  uint8_t result=0;
  result=((0xA<=hexNumber)&&(hexNumber<=0xF))?(hexNumber+0x37):(hexNumber+0x30);
/*  if((0xA<=hexNumber)&&(hexNumber<=0xF))
  {
    result=hexNumber+0x37;      //A-F转换
  }
  else
  {
    result=hexNumber+0x30;      //0-9转换
  }*/
  return result;
}

/*将2个ASCII码字符数转化为一个16进制数字*/
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

/*将16进制数组转化为ASCII码字符数组*/
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

/*将ASCII字符数组转化为16进制字节数组*/
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
