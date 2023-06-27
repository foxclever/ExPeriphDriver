/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：max31856function.c                                             **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：声明和定义适用于MAX31856热偶数字转换芯片的函数和操作。         **/
/**           采用SPI接口，MAX31856寄存器格式如下：                          **/
/**  +-------+-------+------+-----------+----------------------------------+ **/
/**  |  地址 | 读/写 | 名称 |工厂默认值 |           功能                   | **/
/**  +-------+-------+------+-----------+----------------------------------+ **/
/**  |00h/80h| 读/写 |  CR0 |    00h    | 配置0寄存器                      | **/
/**  +-------+-------+------+-----------+----------------------------------+ **/
/**  |01h/81h| 读/写 |  CR1 |    03h    | 配置1寄存器                      | **/
/**  +-------+-------+------+-----------+----------------------------------+ **/
/**  |02h/82h| 读/写 | MASK |    FFh    | 故障屏蔽寄存器                   | **/
/**  +-------+-------+------+-----------+----------------------------------+ **/
/**  |03h/83h| 读/写 | CJHF |    7Fh    | 冷端上限故障                     | **/
/**  +-------+-------+------+-----------+----------------------------------+ **/
/**  |04h/84h| 读/写 | CJLF |    C0h    | 冷端下限故障                     | **/
/**  +-------+-------+------+-----------+----------------------------------+ **/
/**  |05h/85h| 读/写 |LTHFTH|    7Fh    | 线性化温度上限故障，MSB          | **/
/**  +-------+-------+------+-----------+----------------------------------+ **/
/**  |06h/86h| 读/写 |LTHFTL|    FFh    | 线性化温度上限故障，LSB          | **/
/**  +-------+-------+------+-----------+----------------------------------+ **/
/**  |07h/87h| 读/写 |LTLFTH|    80h    | 线性化温度下限故障，MSB          | **/
/**  +-------+-------+------+-----------+----------------------------------+ **/
/**  |08h/88h| 读/写 |LTLFTL|    00h    | 线性化温度下限故障，LSB          | **/
/**  +-------+-------+------+-----------+----------------------------------+ **/
/**  |09h/89h| 读/写 | CJTO |    00h    | 冷端温度偏移寄存器               | **/
/**  +-------+-------+------+-----------+----------------------------------+ **/
/**  |0Ah/8Ah| 读/写 | CJTH |    00h    | 冷端温度寄存器，MSB              | **/
/**  +-------+-------+------+-----------+----------------------------------+ **/
/**  |0Bh/8Bh| 读/写 | CJTL |    00h    | 冷端温度寄存器，LSB              | **/
/**  +-------+-------+------+-----------+----------------------------------+ **/
/**  |  0Ch  | 只读  | LTCBH|    00h    | 线性化TC温度，字节2              | **/
/**  +-------+-------+------+-----------+----------------------------------+ **/
/**  |  0Dh  | 只读  | LTCBM|    00h    | 线性化TC温度，字节1              | **/
/**  +-------+-------+------+-----------+----------------------------------+ **/
/**  |  0Eh  | 只读  | LTCBL|    00h    | 线性化TC温度，字节0              | **/
/**  +-------+-------+------+-----------+----------------------------------+ **/
/**  |  0Fh  | 只读  |  SR  |    00h    | 故障状态寄存器                   | **/
/**  +-------+-------+------+-----------+----------------------------------+ **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2022-11-13          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "max31856function.h"

/*定义寄存器枚举*/
enum Max31856REG {
    REG_CR0=0,          //配置0寄存器
    REG_CR1=1,          //配置1寄存器
    REG_MASK=2,         //故障屏蔽寄存器
    REG_CJHF=3,         //冷端上限故障
    REG_CJLF=4,         //冷端下限故障
    REG_LTHFTH=5,       //线性化温度上限故障，MSB
    REG_LTHFTL=6,       //线性化温度上限故障，LSB
    REG_LTLFTH=7,       //线性化温度下限故障，MSB
    REG_LTLFTL=8,       //线性化温度下限故障，LSB
    REG_CJTO=9,         //冷端温度偏移寄存器
    REG_CJTH=10,        //冷端温度寄存器，MSB
    REG_CJTL=11,        //冷端温度寄存器，LSB
    REG_LTCBH=12,       //线性化TC温度，字节2
    REG_LTCBM=13,       //线性化TC温度，字节1
    REG_LTCBL=14,       //线性化TC温度，字节0
    REG_SR=15           //故障状态寄存器
};


