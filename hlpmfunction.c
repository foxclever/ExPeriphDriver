/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：hlpmfunction.c                                                 **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：实现PM25激光传感器HLPM025K3的数据读取                          **/
/**           (1)波特率：9600bit/S;数据位:8位;停止位:1位;校验位:无           **/
/**           (2)数据发送间隔时间为0.8～1.2S（秒）                           **/
/**           (3)数据格式：7个字节                                           **/
/**             +------+--------+--------+-------+-------+------+------+     **/
/**             |起始位|PM2.5(H)|PM2.5(L)|PM10(H)|PM10(L)|校验位|结束位|     **/
/**             +------+--------+--------+-------+-------+------+------+     **/
/**             | 0xAA | 1字节  | 1字节  | 1字节 | 1字节 | 1字节| 0xFF |     **/
/**             +------+--------+--------+-------+-------+------+------+     **/
/**               其中校验位=PM2.5(H)+PM2.5(L)+PM10(H)+PM10(L)               **/
/**           (4)数据处理：接收到的数据按公式计算后得到PM2.5和PM10的值：     **/
/**               PM2.5=(PM2.5(H)×256+PM2.5(L))×0.1                        **/
/**               PM10=(PM2.5(H)×256+PM2.5(L))×0.1                         **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    1、配置串口通讯，如果接收到的为7个字节长度数据，则传给ParsingPMData函 **/
/**       数解析。同时将接收结果的变量定义为2个浮点数的数组，以参数形式传给  **/
/**       ParsingPMData函数，第一个元素为PM2.5数据,第二个元素为PM10数据。    **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2013-07-28          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "hlpmfunction.h"

/*校验数据是否有效*/
static bool CheckDataIsValid(uint8_t *receivedData);
/*计算具体的PM数值*/
static float SynthesisPMValue(uint8_t *rawData);
/*将接收到的数据加入缓存*/
static void BufferDataEnQueue(HlpmObjectType *hlpm,uint8_t rData);
/*从缓存中读取数据*/
static uint16_t BufferDataDeQueue(HlpmObjectType *hlpm,uint8_t *rxBuf);

/*解析PM2.5和PM10的数据*/
bool ParsingPMData(HlpmObjectType *hlpm)
{
    uint16_t length=0;
    uint8_t receivedData[7]={0};
    bool isValid=false;
    
    length=hlpm->rxBuffer.DeQueue(hlpm,receivedData);
    
    if(length>0)
    {
        isValid=CheckDataIsValid(receivedData);
        if(isValid)
        {
            hlpm->pm25=SynthesisPMValue(receivedData+1);
            hlpm->pm100=SynthesisPMValue(receivedData+3);
        }
    }
    return isValid;
}

/*HLPM对象初始化函数*/
void HlpmInitialization(HlpmObjectType *hlpm)
{
    if(hlpm==NULL)
    {
        return;
    }
    
    hlpm->pm25=0.0;
    hlpm->pm100=0.0;
    
    hlpm->rxBuffer.pRead=0;
    hlpm->rxBuffer.pWrite=0;
    hlpm->rxBuffer.EnQueue=BufferDataEnQueue;
    hlpm->rxBuffer.DeQueue=BufferDataDeQueue;
}

/*校验数据是否有效*/
static bool CheckDataIsValid(uint8_t *receivedData)
{
    bool isValid=false;
    uint8_t checkSum=receivedData[1]+receivedData[2]+receivedData[3]+receivedData[4];
    if((receivedData[0]==0xAA)&&(receivedData[6]==0xFF)&&(checkSum==receivedData[5]))
    {
        isValid=true;
    }
    
    return isValid;
}

/*计算具体的PM数值*/
static float SynthesisPMValue(uint8_t *rawData)
{
    float result=0.0;
    result=(float)((uint16_t)rawData[0]*256+(uint16_t)rawData[1])*0.1;
    return result;
}

/*从缓存中读取数据*/
static uint16_t BufferDataDeQueue(HlpmObjectType *hlpm,uint8_t *rxBuf)
{
    uint16_t length=0;
    uint8_t pTemp;
    
    if(hlpm->rxBuffer.pRead==hlpm->rxBuffer.pWrite)
    {
        length=0;
    }
    else
    {
        while(hlpm->rxBuffer.queue[hlpm->rxBuffer.pRead]!=0xAA)
        {
            if(++(hlpm->rxBuffer.pRead)>=(uint8_t)HLPMRxBufferLength)
            {
                hlpm->rxBuffer.pRead=0;
            }
            
            if(hlpm->rxBuffer.pRead==hlpm->rxBuffer.pWrite)
            {
                length=0;
                return length;
            }
        }
        
        pTemp=hlpm->rxBuffer.pRead;
        
        do
        {
            rxBuf[length++]=hlpm->rxBuffer.queue[hlpm->rxBuffer.pRead];
            
            if(++(hlpm->rxBuffer.pRead)>=(uint8_t)HLPMRxBufferLength)
            {
                hlpm->rxBuffer.pRead=0;
            }
            
            if(hlpm->rxBuffer.pRead==hlpm->rxBuffer.pWrite)
            {
                length=0;
                hlpm->rxBuffer.pRead=pTemp;
                return length;
            }
        }while(hlpm->rxBuffer.queue[hlpm->rxBuffer.pRead]!=0xFF);
        
        rxBuf[length++]=hlpm->rxBuffer.queue[hlpm->rxBuffer.pRead];
    }
    return length;
}

/*将接收到的数据加入缓存*/
static void BufferDataEnQueue(HlpmObjectType *hlpm,uint8_t rData)
{
    hlpm->rxBuffer.queue[hlpm->rxBuffer.pWrite]=rData;
    
    if(++(hlpm->rxBuffer.pWrite)>=(uint8_t)HLPMRxBufferLength)
    {
        hlpm->rxBuffer.pWrite=0;
    }
}
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
