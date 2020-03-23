/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：fm25xxxfunction.c                                              **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现FM25系列串行FRAM的操作，包括4K、16K、64K、128K、256K、 **/
/**           512K、1M等多种容量，不同容量其采用的设备地址位、寄存器地址位存 **/
/**           在差异，具体配置如下：                                         **/
/**           +----------+---------------+-----------------+--------+        **/
/**           |   型号   |   容量结构    |    寄存器地址   |每页字节|        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25L04B | 512x8(4K)     | 9位(1个寄存器)  |  8字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25040B | 512x8(4K)     | 9位(1个寄存器)  |  8字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25C160B| 2048x8(16K)   | 11位(1个寄存器) | 32字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25L16B | 2048x8(16K)   | 11位(1个寄存器) | 32字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25CL64B| 8192x8(64K)   | 13位(2个寄存器) | 32字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25640B | 8192x8(64K)   | 13位(2个寄存器) | 32字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25V01A | 16384x8(128K) | 14位(2个寄存器) | 64字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25W256 | 32768x8(256K) | 15位(2个寄存器) | 64字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25V02A | 32768x8(256K) | 15位(2个寄存器) | 64字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25V05  | 65536x8(512K) | 16位(2个寄存器) | 128字节|        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | FM25V10  | 131072x8(1M)  | 17位(3个寄存器) | 256字节|        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           FM25系列FRAM采用SPI接口，SPI接口模式支持Mode0和Mode3：         **/
/**                   SPI Mode 0 (CPOL = 0, CPHA = 0)                        **/
/**                   SPI Mode 3 (CPOL = 1, CPHA = 1)                        **/
/**           状态寄存器不仅反映状态还用来设置写保护，转台寄存器各位：       **/
/**           +-------+------+------+------+------+------+------+------+     **/
/**           |  Bit7 | Bit6 | Bit5 | Bit4 | Bit3 | Bit2 | Bit1 | Bit0 |     **/
/**           +-------+------+------+------+------+------+------+------+     **/
/**           |WPEN(0)| X(0) | X(0) | X(0) |BP1(0)|BP0(0)|WEL(0)| X(0) |     **/
/**           +-------+------+------+------+------+------+------+------+     **/
/**           其中WEL由写使能所存器决定，WPEN、BP1、BP0可以设定。            **/
/**           BP1和BP0决定保护区域与非保护区域的大小：                       **/
/**             +-----+-----+------------------+                             **/
/**             | BP1 | BP0 |  被保护区域范围  |                             **/
/**             +-----+-----+------------------+                             **/
/**             |  0  |  0  | None             |                             **/
/**             +-----+-----+------------------+                             **/
/**             |  0  |  1  | 1800h-1FFFh(1/4) |                             **/
/**             +-----+-----+------------------+                             **/
/**             |  1  |  0  | 1000h-1FFFh(1/2) |                             **/
/**             +-----+-----+------------------+                             **/
/**             |  1  |  1  | 0000h-1FFFh(全部)|                             **/
/**             +-----+-----+------------------+                             **/
/**           FM25Cxx系列存储器采用多重写保护，关系如下：                    **/
/**           +-----+------+----+----------+----------+----------+           **/
/**           | WEL | WPEN | WP |被保护区域|不保护区域|状态寄存器|           **/
/**           +-----+------+----+----------+----------+----------+           **/
/**           |  0  |  X   | X  |  被保护  |  被保护  |  被保护  |           **/
/**           +-----+------+----+----------+----------+----------+           **/
/**           |  1  |  0   | X  |  被保护  |  不保护  |  不保护  |           **/
/**           +-----+------+----+----------+----------+----------+           **/
/**           |  1  |  1   | 0  |  被保护  |  不保护  |  被保护  |           **/
/**           +-----+------+----+----------+----------+----------+           **/
/**           |  1  |  1   | 1  |  被保护  |  不保护  |  不保护  |           **/
/**           +-----+------+----+----------+----------+----------+           **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-07-17          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/

#ifndef __FM25XXXFUNCTION_H
#define __FM25XXXFUNCTION_H

#include "stdint.h"

/* 定义操作码 */
#define FM25_WREN       0x06	//写使能
#define FM25_WRDI       0x04	//写失能
#define FM25_RDSR       0x05	//读状态寄存器
#define FM25_WRSR       0x01	//写状态寄存器
#define FM25_READ       0x03	//读存储数据
#define FM25_WRITE      0x02	//写存储数据

