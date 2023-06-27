/******************************************************************************/
/** 模块名称：自定义通用功能                                                 **/
/** 文件名称：simui2cfunction.c                                              **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于使用GPIO模拟I2C硬件接口通讯                                **/
/**           基于C语言编写，适用于通用MCU                                   **/
/**           SCL引脚配置为推挽输出                                          **/
/**           SDA引脚配置为开漏输出，在读取时设置为高电平                    **/
/**           速度可设置高于0K，低于500K的整数，默认值为100K                 **/
/**--------------------------------------------------------------------------**/
/**使用说明：                                                                **/
/**    1、声明GPIO模拟I2C对象实体（如：SimuI2CTypeDef si2c），并调用函数     **/
/**       SimuI2CInitialization初始化对象实体。初始化时，给定工作频率1-500K  **/
/**       以kHz为单位，最小始终周期为2微秒，最大为500k微秒。                 **/
/**    2、实现形如void f(SimuI2CPinValue op)的函数，并将其赋值               **/
/**       给SimuI2CTypeDef对象实体的SetSCLPin函数指针。用以操作SCL引脚。     **/
/**    3、实现形如void f(SimuI2CPinValue op)的函数，并将其赋值               **/
/**       给SimuI2CTypeDef对象实体的SetSDAPin函数指针。用以操作SDA引脚。     **/
/**    4、实现形如uint8_t f(void)的函数，并将其赋值给SimuI2CTypeDef对象实体  **/
/**       的ReadSDAPin函数指针。用以读取SDA引脚。                            **/
/**    5、实现形如void f(volatile uint32_t period)的函数，并将其赋值         **/
/**       给SimuI2CTypeDef对象实体的Delayus函数指针。用于周期延时，单位为微秒**/
/**    6、调用相应的函数实现需要的操作。                                     **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2017-05-15          木南              创建文件               **/
/**     V1.0.1  2018-08-28          木南              修改为对象操作         **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "simui2cfunction.h"

/* 模拟I2C通讯起始操作 */
static void SimuI2CStart(SimuI2CObjectType *simuI2CInstance);

/* 模拟I2C通讯起始操作 */
static void SimuI2CStop(SimuI2CObjectType *simuI2CInstance);

/* 模拟I2C等待确认 */
static SimuI2CStatus SimuI2CWaitAck(SimuI2CObjectType *simuI2CInstance,uint16_t timeOut);

/* 模拟I2C产生一个应答*/
static void IIC_Ack(SimuI2CObjectType *simuI2CInstance);

/* 模拟I2C产生一个非应答 */
static void IIC_NAck(SimuI2CObjectType *simuI2CInstance);

/* 通过模拟I2C发送一个字节 */
static void SendByteBySimuI2C(SimuI2CObjectType *simuI2CInstance,uint8_t byte);

/* 通过模拟I2C接收一个字节 */
static uint8_t RecieveByteBySimuI2C(SimuI2CObjectType *simuI2CInstance);

/* GPIO模拟I2C通讯初始化 */
void SimuI2CInitialization(SimuI2CObjectType *simuI2CInstance,
                           uint32_t speed,
                           SimuI2CSetPin setSCL,
                           SimuI2CSetPin setSDA,
                           SimuI2CReadSDAPin readSDA,
                           SimuI2CDelayus delayus)
{
    if((simuI2CInstance==NULL)||(setSCL==NULL)||(setSDA==NULL)||(readSDA==NULL)||(delayus==NULL))
    {
        return;
    }
    
    simuI2CInstance->SetSCLPin=setSCL;
    simuI2CInstance->SetSDAPin=setSDA;
    simuI2CInstance->ReadSDAPin=readSDA;
    simuI2CInstance->Delayus=delayus;
    
    /*初始化速度,默认100K*/
    if((speed>0)&&(speed<=400))
    {
        simuI2CInstance->period=500/speed;
    }
    else
    {
        simuI2CInstance->period=5;
    }
    
    /*拉高总线，使处于空闲状态*/
    simuI2CInstance->SetSDAPin(Set);
    simuI2CInstance->SetSCLPin(Set);
}

