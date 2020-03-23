/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：at25xxxfunction.h                                              **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现SPI串行接口EEPROM AT25XXX系列的操作具有1K、2K、4K、8K、**/
/**           16K、32K、64K、128K、256K、512K、1M、2M等多种容量，不同容量其  **/
/**           采用的设备地址位、寄存器地址位存在差异，每一页所包含的字节数   **/
/**           也不一样。具体配置如下：                                       **/
/**           +----------+---------------+-----------------+--------+        **/
/**           |   型号   |   容量结构    |    寄存器地址   |每页字节|        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | AT25010B | 128x8(1K)     | 7位(1个寄存器)  |  8字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | AT25020B | 256x8(2K)     | 8位(1个寄存器)  |  8字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | AT25040B | 512x8(4K)     | 9位(1个寄存器)  |  8字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | AT25080B | 1024x8(8K)    | 10位(1个寄存器) | 32字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | AT25160B | 2048x8(16K)   | 11位(1个寄存器) | 32字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | AT25320B | 4096x8(32K)   | 12位(2个寄存器) | 32字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | AT25640B | 8192x8(64K)   | 13位(2个寄存器) | 32字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | AT25128B | 16384x8(128K) | 14位(2个寄存器) | 64字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | AT25256B | 32768x8(256K) | 15位(2个寄存器) | 64字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | AT25512  | 65536x8(512K) | 16位(2个寄存器) | 128字节|        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | AT25M01  | 131072x8(1M)  | 17位(3个寄存器) | 256字节|        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | AT25M02  | 262144x8(2M)  | 18位(3个寄存器) | 256字节|        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           AT25040B需要9位寄存器地址，但只用了一个地址字节，最高位地址利用**/
/**           的是操作命令的第3位来实现的。                                  **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-07-17          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __AT25XXXFUNCTION_H
#define __AT25XXXFUNCTION_H

#include "stdint.h"

/* 定义操作码 */
#define AT25_WREN       0x06		//写使能
#define AT25_WRDI       0x04		//写失能
#define AT25_RDSR       0x05		//读状态寄存器
#define AT25_WRSR       0x01		//写状态寄存器
#define AT25_READ       0x03		//读存储数据
#define AT25_WRITE      0x02		//写存储数据

#define AT25_WPEN       0x80	//写保护使能
#define AT25_BPNONE     0x00	//无保护区域
#define AT25_BPQUARTER  0x04	//保护四分之一区域
#define AT25_BPHALF     0x08	//保护一半区域
#define AT25_BPALL      0x0C	//保护全部区域

/*定义AT25XXX类型枚举*/
typedef enum At25Mode {
  AT25010B,
  AT25020B,
  AT25040B,
  AT25080B,
  AT25160B,
  AT25320B,
  AT25640B,
  AT25128B,
  AT25256B,
  AT25512,
  AT25M01,
  AT25M02,
  AT25Number
}At25ModeType;

/*定义存储器寄存器地址字节枚举*/
typedef enum At25MemAddLength {
  AT258BitMemAdd,
  AT2516BitMemAdd,
  AT2524BitMemAdd
}At25MemAddLengthType;

/* 定义片选信号枚举 */
typedef enum AT25xxxCS
{
  AT25CS_Enable,
  AT25CS_Disable
}AT25xxxCSType;

/* 定义写保护操作枚举 */
typedef enum FM25WP {
  AT25WP_Enable,
  AT25WP_Disable
}AT25WPType;

/* 定义片选信号枚举 */
typedef enum AT25Hold {
  AT25Hold_Enable,
  AT25Hold_Disable
}AT25HoldType;

/*定义AT25XXX对象类型*/
typedef struct At25Object {
  uint8_t status;       //状态寄存器
  At25ModeType mode;    //设备类型
  At25MemAddLengthType memAddLength;    //寄存器地址长度
  void (*Read)(uint8_t *rData,uint16_t rSize);  //读数据操作指针
  void (*Write)(uint8_t *wData,uint16_t wSize); //写数据操作指针
  void (*Delayms)(volatile uint32_t nTime);     //毫秒延时操作指针
  void (*ChipSelect)(AT25xxxCSType cs); //使用SPI接口时，片选操作
  void (*WP)(AT25WPType wp);            //写保护操作
  void (*Hold)(AT25HoldType hold);      //保持信号
}At25ObjectType;

/* 定义读数据操作函数指针类型 */
typedef void (*AT25Read)(uint8_t *rData,uint16_t rSize);
/* 定义写数据操作函数指针类型 */
typedef void (*AT25Write)(uint8_t *wData,uint16_t wSize);
/* 定义延时操作函数指针类型 */
typedef void (*AT25Delayms)(volatile uint32_t nTime);
/* 定义使用SPI接口时，片选操作函数指针类型 */
typedef void (*AT25ChipSelect)(AT25xxxCSType cs);
/*写保护操作*/
typedef void (*AT25WP)(AT25WPType wp);
/*保持信号*/
typedef void (*AT25Hold)(AT25HoldType hold);

/*从AT25XXX读取单个字节,从随机地址读取*/
uint8_t ReadByteFromAT25xxx(At25ObjectType *at,uint32_t regAddress);

/*向AT25XXX写入单个字节*/
void WriteByteToAT25xxx(At25ObjectType *at,uint32_t regAddress,uint8_t data);

/* AT25XXX设置写使能所存器*/
void SetWriteEnableLatchForAT25xxx(At25ObjectType *at);

/* AT25XXX复位写使能所存器*/
void ResetWriteEnableLatchForAT25xxx(At25ObjectType *at);

/*读AT25xxx状态寄存器*/
void ReadStatusForAT25xxx(At25ObjectType *at);

/*写AT25xxx状态寄存器*/
void WriteStatusForAT25xxx(At25ObjectType *at,uint8_t cmd);

/*从AT25XXX读取多个字节，从指定地址最多到所在页的结尾*/
void ReadBytesFromAT25xxx(At25ObjectType *at,uint32_t regAddress,uint8_t *rData,uint16_t rSize);

/*向AT25XXX写入多个字节，从指定地址最多到所在页的结尾*/
void WriteBytesToAT25xxx(At25ObjectType *at,uint32_t regAddress,uint8_t *wData,uint16_t wSize);

/* 初始化AT25XXX对象 */
void At25xxxInitialization(At25ObjectType *at,  //AT25XXX对象实体
                           At25ModeType mode,   //AT25XXX对象类型
                           AT25Read read,       //读AT25XXX对象操作指针
                           AT25Write write,     //写AT25XXX对象操作指针
                           AT25Delayms delayms, //延时操作指针
                           AT25ChipSelect cs    //片选操作函数指针
                          );

#endif
/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
