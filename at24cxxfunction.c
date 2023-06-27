/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：at24cxxfunction.c                                              **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现二线制串行（I2C）EEPROM AT24CXX系列的操作具有1K、2K、  **/
/**           4K、8K、16K、32K、64K、128K、256K、512K、1M、2M等多种容量，不  **/
/**           同容量其采用的设备地址位、寄存器地址位存在差异每一页所包含，   **/
/**           的字节数也不一样。具体配置如下：                               **/
/**  +----------+-------------+--------------------+---------------+--------+**/
/**  |   型号   |  容量结构   |     设备地址位     |   寄存器地址  |每页字节|**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24C01C  |128x8(1K)    |1|0|1|0|A2|A1|A0|R/W|7位(1个寄存器) |  8字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24C01D  |128x8(1K)    |1|0|1|0|A2|A1|A0|R/W|7位(1个寄存器) |  8字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24C02C  |256x8(2K)    |1|0|1|0|A2|A1|A0|R/W|8位(1个寄存器) |  8字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24C02D  |256x8(2K)    |1|0|1|0|A2|A1|A0|R/W|8位(1个寄存器) |  8字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24C04C  |512x8(4K)    |1|0|1|0|A2|A1|P0|R/W|9位(1个寄存器) | 16字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24C04D  |512x8(4K)    |1|0|1|0|A2|A1|P0|R/W|9位(1个寄存器) | 16字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24C08C  |1024x8(8K)   |1|0|1|0|A2|P1|P0|R/W|10位(1个寄存器)| 16字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24C08D  |1024x8(8K)   |1|0|1|0|A2|P1|P0|R/W|10位(1个寄存器)| 16字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24C16C  |2048x8(16K)  |1|0|1|0|P2|P1|P0|R/W|11位(1个寄存器)| 16字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24C16D  |2048x8(16K)  |1|0|1|0|P2|P1|P0|R/W|11位(1个寄存器)| 16字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24C32D  |4096x8(32K)  |1|0|1|0|A2|A1|A0|R/W|12位(2个寄存器)| 32字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24C32E  |4096x8(32K)  |1|0|1|0|A2|A1|A0|R/W|12位(2个寄存器)| 32字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24C64B  |8192x8(64K)  |1|0|1|0|A2|A1|A0|R/W|13位(2个寄存器)| 32字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24C64D  |8192x8(64K)  |1|0|1|0|A2|A1|A0|R/W|13位(2个寄存器)| 32字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24C128C |16384x8(128K)|1|0|1|0|A2|A1|A0|R/W|14位(2个寄存器)| 64字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24C256C |32768x8(256K)|1|0|1|0|A2|A1|A0|R/W|15位(2个寄存器)| 64字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24C512C |65536x8(512K)|1|0|1|0|A2|A1|A0|R/W|16位(2个寄存器)| 128字节|**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24CM01  |131072x8(1M) |1|0|1|0|A2|A1|P0|R/W|17位(2个寄存器)| 256字节|**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24CM02  |262144x8(2M) |1|0|1|0|A2|P1|P0|R/W|18位(2个寄存器)| 256字节|**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24CS01  |128x8(1K)    |1|0|1|0|A2|A1|A0|R/W|7位(1个寄存器) |  8字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24CS02  |256x8(2K)    |1|0|1|0|A2|A1|A0|R/W|8位(1个寄存器) |  8字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24CS04  |512x8(4K)    |1|0|1|0|A2|A1|P0|R/W|9位(1个寄存器) | 16字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24CS08  |1024x8(8K)   |1|0|1|0|A2|P1|P0|R/W|10位(1个寄存器)| 16字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24CS16  |2048x8(16K)  |1|0|1|0|P2|P1|P0|R/W|11位(1个寄存器)| 16字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24CS32  |4096x8(32K)  |1|0|1|0|A2|A1|A0|R/W|12位(2个寄存器)| 32字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24CS64  |8192x8(64K)  |1|0|1|0|A2|A1|A0|R/W|13位(2个寄存器)| 32字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24CSW010|128x8(1K)    |1|0|1|0|A2|A1|A0|R/W|7位(1个寄存器) |  8字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24CSW020|256x8(2K)    |1|0|1|0|A2|A1|A0|R/W|8位(1个寄存器) |  8字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24CSW040|512x8(4K)    |1|0|1|0|A2|A1|P0|R/W|9位(1个寄存器) | 16字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24CSW080|1024x8(8K)   |1|0|1|0|A2|P1|P0|R/W|10位(1个寄存器)| 16字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24HC02C |256x8(2K)    |1|0|1|0|A2|A1|A0|R/W|8位(1个寄存器) |  8字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24HC04B |512x8(4K)    |1|0|1|0|A2|A1|P0|R/W|9位(1个寄存器) | 16字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24MAC402|256x8(2K)    |1|0|1|0|A2|A1|A0|R/W|8位(1个寄存器) | 16字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**  |AT24MAC602|256x8(2K)    |1|0|1|0|A2|A1|A0|R/W|8位(1个寄存器) | 16字节 |**/
/**  +----------+-------------+-+-+-+-+--+--+--+---+---------------+--------+**/
/**           有些容量的设备地址段有些位用于寄存器地址（P）寻址，设备地址的可**/
/**           设定位数（A）决定了同一总线上所挂载的器件数量：                **/
/**               3位用于设备地址，0位用于内存寻址：可挂载8个；              **/
/**               2位用于设备地址，1位用于内存寻址：可挂载4个；              **/
/**               1位用于设备地址，2位用于内存寻址：可挂载2个；              **/
/**               0位用于设备地址，3位用于内存寻址：可挂载1个；              **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-07-17          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "at24cxxfunction.h"

static uint16_t devAddMask[AT24CNumber]={0xFE,0xFE,0xFE,0xFE,0xFC,0xFC,0xF8,0xF8,0xF0,0xF0,
0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFC,0xF8,0xFE,
0xFE,0xFC,0xF8,0xF0,0xFE,0xFE,0xFE,0xFE,0xFC,0xF8,
0xFE,0xFC,0xFE,0xFE};
static uint16_t pageBytes[AT24CNumber]={8,8,8,8,16,16,16,16,16,16,32,32,32,32,64,64,128,256,
256,8,8,16,16,16,32,32,8,8,16,16,8,16,16,16};
static uint16_t regAddMask[AT24CNumber]={0x07,0x07,0x07,0x07,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,
0x1F,0x1F,0x1F,0x1F,0x3F,0x3F,0x7F,0xFF,0xFF,0x07,
0x07,0x0F,0x0F,0x0F,0x1F,0x1F,0x07,0x07,0x0F,0x0F,
0x07,0x0F,0x0F,0x0F};

/*从AT24CXX读取单个字节,从随机地址读取*/
uint8_t ReadByteFromAT24CXX(At24cObjectType *at,uint32_t regAddress)
{
    uint8_t rData;
    uint16_t regAdd;
    uint8_t temp;
    
    if(at->memAddLength==AT24C8BitMemAdd)
    {
        regAdd=(uint16_t)(regAddress&0xFF);
        temp=(uint8_t)(regAddress>>8);
    }
    else
    {
        regAdd=(uint16_t)regAddress;
        temp=(uint8_t)(regAddress>>16);
    }
    temp=(temp&(~(devAddMask[at->mode]>>1)))<<1;
    at->devAddress=(at->devAddress & devAddMask[at->mode])|temp;
    
    at->Read(at,regAdd,&rData,1);
    
    return rData;
}

