/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：hp203bfunction.h                                               **/
/** 版    本：V1.0.1                                                         **/
/** 简    介：实现HP203B系列气压、高度传感器的功能                           **/
/**         +---------+-----------+------------+---------------------------+ **/
/**         |   名称  |十六进制编码|  二进制编码 |          描述           | **/
/**         +---------+-----------+------------+---------------------------+ **/
/**         | SOFT_RST|   0x06    | 0000  0110 |软复位命令                 | **/
/**         +---------+-----------+------------+---------------------------+ **/
/**         | ADC_CVT |    NA     |010_dsr_chnl|OSR设定，通道选择，执行转换| **/
/**         +---------+-----------+------------+---------------------------+ **/
/**         | READ_PT |   0x10    | 0001  0000 |读取温度和压力值           | **/
/**         +---------+-----------+------------+---------------------------+ **/
/**         | READ_AT |   0x11    | 0001  0001 |读取温度和海拔值           | **/
/**         +---------+-----------+------------+---------------------------+ **/
/**         | READ_P  |   0x30    | 0011  0000 |只读取压力值               | **/
/**         +---------+-----------+------------+---------------------------+ **/
/**         | READ_A  |   0x31    | 0011  0001 |只读取高度值               | **/
/**         +---------+-----------+------------+---------------------------+ **/
/**         | READ_T  |   0x32    | 0011  0010 |只读取温度值               | **/
/**         +---------+-----------+------------+---------------------------+ **/
/**         | ANA_CAL |   0x28    | 0010  1000 |重新校准内部模拟电路       | **/
/**         +---------+-----------+------------+---------------------------+ **/
/**         | READ_REG|    NA     |  10_addr   |读取控制寄存器             | **/
/**         +---------+-----------+------------+---------------------------+ **/
/**         |WRITE_REG|    NA     |  11_addr   |写入控制寄存器             | **/
/**         +---------+-----------+------------+---------------------------+ **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期          作者          说明                           **/
/**     V1.0.0  2022-09-25      木南      创建文件                           **/
/**                                                                          **/
/******************************************************************************/

#include "stddef.h"
#include "hp203bfunction.h"

/*定义控制寄存器地址*/
#define REG_ALT_OFF_LS_ADDRESS  0x00
#define REG_ALT_OFF_M_ADDRESS	0x01
#define REG_PA_H_TH_LS_ADDRESS	0x02
#define REG_PA_H_TH_M_ADDRESS	0x03
#define REG_PA_M_TH_LS_ADDRESS	0x04
#define REG_PA_M_TH_M_ADDRESS	0x05
#define REG_PA_L_TH_LS_ADDRESS	0x06
#define REG_PA_L_TH_MS_ADDRESS	0x07
#define REG_T_H_TH_ADDRESS	    0x08
#define REG_T_M_TH_ADDRESS	    0x09
#define REG_T_L_TH_ADDRESS	    0x0A
#define REG_INT_EN_ADDRESS	    0x0B
#define REG_INT_CFG_ADDRESS	    0x0C
#define REG_INT_SRC_ADDRESS	    0x0D
#define REG_INT_DIR_ADDRESS	    0x0E
#define REG_PARA_ADDRESS	    0X0F

/*操作命令*/
#define CMD_SOFT_RST    0x06    //软复位命令 
#define CMD_ADC_CVT     0x40    //OSR设定，通道选择，执行转换 
#define CMD_READ_PT     0x10    //读取温度和压力值 
#define CMD_READ_AT     0x11    //读取温度和海拔值 
#define CMD_READ_P      0x30    //只读取压力值 
#define CMD_READ_A      0x31    //只读取高度值 
#define CMD_READ_T      0x32    //只读取温度值 
#define CMD_ANA_CAL     0x28    //重新校准内部模拟电路 
#define CMD_READ_REG    0x80    //读取控制寄存器 
#define CMD_WRITE_REG   0xC0    //写入控制寄存器

/*HP203B对象初始化*/
void Hp203bInitialization(Hp203bObjectType *hp,
                          uint8_t deviceAddress,
                          Ph203bReceiveType recieve,
                          Ph203bTransmitType transmit)
{
    if((hp==NULL)||(recieve==NULL)||(transmit==NULL))
    {
        return;
    }
    
    hp->Receive=recieve;
    hp->Transmit=transmit;
    
    if((deviceAddress==0xEE)||(deviceAddress==0xEC))
    {
        hp->deviceAddress=deviceAddress;
    }
    else if((deviceAddress==0x77)||(deviceAddress==0x76))
    {
        hp->deviceAddress=(deviceAddress<<1);
    }
    else
    {
        hp->deviceAddress=0;
    }
    
    /*软复位命令*/
    Hp203bSoftReset(hp);
}

/*软复位命令*/
void Hp203bSoftReset(Hp203bObjectType *hp)
{
    uint8_t cmd=CMD_SOFT_RST;
    
    hp->Transmit(hp,&cmd,1);
}

/*OSR设定，通道选择，执行转换*/
void Hp203bConvertSetting(Hp203bObjectType *hp,Hp203bOSRType osr,Hp203bChannelType channel)
{
    uint8_t cmd=CMD_ADC_CVT|osr|channel;
    
    hp->Transmit(hp,&cmd,1);
}
                          