/* 默认片选操作函数 */
static void DefaultChipSelect(Max31856CSType cs);
/*计算热偶温度*/
static float CalcMeasureTemperature(uint32_t tCode);
/*计算参考温度*/
static float CalcColdEndTemperature(uint16_t rCode);
/*读寄存器操作*/
static void ReadRegister(Max31856ObjectType *tc,uint8_t regAddr,uint8_t *rData,uint8_t rSize);
/*写寄存器操作*/
static void WriteRegister(Max31856ObjectType *tc,uint8_t regAddr,uint8_t value);

/*获取MAX31855测量数据*/
void Max31856GetDatas(Max31856ObjectType *tc)
{
    uint8_t rData[6]={0};

    if(tc->Ready())
    {
        ReadRegister(tc,REG_CR0,rData,1);
        if((tc->regValue[REG_CR0]&0x80) != 0x80)
        {
            WriteRegister(tc,REG_CR0,0x80);
            tc->regValue[REG_CR0]=rData[0];
        }
        return;
    }
    
    ReadRegister(tc,REG_CJTH,rData,6);

    tc->rDataCode=(rData[0]<<8)+rData[1];
    
    tc->mDataCode=(rData[2]<<16)+(rData[3]<<8)+rData[4];

    tc->regValue[REG_SR]=rData[5];
    
    tc->mTemperature=CalcMeasureTemperature(tc->mDataCode);
    tc->rTemperature=CalcColdEndTemperature(tc->rDataCode);

}

/*计算热偶温度*/
static float CalcMeasureTemperature(uint32_t tCode)
{
    float result=0.0;
    uint32_t code=(tCode>>5);

    code=code&0x7FFFF;
    if(code<=0x3FFFF)
    {
        result=(float)code*0.0078125;
    }
    else
    {
        code=~(code-1);
        code=code&0x7FFFF;
        result=0.0-(float)code*0.0078125;
    }
    
    return result;
}

/*计算参考温度*/
static float CalcColdEndTemperature(uint16_t rCode)
{
    float result=0.0;
    uint16_t code=(rCode>>2);
    
    code=code&0x3FFF;
    if(rCode<=0x1FFF)
    {
        result=rCode*0.015625;
    }
    else
    {
        code=~(code-1);
        code=code&0x3FFF;
        result=0.0-code*0.015625;
    }
    
    return result;
}

/*初始化MAX31855对象*/
void Max31856Initialization(Max31856ObjectType *tc,         //MAX31856对象变量
                            Max31856Ready ready,            //就绪信号
                            Max31856ReadDataType read,      //读MAX31856函数指针
                            Max31856WriteDataType write,    //写MAX31856函数指针
                            Max31856ChipSelcetType cs       //片选操作函数指针
                                )
{
    uint8_t regValue=0;
    uint8_t rData[16]={0};
    
    if((tc==NULL)||(ready==NULL)||(read==NULL)||(write==NULL))
    {
        return;
    }
    
    tc->Ready=ready;
    tc->ReadData=read;
    tc->WriteData=write;
    
    if(cs!=NULL)
    {
        tc->ChipSelcet=cs;
    }
    else
    {
        tc->ChipSelcet=DefaultChipSelect;
    }
    
    tc->mDataCode=0;
    tc->rDataCode=0;
    tc->mTemperature=0.0;
    tc->rTemperature=0.0;
    
    tc->ChipSelcet(Max31856CS_Disable);
    
    regValue=0x81;
    
    WriteRegister(tc,REG_CR0,regValue);
    
    ReadRegister(tc,REG_CR0,rData,16);
    for(int i=0;i<16;i++)
    {
        tc->regValue[i]=rData[i];
    }
}

/*读寄存器操作*/
static void ReadRegister(Max31856ObjectType *tc,uint8_t regAddr,uint8_t *rData,uint8_t rSize)
{
    uint8_t wData=regAddr;

    if(rSize<1)
    {
        return;
    }
    
    tc->ChipSelcet(Max31856CS_Enable);
    
    tc->WriteData(&wData,1);
    
    tc->ReadData(rData,rSize);
    
    tc->ChipSelcet(Max31856CS_Disable);

}

/*写寄存器操作*/
static void WriteRegister(Max31856ObjectType *tc,uint8_t regAddr,uint8_t value)
{
    uint8_t wData[2];
    
    if(regAddr>11)
    {
        return;
    }
    
    
    wData[0]=regAddr+0x80;
    wData[1]=value;
    
    tc->ChipSelcet(Max31856CS_Enable);
    tc->WriteData(wData,2);
    tc->ChipSelcet(Max31856CS_Disable);
}

/* 默认片选操作函数 */
static void DefaultChipSelect(Max31856CSType cs)
{
    return;
}

/*********** (C) COPYRIGHT 1999-2022 Moonan Technology *********END OF FILE****/