/******************************************************************************/
/** 模块名称：以太网通讯                                                     **/
/** 文件名称：w5500function.h                                                **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于使用W5500实现以太网通讯的相关处理                          **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2015-07-30          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 


#ifndef __w5500function_h
#define __w5500function_h

#include "stdint.h"
#include "socket.h"

/* 定义W5500对象类型 */
typedef struct W5500Object {
  wiz_NetInfo gWIZNETINFO;
  uint16_t (*DataParsing)(uint8_t *rxBuffer,uint16_t rxSize,uint8_t *txBuffer);//接收消息解析及返回消息生成，返回值为返回消息的字节长度
  uint16_t (*RequestData)(uint8_t *rqBuffer);   //得到请求命令，一般用于客户端发起访问
}W5500ObjectType;

/*定义Socket枚举，共8个*/
typedef enum W5500Socket {
  Socket0=0,
  Socket1=1,
  Socket2=2,
  Socket3=3,
  Socket4=4,
  Socket5=5,
  Socket6=6,
  Socket7=7
}W5500SocketType;

/*解析接收到的数据*/
typedef uint16_t (*W5500DataParsingType)(uint8_t *rxBuffer,uint16_t rxSize,uint8_t *txBuffer);
/*得到请求命令，一般用于客户端发起访问*/
typedef uint16_t (*W5500RequestDataType)(uint8_t *rqBuffer);   

/*定义片选及临界区操作函数类型*/
typedef void (*W5500CSCrisType)(void);
/*定义SPI读一个字节函数类型*/
typedef uint8_t (*W5500SPIReadByteTYpe)(void);
/*定义SPI写一个字节函数类型*/
typedef void (*W5500SPIWriteByteTYpe)(uint8_t wb);

/*TCP服务器数据通讯*/
int32_t W5500TCPServer(W5500ObjectType *w5500,W5500SocketType sn, uint16_t port);
/*TCP客户端数据通讯*/
int32_t W5500TCPClient(W5500ObjectType *w5500,W5500SocketType sn,uint16_t lPort,uint8_t *rIP,uint16_t rPort);
/*UDP服务器数据通讯*/
int32_t W5500UDPServer(W5500ObjectType *w5500,W5500SocketType sn, uint16_t port,uint8_t *rIP,uint16_t rPort);
/*UDP客户端数据通讯*/
int32_t W5500UDPClient(W5500ObjectType *w5500,W5500SocketType sn, uint16_t lPort,uint8_t *rIP,uint16_t rPort);

/*W5500对象初始化函数*/
void W5500Initialization(W5500ObjectType *w5500,
                         uint8_t mac[6],        //本地Mac地址
                         uint8_t ip[4],         //本地IP地址
                         uint8_t sn[4],         //子网掩码
                         uint8_t gw[4],         //网关地址
                         uint8_t dns[4],        //DNS服务器地址
                         dhcp_mode dhcp,        //DHCP类型
                         W5500CSCrisType cris_en,
                         W5500CSCrisType cris_ex,
                         W5500CSCrisType cs_sel,
                         W5500CSCrisType cs_desel,
                         W5500SPIReadByteTYpe spi_rb,
                         W5500SPIWriteByteTYpe spi_wb,
                         W5500DataParsingType dataParse,
                         W5500RequestDataType requst
                        );

#endif
/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
