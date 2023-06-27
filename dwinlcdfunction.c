/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名   dwinlcdfunction.c                                               **/
/** 版  本   V1.1.0                                                          **/
/** 简  介   实现迪文LCD显示屏读写操作                                       **/
/**          帧格式：帧头+数据长度+指令+数据+CRC校验                         **/
/**          其中：帧头2个字节，有配置文件决定帧头的内容                     **/
/**                数据长度1个字节，包括指令、数据和校验                     **/
/**                指令有5个：0x80,0x81,0x82,0x83,0x84,对应不同操作          **/
/**                     0x80写寄存器                                         **/
/**                     0x81读寄存器                                         **/
/**                     0x82写变量存储器                                     **/
/**                     0x83度变量存储器                                     **/
/**                     0x84写曲线显示                                       **/
/**                数据的长度因操作而定                                      **/
/**                CRC校验是否启用，在配置文件中设定                         **/
/**          寄存器的地址为1个字节，变量的地址为2个字节                      **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2014-01-18          木南              创建文件               **/
/**     V1.1.0  2019-08-01          木南              修改为基于对象的操作   **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "dwinlcdfunction.h"

/*写寄存器数据，一次最多允许写4个字节，即length<=4*/
static void SetRegisterDataToDwinLCD(DwinObjectType *dwin,uint8_t regAddress,uint8_t *txData,uint16_t length);
/*读寄存器数据*/
static void GetRegisterDataFromDwinLCD(DwinObjectType *dwin,uint8_t regAddress,uint8_t readByteLength);
/* CRC16校验（X16+X15+X2+1）*/
static uint16_t CalcDwinCRC16(uint8_t *buf,uint16_t length);
/*将接收到的数据加入缓存*/
static void BufferDataEnQueue(DwinObjectType *dwin,uint8_t rData);
/*从缓存中读取数据*/
static uint16_t BufferDataDeQueue(DwinObjectType *dwin,uint8_t *rxBuf);

/*写数据变量存储器，一次最多允许写47个字，即length<=249*/
void WriteFlashDataToDwinLCD(DwinObjectType *dwin,uint16_t startAddress,uint8_t *txData,uint16_t length)
{
    /*命令的长度由帧头（2个字节）+数据长度（1个字节）+指令（1个字节）+起始地址（2个字节）+数据（长度为length）+CRC校验（2个字节）*/
    uint16_t cmd_Length=length+6;
    uint8_t cmd_VAR_Write[255]={0x00};
    cmd_VAR_Write[0]=0x5A;
    cmd_VAR_Write[1]=0xA5;
    cmd_VAR_Write[2]=(uint8_t)(length+3);
    cmd_VAR_Write[3]= FC_VAR_Write;
    cmd_VAR_Write[4]=(uint8_t)(startAddress>>8);//起始地址
    cmd_VAR_Write[5]=(uint8_t)startAddress;//起始地址
    for(int dataIndex=0;dataIndex<length;dataIndex++)
    {
        cmd_VAR_Write[dataIndex+6]=txData[dataIndex];
    }
    
    if(dwin->checkMode>DwinNone)
    {
        uint16_t checkCode=CalcDwinCRC16(&cmd_VAR_Write[3],length+2);
        cmd_VAR_Write[length+6]=(uint8_t)checkCode;
        cmd_VAR_Write[length+7]=(uint8_t)(checkCode>>8);
        cmd_Length=cmd_Length+2;
        cmd_VAR_Write[2] += 2;
    }
    
    dwin->SendData(cmd_VAR_Write,cmd_Length);
}

