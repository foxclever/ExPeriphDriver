/******************************************************************************/
/** 模块名称：自定义通用功能                                                 **/
/** 文件名称：simuspifunction.c                                              **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于使用GPIO模拟SPI硬件接口通讯，基于C语言实现，主站模式       **/
/**           SCK为时钟引脚，主站为输出模式                                  **/
/**           MOSI为主站数据输出引脚，配置为输出模式                         **/
/**           MISO为主站数据输入引脚，配置为输入模式                         **/
/**           SPI接口根据始终空闲状态及采样时刻的不同具有4种工作模式：       **/
/**           +-------+----+----+-------------+--------+                     **/
/**           |SPI模式|CPOL|CPHA|空闲时SCK电平|采样时刻|                     **/
/**           +-------+----+----+-------------+--------+                     **/
/**           |   0   |  0 |  0 |    低电平   |奇数边沿|                     **/
/**           +-------+----+----+-------------+--------+                     **/
/**           |   1   |  0 |  1 |    低电平   |偶数边沿|                     **/
/**           +-------+----+----+-------------+--------+                     **/
/**           |   2   |  1 |  0 |    高电平   |奇数边沿|                     **/
/**           +-------+----+----+-------------+--------+                     **/
/**           |   3   |  1 |  1 |    高电平   |偶数边沿|                     **/
/**           +-------+----+----+-------------+--------+                     **/
/**           速度可设置高于0K，低于400K的整数，默认值为100K                 **/
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
/**     版本      日期          作者      说明                               **/
/**     V1.0.0  2020-10-06      木南      创建文件                           **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "simuspifunction.h"

/* 通过模拟SPI发送一个字节 */
static void SendByteBySimuSPI(SimuSPIObjectType *simuSPIInstance,uint8_t byte);
/* 通过模拟SPI接收一个字节 */
static uint8_t RecieveByteBySimuSPI(SimuSPIObjectType *simuSPIInstance);
/*通过模拟SPI同时发送并接收一个字节*/
static uint8_t SendRecieveByteBySimuSPI(SimuSPIObjectType *simuSPIInstance,uint8_t byte);

/* GPIO模拟SPI通讯初始化 */
void SimuSPIInitialization(SimuSPIObjectType *simuSPIInstance,//初始化的模拟SPI对象
                           uint32_t speed,      //时钟频率
                           SimuSPICPOLType CPOL,        //时钟极性
                           SimuSPICPHAType CPHA,        //时钟频率
                           SimuSPIDataSizeType dataSize,//数据长度
                           SimuSPISetSCKPin setSCK,     //SCK时钟操作函数指针
                           SimuSPISetMOSIPin setMOSI,   //MOSI操作函数指针
                           SimuSPIReadMISOPin getMISO,  //MISO操作函数指针
                           SimuSPIDelayus delayus       //微秒延时操作函数指针
                               )
{
    if((simuSPIInstance==NULL)||(setSCK==NULL)||(setMOSI==NULL)||(getMISO==NULL)||(delayus==NULL))
    {
        return;
    }
    
    simuSPIInstance->SetSCKPin=setSCK;
    simuSPIInstance->SetMOSIPin=setMOSI;
    simuSPIInstance->ReadMISOPin=getMISO;
    simuSPIInstance->Delayus=delayus;
    
    /*初始化速度,默认100K*/
    if((speed>0)&&(speed<=500))
    {
        simuSPIInstance->period=500/speed;
    }
    else
    {
        simuSPIInstance->period=5;
    }
    
    simuSPIInstance->CPOL=CPOL;
    simuSPIInstance->CPHA=CPHA;
    simuSPIInstance->dataSize=dataSize;
    
    /*拉高总线，使处于空闲状态*/
    
    if(simuSPIInstance->CPOL==SimuSPI_POLARITY_LOW)
    {
        simuSPIInstance->SetSCKPin(SimuSPI_Reset);
    }
    else
    {
        simuSPIInstance->SetSCKPin(SimuSPI_Set);
    }
}

