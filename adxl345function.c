/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：adxl345function.c                                              **/
/** 版    本：V1.1.0                                                         **/
/** 简    介：用于实现adxl345 3轴数字加速度计的操作，采用I2C或SPI通讯        **/
/**           CS引脚上拉至VDD，I2C模式使能。                                 **/
/**           SPI模式下， CS引脚由总线主机控制。                             **/
/**           寄存器地址分配如下：                                           **/
/**           采用SPI通讯接口时，最大SPI时钟速度为5 MHz，时序方案按照时钟极  **/
/**           性(CPOL)= 1、时钟相位(CPHA)= 1执行。                           **/
/**           采用I2C通讯接口时，ALT ADDRESS引脚处于高电平，器件的7位I2C地址 **/
/**           是0x1D，随后为R/W位。这转化为0x3A写入，0x3B读取。              **/
/**           通过ALT ADDRESS引脚(引脚12)接地，可以选择备用I2C地址0x53(随    **/
/**           后为R/W位)。这转化为0xA6写入，0xA7读取。根据应用实际配置       **/
/**           寄存器地址分配如下：                                           **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |十六进制|十进制|     名称     |类型| 复位值 | 描述                   | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x00  |   0  |    DEVID     | R  |11100101|器件ID                  | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x1D  |  29  |  THRESH_TAP  | R/W|00000000|敲击阈值                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x1E  |  30  |     OFSX     | R/W|00000000|X轴偏移                 | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x1F  |  31  |     OFSY     | R/W|00000000|Y轴偏移                 | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x20  |  32  |     OFSZ     | R/W|00000000|Z轴偏移                 | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x21  |  33  |     DUR      | R/W|00000000|敲击持续时间            | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x22  |  34  |    Latent    | R/W|00000000|敲击延迟                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x23  |  35  |    Window    | R/W|00000000|敲击窗口                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x24  |  36  |  THRESH_ACT  | R/W|00000000|活动阈值                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x25  |  37  | THRESH_INACT | R/W|00000000|静止阈值                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x26  |  38  |  TIME_INACT  | R/W|00000000|静止时间                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x27  |  39  | ACT_INACT_CTL| R/W|00000000|轴使能控制活动和静止检测| **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x28  |  40  |   THRESH_FF  | R/W|00000000|自由落体阈值            | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x29  |  41  |   TIME_FF    | R/W|00000000|自由落体时间            | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x2A  |  42  |   TAP_AXES   | R/W|00000000|单击/双击轴控制         | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x2B  |  43  |ACT_TAP_STATUS| R  |00000000|单击/双击源             | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x2C  |  44  |   BW_RATE    | R/W|00001010|数据速率及功率模式控制  | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x2D  |  45  |  POWER_CTL   | R/W|00000000|省电特性控制            | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x2E  |  46  |  INT_ENABLE  | R/W|00000000|中断使能控制            | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x2F  |  47  |   INT_MAP    | R/W|00000000|中断映射控制            | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x30  |  48  |   INT_SOURCE | R  |00000010|中断源                  | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x31  |  49  | DATA_FORMAT  | R/W|00000000|数据格式控制            | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x32  |  50  |    DATAX0    | R  |00000000|X轴数据0                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x33  |  51  |    DATAX1    | R  |00000000|X轴数据1                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x34  |  52  |    DATAY0    | R  |00000000|Y轴数据0                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x35  |  53  |    DATAY1    | R  |00000000|Y轴数据1                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x36  |  54  |    DATAZ0    | R  |00000000|Z轴数据0                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x37  |  55  |    DATAZ1    | R  |00000000|Z轴数据1                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x38  |  56  |   FIFO_CTL   | R/W|00000000|FIFO控制                | **/
/**  +--------+------+--------------+----+--------+------------------------+ **/
/**  |  0x39  |  57  |  FIFO_STATUS | R  |00000000|FIFO状态                | **/
/**  +--------+------+-------------------+--------+------------------------+ **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-07-17          木南              创建文件               **/
/**     V1.1.0  2019-08-07          木南              修改为基于对象的操作   **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "adxl345function.h"

/* 读取ADXL345的寄存器 */
static uint8_t Adxl345ReadRegister(Adxl345ObjectType *adxl,uint8_t regAdd);
/* 写ADXL345的寄存器 */
static void Adxl345WriteRegister(Adxl345ObjectType *adxl,uint8_t regAdd,uint8_t wData);
/* 读取多个寄存器的值 */
static void Adxl345ReadMultiReg(Adxl345ObjectType *adxl,uint8_t startRegAdd,uint8_t *rData,uint16_t rSize);
/* 定义一个默认的片选处理函数，以便在硬件使能是使用 */
static void Adxl345ChipSelect(Adxl345CSType en);

/*读取数据值，分辨率(3.9 mg/LSB)*/
void GetValueFromAdxl345(Adxl345ObjectType *adxl)
{
    uint8_t devID = 0;
    uint8_t dataTemp[6];
    
    /*读取设备ID，在每次操作前读一次*/
    devID=Adxl345ReadRegister(adxl,REG_DEVID);
    if(adxl->devID!=devID)
    {
        return;
    }
    adxl->Delayus(200);
    
    /*读取三轴数据值*/
    Adxl345ReadMultiReg(adxl,REG_DATAX0,dataTemp,6);
    
    /*数据解析*/
    adxl->incidence_X = (int16_t)(dataTemp[0] + ((uint16_t)dataTemp[1] << 8));
    adxl->incidence_Y = (int16_t)(dataTemp[2] + ((uint16_t)dataTemp[3] << 8));
    adxl->incidence_Z = (int16_t)(dataTemp[4] + ((uint16_t)dataTemp[5] << 8));
    
    adxl->incidence_Xf = (float)(adxl->incidence_X * 0.0039);
    adxl->incidence_Yf = (float)(adxl->incidence_Y * 0.0039);
    adxl->incidence_Zf = (float)(adxl->incidence_Z * 0.0039);
}