/* 模拟I2C通讯起始操作 */
static void SimuI2CStart(SimuI2CObjectType *simuI2CInstance)
{
    /*拉高总线，使处于空闲状态*/
    simuI2CInstance->SetSDAPin(Set);
    simuI2CInstance->SetSCLPin(Set);
    simuI2CInstance->Delayus(simuI2CInstance->period);
    
    /*当SCL为高时，SDA一个下降代表开始*/
    simuI2CInstance->SetSDAPin(Reset);
    simuI2CInstance->Delayus(simuI2CInstance->period);
    
    /*嵌住SCL以便发送数据*/
    simuI2CInstance->SetSCLPin(Reset);
    simuI2CInstance->Delayus(simuI2CInstance->period);
}

/* 模拟I2C通讯停止操作 */
static void SimuI2CStop(SimuI2CObjectType *simuI2CInstance)
{
    simuI2CInstance->SetSCLPin(Reset);
    simuI2CInstance->Delayus(simuI2CInstance->period);
    
    simuI2CInstance->SetSDAPin(Reset);
    simuI2CInstance->Delayus(simuI2CInstance->period);
    
    simuI2CInstance->SetSCLPin(Set);
    simuI2CInstance->Delayus(simuI2CInstance->period);
    
    simuI2CInstance->SetSDAPin(Set);
    simuI2CInstance->Delayus(simuI2CInstance->period);
}

/* 模拟I2C等待确认 */
static SimuI2CStatus SimuI2CWaitAck(SimuI2CObjectType *simuI2CInstance,uint16_t timeOut)
{
    simuI2CInstance->SetSDAPin(Set);//拉高SDA
    simuI2CInstance->Delayus(simuI2CInstance->period);
    simuI2CInstance->SetSCLPin(Set);//拉高SCL
    simuI2CInstance->Delayus(simuI2CInstance->period);
    
    while(simuI2CInstance->ReadSDAPin())           //读取数据引脚
    {
        if(--timeOut)
        {
            SimuI2CStop(simuI2CInstance);
            return I2C_ERROR;
        }
        simuI2CInstance->Delayus(simuI2CInstance->period);
    }
    simuI2CInstance->SetSCLPin(Reset);
    simuI2CInstance->Delayus(simuI2CInstance->period);
    return I2C_OK;
}

/* 模拟I2C产生一个应答*/
static void IIC_Ack(SimuI2CObjectType *simuI2CInstance)
{
    simuI2CInstance->SetSDAPin(Reset);
    simuI2CInstance->Delayus(simuI2CInstance->period);
    
    simuI2CInstance->SetSCLPin(Set);
    simuI2CInstance->Delayus(simuI2CInstance->period);
    
    simuI2CInstance->SetSCLPin(Reset);
    simuI2CInstance->Delayus(simuI2CInstance->period);
    
    simuI2CInstance->SetSDAPin(Set);
}

/* 模拟I2C产生一个非应答 */
static void IIC_NAck(SimuI2CObjectType *simuI2CInstance)
{
    simuI2CInstance->SetSCLPin(Reset);
    simuI2CInstance->SetSDAPin(Set);
    simuI2CInstance->Delayus(simuI2CInstance->period);
    
    simuI2CInstance->SetSCLPin(Set);
    simuI2CInstance->Delayus(simuI2CInstance->period);
    simuI2CInstance->SetSCLPin(Reset);
    simuI2CInstance->Delayus(simuI2CInstance->period);
}

/* 通过模拟I2C发送一个字节 */
static void SendByteBySimuI2C(SimuI2CObjectType *simuI2CInstance,uint8_t byte)
{
    /*拉低SCL引脚准备数据传输*/
    simuI2CInstance->SetSCLPin(Reset);
    
    for(uint8_t count = 0; count < 8; count++)
    {
        if(byte & 0x80)     //每次发送最高位
        {
            simuI2CInstance->SetSDAPin(Set);
        }
        else
        {
            simuI2CInstance->SetSDAPin(Reset);
        }
        byte <<= 1;         //发送一位后，左移一位
        
        simuI2CInstance->Delayus(simuI2CInstance->period);
        simuI2CInstance->SetSCLPin(Set);
        simuI2CInstance->Delayus(simuI2CInstance->period);
        simuI2CInstance->SetSCLPin(Reset);
    }
    
    //  simuI2CInstance->Delayus(simuI2CInstance->period);
    //  simuI2CInstance->SetSDAPin(Set);
}