/* 通过模拟SPI发送一个字节 */
static void SendByteBySimuSPI(SimuSPIObjectType *simuSPIInstance,uint8_t byte)
{
//    uint8_t length[2]={8,16};
    
    if(simuSPIInstance->CPOL==SimuSPI_POLARITY_LOW)
    {
        /*拉低SCL引脚准备数据传输*/
        simuSPIInstance->SetSCKPin(SimuSPI_Reset);
        
        if(simuSPIInstance->CPHA==SimuSPI_PHASE_1EDGE)  //模式0
        {
            for(uint8_t count = 0; count < 8; count++)
            {
                if(byte & 0x80)     //每次发送最高位
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Set);
                }
                else
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Reset);
                }
                byte <<= 1;         //发送一位后，左移一位
                
                simuSPIInstance->Delayus(simuSPIInstance->period);
                simuSPIInstance->SetSCKPin(SimuSPI_Set);
                simuSPIInstance->Delayus(simuSPIInstance->period);
                simuSPIInstance->SetSCKPin(SimuSPI_Reset);
            }
        }
        else    //模式1
        {
            for(uint8_t count = 0; count < 8; count++)
            {
                if(byte & 0x80)     //每次发送最高位
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Set);
                }
                else
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Reset);
                }
                byte <<= 1;         //发送一位后，左移一位
                
                simuSPIInstance->SetSCKPin(SimuSPI_Set);
                simuSPIInstance->Delayus(simuSPIInstance->period);
                simuSPIInstance->SetSCKPin(SimuSPI_Reset);
                simuSPIInstance->Delayus(simuSPIInstance->period);
            }
        }
    }
    else
    {
        /*拉低SCL引脚准备数据传输*/
        simuSPIInstance->SetSCKPin(SimuSPI_Set);
        
        if(simuSPIInstance->CPHA==SimuSPI_PHASE_1EDGE)  //模式2
        {
            for(uint8_t count = 0; count < 8; count++)
            {
                if(byte & 0x80)     //每次发送最高位
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Set);
                }
                else
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Reset);
                }
                byte <<= 1;         //发送一位后，左移一位
                
                simuSPIInstance->Delayus(simuSPIInstance->period);
                simuSPIInstance->SetSCKPin(SimuSPI_Reset);
                simuSPIInstance->Delayus(simuSPIInstance->period);
                simuSPIInstance->SetSCKPin(SimuSPI_Set);
            }
        }
        else    //模式3
        {
            for(uint8_t count = 0; count < 8; count++)
            {
                if(byte & 0x80)     //每次发送最高位
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Set);
                }
                else
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Reset);
                }
                byte <<= 1;         //发送一位后，左移一位
                
                simuSPIInstance->Delayus(simuSPIInstance->period);
                simuSPIInstance->SetSCKPin(SimuSPI_Reset);
                simuSPIInstance->Delayus(simuSPIInstance->period);
                simuSPIInstance->SetSCKPin(SimuSPI_Set);
            }
        }
    }
}

