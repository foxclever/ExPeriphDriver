/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：ads1256function.c                                              **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：声明和定义适用于TI公司ADS1256数模转换芯片的函数和操作。        **/
/**           采用SPI接口，ADS1256个寄存器地址及格式如下：                   **/
/**     +----+------+------+-----+-----+-----+-----+-----+-----+-----+-----+ **/
/**     |地址|寄存器|复位值| BIT7| BIT6| BIT5| BIT4| BIT3| BIT2| BIT1| BIT0| **/
/**     +----+------+------+-----+-----+-----+-----+-----+-----+-----+-----+ **/
/**     |00h |STATUS|  x1H | ID3 | ID2 | ID1 | ID0 |ORDER| ACAL|BUFEN| DRDY| **/
/**     +----+------+------+-----+-----+-----+-----+-----+-----+-----+-----+ **/
/**     |01h |  MUX |  01H |PSEL3|PSEL2|PSEL1|PSEL0|NSEL3|NSEL2|NSEL1|NSEL0| **/
/**     +----+------+------+-----+-----+-----+-----+-----+-----+-----+-----+ **/
/**     |02h | ADCON|  20H |  0  | CLK1| CLK0|SDCS1|SDCS0| PGA2| PGA1| PGA0| **/
/**     +----+------+------+-----+-----+-----+-----+-----+-----+-----+-----+ **/
/**     |03h | DRATE|  F0H | DR7 | DR6 | DR5 | DR4 | DR3 | DR2 | DR1 | DR0 | **/
/**     +----+------+------+-----+-----+-----+-----+-----+-----+-----+-----+ **/
/**     |04h |  IO  |  E0H | DIR3| DIR2| DIR1| DIR0| DIO3| DIO2| DIO1| DIO0| **/
/**     +----+------+------+-----+-----+-----+-----+-----+-----+-----+-----+ **/
/**     |05h | OFC0 |  xxH |OFC07|OFC06|OFC05|OFC04|OFC03|OFC02|OFC01|OFC00| **/
/**     +----+------+------+-----+-----+-----+-----+-----+-----+-----+-----+ **/
/**     |06h | OFC1 |  xxH |OFC15|OFC14|OFC13|OFC12|OFC11|OFC10|OFC09|OFC08| **/
/**     +----+------+------+-----+-----+-----+-----+-----+-----+-----+-----+ **/
/**     |07h | OFC2 |  xxH |OFC23|OFC22|OFC21|OFC20|OFC19|OFC18|OFC17|OFC16| **/
/**     +----+------+------+-----+-----+-----+-----+-----+-----+-----+-----+ **/
/**     |08h | FSC0 |  xxH |FSC07|FSC06|FSC05|FSC04|FSC03|FSC02|FSC01|FSC00| **/
/**     +----+------+------+-----+-----+-----+-----+-----+-----+-----+-----+ **/
/**     |09h | FSC1 |  xxH |FSC15|FSC14|FSC13|FSC12|FSC11|FSC10|FSC09|FSC08| **/
/**     +----+------+------+-----+-----+-----+-----+-----+-----+-----+-----+ **/
/**     |0Ah | FSC2 |  xxH |FSC23|FSC22|FSC21|FSC20|FSC19|FSC18|FSC17|FSC16| **/
/**     +----+------+------+-----+-----+-----+-----+-----+-----+-----+-----+ **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2020-06-23          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "ads1256function.h"

/*定义ADS1256操作命令宏*/
#define WAKEUP  0x00    //Completes SYNC and Exits Standby Mode
#define RDATA   0x01    //Read Data
#define RDATAC  0x03    //Read Data Continuously
#define SDATAC  0x0F    //Stop Read Data Continuously
#define RREG    0x10    //Read from REG rrr
#define WREG    0x50    //Write to REG rrr
#define SELFCAL 0xF0    //Offset and Gain Self-Calibration
#define SELFOCAL        0xF1    //Offset Self-Calibration
#define SELFGCAL        0xF2    //Gain Self-Calibration
#define SYSOCAL 0xF3    //System Offset Calibration
#define SYSGCAL 0xF4    //System Gain Calibration
#define SYNC    0xFC    //Synchronize the A/D Conversion
#define STANDBY 0xFD    //Begin Standby Mode
#define RESET   0xFE    //Reset to Power-Up Values
//#define WAKEUP  0xFF    //Completes SYNC and Exits Standby Mode

