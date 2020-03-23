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

#ifndef _aibusfunction_h
#define _aibusfunction_h

#include "stdint.h"

/* 定义AI-BUS设每类型枚举 */
typedef enum AIbusDev{
  AIcontroller,         //AI控制器
  AIinspection,         //AI巡检仪
  AIflowtotalizer,      //AI流量积算器
  AIfreqmodulator,      //AI频率调节器
  AIregulator           //PID调节器
}AIbusDevType;

/* 定义AI-BUS设备对象 */
typedef struct AIbusObject {
  uint8_t deviceAddr;
  uint8_t status;
  void (*SendBytes)(uint8_t *cmd,uint16_t size);
}AIbusObjectType;

/*通过AIBUS发送字节数据*/
typedef void (*AiBusSendBytes)(uint8_t *cmd,uint16_t size);

/*设置目标设备的参数值*/
void WriteAiBusDeviceParameter(AIbusObjectType *aibus,uint8_t paraAddr,uint16_t data);

/*读取目标设备的参数值*/
void ReadAiBusDeviceParameter(AIbusObjectType *aibus,uint8_t paraAddr);

/*解析返回数据,返回值为读或者写的参数值*/
int ParsingReturnData(AIbusObjectType *aibus,uint8_t *receiveData,uint16_t *returnData,uint16_t deviceNum);

/* AI-BUS对象初始化 */
void AIbusInitialization(AIbusObjectType *aibus,uint8_t addr,AiBusSendBytes send);

#endif
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