/* 通过模拟SPI接收一个字节 */
static uint8_t RecieveByteBySimuSPI(SimuSPIObjectType *simuSPIInstance)
{
    uint8_t receive = 0;
    
    if(simuSPIInstance->CPOL==SimuSPI_POLARITY_LOW)
    {
        /*拉低SCL引脚准备数据传输*/
        simuSPIInstance->SetSCKPin(SimuSPI_Reset);
        
        if(simuSPIInstance->CPHA==SimuSPI_PHASE_1EDGE)  //模式0
        {
            for(uint8_t count = 0; count < 8; count++ )
            {
                simuSPIInstance->SetSCKPin(SimuSPI_Set);
                simuSPIInstance->Delayus(simuSPIInstance->period);
                
                receive <<= 1;
                
                if(simuSPIInstance->ReadMISOPin())
                {
                    receive++;
                }
                simuSPIInstance->SetSCKPin(SimuSPI_Reset);
                simuSPIInstance->Delayus(simuSPIInstance->period);
            }
        }
        else    //模式1
        {
            simuSPIInstance->SetSCKPin(SimuSPI_Set);
            simuSPIInstance->Delayus(simuSPIInstance->period);
            for(uint8_t count = 0; count < 8; count++ )
            {
                simuSPIInstance->SetSCKPin(SimuSPI_Reset);
                simuSPIInstance->Delayus(simuSPIInstance->period);
                
                receive <<= 1;
                
                if(simuSPIInstance->ReadMISOPin())
                {
                    receive++;
                }
                simuSPIInstance->SetSCKPin(SimuSPI_Set);
                simuSPIInstance->Delayus(simuSPIInstance->period);
            }
            simuSPIInstance->SetSCKPin(SimuSPI_Reset);
        }
    }
    else
    {
        /*拉低SCL引脚准备数据传输*/
        simuSPIInstance->SetSCKPin(SimuSPI_Set);
        
        if(simuSPIInstance->CPHA==SimuSPI_PHASE_1EDGE)  //模式2
        {
            for(uint8_t count = 0; count < 8; count++ )
            {
                simuSPIInstance->SetSCKPin(SimuSPI_Reset);
                simuSPIInstance->Delayus(simuSPIInstance->period);
                
                receive <<= 1;
                
                if(simuSPIInstance->ReadMISOPin())
                {
                    receive++;
                }
                simuSPIInstance->SetSCKPin(SimuSPI_Set);
                simuSPIInstance->Delayus(simuSPIInstance->period);
            }
        }
        else    //模式3
        {
            simuSPIInstance->SetSCKPin(SimuSPI_Reset);
            simuSPIInstance->Delayus(simuSPIInstance->period);
            for(uint8_t count = 0; count < 8; count++ )
            {
                simuSPIInstance->SetSCKPin(SimuSPI_Set);
                simuSPIInstance->Delayus(simuSPIInstance->period);
                
                receive <<= 1;
                
                if(simuSPIInstance->ReadMISOPin())
                {
                    receive++;
                }
                simuSPIInstance->SetSCKPin(SimuSPI_Reset);
                simuSPIInstance->Delayus(simuSPIInstance->period);
            }
            simuSPIInstance->SetSCKPin(SimuSPI_Set);
        }
    }
    
    return receive;
}

/*通过模拟SPI同时发送并接收一个字节*/
static uint8_t SendRecieveByteBySimuSPI(SimuSPIObjectType *simuSPIInstance,uint8_t byte)
{
    uint8_t receive = 0;
    
    if(simuSPIInstance->CPOL==SimuSPI_POLARITY_LOW)
    {
        /*拉低SCL引脚准备数据传输*/
        simuSPIInstance->SetSCKPin(SimuSPI_Reset);
        
        if(simuSPIInstance->CPHA==SimuSPI_PHASE_1EDGE)  //模式0
        {
            for(uint8_t count = 0; count < 8; count++)
            {
                if(byte & 0x80)     //每次发送最高位
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Set);
                }
                else
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Reset);
                }
                byte <<= 1;         //发送一位后，左移一位
                
                simuSPIInstance->Delayus(simuSPIInstance->period);
                simuSPIInstance->SetSCKPin(SimuSPI_Set);
                simuSPIInstance->Delayus(simuSPIInstance->period);
                
                receive <<= 1;
                if(simuSPIInstance->ReadMISOPin())
                {
                    receive++;
                }
                simuSPIInstance->SetSCKPin(SimuSPI_Reset);
            }
        }
        else    //模式1
        {
            simuSPIInstance->SetSCKPin(SimuSPI_Set);
            for(uint8_t count = 0; count < 8; count++)
            {
                if(byte & 0x80)     //每次发送最高位
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Set);
                }
                else
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Reset);
                }
                byte <<= 1;         //发送一位后，左移一位
                
                simuSPIInstance->Delayus(simuSPIInstance->period);
                simuSPIInstance->SetSCKPin(SimuSPI_Reset);
                simuSPIInstance->Delayus(simuSPIInstance->period);
                
                receive <<= 1;
                if(simuSPIInstance->ReadMISOPin())
                {
                    receive++;
                }
                simuSPIInstance->SetSCKPin(SimuSPI_Set);
            }
            simuSPIInstance->SetSCKPin(SimuSPI_Reset);
        }
    }
    else
    {
        /*拉低SCL引脚准备数据传输*/
        simuSPIInstance->SetSCKPin(SimuSPI_Set);
        
        if(simuSPIInstance->CPHA==SimuSPI_PHASE_1EDGE)  //模式2
        {
            for(uint8_t count = 0; count < 8; count++)
            {
                if(byte & 0x80)     //每次发送最高位
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Set);
                }
                else
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Reset);
                }
                byte <<= 1;         //发送一位后，左移一位
                
                simuSPIInstance->Delayus(simuSPIInstance->period);
                simuSPIInstance->SetSCKPin(SimuSPI_Reset);
                simuSPIInstance->Delayus(simuSPIInstance->period);
                
                receive <<= 1;
                if(simuSPIInstance->ReadMISOPin())
                {
                    receive++;
                }
                simuSPIInstance->SetSCKPin(SimuSPI_Set);
            }
        }
        else    //模式3
        {
            simuSPIInstance->SetSCKPin(SimuSPI_Reset);
            for(uint8_t count = 0; count < 8; count++)
            {
                if(byte & 0x80)     //每次发送最高位
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Set);
                }
                else
                {
                    simuSPIInstance->SetMOSIPin(SimuSPI_Reset);
                }
                byte <<= 1;         //发送一位后，左移一位
                
                simuSPIInstance->Delayus(simuSPIInstance->period);
                simuSPIInstance->SetSCKPin(SimuSPI_Set);
                simuSPIInstance->Delayus(simuSPIInstance->period);
                
                receive <<= 1;
                if(simuSPIInstance->ReadMISOPin())
                {
                    receive++;
                }
                simuSPIInstance->SetSCKPin(SimuSPI_Reset);
            }
        }
    }
    
    return receive;
}