/*定义ADS1256寄存器宏*/
#define REG_STATUS      0x00
#define REG_MUX         0x01
#define REG_ADCON       0x02
#define REG_DRATE       0x03
#define REG_IO          0x04
#define REG_OFC0        0x05
#define REG_OFC1        0x06
#define REG_OFC2        0x07
#define REG_FSC0        0x08
#define REG_FSC1        0x09
#define REG_FSC2        0x0A

/*定义状态寄存器配置宏*/
#define STATUS_ORDER_MOST       0x00
#define STATUS_ORDER_LEAST      0x08
#define STATUS_ACAL_DISABLE     0x00
#define STATUS_ACAL_ENABLE      0x04
#define STATUS_BUFEN_DISABLE    0x00
#define STATUS_BUFEN_ENABLE     0x02

/*定义AD控制寄存器配置宏*/
#define ADCON_CLOCK_OFF         0x00
#define ADCON_CLOCK_FCLKIN      0x20
#define ADCON_CLOCK_HALF        0x40
#define ADCON_CLOCK_QUARTER     0x60
#define ADCON_SDCS_OFF          0x00
#define ADCON_SDCS_05uS         0x08
#define ADCON_SDCS_2uS          0x10
#define ADCON_SDCS_10uS         0x18
#define ADCON_PGA_GAIN1         0x00
#define ADCON_PGA_GAIN2         0x01
#define ADCON_PGA_GAIN4         0x02
#define ADCON_PGA_GAIN8         0x03
#define ADCON_PGA_GAIN16        0x04
#define ADCON_PGA_GAIN32        0x05
#define ADCON_PGA_GAIN64        0x06

/*定义数据输出寄存器配置宏*/
#define DRATE_30000SPS  0xF0
#define DRATE_15000SPS  0xE0
#define DRATE_7500SPS   0xD0
#define DRATE_3750SPS   0xC0
#define DRATE_2000SPS   0xB0
#define DRATE_1000SPS   0xA1
#define DRATE_500SPS    0x92
#define DRATE_100SPS    0x82
#define DRATE_60SPS     0x73
#define DRATE_50SPS     0x63
#define DRATE_30SPS     0x53
#define DRATE_25SPS     0x43
#define DRATE_15SPS     0x33
#define DRATE_10SPS     0x23
#define DRATE_5SPS      0x13
#define DRATE_2_5SPS    0x03

/*定义GPIO控制寄存器配置宏*/
#define GPIO_DIR3_OUTPUT        0x00
#define GPIO_DIR3_INPUT         0x80
#define GPIO_DIR2_OUTPUT        0x00
#define GPIO_DIR2_INPUT         0x40
#define GPIO_DIR1_OUTPUT        0x00
#define GPIO_DIR1_INPUT         0x20
#define GPIO_DIR0_OUTPUT        0x00
#define GPIO_DIR0_INPUT         0x10

/* 默认的片选信号处理函数 */
static void ADS1256ChipSelect(ADS1256CSType cs);
/*读ADS1256寄存器*/
static void ReadADS1256Register(ADS1256ObjectType *ads,uint8_t regAddr,uint8_t regNum);
/*写ADS1256寄存器*/
static void WriteADS1256Register(ADS1256ObjectType *ads,uint8_t regAddr,uint8_t regNum);
/*ADS1256校准操作*/
static void ADS1256Calibration(ADS1256ObjectType *ads,uint8_t calCom);
/*ADS1256读取数据*/
static uint32_t ADS1256ReadData(ADS1256ObjectType *ads);
/*设置ADS操作命令*/
static void ADS1256SetCommand(ADS1256ObjectType *ads,uint8_t command);

