/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：by25qxxfunction.c                                              **/
/** 版    本：V1.1.0                                                         **/
/** 简    介：用于实现在间接和自动轮询模式下，访问BY25Qxx NOR FLASH的相关操作**/
/**     及配置，BY25Qxx有一个状态寄存器指示和配置设备，其各位定义如下：      **/
/**     +------+------+------+-----+------+------+------+------+             **/
/**     |  S23 |  S22 |  S21 | S20 |  S19 |  S18 |  S17 |  S16 |             **/
/**     +------+------+------+-----+------+------+------+------+             **/
/**     | 保留 | DRV1 | DRV0 | HPF | 保留 | 保留 | 保留 | 保留 |             **/
/**     +------+------+------+-----+------+------+------+------+             **/
/**     |  S15 |  S14 |  S13 | S12 |  S11 |  S10 |   S9 |  S8  |             **/
/**     +------+------+------+-----+------+------+------+------+             **/
/**     | SUS1 |  CMP |  LB3 | LB2 |  LB1 | SUS2 |  QE  | SRP1 |             **/
/**     +------+------+------+-----+------+------+------+------+             **/
/**     |  S7  |   S6 |  S5  |  S4 |  S3  |  S2  |  S1  |  S0  |             **/
/**     +------+------+------+-----+------+------+------+------+             **/
/**     | SRP0 |  BP4 |  BP3 | BP2 |  BP1 |  BP0 |  WEL |  WIP |             **/
/**     +------+------+------+-----+------+------+------+------+             **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2021-08-04          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "by25qxxfunction.h"

/* 定义BY25QXX操作命令宏 */
#define WRITE_ENABLE                    0x06    /* 写使能指令 */
#define WRITE_DISABLE                   0x04    /* 写失能指令 */
#define READ_STATUS_REGISTER1           0x05    /* 读状态寄存器1 */
#define READ_STATUS_REGISTER2           0x35    /* 读状态寄存器2 */
#define READ_STATUS_REGISTER3           0x15    /* 读状态寄存器3 */
#define STATUS_REGISTER_WRITE_ENABLE    0x50    /* 写状态寄存器使能 */
#define WRITE_STATUS_REGISTER1          0x01    /* 写状态寄存器1 */
#define WRITE_STATUS_REGISTER2          0x31    /* 写状态寄存器2 */
#define WRITE_STATUS_REGISTER3          0x11    /* 写状态寄存器3 */
#define READ_DATA                       0x03    /* 读数据命令 */
#define FAST_READ                       0x0B    /* 快速度数据命令 */
#define DUAL_OUTPUT_FAST_READ           0x3B    /* 双输出快速读数据命令 */
#define DUAL_IO_FAST_READ               0xBB    /* 双IO快速读数据命令 */
#define QUAD_OUTPUT_FAST_READ           0x6B    /* 四输出快速读数据命令 */
#define QUAD_IO_FAST_READ               0xEB    /* 四IO快速读数据命令 */
#define QUAD_IO_WORD_FAST_READ          0xE7    /* 四IO Word快速读数据命令 */
#define PAGE_PROGRAM                    0x02    /* 页编程命令 */
#define QUAD_PAGE_PROGRAM               0x32    /* 四页编程命令 */
#define FAST_PAGE_PROGROM               0xF2    /* 快速页编程命令 */
#define SESTOR_ERASE                    0x20    /* 扇区擦除命令 */
#define BLOCK_ERASE32K                  0x52    /* 32K块擦除命令 */
#define BLOCK_ERASE64K                  0xD8    /* 64K块擦除命令 */
#define CHIP_ERASE                      0xC7    /* 整片擦除命令 */
#define CHIP_ERASE2                     0x60    /* 整片擦除命令 */
#define ENABLE_RESET                    0x66    /* 使能复位命令 */
#define CHIP_RESET                      0x99    /* 复位命令 */
#define SET_BURST_WRAP                  0x77    /* 设置分页命令 */
#define PROGRAM_ERASE_SUSPEND           0x75    /* 编程擦除挂起命令 */
#define PROGRAM_ERASE_RESUME            0x7A    /* 编程擦除恢复命令 */
#define DEEP_POWER_DOWN                 0xB9    /* 关机操作命令 */
#define RELEASE_DEEP_POWER_DOWN         0xAB    /* 释放关机操作命令 */
#define MANUFACTURER_DEVICE_ID          0x90    /* 读制造商及设备ID命令 */
#define MANUFACTURER_DEVICE_ID_DUAL_IO  0x92    /* 读制造商及设备ID命令 */
#define MANUFACTURER_DEVICE_ID_QUAD_IO  0x94    /* 读制造商及设备ID命令 */
#define JEDEC_ID                        0x9F    /* JEDEC ID命令 */
#define HIGH_PERFORMANCE_MODE           0xA3    /* 高性能模式命令 */
#define READ_DISCOVERABLE_PARAMETER     0x5A    /* 读串行Flash不可覆盖参数命令 */
#define ERASE_SECURITY_REGISTER         0x44    /* 擦除安全寄存器命令 */
#define PROGRAM_SECURITY_REGISTER       0x42    /* 编程安全寄存器命令 */
#define READ_SECURITY_REGISTER          0x48    /* 读安全寄存器命令 */

