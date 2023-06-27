/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ddc114function.c                                               **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：声明和定义适用于TI公司DDC114电流输入模数转换芯片的函数和操作。 **/
/**           采用SPI接口：                                                  **/
/**             +----+-------------------+--------------+                    **/
/**             |编号|          描述     |所需时钟周期数|                    **/
/**             +----+-------------------+--------------+                    **/
/**             |  1 |连续模式测量周期   |    1470      |                    **/
/**             +----+-------------------+--------------+                    **/
/**             |  2 |连续模式就绪周期   |    1380      |                    **/
/**             +----+-------------------+--------------+                    **/
/**             |  3 |第1个非连续模式就绪|    1379      |                    **/
/**             +----+-------------------+--------------+                    **/
/**             |  4 |第2个非连续模式就绪|    1450      |                    **/
/**             +----+-------------------+--------------+                    **/
/**             |  5 |非连续模式测量周期 |    2901      |                    **/
/**             +----+-------------------+--------------+                    **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2021-02-15          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "ddc114function.h"

/*解析从DDC114读取的数据*/
static void Ddc114ParseDatas(Ddc114ObjectType *ddc,uint8_t *rData);

/*DDC114获取各通道的转换数据*/
void Ddc114GetDataCode(Ddc114ObjectType *ddc)
{
    uint8_t rData[10];
    uint16_t timeOut=0;
    
    if(ddc->convStatus==DDC114_Pin_Reset)
    {
        ddc->convStatus=DDC114_Pin_Set;
    }
    else
    {
        ddc->convStatus=DDC114_Pin_Reset;
    }
    
    ddc->SetConv(ddc->convStatus);
    
    while((ddc->GetValid())&&(timeOut<500))
    {
        timeOut++;
    }
    
    if(ddc->format == DDC114_OUT20)
    {
        ddc->GetDatas(rData,10);
    }
    else
    {
        ddc->GetDatas(rData,8);
    }
    
    Ddc114ParseDatas(ddc,rData);
}

/*DDC114对象初始化*/
void Ddc114Initialization(Ddc114ObjectType *ddc,    //DDC114对象变量
                          Ddc114FormatType format,  //数据输出格式
                          DDC114GetDatas getDatas,  //获取测量数据函数指针
                          DDC114GetValid getValid,  //数据有效性状态获取函数指针
                          DDC114SetConv conv,       //转换设置函数指针
                          DDC114SetReset reset,     //复位操作函数指针
                          DDC114SetTest test,       //测试模式操作函数指针
                          DDC114Delayus delayus     //微秒延时函数指针
                              )
{
    if((ddc==NULL)||(getDatas==NULL)||(getValid==NULL)||(conv==NULL)||(reset==NULL)||(test==NULL)||(delayus==NULL))
    {
        return ;
    }
    ddc->GetDatas=getDatas;
    ddc->GetValid=getValid;
    ddc->SetConv=conv;
    ddc->SetReset=reset;
    ddc->SetTest=test;
    ddc->Delayus=delayus;
    
    ddc->format=format;
    if(format==DDC114_OUT20)
    {
        ddc->codeRange=1048575;
        ddc->codeZero=4096;
    }
    else
    {
        ddc->codeRange=65535;
        ddc->codeZero=256;
    }
    
    for(int i=0;i<4;i++)
    {
        ddc->dCode[i]=ddc->codeZero;
    }
    
    ddc->SetTest(DDC114_Pin_Reset);
    
    ddc->SetReset(DDC114_Pin_Reset);
    ddc->Delayus(100);
    ddc->SetReset(DDC114_Pin_Set);
    
    ddc->convStatus=DDC114_Pin_Reset;
    ddc->SetConv(ddc->convStatus);
}

/*解析从DDC114读取的数据*/
static void Ddc114ParseDatas(Ddc114ObjectType *ddc,uint8_t *rData)
{
    uint32_t tCode[10]={0};
    
    if(ddc->format==DDC114_OUT20)
    {
        for(int i=0;i<10;i++)
        {
            tCode[i]=(uint32_t)(rData[i]);
        }

        ddc->dCode[0]=((tCode[7]&0x0F)<<16)+(tCode[8]<<8)+tCode[9];
        ddc->dCode[1]=(tCode[5]<<12)+(tCode[6]<<4)+((tCode[7]&0xF0)>>4);
        ddc->dCode[2]=((tCode[2]&0x0F)<<16)+(tCode[3]<<8)+tCode[4];
        ddc->dCode[3]=(tCode[0]<<12)+(tCode[1]<<4)+((tCode[2]&0xF0)>>4);
    }
    else
    {
        for(int i=0;i<8;i++)
        {
            tCode[i]=(uint32_t)rData[i];
        }
        
        ddc->dCode[0]=(tCode[6]<<8)+tCode[7];
        ddc->dCode[1]=(tCode[4]<<8)+tCode[5];
        ddc->dCode[2]=(tCode[2]<<8)+tCode[3];
        ddc->dCode[3]=(tCode[0]<<8)+tCode[1];
    }
}

/*********** (C) COPYRIGHT 1999-2021 Moonan Technology *********END OF FILE****/