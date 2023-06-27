/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：at25xxxfunction.c                                              **/
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
/**           | AT25080B | 1024x8(8K)    | 10位(2个寄存器) | 32字节 |        **/
/**           +----------+---------------+-----------------+--------+        **/
/**           | AT25160B | 2048x8(16K)   | 11位(2个寄存器) | 32字节 |        **/
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

#include "stddef.h"
#include "at25xxxfunction.h"

static uint16_t pageBytes[AT25Number]={8,8,8,32,32,32,32,64,64,128,256,256};
static uint16_t regAddMask[AT25Number]={0x07,0x07,0x07,0x1F,0x1F,0x1F,0x1F,
0x3F,0x3F,0x7F,0xFF,0xFF};

/*缺省片选处理函数*/
static void AT25ChipSelectDefault(AT25xxxCSType cs);

/*从AT25XXX读取单个字节,从随机地址读取*/
uint8_t ReadByteFromAT25xxx(At25ObjectType *at,uint32_t regAddress)
{
    uint8_t rData;
    
    ReadBytesFromAT25xxx(at,regAddress,&rData,1);
    
    return rData;
}

/*向AT25XXX写入单个字节*/
void WriteByteToAT25xxx(At25ObjectType *at,uint32_t regAddress,uint8_t data)
{
    WriteBytesToAT25xxx(at,regAddress,&data,1);
}

/* AT25XXX设置写使能所存器*/
void SetWriteEnableLatchForAT25xxx(At25ObjectType *at)
{
    uint8_t opCode=AT25_WREN;
    
    at->ChipSelect(AT25CS_Enable);
    at->Write(&opCode,1);
    at->ChipSelect(AT25CS_Enable);
    
    ReadStatusForAT25xxx(at);
}

/* AT25XXX复位写使能所存器*/
void ResetWriteEnableLatchForAT25xxx(At25ObjectType *at)
{
    uint8_t opCode=AT25_WRDI;
    
    at->ChipSelect(AT25CS_Enable);
    at->Write(&opCode,1);
    at->ChipSelect(AT25CS_Enable);
    
    ReadStatusForAT25xxx(at);
}

/*读AT25xxx状态寄存器*/
void ReadStatusForAT25xxx(At25ObjectType *at)
{
    uint8_t opCode=AT25_RDSR;
    uint8_t status;
    
    at->ChipSelect(AT25CS_Enable);
    at->Write(&opCode,1);
    at->Delayms(1);
    at->Read(&status,1);
    at->ChipSelect(AT25CS_Enable);
    
    at->status=status;
}

/*写AT25xxx状态寄存器*/
void WriteStatusForAT25xx(At25ObjectType *at,uint8_t cmd)
{
    uint8_t data[2];
    
    data[0]=AT25_WRSR;
    data[1]=cmd;
    
    if(((at->status)&0x02)!=0x02)
    {
        SetWriteEnableLatchForAT25xxx(at);
    }
    
    if((((at->status)&AT25_WPEN)!=AT25_WPEN)&&(at->WP!=NULL))
    {
        at->WP(AT25WP_Disable);
    }
    
    at->ChipSelect(AT25CS_Enable);
    at->Write(data,2);
    at->ChipSelect(AT25CS_Disable);
    
    ReadStatusForAT25xxx(at);
    
    if(at->WP!=NULL)
    {
        at->WP(AT25WP_Enable);
    }
}

/*从AT25xxx读取数据*/
void ReadBytesFromAT25xxx(At25ObjectType *at,uint32_t regAddress,uint8_t *rData,uint16_t rSize)
{
    uint8_t data[4];
    uint16_t index=0;
    uint8_t temp;
    uint16_t size=0;
    
    data[index++]=AT25_READ;
    
    if(at->memAddLength==AT258BitMemAdd)
    {
        data[index++]=(uint8_t)regAddress;
        
        if(at->mode==AT25040B)
        {
            temp=(uint8_t)(regAddress>>8);
            data[0]|=((temp&0x01)<<3);
        }
    }
    else if(at->memAddLength==AT2516BitMemAdd)
    {
        data[index++]=(uint8_t)(regAddress>>8);
        data[index++]=(uint8_t)regAddress;
    }
    else
    {
        data[index++]=(uint8_t)(regAddress>>16);
        data[index++]=(uint8_t)(regAddress>>8);
        data[index++]=(uint8_t)regAddress;
    }
    
    temp=(uint8_t)(regAddress&regAddMask[at->mode]);
    if((rSize<=pageBytes[at->mode])&&(rSize<=(pageBytes[at->mode]-temp)))
    {
        size=rSize;
    }
    else
    {
        size=pageBytes[at->mode]-temp;
    }
    
    at->ChipSelect(AT25CS_Enable);
    at->Write(data,index);
    at->Delayms(1);
    at->Read(rData,size);
    at->ChipSelect(AT25CS_Disable);
}

