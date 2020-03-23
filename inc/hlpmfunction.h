/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：hlpmfunction.h                                                 **/
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
/**               PM2.5=(PM2.5(H)×256+PM2.5(L))×0.1                          **/
/**               PM10=(PM2.5(H)×256+PM2.5(L))×0.1                           **/
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

#ifndef __HLPMFUNCTION_H
#define __HLPMFUNCTION_H

#include "stdbool.h"
#include "stdint.h"

#ifndef HLPMRxBufferLength
#define HLPMRxBufferLength 35
#endif

/*定义HLPM025K3对象类型*/
typedef struct HlpmObject {
  float pm25;   //PM2.5测量值
  float pm100;  //PM10测量值
  struct HlpmRxBuffer{
    uint8_t queue[HLPMRxBufferLength];  //键值存储队列
    uint8_t pRead;      //读队列指针
    uint8_t pWrite;     //写队列指针
    uint16_t (*DeQueue)(struct HlpmObject *hlpm,uint8_t *rxBuf);        //出队操作
    void (*EnQueue)(struct HlpmObject *hlpm,uint8_t rData);             //入队操作
  }rxBuffer;    //定义接收缓存队列
}HlpmObjectType;

/*解析PM2.5和PM10的数据*/
bool ParsingPMData(HlpmObjectType *hlpm);

/*HLPM对象初始化函数*/
void HlpmInitialization(HlpmObjectType *hlpm);
#endif
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