/*向AT24CXX写入单个字节*/
void WriteByteToAT24CXX(At24cObjectType *at,uint32_t regAddress,uint8_t data)
{
    uint8_t temp;
    uint16_t regAdd;
    
    if(at->memAddLength==AT24C8BitMemAdd)
    {
        regAdd=(uint16_t)(regAddress&0xFF);
        temp=(uint8_t)(regAddress>>8);
    }
    else
    {
        regAdd=(uint16_t)regAddress;
        temp=(uint8_t)(regAddress>>16);
    }
    temp=(temp&(~(devAddMask[at->mode]>>1)))<<1;
    at->devAddress=(at->devAddress & devAddMask[at->mode])|temp;
    
    at->Write(at,regAdd,&data,1);
}

/*从AT24CXX读取多个字节，从指定地址最多到所在页的结尾*/
void ReadBytesFromAT24CXX(At24cObjectType *at,uint32_t regAddress,uint8_t *rData,uint16_t rSize)
{
    uint16_t regAdd;
    uint16_t size;
    uint8_t temp;
    
    if(at->memAddLength==AT24C8BitMemAdd)
    {
        regAdd=(uint16_t)(regAddress&0xFF);
        temp=(uint8_t)(regAddress>>8);
    }
    else
    {
        regAdd=(uint16_t)regAddress;
        temp=(uint8_t)(regAddress>>16);
    }
    temp=(temp&(~(devAddMask[at->mode]>>1)))<<1;
    at->devAddress=(at->devAddress & devAddMask[at->mode])|temp;
    
    if((rSize<=pageBytes[at->mode])&&(rSize<=(pageBytes[at->mode]-(regAddress&regAddMask[at->mode]))))
    {
        size=rSize;
    }
    else
    {
        size=pageBytes[at->mode]-(regAddress&regAddMask[at->mode]);
    }
    
    at->Read(at,regAdd,rData,size);
}