/* 写使能 */
static void WriteEnable(BY25QObjectType *by250q);
/*读取数据*/
static void QuadFastRead(BY25QObjectType *by250q,uint32_t readAddress,uint8_t *readBuffer,uint32_t readSize);
/*写数据*/
static void QuadPageProgram(BY25QObjectType *by250q,uint32_t writeAddress,uint8_t *writeBuffer,uint32_t writeSize);
/*擦除指定的扇区，扇区大小4KB*/
static void SectorErase(BY25QObjectType *by250q,uint32_t eraseAddress);


/*读数据*/
void ReadDataFromBy25q(BY25QObjectType *by250q,uint32_t readAddress,uint8_t *readBuffer,uint32_t readSize)
{
    QuadFastRead(by250q,readAddress,readBuffer,readSize);
}

/*写数据*/
void WriteDataToBy25q(BY25QObjectType *by250q,uint32_t writeAddress,uint8_t *writeBuffer,uint32_t writeSize)
{
    WriteEnable(by250q);

    QuadPageProgram(by250q,writeAddress,writeBuffer,writeSize);
    
    by250q->Ready();
}

/*擦除指定的扇区，扇区大小4KB*/
void EraseSectorForBy25q(BY25QObjectType *by250q,uint32_t eraseAddress)
{
	/* 写使能 */
	WriteEnable(by250q);	

    SectorErase(by250q,eraseAddress);
    
	by250q->Ready();	
}


/*写状态寄存器*/
void WriteStatusRegister(BY25QObjectType *by250q,uint32_t reg,uint8_t value)
{
    BY25QCommandConfigType config;
    uint8_t sValue=value;
    uint32_t inst[3]={WRITE_STATUS_REGISTER1,WRITE_STATUS_REGISTER2,WRITE_STATUS_REGISTER3};
 
    WriteEnable(by250q);
    
    config.Instruction=inst[reg];    // 读ID指令0x31
    config.DummyCycles=0;       // 空指令周期数
    config.AddressMode=0;
    config.Address=0;
    config.DataMode=1;
    config.NbData=1;
    
    by250q->Write(config,&sValue);

    by250q->Ready();
}

/*读状态寄存器*/
uint32_t ReadStatusRegister(BY25QObjectType *by250q)
{
    uint32_t status=0;
    
    BY25QCommandConfigType config;
    
    config.Instruction=READ_STATUS_REGISTER1;    // 读状态寄存器1指令0x05
    config.DummyCycles=0;       // 空指令周期数
    config.AddressMode=0;
    config.Address=0;
    config.DataMode=1;
    config.NbData=1;
    
    by250q->Read(config,&(by250q->status[0]));
    
    config.Instruction=READ_STATUS_REGISTER2;    // 读状态寄存器1指令0x35
    config.DummyCycles=0;       // 空指令周期数
    config.AddressMode=0;
    config.Address=0;
    config.DataMode=1;
    config.NbData=1;
    
    by250q->Read(config,&(by250q->status[1]));
    
    config.Instruction=READ_STATUS_REGISTER3;    // 读状态寄存器1指令0x15
    config.DummyCycles=0;       // 空指令周期数
    config.AddressMode=0;
    config.Address=0;
    config.DataMode=1;
    config.NbData=1;
    
    by250q->Read(config,&(by250q->status[2]));
    
    status=by250q->status[2];
    status=(status<<8)+by250q->status[1];
    status=(status<<8)+by250q->status[0];
    
    return status;
}