/*从ADS1256获取单次数据*/
int32_t ADS1256SingleReadData(ADS1256ObjectType *ads,ADS1256ChannelType ainP,ADS1256ChannelType ainN)
{
    uint8_t AINP[9]={0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80};
    uint8_t AINN[9]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
    int32_t result=0;
    
    ads->Register[REG_MUX]=AINP[ainP]||AINN[ainN];
    WriteADS1256Register(ads,REG_MUX,1);
    ADS1256SetCommand(ads,SYNC);
    ADS1256SetCommand(ads,WAKEUP);
    
    result=ADS1256ReadData(ads);
    
    result=result>0x7FFFFF?(result-0x1000000):result;
    
    return result;
}

/*ADS1256读取数据*/
static uint32_t ADS1256ReadData(ADS1256ObjectType *ads)
{
    uint8_t cmd[1]={RDATA};
    uint8_t rData[3];
    uint32_t result=0;
    
    while(ads->GetReadyInput()==1);
    
    ads->ChipSelect(ADS1256CS_Enable);
    
    ads->ReadWrite(cmd,rData,3);
    
    ads->ChipSelect(ADS1256CS_Disable);
    
    result=rData[0];
    result=(result<<8)+rData[1];
    result=(result<<8)+rData[2];
    
    return result;
}

/*ADS1256初始化配置函数*/
void ADS1256Initialization(ADS1256ObjectType *ads,              //待初始化的ADS1256对象
                           ADS1256OrderType order,              //数据顺序
                           ADS1256ACALType acal,                //自动校准使能
                           ADS1256BufenType bufEn,              //模拟量缓存使能
                           ADS1256ClkoutType clkOut,            //时钟输出类型
                           ADS1256SDCSType sdcs,                //传感器检测电流
                           ADS1256GainType gain,                //增益
                           ADS1256DRateType dataRate,           //数据输出速率
                           ADS1256DIOType *dio,                 //输入输出配置
                           ADS1256ReadWriteType readWrite,      //读写函数指针
                           ADS1256ChipSelectType cs,            //片选函数指针
                           ADS1256GetReadyInputType ready,      //就绪函数指针
                           ADS1256DelaymsType delayms           //毫秒延时函数指针
                               )
{
    uint8_t Order[]={STATUS_ORDER_MOST,STATUS_ORDER_LEAST};
    uint8_t ACAL[]={STATUS_ACAL_DISABLE,STATUS_ACAL_ENABLE};
    uint8_t BUFEN[]={STATUS_BUFEN_DISABLE,STATUS_BUFEN_ENABLE};
    
    uint8_t clkck[]={ADCON_CLOCK_OFF,ADCON_CLOCK_FCLKIN,ADCON_CLOCK_HALF,ADCON_CLOCK_QUARTER};
    uint8_t sDCS[]={ADCON_SDCS_OFF,ADCON_SDCS_05uS,ADCON_SDCS_2uS,ADCON_SDCS_10uS};
    uint8_t gains[]={ADCON_PGA_GAIN1,ADCON_PGA_GAIN2,ADCON_PGA_GAIN4,ADCON_PGA_GAIN8,
    ADCON_PGA_GAIN16,ADCON_PGA_GAIN32,ADCON_PGA_GAIN64};
    
    uint8_t dRate[]={DRATE_30000SPS,DRATE_15000SPS,DRATE_7500SPS,DRATE_3750SPS,
    DRATE_2000SPS,DRATE_1000SPS,DRATE_500SPS,DRATE_100SPS,
    DRATE_60SPS,DRATE_50SPS,DRATE_30SPS,DRATE_25SPS,
    DRATE_15SPS,DRATE_10SPS,DRATE_5SPS,DRATE_2_5SPS};
    
    uint8_t dir[4][2]={{GPIO_DIR0_OUTPUT,GPIO_DIR0_INPUT},
    {GPIO_DIR1_OUTPUT,GPIO_DIR1_INPUT},
    {GPIO_DIR2_OUTPUT,GPIO_DIR2_INPUT},
    {GPIO_DIR3_OUTPUT,GPIO_DIR3_INPUT}};
    
    if((ads==NULL)||(readWrite==NULL)||(ready==NULL)||(delayms==NULL))
    {
        return;
    }
    ads->ReadWrite=readWrite;
    ads->GetReadyInput=ready;
    ads->Delay=delayms;
    if(cs==NULL)
    {
        ads->ChipSelect=ADS1256ChipSelect;
    }
    else
    {
        ads->ChipSelect=cs;
    }
    
    for(int i=0; i<11;i++)
    {
        ads->Register[i]=0x00;
    }
    
    ads->Register[REG_STATUS]=Order[order]||ACAL[acal]||BUFEN[bufEn];
    ads->Register[REG_MUX]=0x00;
    ads->Register[REG_ADCON]=clkck[clkOut]||sDCS[sdcs]||gains[gain];
    ads->Register[REG_DRATE]=dRate[dataRate];
    ads->Register[REG_IO]=dir[0][dio[0]]||dir[1][dio[1]]||dir[2][dio[2]]||dir[3][dio[3]];
    
    WriteADS1256Register(ads,REG_STATUS,1);
    WriteADS1256Register(ads,REG_MUX,1);
    WriteADS1256Register(ads,REG_ADCON,1);
    WriteADS1256Register(ads,REG_DRATE,1);
    WriteADS1256Register(ads,REG_IO,1);
    
    ADS1256Calibration(ads,SELFCAL);
    
    ReadADS1256Register(ads,REG_STATUS,11);
}