/*读变量存储器数据*/
void ReadFlashDataFromDwinLCD(DwinObjectType *dwin,uint16_t startAddress,uint8_t readWordLength)
{
    /*命令的长度由帧头（2个字节）+数据长度（1个字节）+指令（1个字节）+起始地址（2个字节）+读取的字长度（1个字节）+CRC校验（2个字节）*/
    uint16_t cmd_Length=7;
    uint8_t cmd_VAR_Read[]={0x5A,0xA5,0x04,FC_VAR_Read,0x00,0x00,0x00,0x00,0x00};//读数据命令
    cmd_VAR_Read[4]=(uint8_t)(startAddress>>8);//起始地址
    cmd_VAR_Read[5]=(uint8_t)startAddress;//起始地址
    cmd_VAR_Read[6]=readWordLength;//读取长度
    
    if(dwin->checkMode>DwinNone)
    {
        uint16_t checkCode=CalcDwinCRC16(&cmd_VAR_Read[3],4);
        cmd_VAR_Read[7]=(uint8_t)checkCode;
        cmd_VAR_Read[8]=(uint8_t)(checkCode>>8);
        cmd_Length=cmd_Length+2;
        cmd_VAR_Read[2] += 2;
    }
    
    dwin->SendData(cmd_VAR_Read,cmd_Length);
}

/*写曲线缓冲区，一次最多允许写8个字，即length<=16*/
void WriteCurveToDwinLCD(DwinObjectType *dwin,uint8_t *txData,uint16_t length,uint8_t channelMode)
{
    /*命令的长度由帧头（2个字节）+数据长度（1个字节）+指令（1个字节）+通道模式（1个字节）+数据（length，最多8个字）+CRC校验（2个字节）*/
    uint16_t cmd_Length=length+5;
    uint8_t cmd_Curve_Write[23];//写曲线缓冲区命令
    cmd_Curve_Write[0]=0x5A;
    cmd_Curve_Write[1]=0xA5;
    cmd_Curve_Write[2]=(uint8_t)(length+2);
    cmd_Curve_Write[3]= FC_Curve_Write;
    cmd_Curve_Write[4]=channelMode;
    for(int dataIndex=0;dataIndex<length;dataIndex++)
    {
        cmd_Curve_Write[dataIndex+5]=txData[dataIndex];
    }
    
    if(dwin->checkMode>DwinNone)
    {
        uint16_t checkCode=CalcDwinCRC16(&cmd_Curve_Write[3],length+2);
        cmd_Curve_Write[length+5]=(uint8_t)checkCode;
        cmd_Curve_Write[length+6]=(uint8_t)(checkCode>>8);
        cmd_Length=cmd_Length+2;
        cmd_Curve_Write[2] += 2;
    }
    
    dwin->SendData(cmd_Curve_Write,cmd_Length);
}

/*读取LCD系统时间*/
void GetDateTimeFromDwinLCD(DwinObjectType *dwin)
{
    if(dwin->sort==DGUS)
    {
        uint8_t regAddress=0x20;      /*RTC寄存器地址为0x20*/
        uint8_t readByteLength=7;     /*日期格式：YY：MM：DD：WW：HH：MM：SS*/
        
        GetRegisterDataFromDwinLCD(dwin,regAddress,readByteLength);
    }
    else
    {
        uint16_t startAddress=0x0010;
        uint8_t readWordLength=0x04;
        
        ReadFlashDataFromDwinLCD(dwin,startAddress,readWordLength);
    }
}

/*校准LCD系统时间*/
void CalibrationDateTimeForDwinLCD(DwinObjectType *dwin,uint8_t *dateTime)
{
    uint8_t txData[8];
    uint16_t length=8;
    
    if(dwin->sort==DGUS)
    {
        uint8_t regAddress=0x1F;
        
        txData[0]=0x5A;
        txData[1]=dateTime[0];
        txData[2]=dateTime[1];
        txData[3]=dateTime[2];
        txData[4]=0x00;
        txData[5]=dateTime[3];
        txData[6]=dateTime[4];
        txData[7]=dateTime[5];
        SetRegisterDataToDwinLCD(dwin,regAddress,txData,length);
    }
    else
    {
        uint16_t startAddress=0x0010;
        txData[0]=dateTime[0];
        txData[1]=dateTime[1];
        txData[2]=dateTime[2];
        txData[3]=0x00;
        txData[4]=dateTime[3];
        txData[5]=dateTime[4];
        txData[6]=dateTime[5];
        txData[7]=0x00;
        WriteFlashDataToDwinLCD(dwin,startAddress,txData,length);
    }
}

