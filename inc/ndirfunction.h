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

#ifndef __ndirfunction_h
#define __ndirfunction_h

#include "stdint.h"

#define ReadRegisterFC    0x03    //从寄存器读功能码
#define WriteRegisterFC   0x06    //往寄存器写功能码

/*定义读取参数类型的枚举*/
typedef enum NdirParameter{
  SoftVersion=0x0084,
  Concentration=0x000A,
  Temperature=0x0003,
  StatusFlags=0x0009
}NdirParameterType;

/*定义NDIR对象类型*/
typedef struct NdirObject {
  uint8_t moduleAddress;
  uint16_t status;
  uint32_t softVersion;
  float concentration;
  float temperature;
  void (*SendByte)(uint8_t data);
  void (*Delayms)(volatile uint32_t nTime);
}NdirObjectType;

/*发送一个字节操作函数指针类型*/
typedef void (*NdirSendByteType)(uint8_t data);
/*毫秒延时函数指针雷响*/
typedef void (*NdirDelaymsType)(volatile uint32_t nTime);

/*NDIR初始化配置函数*/
void NdirInitialization(NdirObjectType *ndir,   //NDIR对象
                        uint8_t moduleAddress,  //模块地址
                        NdirSendByteType send,  //发送数据操作函数
                        NdirDelaymsType delayms //毫秒延时操作函数
                        );

/*数据解析，用以解析接收到的数据信息*/
void NdirParseReceiveData(NdirObjectType *ndir,uint8_t * receiveDataBuffer);

/*读软件版本*/
uint32_t ReadNDIRSoftVersion(NdirObjectType *ndir,uint8_t *receiveDataBuffer);

/*从舒茨非分光红外气体检测模块读取浓度值*/
float ReadConcentrationData(NdirObjectType *ndir,uint8_t * receiveDataBuffer);

/*从舒茨非分光红外气体检测模块读取内部温度值*/
float ReadTemperatureData(NdirObjectType *ndir,uint8_t * receiveDataBuffer);

/*从舒茨非分光红外气体检测模块读取状态标志*/
uint16_t ReadNDIRStatusflags(NdirObjectType *ndir,uint8_t * receiveDataBuffer);

#endif
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