/*获取BY25Q的ID编号*/
void GetBy25qxxID(BY25QObjectType *by250q)
{
    BY25QCommandConfigType config;

    config.Instruction=MANUFACTURER_DEVICE_ID;    // 读ID指令0x90
    config.DummyCycles=0;       // 空指令周期数
    config.AddressMode=1;
    config.Address=0;
    config.DataMode=1;
    config.NbData=2;
    
    by250q->Read(config,by250q->mfrID);

    config.Instruction=JEDEC_ID;    // 读ID指令0x9F
    config.DummyCycles=0;       // 空指令周期数
    config.AddressMode=0;
    config.Address=0;
    config.DataMode=1;
    config.NbData=3;
    
    by250q->Read(config,by250q->jedecID);
    
    config.Instruction=0x4B;    // 读ID指令0x4B
    config.DummyCycles=32;       // 空指令周期数
    config.AddressMode=0;
    config.Address=0;
    config.DataMode=1;
    config.NbData=8;
    
    by250q->Read(config,by250q->uniqueID);
}

/*实现BY25Q初始化配置*/
void BY25QInitialization(BY25QObjectType *by250q,   /*BY250Q存储器对象*/
                         BY25QWriteType write,      /*写函数指针*/
                         BY25QReadType read,        /*读函数指针*/
                         BY25QCommandType command,  /*命令下发函数指针*/
                         BY25QReadyType ready       /*就绪检测函数指针*/
                         )
{
    if((by250q==NULL)||(write==NULL)||(read==NULL)||(command==NULL)||(ready==NULL))
    {
        return;
    }
    
    by250q->Write=write;
    by250q->Read=read;
    by250q->Command=command;
    by250q->Ready=ready;
    
    GetBy25qxxID(by250q);
    
    
    ReadStatusRegister(by250q);
    
    uint8_t value=by250q->status[1];
    value=value|0x02;
    
    WriteStatusRegister(by250q,1,value);
        
    ReadStatusRegister(by250q);
}

/*擦除指定的扇区，扇区大小4KB*/
static void SectorErase(BY25QObjectType *by250q,uint32_t eraseAddress)
{
	BY25QCommandConfigType config;

    config.Instruction=0x20;    // 读ID指令0x90
    config.DummyCycles=0;       // 空指令周期数
    config.AddressMode=1;
    config.Address=eraseAddress;
    config.DataMode=0;
    config.NbData=0;

    by250q->Command(config);
}

/*写数据*/
static void QuadPageProgram(BY25QObjectType *by250q,uint32_t writeAddress,uint8_t *writeBuffer,uint32_t writeSize)
{
    BY25QCommandConfigType config;

    config.Instruction=0x32;    // 读ID指令0x32
    config.DummyCycles=0;       // 空指令周期数
    config.AddressMode=1;
    config.Address=writeAddress;
    config.DataMode=3;
    config.NbData=writeSize;
    
    by250q->Write(config,writeBuffer);
}

/*读取数据*/
static void QuadFastRead(BY25QObjectType *by250q,uint32_t readAddress,uint8_t *readBuffer,uint32_t readSize)
{
    BY25QCommandConfigType config;
    
    config.Instruction=0xEB;    // 读ID指令0xEB
    config.DummyCycles=6;       // 空指令周期数
    config.AddressMode=3;
    config.Address=readAddress;
    config.DataMode=3;
    config.NbData=readSize;
    
    by250q->Read(config,readBuffer);
}

/* 写使能 */
static void WriteEnable(BY25QObjectType *by250q)
{
    BY25QCommandConfigType config;
    
    config.Instruction=WRITE_ENABLE;    // 写使能指令0x06
    config.DummyCycles=0;       // 空指令周期数
    config.AddressMode=0;
    config.Address=0;
    config.DataMode=0;
    config.NbData=0;

    by250q->Command(config);
}
/*********** (C) COPYRIGHT 1999-2021 Moonan Technology *********END OF FILE****/