/*读取温度和压力值*/
void Hp203bReadTemperaturPressure(Hp203bObjectType *hp)
{
    uint8_t cmd=CMD_READ_PT;
    uint8_t rData[6];
    uint32_t temp=0;
    
    hp->Transmit(hp,&cmd,1);
    
    hp->Receive(hp,rData,6);
    
    hp->cTemperature=(rData[0]<<16)+(rData[1]<<8)+rData[2];
    
    if(rData[0]>0x0F)
    {
        hp->cTemperature=hp->cTemperature|0xFFF00000;
        temp=~(hp->cTemperature-0x01);
        hp->fTemperature=(float)0.0-(float)temp/(float)100.0;
    }
    else
    {
        hp->fTemperature=(float)hp->cTemperature/(float)100.0;
    }
    
    hp->cPressure=(rData[3]<<16)+(rData[4]<<8)+rData[5];
    hp->fPressure=(float)(hp->cPressure)/(float)100.0;
}

/*读取温度和海拔值*/
void Hp203bReadTemperaturAltitude(Hp203bObjectType *hp)
{
    uint8_t cmd=CMD_READ_AT;
    uint8_t rData[6];
    uint32_t temp=0;
    
    hp->Transmit(hp,&cmd,1);
    
    hp->Receive(hp,rData,6);
    
    hp->cTemperature=(rData[0]<<16)+(rData[1]<<8)+rData[2];
    
    if(rData[0]>0x0F)
    {
        hp->cTemperature=hp->cTemperature|0xFFF00000;
        temp=~(hp->cTemperature-0x01);
        hp->fTemperature=(float)0.0-(float)temp/(float)100.0;
    }
    else
    {
        hp->fTemperature=(float)hp->cTemperature/(float)100.0;
    }
    
    hp->cAltitude=(rData[3]<<16)+(rData[4]<<8)+rData[5];
    
    if(rData[3]>0x0F)
    {
        hp->cAltitude=hp->cAltitude|0xFFF00000;
        temp=~(hp->cAltitude-0x01);
        hp->fAltitude=(float)0.0-(float)temp/(float)100.0;
    }
    else
    {
        hp->fAltitude=(float)hp->cAltitude/(float)100.0;
    }
}

/*只读取压力值*/
void Hp203bReadPressure(Hp203bObjectType *hp)
{
    uint8_t cmd=CMD_READ_P;
    uint8_t rData[3];
    
    hp->Transmit(hp,&cmd,1);
    
    hp->Receive(hp,rData,3);

    hp->cPressure=(rData[0]<<16)+(rData[1]<<8)+rData[2];
    hp->fPressure=(float)(hp->cPressure)/(float)100.0;
}

/*只读取高度值*/
void Hp203bReadAltitude(Hp203bObjectType *hp)
{
    uint8_t cmd=CMD_READ_A;
    uint8_t rData[3];
    uint32_t temp=0;
    
    hp->Transmit(hp,&cmd,1);
    
    hp->Receive(hp,rData,3);
        
    hp->cAltitude=(rData[0]<<16)+(rData[1]<<8)+rData[2];
    
    if(rData[0]>0x0F)
    {
        hp->cAltitude=hp->cAltitude|0xFFF00000;
        temp=~(hp->cAltitude-0x01);
        hp->fAltitude=(float)0.0-(float)temp/(float)100.0;
    }
    else
    {
        hp->fAltitude=(float)hp->cAltitude/(float)100.0;
    }
}

/*只读取温度值*/
void Hp203bReadTemperatur(Hp203bObjectType *hp)
{
    uint8_t cmd=CMD_READ_T;
    uint8_t rData[3];
    uint32_t temp=0;
    
    hp->Transmit(hp,&cmd,1);
    
    hp->Receive(hp,rData,3);
    
    hp->cTemperature=(rData[0]<<16)+(rData[1]<<8)+rData[2];
    
    if(rData[0]>0x0F)
    {
        hp->cTemperature=hp->cTemperature|0xFFF00000;
        temp=~(hp->cTemperature-0x01);
        hp->fTemperature=(float)0.0-(float)temp/(float)100.0;
    }
    else
    {
        hp->fTemperature=(float)hp->cTemperature/(float)100.0;
    }
}

/*重新校准内部模拟电路*/
void Hp203bReadInternalCalibration(Hp203bObjectType *hp)
{
    uint8_t cmd=CMD_READ_REG;
    
    hp->Transmit(hp,&cmd,1);
}

/*读寄存器值*/
uint8_t Hp203bReadRegister(Hp203bObjectType *hp,Hp203bRegisterType reg)
{
    uint8_t cmd=CMD_ANA_CAL;
    uint8_t rData=0;
    
    hp->Transmit(hp,&cmd,1);
    
    hp->Receive(hp,&rData,1);
    
    return rData;
}

/*写寄存器值*/
void Hp203bWriteRegister(Hp203bObjectType *hp,Hp203bRegisterType reg,uint8_t value)
{
    uint8_t cmd[2];
    
    cmd[0]=CMD_WRITE_REG;
    cmd[1]=value;
    hp->Transmit(hp,cmd,2);
}

/*********** (C) COPYRIGHT 1999-2022 Moonan Technology *********END OF FILE****/