/* 通过模拟SPI向从站写数据 */
SimuSPIStatusType WriteDataBySimuSPI(SimuSPIObjectType *simuSPIInstance,uint8_t *wData,uint16_t wSize,uint16_t timeOut)
{
    SimuSPIStatusType ret=SimuSPI_OK;
        
    for(int i=0;i<wSize;i++)
    {
        SendByteBySimuSPI(simuSPIInstance,wData[i]);
    }
    
    return ret;
}

/* 通过模拟SPI自从站读数据 */
SimuSPIStatusType ReadDataBySimuSPI(SimuSPIObjectType *simuSPIInstance,uint8_t *rData, uint16_t rSize,uint16_t timeOut)
{
    SimuSPIStatusType ret=SimuSPI_OK;
    
    for(int i=0;i<rSize;i++)
    {
        rData[i]=RecieveByteBySimuSPI(simuSPIInstance);
    }
    
    return ret;
}

/* 通过模拟SPI实现对从站先写数据紧接读数据组合操作 */
SimuSPIStatusType WriteReadDataBySimuSPI(SimuSPIObjectType *simuSPIInstance, uint8_t *wData,uint16_t wSize,uint8_t *rData, uint16_t rSize,uint16_t timeOut)
{
    SimuSPIStatusType ret=SimuSPI_OK;
    
    for(int i=0;i<wSize;i++)
    {
        SendByteBySimuSPI(simuSPIInstance,wData[i]);
    }
    
    for(int i=0;i<rSize;i++)
    {
        rData[i]=RecieveByteBySimuSPI(simuSPIInstance);
    }
    
    return ret;
}

/* 通过模拟SPI实现对从站同时写和读数据组合操作*/
SimuSPIStatusType WriteWhileReadDataBySimuSPI(SimuSPIObjectType *simuSPIInstance, uint8_t *wData,uint8_t *rData,uint16_t pSize,uint16_t timeOut)
{
    SimuSPIStatusType ret=SimuSPI_OK;
        
    for(int i=0;i<pSize;i++)
    {
        rData[i]= SendRecieveByteBySimuSPI(simuSPIInstance,wData[i]);
    }
    
    return ret;
}

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/