/*音乐播放控制playNum为0时表示停止播放*/
void HandleDwinLCDToPlayMusic(DwinObjectType *dwin,uint16_t playStart,uint16_t playNum,uint16_t volume)
{
    uint8_t regAddress=0x50;
    uint16_t length;
    uint8_t txData[5];
    
    if(playNum>0)
    {
        length=5;
        txData[0]=0x5A;
        txData[1]=playStart;
        txData[2]=playNum;
        txData[3]=0x5A;
        txData[4]=(uint8_t)(volume*64/100);
    }
    else
    {
        length=3;
        txData[0]=0x5A;
        txData[1]=playStart;
        txData[2]=0x00;
    }
    
    SetRegisterDataToDwinLCD(dwin,regAddress,txData,length);
}

/*设置屏显示画面*/
void SetDwinLCDDisplay(DwinObjectType *dwin,uint16_t picID)
{
    uint8_t regAddress=0x03;      //画面显示寄存器
    uint16_t length=2;
    uint8_t txData[2];
    txData[0]=(picID>>8);
    txData[1]=picID;
    SetRegisterDataToDwinLCD(dwin,regAddress,txData,length);
}

/*写寄存器数据，一次最多允许写16个字节，即length<=16*/
static void SetRegisterDataToDwinLCD(DwinObjectType *dwin,uint8_t regAddress,uint8_t *txData,uint16_t length)
{
    /*命令的长度由帧头（2个字节）+数据长度（1个字节）+指令（1个字节）+寄存器地址（1个字节）+写的数据(最多16字节)+CRC校验（2个字节）*/
    uint16_t cmd_Length=length+5;
    uint8_t cmd_Reg_Write[23];
    cmd_Reg_Write[0]=0x5A;
    cmd_Reg_Write[1]=0xA5;
    cmd_Reg_Write[2]=(uint8_t)(length+2);
    cmd_Reg_Write[3]= FC_REG_Write;
    cmd_Reg_Write[4]=regAddress;
    for(int dataIndex=0;dataIndex<length;dataIndex++)
    {
        cmd_Reg_Write[dataIndex+5]=txData[dataIndex];
    }
    
    if(dwin->checkMode>DwinNone)
    {
        uint16_t checkCode=CalcDwinCRC16(&cmd_Reg_Write[3],length+2);
        cmd_Reg_Write[length+5]=(uint8_t)checkCode;
        cmd_Reg_Write[length+6]=(uint8_t)(checkCode>>8);
        cmd_Length=cmd_Length+2;
    }
    
    dwin->SendData(cmd_Reg_Write,cmd_Length);
}

/*读寄存器数据*/
static void GetRegisterDataFromDwinLCD(DwinObjectType *dwin,uint8_t regAddress,uint8_t readByteLength)
{
    /*命令的长度由帧头（2个字节）+数据长度（1个字节）+指令（1个字节）+寄存器地址（1个字节）+读取寄存器的字节长度（1个字节）+CRC校验（2个字节）*/
    uint16_t cmd_Length=6;
    uint8_t cmd_Reg_Read[]={0x5A,0xA5,0x03,FC_REG_Read,0x00,0x00,0x00,0x00};//读数据命令
    cmd_Reg_Read[4]=regAddress;
    cmd_Reg_Read[5]=readByteLength;
    
    if(dwin->checkMode>DwinNone)
    {
        uint16_t checkCode=CalcDwinCRC16(&cmd_Reg_Read[3],3);
        cmd_Reg_Read[6]=(uint8_t)checkCode;
        cmd_Reg_Read[7]=(uint8_t)(checkCode>>8);
        cmd_Length=cmd_Length+2;
    }
    
    dwin->SendData(cmd_Reg_Read,cmd_Length);
}