/*向AT24CXX写入多个字节，从指定地址最多到所在页的结尾*/
void WriteBytesToAT24CXX(At24cObjectType *at,uint32_t regAddress,uint8_t *wData,uint16_t wSize)
{
    uint16_t regAdd;
    uint8_t size;
    uint8_t temp;
    
    if(at->memAddLength==AT24C8BitMemAdd)
    {
        regAdd=(uint16_t)(regAddress&0xFF);
        temp=(uint8_t)(regAddress>>8);
    }
    else
    {
        regAdd=(uint16_t)regAddress;
        temp=(uint8_t)(regAddress>>16);
    }
    temp=(temp&(~(devAddMask[at->mode]>>1)))<<1;
    at->devAddress=(at->devAddress & devAddMask[at->mode])|temp;
    
    if((wSize<=pageBytes[at->mode])&&(wSize<=(pageBytes[at->mode]-(regAddress&regAddMask[at->mode]))))
    {
        size=wSize;
    }
    else
    {
        size=pageBytes[at->mode]-(regAddress&regAddMask[at->mode]);
    }
    
    at->Write(at,regAdd,wData,size);
}

/* 初始化AT24CXX对象 */
void At24cxxInitialization(At24cObjectType *at, //AT24CXX对象实体
                           uint8_t devAddress,  //AT24CXX设备地址
                           At24cModeType mode,  //AT24CXX对象类型
                           At24cRead read,      //读AT24CXX对象操作指针
                           At24cWrite write,    //写AT24CXX对象操作指针
                           At24cDelayms delayms //延时操作指针
                               )
{
    if((at==NULL)||(read==NULL)||(write==NULL)||(delayms==NULL))
    {
        return;
    }
    
    if((devAddress&0xF0)==0xA0)
    {
        at->devAddress=devAddress;
    }
    else
    {
        at->devAddress=0x00;
    }
    
    if(mode>=AT24CNumber)
    {
        return;
    }
    at->mode=mode;
    
    if(((mode>=AT24C32D)&&(mode<=AT24CM02))||((mode>=AT24CS32)&&(mode<=AT24CS64)))
    {
        at->memAddLength=AT24C16BitMemAdd;
    }
    else
    {
        at->memAddLength=AT24C8BitMemAdd;
    }
    
    at->Read=read;
    at->Write=write;
    at->Delayms=delayms;
}

/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/