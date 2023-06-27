/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ad840xfunction.c                                               **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：声明和定义ad8400数字电位器芯片相关的各种函数和操作             **/
/**           采用SPI接口，支持SPI模式0                                      **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2016-11-08          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "ad840xfunction.h"

#define RESISTANCEINDEX (256) //定义电阻的分度
#define RESISTANCEINITIAL  (50) //定义电阻零分度的电阻

/*设置AD8400游标的位置*/
void SetAD8400ResistorValue(AD840xObjectType *vr,uint16_t resistorValue,AD840xLdacType ldac)
{
    float mValue=0.0;
    uint16_t sendValue=0;
    uint8_t txData[2];
    
    if((resistorValue<50)||(resistorValue>vr->nominalValue))
    {
        return;
    }
    
    if((vr->type==AD8400)&&(ldac!=AD840x_VR1))
    {
        return;
    }
    
    if((vr->type==AD8402)&&(ldac>AD840x_VR2))
    {
        return;
    }
    
    mValue=((float)resistorValue-(float)RESISTANCEINITIAL)/(float)vr->nominalValue;
    sendValue=(uint8_t)(mValue*RESISTANCEINDEX);
    
    sendValue=sendValue||(ldac<<8);
    
    txData[0]=(sendValue>>2);
    txData[1]=(sendValue<<6);
    vr->Write(txData[0]);
    vr->Write(txData[1]);
}

/* 初始化AD840x对象 */
void AD840xInitialization(AD840xObjectType *vr,AD840xType type,AD840xWriteByte write,AD840xNRType nr)
{
    float nValue[4]={1000,10000,50000,100000};
    
    if((vr==NULL)||(write==NULL))
    {
        return;
    }
    
    vr->type=type;
    
    vr->Write=write;
    
    vr->nominalValue=nValue[nr];
}

/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