#define FM25_WPEN       0x80	//写保护使能
#define FM25_BPNONE     0x00	//无保护区域
#define FM25_BPQUARTER  0x04	//保护四分之一区域
#define FM25_BPHALF     0x08	//保护一半区域
#define FM25_BPALL      0x0C	//保护全部区域

/*定义FM25XXX类型枚举*/
typedef enum FM25Mode {
  FM25L04B,
  FM25040B,
  FM25C160B,
  FM25L16B,
  FM25CL64B,
  FM25640B,
  FM25V01A,
  FM25W256,
  FM25V02A,
  FM25V05,
  FM25V10,
  FM25Number
}FM25ModeType;

/*定义存储器寄存器地址字节枚举*/
typedef enum FM25MemAddLength {
  FM258BitMemAdd,
  FM2516BitMemAdd,
  FM2524BitMemAdd
}FM25MemAddLengthType;

/* 定义写保护操作枚举 */
typedef enum FM25WP {
  FM25WP_Enable,
  FM25WP_Disable
}FM25WPType;

/* 定义片选信号枚举 */
typedef enum FM25CS {
  FM25CS_Enable,
  FM25CS_Disable
}FM25CSType;

/* 定义片选信号枚举 */
typedef enum FM25Hold {
  FM25Hold_Enable,
  FM25Hold_Disable
}FM25HoldType;

/* 定义FM25C对象类型 */
typedef struct FM25Object {
  uint8_t status;       //状态寄存器
  FM25ModeType mode;    //设备类型
  FM25MemAddLengthType memAddLength;    //寄存器地址长度
  void (*Read)(uint8_t *rData,uint16_t rSize);  //读数据操作指针
  void (*Write)(uint8_t *wData,uint16_t wSize); //写数据操作指针
  void (*WP)(FM25WPType wp);    //写保护操作
  void (*ChipSelect)(FM25CSType cs);    //片选信号
  void (*Hold)(FM25HoldType hold);      //保持信号
  void (*Delayms)(volatile uint32_t nTime);     //延时操作指针
}FM25ObjectType;

/* 定义读数据操作函数指针类型 */
typedef void (*Fm25Read)(uint8_t *rData,uint16_t rSize);       
/* 定义写数据操作函数指针类型 */
typedef void (*Fm25Write)(uint8_t *wData,uint16_t wSize);    
/* 定义延时操作函数指针类型 */
typedef void (*Fm25Delayms)(volatile uint32_t nTime);
/* 定义写保护操作函数指针类型 */
typedef void (*Fm25WP)(FM25WPType wp);
/* 定义片选操作函数指针类型 */
typedef void (*Fm25ChipSelect)(FM25CSType cs);
/* 定义保持操作函数指针类型 */
typedef void (*Fm25Hold)(FM25HoldType hold);

/*FM24C对象初始化*/
void Fm25cxxInitialization(FM25ObjectType *fram,        //FM25xxx对象实体
                           FM25ModeType mode,           //设备类型
                           Fm25Read read,               //读FM25xxx对象操作指针
                           Fm25Write write,             //写FM25xxx对象操作指针
                           Fm25Delayms delayms,         //延时操作指针
                           Fm25WP wp,                   //写保护操作函数指针
                           Fm25ChipSelect cs,           //片选信号函数指针
                           Fm25Hold hold                //保持信号操作函数指针
                          );

/* 设置写使能所存器*/
void SetWriteEnableLatchForFM25xxx(FM25ObjectType *fram);

/* 复位写使能所存器*/
void ResetWriteEnableLatchForFM25xxx(FM25ObjectType *fram);

/*读FM25xxx状态寄存器*/
void ReadStatusForFM25xxx(FM25ObjectType *fram);

/*写FM25xxx状态寄存器*/
void WriteStatusForFM25xx(FM25ObjectType *fram,uint8_t cmd);

/*从FM25XXX读取单个字节,从随机地址读取*/
uint8_t ReadByteFromFM25xxx(FM25ObjectType *fram,uint32_t regAddress);

/*向FM25XXX写入单个字节*/
void WriteByteToFM25xxx(FM25ObjectType *fram,uint32_t regAddress,uint8_t data);

/*从FM25xxx读取数据*/
void ReadBytesFromFM25xxx(FM25ObjectType *fram,uint32_t regAddress,uint8_t *rData,uint16_t rSize);

/*向FM25xxx写入数据*/
void WriteBytesToFM25xxx(FM25ObjectType *fram,uint32_t regAddress,uint8_t *wData,uint16_t wSize);

#endif
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