/* 通过模拟I2C接收一个字节 */
static uint8_t RecieveByteBySimuI2C(SimuI2CObjectType *simuI2CInstance)
{
    uint8_t receive = 0;
    
    simuI2CInstance->SetSDAPin(Set);//引脚配置为开漏输出时，需要置高才能读取数据
    
    for(uint8_t count = 0; count < 8; count++ )
    {
        //    simuI2CInstance->SetSCLPin(Reset);
        //    simuI2CInstance->Delayus(simuI2CInstance->period);
        simuI2CInstance->SetSCLPin(Set);
        simuI2CInstance->Delayus(simuI2CInstance->period);
        
        receive <<= 1;
        
        if(simuI2CInstance->ReadSDAPin())
        {
            receive++;
        }
        simuI2CInstance->SetSCLPin(Reset);
        simuI2CInstance->Delayus(simuI2CInstance->period);
    }
    return receive;	
}

/* 通过模拟I2C向从站写数据 */
SimuI2CStatus WriteDataBySimuI2C(SimuI2CObjectType *simuI2CInstance,uint8_t deviceAddress,uint8_t *wData,uint16_t wSize)
{
    //启动通讯
    SimuI2CStart(simuI2CInstance);
    //发送从站地址（写）
    SendByteBySimuI2C(simuI2CInstance,deviceAddress);
    if(SimuI2CWaitAck(simuI2CInstance,5000))
    {
        return I2C_ERROR;
    }
    
    while(wSize--)
    {
        SendByteBySimuI2C(simuI2CInstance,*wData);
        if(SimuI2CWaitAck(simuI2CInstance,5000))
        {
            return I2C_ERROR;
        }
        wData++;
        simuI2CInstance->Delayus(10);
    }
    
    SimuI2CStop(simuI2CInstance);
    return I2C_OK;
}

/* 通过模拟I2C自从站读数据 */
SimuI2CStatus ReadDataBySimuI2C(SimuI2CObjectType *simuI2CInstance,uint8_t deviceAddress,uint8_t *rData, uint16_t rSize)
{
    //启动通讯
    SimuI2CStart(simuI2CInstance);
    
    //发送从站地址（读）
    SendByteBySimuI2C(simuI2CInstance,deviceAddress+1);
    if(SimuI2CWaitAck(simuI2CInstance,5000))
    {
        return I2C_ERROR;
    }
    
    simuI2CInstance->Delayus(1000);
    
    while(rSize--)
    {
        *rData=RecieveByteBySimuI2C(simuI2CInstance);
        rData++;
        if(rSize==0)
        {
            IIC_NAck(simuI2CInstance);
        }
        else
        {
            IIC_Ack(simuI2CInstance);
            simuI2CInstance->Delayus(1000);
        }
    }
    
    //结束通讯
    SimuI2CStop(simuI2CInstance);
    return I2C_OK;
}

/* 通过模拟I2C实现对从站先写数据紧接读数据组合操作 */
SimuI2CStatus WriteReadDataBySimuI2C(SimuI2CObjectType *simuI2CInstance,uint8_t deviceAddress, uint8_t *wData,uint16_t wSize,uint8_t *rData, uint16_t rSize)
{
    //启动通讯
    SimuI2CStart(simuI2CInstance);
    //发送从站地址（写）
    SendByteBySimuI2C(simuI2CInstance,deviceAddress);
    if(SimuI2CWaitAck(simuI2CInstance,5000))
    {
        return I2C_ERROR;
    }
    
    while(wSize--)
    {
        SendByteBySimuI2C(simuI2CInstance,*wData);
        if(SimuI2CWaitAck(simuI2CInstance,5000))
        {
            return I2C_ERROR;
        }
        wData++;
        simuI2CInstance->Delayus(10);
    }
    
    //再启动
    SimuI2CStart(simuI2CInstance);
    //发送从站地址（读）
    SendByteBySimuI2C(simuI2CInstance,deviceAddress+1);
    if(SimuI2CWaitAck(simuI2CInstance,5000))
    {
        return I2C_ERROR;
    }
    
    while(rSize--)
    {
        *rData=RecieveByteBySimuI2C(simuI2CInstance);
        rData++;
        if(rSize==0)
        {
            IIC_NAck(simuI2CInstance);
        }
        else
        {
            IIC_Ack(simuI2CInstance);
        }
    }
    //结束通讯
    SimuI2CStop(simuI2CInstance);
    return I2C_OK;
}
/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/
