/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ms45x5dofunction.h                                             **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现压力变送器（MS45x5DO）的相关操作                       **/
/**           MS45x5DO压力变送器使用I2C接口通讯方式，具有四种操作命令：      **/
/**           （1）测量请求命令（Read_MR）格式如下：                         **/
/**           +-+--------------+-+-+-+                                       **/
/**           |S|Device Address|W|A|P|                                       **/
/**           +-+--------------+-+-+-+                                       **/
/**           （2）读取2字节命令（Read_DF2）格式如下：                       **/
/**           +-+--------------+-+-+----------+-+----+-+-+                   **/
/**           |S|Device Address|R|A|S1S0|P13-8|A|P7-0|N|P|                   **/
/**           +-+--------------+-+-+----------+-+----+-+-+                   **/
/**           （3）读取2字节命令（Read_DF3）格式如下：                       **/
/**      +-+--------------+-+-+-----------+-+-----+-+-----+-+-+              **/
/**      |S|Device Address|R|A|S1S0| P13-8|A|P7-0 |A|T10-3|N|P|              **/
/**      +-+--------------+-+-+-----------+-+-----+-+-----+-+-+              **/
/**           （4）读取2字节命令（Read_DF4）格式如下：                       **/
/**      +-+--------------+-+-+-----------+-+-----+-+-----+-+---------+-+-+  **/
/**      |S|Device Address|R|A|S1S0| P13-8|A|P7-0 |A|T10-3|A|T2-0|x4-0|N|P|  **/
/**      +-+--------------+-+-+-----------+-+-----+-+-----+-+---------+-+-+  **/
/**           S=Start Condition   W=Write   A=Acknowledge                    **/
/**           P=Stop Condition    R=Read    N=Not Acknowledge                **/
/**           其中Device Address为7位，Command为8位                          **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    1、声明MS45x5DOObjectType对象实体，并调用函数MS45x5DOInitialization初 **/
/**       始化对象实体。给定设备地址，并读取校准数据                         **/
/**    2、实现形如uint8_t f(struct MS45x5DOObject *ms,uint8_t *wData,        **/
/**       uint16_t wSize)的函数，并将其赋值给MS45x5DOObjectType对象实体的    **/
/**       Write函数指针。实现对MS45x5DO写数据。                              **/
/**    3、实现形如void f(struct MS45x5DOObject *ms,uint8_t *rData,           **/
/**       uint16_t rSize)的函数，并将其赋值给MS45x5DOObjectType对象实体的    **/
/**       Read函数指针。实现从MS45x5DO读数据                                 **/
/**    4、实现形如void f(volatile uint32_t nTime)的函数，并将其赋值给        **/
/**       MS45x5DOObjectType对象实体的Delayms函数指针。实现操作延时，单位毫秒**/
/**    5、调用相应的函数实现需要的操作。                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2020-11-26          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "ms45x5dofunction.h"

/*获取转换值，包括温度和压力*/
void GetMS45x5DOConversionValue(MS45x5DOObjectType *ms)
{
    uint8_t rData[4]={0,0,0,0};
    float maxCount=16383;
    float minCount=0;
    
    if(ms->type==MS45x5DO_TypeA)
    {
        maxCount=13106;
        minCount=1638;
    }
    else
    {
        maxCount=14746;
        minCount=819;
    }
    
    ms->Read(ms,rData,4);
    
    ms->msData.rData[0]=rData[1];
    ms->msData.rData[1]=rData[0];
    ms->msData.rData[2]=rData[3];
    ms->msData.rData[3]=rData[2];
    
    if(ms->msData.pData.status!=MS45x5DO_Fault)
    {
        ms->fPressure=(((float)ms->msData.pData.pressure-minCount)/maxCount)*(ms->pUpperRange-ms->pLowerRange)+ms->pLowerRange;
        ms->fTemperature=((float)ms->msData.pData.temperature/2047.0)*200.0-50.0;
    }
}

/* 初始化MS45x5DO对象 */
void MS45x5DOInitialization(MS45x5DOObjectType *ms, //MS5837对象
                            uint8_t devAddress,     //设备地址
                            MS45x5DOType type,      //MS4515DO的类型
                            float pMax,             //压力量程上限
                            float pMin,             //压力量程下限
                            MS45x5DOWrite write,    //向MS45x5DO写数据函数指针
                            MS45x5DORead read,      //从MS45x5DO读数据函数指针
                            MS45x5DODelayms delayms //毫秒延时函数指针
                                )
{
    if((ms==NULL)||(write==NULL)||(read==NULL)||(delayms==NULL))
    {
        return;	
    }
    
    ms->Write=write;
    ms->Read=read;
    ms->Delayms=delayms;
    
    if((devAddress==0x28)||(devAddress==0x36)||(devAddress==0x46)||((0x48<=devAddress)&&(devAddress<=0x51)))
    {
        ms->devAddress=(devAddress<<1);
    }
    else if((devAddress==0x50)||(devAddress==0x6C)||(devAddress==0x8C)||((0x48<=(devAddress/2))&&((devAddress/2)<=0x51)))
    {
        ms->devAddress=devAddress;
    }
    else
    {
        ms->devAddress=0x00;
    }
    
    ms->type=type;
    
    ms->fPressure=0.0;
    ms->fTemperature=0.0;
    ms->msData.rData[0]=0;
    ms->msData.rData[1]=0;
    ms->msData.rData[2]=0;
    ms->msData.rData[3]=0;
    
    if((fabs(pMax)<=0.0000001)&&(fabs(pMin)<=0.0000001))
    {
        ms->pUpperRange=100.0;
        ms->pLowerRange=0.0;
    }
    else
    {
        ms->pUpperRange=pMax;
        ms->pLowerRange=pMin;
    }
}

/*修改MS45x5DO的设备地址*/
void ModifyMS45x5DODecAddress(MS45x5DOObjectType *ms,uint8_t newAddress)
{
    uint8_t eepromByte[3];
    uint16_t eepromTemp=0x00;
    
    //第1步、进入命令模式
    eepromByte[0]=0xA0;
    eepromByte[1]=0x00;
    eepromByte[2]=0x00;
    
    ms->Write(ms,eepromByte,3);
    
    //第2步、发送读EEPROM命令
    eepromByte[0]=0x02;
    eepromByte[1]=0x00;
    eepromByte[2]=0x00;
    
    ms->Write(ms,eepromByte,3);
    
    //第3步、获取EEPROM的值
    ms->Read(ms,eepromByte,3);
    
    //第4步、修改为新地址
    if(eepromByte[0]==0x5A)
    {
        eepromTemp=(eepromByte[1]<<8)+eepromByte[2];
        eepromTemp=(eepromTemp&0xE007)+0xC00+(newAddress<<3);
        
        eepromByte[1]=(uint8_t)((eepromTemp&0xFF00)>>8);
        eepromByte[1]=(uint8_t)(eepromTemp&0x00FF);
    }
    else
    {
        return;
    }
    
    //第5步、将新地址写入EEPROM
    eepromByte[0]=0x02;
    
    ms->Write(ms,eepromByte,3);
    
    //第6步、退出命令模式
    eepromByte[0]=0x80;
    eepromByte[1]=0x00;
    eepromByte[2]=0x00;
    
    ms->Write(ms,eepromByte,3);
}

/*********** (C) COPYRIGHT 1999-2020 Moonan Technology *********END OF FILE****/