/*向AT25xxx写入数据*/
void WriteBytesToAT25xxx(At25ObjectType *at,uint32_t regAddress,uint8_t *wData,uint16_t wSize)
{
    uint8_t data[128];
    uint16_t index=0;
    uint8_t temp;
    uint16_t size=0;
    
    data[index++]=AT25_WRITE;
    
    if(at->memAddLength==AT258BitMemAdd)
    {
        data[index++]=(uint8_t)regAddress;
        
        if(at->mode==AT25040B)
        {
            temp=(uint8_t)(regAddress>>8);
            data[0]|=((temp&0x01)<<3);
        }
    }
    else if(at->memAddLength==AT2516BitMemAdd)
    {
        data[index++]=(uint8_t)(regAddress>>8);
        data[index++]=(uint8_t)regAddress;
    }
    else
    {
        data[index++]=(uint8_t)(regAddress>>16);
        data[index++]=(uint8_t)(regAddress>>8);
        data[index++]=(uint8_t)regAddress;
    }
    
    temp=(uint8_t)(regAddress&regAddMask[at->mode]);
    if((wSize<=pageBytes[at->mode])&&(wSize<=(pageBytes[at->mode]-temp)))
    {
        size=wSize;
    }
    else
    {
        size=pageBytes[at->mode]-temp;
    }
    
    for(int i=0;i<size;i++)
    {
        data[index++]=wData[i];
    }
    
    if(((at->status)&0x02)!=0x02)
    {
        SetWriteEnableLatchForAT25xxx(at);
    }
    
    if(((at->status)&0x0C)!=0x00)
    {
        WriteStatusForAT25xx(at,at->status|AT25_BPNONE);
    }
    
    at->ChipSelect(AT25CS_Enable);
    at->Write(data,index);
    at->ChipSelect(AT25CS_Disable);
    
    WriteStatusForAT25xx(at,at->status|AT25_BPALL);
}

/* 初始化AT25XXX对象 */
void At25xxxInitialization(At25ObjectType *at,  //AT25XXX对象实体
                           At25ModeType mode,   //AT25XXX对象类型
                           AT25Read read,       //读AT25XXX对象操作指针
                           AT25Write write,     //写AT25XXX对象操作指针
                           AT25Delayms delayms, //延时操作指针
                           AT25ChipSelect cs    //片选操作函数指针
                               )
{
    if((at==NULL)||(read==NULL)||(write==NULL)||(delayms==NULL))
    {
        return;
    }
    at->Read=read;
    at->Write=write;
    at->Delayms=delayms;
    
    if(cs!=NULL)
    {
        at->ChipSelect=cs;
    }
    else
    {
        at->ChipSelect=AT25ChipSelectDefault;
    }
    
    if(mode>=AT25Number)
    {
        return;
    }
    at->mode=mode;
    
    if(mode<AT25080B)
    {
        at->memAddLength=AT258BitMemAdd;
    }
    else if(mode<AT25M01)
    {
        at->memAddLength=AT2516BitMemAdd;
    }
    else
    {
        at->memAddLength=AT2524BitMemAdd;
    }
    
    ReadStatusForAT25xxx(at);
    
    //写允许
    SetWriteEnableLatchForAT25xxx(at);
    
    uint8_t cmd;
    //使能写保护，保护全部区域
    cmd=at->status|AT25_WPEN|AT25_BPALL;
    WriteStatusForAT25xx(at,cmd);
    
    ReadStatusForAT25xxx(at);
}

/*缺省片选处理函数*/
static void AT25ChipSelectDefault(AT25xxxCSType cs)
{
    //用于在SPI通讯时，片选信号硬件电路选中的情况
    return;
}
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
