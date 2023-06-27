/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：aibusfunction.c                                                **/
/** 版    本：V1.1.0                                                         **/
/** 简    介：用于实现厦门宇电AIBUS通讯协议                                  **/
/**           AIBUS采用RS485电气接口                                         **/
/**           数据格式：1个起始位，8位数据，无校验位，1个或2个停止位         **/
/**           波特率可调为4800~19200bit/S，通常建议9600bit/S                 **/
/**           AIBUS通讯协议仅包括读写2条：                                   **/
/**           读：地址代号+52H（82）+要读的参数代号+0+0+                     **/
/**           写：地址代号+43H（67）+要写的参数代号+写入数低字节             **/
/**               +写入数高字节+校验码                                       **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-01-15          木南              创建文件               **/
/**     V1.1.0  2019-07-27          木南              修改为对象操作         **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "aibusfunction.h"

/*定义AIBUS的指令码*/
#define READ_INSTRUCTION 0x52
#define WRITE_INSTRUCTION 0x43

/*定义指令和数据长度*/
#define INSTRUCTION_LENGTH 8 //读写指令的长度均为8字节
#define RETURNING_DATA_LENGTH 10 //返回数据均为10个字节


/*读取目标设备的参数值*/
void ReadAiBusDeviceParameter(AIbusObjectType *aibus,uint8_t paraAddr)
{
    uint8_t readCommand[INSTRUCTION_LENGTH];
    uint16_t index=0;
    readCommand[index++]=0x80+aibus->deviceAddr;
    readCommand[index++]=0x80+aibus->deviceAddr;
    readCommand[index++]=READ_INSTRUCTION;
    readCommand[index++]=paraAddr;
    readCommand[index++]=0x0;
    readCommand[index++]=0x0;
    
    uint16_t checkSum=(uint16_t)paraAddr*256+READ_INSTRUCTION+(uint16_t)aibus->deviceAddr;
    
    readCommand[index++]=checkSum;
    readCommand[index++]=(checkSum>>8);
    
    aibus->SendBytes(readCommand,INSTRUCTION_LENGTH);
}

/*设置目标设备的参数值*/
void WriteAiBusDeviceParameter(AIbusObjectType *aibus,uint8_t paraAddr,uint16_t data)
{
    uint8_t writeCommand[INSTRUCTION_LENGTH];
    uint16_t index=0;
    writeCommand[index++]=0x80+aibus->deviceAddr;
    writeCommand[index++]=0x80+aibus->deviceAddr;
    writeCommand[index++]=WRITE_INSTRUCTION;
    writeCommand[index++]=paraAddr;
    writeCommand[index++]=data;
    writeCommand[index++]=(data>>8);
    
    uint16_t checkSum=(uint16_t)paraAddr*256+WRITE_INSTRUCTION+(uint16_t)aibus->deviceAddr+data;
    
    writeCommand[index++]=checkSum;
    writeCommand[index++]=(checkSum>>8);
    
    aibus->SendBytes(writeCommand,INSTRUCTION_LENGTH);
}

/*解析返回数据,返回值为读或者写的参数值*/
int ParsingReturnData(AIbusObjectType *aibus,uint8_t *receiveData,uint16_t *returnData,uint16_t deviceNum)
{
    int status=-1;
    
    uint16_t pValue=0;
    uint16_t sValue=0;
    uint16_t mValue=0;
    uint16_t alarmStatus=0;
    uint16_t paraValue=0;
    uint16_t checkSum=0;
    
    pValue=receiveData[0]+receiveData[1]*256;
    sValue=receiveData[2]+receiveData[3]*256;
    mValue=(uint16_t)receiveData[4];
    alarmStatus=(uint16_t)receiveData[5];
    paraValue=receiveData[6]+receiveData[7]*256;
    checkSum=receiveData[8]+receiveData[9]*256;
    
    uint16_t chk=pValue+sValue+alarmStatus*256+mValue+paraValue;
    
    for(int i=0;i<deviceNum;i++)
    {
        if(checkSum==chk+aibus[i].deviceAddr)
        {
            status=i;
            returnData[0]=pValue;
            returnData[1]=sValue;
            returnData[2]=mValue;
            returnData[3]=alarmStatus;
            returnData[4]=paraValue;
            break;
        }
    }
    
    if(status>=0)
    {
        aibus[status].status=alarmStatus;
    }
    
    return status;
}

/* AI-BUS对象初始化 */
void AIbusInitialization(AIbusObjectType *aibus,uint8_t addr,AiBusSendBytes send)
{
    if((aibus==NULL)||(send==NULL))
    {
        return;
    }
    
    aibus->deviceAddr=addr;
    aibus->SendBytes=send;
}
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
