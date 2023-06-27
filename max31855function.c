/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：max31855function.c                                             **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：声明和定义适用于MAX31855热偶数字转换芯片的函数和操作。         **/
/**           采用SPI接口，MAX31855寄存器格式如下：                          **/
/**  +---+--------------+----+------+--------------+----+-----+-----+------+ **/
/**  |   | 14位热偶温度 | RES|故障位| 12位内部温度 | RES| SCV | SCG |  OC- | **/
/**  +---+--------------+----+------+--------------+----+-----+-----+------+ **/
/**  |BIT|D31 D30 … D18 | D17|  D16 | D15 D14 … D4 | D3 |  D2 |  D1 |  D0  | **/
/**  +---+--------------+----+------+--------------+----+-----+-----+------+ **/
/**  | 值| 符号MSB … LSB|保留|1=故障| 符号MSB … LSB|保留|1=VCC|1=GND|1=Open| **/
/**  +---+--------------+----+------+--------------+----+-----+-----+------+ **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2021-02-15          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "max31855function.h"

/* 默认片选操作函数 */
static void DefaultChipSelect(Max31855CSType cs);
/*计算热偶温度*/
static float CalcMeasureTemperature(uint16_t tCode);
/*计算参考温度*/
static float CalcColdEndTemperature(uint16_t rCode);

/*获取MAX31855测量数据*/
void Max31855GetDatas(Max31855ObjectType *tc)
{
    uint8_t rData[4]={0};
    uint16_t tCode=0;
    uint16_t rCode=0;
    
    tc->ChipSelcet(Max31855CS_Enable);

    tc->ReadData(rData,4);
    
    tCode=(rData[0]<<8)+rData[1];
    tCode=(tCode>>2);
    
    rCode=(rData[2]<<8)+rData[3];
    rCode=(rCode>>4);
    
    tc->mTemperature=CalcMeasureTemperature(tCode);
    tc->rTemperature=CalcColdEndTemperature(rCode);
    
    tc->dataCode=(rData[0]<<24)+(rData[1]<<16)+(rData[2]<<8)+rData[3];
    
    tc->status=(((rData[1]&0x03)<<4)|(rData[3]&0x0F));
    
    tc->ChipSelcet(Max31855CS_Disable);
}

/*计算热偶温度*/
static float CalcMeasureTemperature(uint16_t tCode)
{
    float result=0.0;
    uint16_t code=0;
    
    if(tCode<=0x1FFF)
    {
        result=(float)tCode*(float)0.25;
    }
    else
    {
        code=~(tCode-1);
        code=code&0x3FFF;
        result=(float)0.0-(float)code*(float)0.25;
    }
    
    return result;
}

/*计算参考温度*/
static float CalcColdEndTemperature(uint16_t rCode)
{
    float result=0.0;
    uint16_t code=0;
    
    if(rCode<=0x7FF)
    {
        result=rCode*(float)0.0625;
    }
    else
    {
        code=~rCode;
        code=code&0xFFF;
        code=code+0x0001;
        result=(float)0.0-code*(float)0.0625;
    }
    
    return result;
}

/*初始化MAX31855对象*/
void Max31855Initialization(Max31855ObjectType *tc,
                            Max31855ReadDataType read,
                            Max31855ChipSelcetType cs
                                )
{
    if((tc==NULL)||(read==NULL))
    {
        return;
    }
    tc->ReadData=read;
    
    if(cs!=NULL)
    {
        tc->ChipSelcet=cs;
    }
    else
    {
        tc->ChipSelcet=DefaultChipSelect;
    }
    
    tc->status=0;
    tc->dataCode=0;
    tc->mTemperature=0.0;
    tc->rTemperature=0.0;
    
    tc->ChipSelcet(Max31855CS_Disable);
}

/* 默认片选操作函数 */
static void DefaultChipSelect(Max31855CSType cs)
{
    return;
}

/*********** (C) COPYRIGHT 1999-2021 Moonan Technology *********END OF FILE****/