/*读ADS1256寄存器*/
static void ReadADS1256Register(ADS1256ObjectType *ads,uint8_t regAddr,uint8_t regNum)
{
    uint8_t cmd[2];
    uint8_t rData[11];
    
    cmd[0]=RREG|regAddr;
    cmd[1]=regNum-1;
    
    ads->ChipSelect(ADS1256CS_Enable);
    
    ads->ReadWrite(cmd,rData,2);
    
    cmd[0]=0;
    cmd[1]=0;
    ads->ReadWrite(cmd,rData,regNum);
    
    ads->ChipSelect(ADS1256CS_Disable);
    
    for(int i=0;i<regNum;i++)
    {
        ads->Register[regAddr+i]=rData[i];
    }
}

/*写ADS1256寄存器*/
static void WriteADS1256Register(ADS1256ObjectType *ads,uint8_t regAddr,uint8_t regNum)
{
    uint8_t wData[7];
    uint16_t index=0;
    uint8_t rData[2];
    
    wData[index++]=WREG|regAddr;
    wData[index++]=regNum-1;
    
    for(int i=0;i<regNum;i++)
    {
        wData[index++]=ads->Register[regAddr+i];
    }
    
    ads->ChipSelect(ADS1256CS_Enable);
    
    ads->ReadWrite(wData,rData,index);
    
    ads->ChipSelect(ADS1256CS_Disable);
}

/*ADS1256校准操作*/
static void ADS1256Calibration(ADS1256ObjectType *ads,uint8_t calCom)
{
    uint8_t wData[1];
    uint8_t rData[1];
    
    wData[0]=calCom;
    
    ads->ChipSelect(ADS1256CS_Enable);
    
    ads->ReadWrite(wData,rData,1);
    while(ads->GetReadyInput()==0);
    while(ads->GetReadyInput()==1);
    
    ads->ChipSelect(ADS1256CS_Disable);
}

/*设置ADS操作命令*/
static void ADS1256SetCommand(ADS1256ObjectType *ads,uint8_t command)
{
    uint8_t wData[1];
    uint8_t rData[1];
    
    wData[0]=command;
    
    ads->ChipSelect(ADS1256CS_Enable);
    
    ads->ReadWrite(wData,rData,1);
    
    ads->ChipSelect(ADS1256CS_Disable);
}

/* 默认的片选信号处理函数 */
static void ADS1256ChipSelect(ADS1256CSType cs)
{
    return;
}

/*********** (C) COPYRIGHT 1999-2020 Moonan Technology *********END OF FILE****/