/*对ADXL345进行初始化配置*/
void Adxl345Initialization(Adxl345ObjectType *adxl,uint8_t devAdd,
                           Adxl345PortType port,
                           Adxl345ReadBytesType read,
                           Adxl345WriteBytesType write,
                           Adxl345ChipSelectType cs,
                           Adxl345DelayType delay)
{
    uint8_t devID=0;
    uint8_t setValue=0;
    
    if((adxl==NULL)||(read==NULL)||(write==NULL)||(delay==NULL))
    {
        return;
    }
    
    if(port==ADXL345_SPI)
    {
        if(cs==NULL)
        {
            adxl->ChipSelect=Adxl345ChipSelect;
        }
        else
        {
            adxl->ChipSelect=cs;
        }
        adxl->devAddress=0x00;
    }
    else
    {
        if((devAdd==0xA6)||(devAdd==0x3A))
        {
            adxl->devAddress=devAdd;
        }
        else if((devAdd==0x53)||(devAdd==0x1D))
        {
            adxl->devAddress=(devAdd<<1);
        }
        else
        {
            adxl->devAddress=0x00;
        }
        adxl->ChipSelect=NULL;
    }
    
    adxl->port=port;
    adxl->devID=0xE5;
    adxl->incidence_X=0;
    adxl->incidence_Xf=0.0;
    adxl->incidence_Y=0;
    adxl->incidence_Yf=0.0;
    adxl->incidence_Z=0;
    adxl->incidence_Zf=0.0;
    
    adxl->ReadBytes=read;
    adxl->WriteBytes=write;
    adxl->Delayus=delay;
    
    /*读取设备ID，在每次操作前读一次*/
    devID=Adxl345ReadRegister(adxl,REG_DEVID);
    if(adxl->devID!=devID)
    {
        return;
    }
    adxl->Delayus(300);
    
    /*配置数据格式*/
    setValue = 0x2B;
    Adxl345WriteRegister(adxl,REG_DATA_FORMAT,setValue);
    adxl->Delayus(50);
    
    /*配置数据速率及功率模式*/
    setValue = 0x0A;
    Adxl345WriteRegister(adxl,REG_BW_RATE,setValue);
    adxl->Delayus(50);
    
    /*配置电源特性*/
    setValue = 0x28;
    Adxl345WriteRegister(adxl,REG_POWER_CTL,setValue);
    adxl->Delayus(50);
    
    /*配置中断使能*/
    setValue = 0;
    Adxl345WriteRegister(adxl,REG_INT_ENABLE,setValue);
    adxl->Delayus(50);
    
    /*配置X轴偏移*/
    Adxl345WriteRegister(adxl,REG_OFSX,setValue);
    adxl->Delayus(50);
    
    /*配置Y轴偏移*/
    Adxl345WriteRegister(adxl,REG_OFSY,setValue);
    adxl->Delayus(50);
    
    /*配置Z轴偏移*/
    Adxl345WriteRegister(adxl,REG_OFSZ,setValue);
    adxl->Delayus(500);
}

/* 读取ADXL345的寄存器 */
static uint8_t Adxl345ReadRegister(Adxl345ObjectType *adxl,uint8_t regAdd)
{
    uint8_t regValue=0;
    
    if(adxl->port==ADXL345_SPI)
    {
        adxl->ChipSelect(ADXL345CS_Enable);
        adxl->Delayus(50);
        adxl->ReadBytes(adxl,regAdd,&regValue,1);
        adxl->Delayus(50);
        adxl->ChipSelect(ADXL345CS_Disable);
    }
    else
    {
        adxl->ReadBytes(adxl,regAdd,&regValue,1);
    }
    
    return regValue;
}

/* 写ADXL345的寄存器 */
static void Adxl345WriteRegister(Adxl345ObjectType *adxl,uint8_t regAdd,uint8_t wData)
{
    if(adxl->port==ADXL345_SPI)
    {
        adxl->ChipSelect(ADXL345CS_Enable);
        adxl->Delayus(50);
        adxl->WriteBytes(adxl,regAdd,&wData,1);
        adxl->Delayus(50);
        adxl->ChipSelect(ADXL345CS_Disable);
    }
    else
    {
        adxl->WriteBytes(adxl,regAdd,&wData,1);
    }
}

/* 读取多个寄存器的值 */
static void Adxl345ReadMultiReg(Adxl345ObjectType *adxl,uint8_t startRegAdd,uint8_t *rData,uint16_t rSize)
{
    if(adxl->port==ADXL345_SPI)
    {
        adxl->ChipSelect(ADXL345CS_Enable);
        adxl->Delayus(50);
        
        for(int i=0;i<rSize;i++)
        {
            adxl->ReadBytes(adxl,startRegAdd+i,&rData[i],1);
        }
        
        adxl->Delayus(50);
        adxl->ChipSelect(ADXL345CS_Disable);
    }
    else
    {
        adxl->ReadBytes(adxl,startRegAdd,rData,rSize);
    }
}

/* 定义一个默认的片选处理函数，以便在硬件使能是使用 */
static void Adxl345ChipSelect(Adxl345CSType en)
{
    return;
}
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