/* CRC16校验（X16+X15+X2+1）*/
static uint16_t CalcDwinCRC16(uint8_t *buf,uint16_t length)
{
    uint16_t crc16,data,val;
    
    crc16 = 0x0000;
    
    for(int i=0;i<length;i++)
    {
        if((i % 8) == 0)
        {
            data = (*buf++)<<8;
        }
        val = crc16 ^ data;
        crc16 = crc16<<1;
        data = data <<1;
        if(val&0x8000)
        {
            crc16 = crc16 ^ 0x8005;
        }
    }
    return crc16;
}

/* 初始化迪文串口屏对象 */
void DwinInitialization(DwinObjectType *dwin,
                        DwinCheckCodeType checkMode,
                        DwinSortType sort,
                        SendDataForDwinType SendData
                            )
{
    if((dwin==NULL)||(SendData==NULL))
    {
        return;
    }
    
    dwin->checkMode=checkMode;
    dwin->SendData=SendData;
    
    dwin->sort=sort;
    
    dwin->GetRegister=GetRegisterDataFromDwinLCD;
    dwin->SetRegister=SetRegisterDataToDwinLCD;
    
    dwin->rxBuffer.pRead=0;
    dwin->rxBuffer.pWrite=0;
    
    dwin->rxBuffer.DeQueue=BufferDataDeQueue;
    dwin->rxBuffer.EnQueue=BufferDataEnQueue;
    
    for(int i=0;i<RxBufferLength;i++)
    {
        dwin->rxBuffer.queue[i]=0x00;
    }
}

/*从缓存中读取数据*/
static uint16_t BufferDataDeQueue(DwinObjectType *dwin,uint8_t *rxBuf)
{
    uint16_t length=0;
    uint8_t pTemp;
    
    if(dwin->rxBuffer.pRead==dwin->rxBuffer.pWrite)
    {
        length=0;
    }
    else
    {
        while((dwin->rxBuffer.queue[dwin->rxBuffer.pRead]!=0x5A)||(dwin->rxBuffer.queue[dwin->rxBuffer.pRead+1]!=0xA5))
        {
            if(++(dwin->rxBuffer.pRead)>=(uint8_t)RxBufferLength)
            {
                dwin->rxBuffer.pRead=0;
            }
            
            if(dwin->rxBuffer.pRead==dwin->rxBuffer.pWrite)
            {
                length=0;
                //break;
                return length;
            }
        }
        
        pTemp=dwin->rxBuffer.pRead;
        
        do
        {
            rxBuf[length++]=dwin->rxBuffer.queue[dwin->rxBuffer.pRead];
            
            if(++(dwin->rxBuffer.pRead)>=(uint8_t)RxBufferLength)
            {
                dwin->rxBuffer.pRead=0;
            }
            
            if(dwin->rxBuffer.pRead==dwin->rxBuffer.pWrite)
            {
                if(length!=(rxBuf[2]+3))
                {
                    length=0;
                    dwin->rxBuffer.pRead=pTemp;
                }
                
                break;
            }
        }while((dwin->rxBuffer.queue[dwin->rxBuffer.pRead]!=0x5A)||(dwin->rxBuffer.queue[dwin->rxBuffer.pRead+1]!=0xA5));
        
    }
    return length;
}

/*将接收到的数据加入缓存*/
static void BufferDataEnQueue(DwinObjectType *dwin,uint8_t rData)
{
    dwin->rxBuffer.queue[dwin->rxBuffer.pWrite]=rData;

    if(++(dwin->rxBuffer.pWrite)>=(uint8_t)RxBufferLength)
    {
        dwin->rxBuffer.pWrite=0;
    }
}

/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
