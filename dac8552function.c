/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：dac8552function.c                                              **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：声明和定义适用于TI公司DAC8552和DAC8532系列数模转换芯片的函数和 **/
/**           操作。采用SPI接口，具备24位输入移位寄存器。                    **/
/**     24位移位寄存器数据格式如下：                                         **/
/**     +-+-+---+---+--+--+---+---+---+---+---+---+-------+--+--+--+--+      **/
/**     | - | 加载位| X|BS| 模式位|                数据位             |      **/
/**     +-+-+---+---+--+--+---+---+---+---+---+---+-------+--+--+--+--+      **/
/**     |x|x|LDB|LDA| x|C0|PD1|PD0|D15|D14|D13|D12|.......|D3|D2|D1|D0|      **/
/**     +-+-+---+---+--+--+---+---+---+---+---+---+-------+--+--+--+--+      **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2020-05-28          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "dac8552function.h"

/* 默认片选操作函数 */
static void DefaultChipSelect(DAC8552CSType cs);

/*操作DAC8552输出通道*/
void SetDAC8552ChannelValue(DAC8552xObjectType *dac,DAC8552LDType ld,DAC8552BSType bs,DAC8552PDType pd,uint16_t data)
{
    uint32_t inputShiftData=0;
    uint8_t sData[3];
    
    inputShiftData=data;
    
    inputShiftData=inputShiftData|(ld<<20);
    inputShiftData=inputShiftData|(bs<<18);
    inputShiftData=inputShiftData|(pd<<16);
    
    sData[0]=(uint8_t)(inputShiftData>>16);
    sData[1]=(uint8_t)(inputShiftData>>8);
    sData[2]=(uint8_t)inputShiftData;
    
    dac->ChipSelcet(DAC8552CS_Enable);
    dac->WriteDataToDAC(sData,3);
    dac->ChipSelcet(DAC8552CS_Disable);
}

/*初始化DAC8552对象*/
void DAC8552Initialization(DAC8552xObjectType *dac,     //DAC8552对象变量
                           DAC8552WriteType write,      //写数据函数指针
                           DAC8552ChipSelectType cs             //片选操作函数指针
                               )
{
    if((dac==NULL)||(write==NULL))
    {
        return;
    }
    
    if(cs!=NULL)
    {
        dac->ChipSelcet=cs;
    }
    else
    {
        dac->ChipSelcet=DefaultChipSelect;
    }
}

/* 默认片选操作函数 */
static void DefaultChipSelect(DAC8552CSType cs)
{
    return;
}
/*********** (C) COPYRIGHT 1999-2020 Moonan Technology *********END OF FILE****/