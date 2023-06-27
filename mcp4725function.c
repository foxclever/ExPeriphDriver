/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：mcp4725function.c                                              **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：声明和定义适用于MCP4725数模转换芯片的函数和操作。              **/
/**           采用I2C接口接口，MCP4725数模转换芯片写命令如下：               **/
/**     +--+--+--+-----------------------------+----------------+            **/
/**     |C2|C1|C0|        Command Name         |    Function    |            **/
/**     +--+--+--+-----------------------------+----------------+            **/
/**     |0 |0 |X |          Fast Mode          |    快速模式    |            **/
/**     +--+--+--+-----------------------------+----------------+            **/
/**     |0 |0 |X |          Fast Mode          |    快速模式    |            **/
/**     +--+--+--+-----------------------------+----------------+            **/
/**     |0 |1 |0 |      Write DAC Register     |    写寄存器    |            **/
/**     +--+--+--+-----------------------------+----------------+            **/
/**     |0 |1 |1 |Write DAC Register and EEPROM|写寄存器和EEPROM|            **/
/**     +--+--+--+-----------------------------+----------------+            **/
/**     |1 |0 |0 |          Reserved           |      保留      |            **/
/**     +--+--+--+-----------------------------+----------------+            **/
/**     |1 |0 |1 |          Reserved           |      保留      |            **/
/**     +--+--+--+-----------------------------+----------------+            **/
/**     |1 |1 |0 |          Reserved           |      保留      |            **/
/**     +--+--+--+-----------------------------+----------------+            **/
/**     |1 |1 |1 |          Reserved           |      保留      |            **/
/**     +--+--+--+-----------------------------+----------------+            **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2021-02-15          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "mcp4725function.h"

#define Fast_Mode   0x00
#define Write_DAC_Register  0x40
#define Write_DAC_Register_EEPROM   0x60

/*MCP4725通用操作*/
static void Mcp4725GeneralCall(Mcp4725ObjectType *mcp,uint8_t cmd);

/*设置MCP4725输出*/
void Mcp4725SetDatas(Mcp4725ObjectType *mcp,Mcp4725CommandType cmd,uint16_t data)
{
    uint8_t wData[3];
    uint8_t pdMode=0;
    uint16_t wSize=0;
    uint8_t command[]={Fast_Mode,Write_DAC_Register,Write_DAC_Register_EEPROM};
    
    pdMode=(uint8_t)(mcp->pdMode);
    
    if(cmd==Mcp4725_Fast_Mode)  //快速模式
    {
        wData[1]=(uint8_t)data;
        wData[0]=(uint8_t)(data>>8);
        wData[0]=wData[0]|command[cmd];
        wData[0]=wData[0]|(pdMode<<4);
        wSize=2;
    }
    else    //普通模式
    {
        wData[0]=command[cmd];
        wData[0]=wData[0]|(pdMode<<1);
        wData[1]=(uint8_t)(data>>4);
        wData[2]=(uint8_t)(data<<4);
        wSize=3;
    }
    
    mcp->Write(mcp,wData,wSize);
}

/*读取MCP4725数据*/
void Mcp4725GetDatas(Mcp4725ObjectType *mcp,uint8_t *rData)
{
    mcp->Read(mcp,rData,5);
}

/*MCP4725初始化配置*/
void Mcp4725Initialization(Mcp4725ObjectType *mcp,  //MCP4725对象变量
                           uint8_t slaveAddress,    //从站设备的地址
                           Mcp4725PDModeType pdMode,//掉电操作模式
                           Mcp4725Write write,      //写数据函数指针
                           Mcp4725Read read         //读数据函数指针
                               )
{
    if((mcp==NULL)||(write==NULL)||(read==NULL))
    {
        return;
    }
    
    mcp->Write=write;
    mcp->Read=read;
    
    if((slaveAddress==0x60)||(slaveAddress==0x61))
    {
        mcp->devAddress=(slaveAddress<<1);
    }
    else if((slaveAddress==0xC0)||(slaveAddress==0xC2))
    {
        mcp->devAddress=slaveAddress;
    }
    else
    {
        mcp->devAddress=0x00;
    }
    
    mcp->pdMode=pdMode;
    
}

/*执行MCP4725复位*/
void Mcp4725Reset(Mcp4725ObjectType *mcp)
{
    Mcp4725GeneralCall(mcp,0x06);
}

/*执行MCP4725唤醒*/
void Mcp4725Wakeup(Mcp4725ObjectType *mcp)
{
    Mcp4725GeneralCall(mcp,0x09);
}

/*MCP4725通用操作*/
static void Mcp4725GeneralCall(Mcp4725ObjectType *mcp,uint8_t cmd)
{
    uint8_t wData=cmd;
    uint8_t devAdd=mcp->devAddress;
    
    mcp->devAddress=0x00;
    
    mcp->Write(mcp,&wData,1);
    
    mcp->devAddress=devAdd;
}

/*********** (C) COPYRIGHT 1999-2021 Moonan Technology *********END OF FILE****/