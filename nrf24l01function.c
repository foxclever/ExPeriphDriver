/******************************************************************************/
/** ģ�����ƣ���չ��������                                                   **/
/** �ļ����ƣ�nrf24l01function.h                                             **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʵ��nRF24L01����ͨѶģ��Ĳ���                             **/
/**           nRF24L01�������ʽ��ָ��+����                                  **/
/**           ָ��Ϊ��λ��ǰ�������ǵ��ֽ���ǰ������ָ�����£�               **/
/**           +------------+--------+---------------------------+            **/
/**           |  ָ������  |ָ���ʽ|          ʵ�ֲ���         |            **/
/**           +------------+--------+---------------------------+            **/
/**           | R_REGISTER |000AAAAA|���Ĵ�����AAAAAΪ�Ĵ�����ַ|            **/
/**           +------------+--------+---------------------------+            **/
/**           | W_REGISTER |001AAAAA|д�Ĵ�����AAAAAΪ�Ĵ�����ַ|            **/
/**           +------------+--------+---------------------------+            **/
/**           |R_RX_PAYLOAD|01100001|��RX��Ч���ݣ���RXģʽ��Ч |            **/
/**           +------------+--------+---------------------------+            **/
/**           |W_TX_PAYLOAD|10100000|дTX��Ч���ݣ���TXģʽ��Ч |            **/
/**           +------------+--------+---------------------------+            **/
/**           |  FLUSH_TX  |11100001|���TX_FIFO�Ĵ���          |            **/
/**           +------------+--------+---------------------------+            **/
/**           |  FLUSH_RX  |11100010|���RX_FIFO�Ĵ���          |            **/
/**           +------------+--------+---------------------------+            **/
/**           | REUSE_TX_PL|11100011|����ʹ����һ����Ч����     |            **/
/**           +------------+--------+---------------------------+            **/
/**           |    NOP     |11111111|�ղ�������������״̬�Ĵ��� |            **/
/**           +------------+--------+---------------------------+            **/
/**           �Ĵ����������ַ���£�                                         **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |��ַ(Hex)|   ����    |����|    ˵��                   |       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  00     |  CONFIG   |��д| ���üĴ���                |       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  01     |   EN_AA   |��д| ʹ�� �Զ�Ӧ�� ����        |       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  02     | EN_RXADDR |��д| ���յ�ַ����              |       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  03     |  SETUP_AW |��д|���õ�ַ���,��������ͨ��  |       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  04     | SETUP_RETR|��д|�����Զ��ط�               |       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  05     |  RF_CH    |��д|��Ƶͨ��                   |       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  06     |  RF_SETUP |��д|��Ƶ�Ĵ���                 |       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  07     |  STATUS   |��д|״̬�Ĵ���                 |       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  08     | OBSERVE_TX|ֻ��|���ͼ��Ĵ���             |       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  09     |     CD    |ֻ��|�ز����                   |       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  0A     | RX_ADDR_P0|��д|����ͨ�� 0 ���յ�ַ        |       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  0B     | RX_ADDR_P1|��д|����ͨ�� 1 ���յ�ַ        |       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  0C     | RX_ADDR_P2|��д|����ͨ�� 2 ���յ�ַ        |       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  0D     | RX_ADDR_P3|��д|����ͨ�� 3 ���յ�ַ        |       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  0E     | RX_ADDR_P4|��д|����ͨ�� 4 ���յ�ַ        |       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  0F     | RX_ADDR_P5|��д|����ͨ�� 5 ���յ�ַ        |       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  10     |  TX_ADDR  |��д|���͵�ַ                   |       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  11     |  RX_PW_P0 |��д|��������ͨ�� 0 ��Ч���ݿ��|       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  12     |  RX_PW_P1 |��д|��������ͨ�� 1 ��Ч���ݿ��|       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  13     |  RX_PW_P2 |��д|��������ͨ�� 2 ��Ч���ݿ��|       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  14     |  RX_PW_P3 |��д|��������ͨ�� 3 ��Ч���ݿ��|       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  15     |  RX_PW_P4 |��д|��������ͨ�� 4 ��Ч���ݿ��|       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  16     |  RX_PW_P5 |��д|��������ͨ�� 5 ��Ч���ݿ��|       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  17     |FIFO_STATUS|ֻ��|FIFO ״̬�Ĵ���            |       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  N/A    |   TX_PLD  |ֻд|TXdata payload register    |       **/
/**           +---------+-----------+----+---------------------------+       **/
/**           |  N/A    |   RX_PLD  |ֻ��|RX data payload register   |       **/
/**           +---------+-----------+----+---------------------------+       **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2018-01-15          ľ��              �����ļ�               **/
/**     V1.0.1  2019-12-17          ľ��              �޸�Ϊ���ڶ���Ĳ���   **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "nrf24l01function.h"

/*nRF24L01�Ĵ�����������*/
#define READ_REG_NRF    0x00  //�����üĴ���,��5λΪ�Ĵ�����ַ
#define WRITE_REG_NRF   0x20  //д���üĴ���,��5λΪ�Ĵ�����ַ
#define RD_RX_PLOAD     0x61  //��RX��Ч����,1~32�ֽ�
#define WR_TX_PLOAD     0xA0  //дTX��Ч����,1~32�ֽ�
#define FLUSH_TX        0xE1  //���TX FIFO�Ĵ���.����ģʽ����
#define FLUSH_RX        0xE2  //���RX FIFO�Ĵ���.����ģʽ����
#define REUSE_TX_PL     0xE3  //����ʹ����һ������,CEΪ��,���ݰ������Ϸ���.
#define NOP             0xFF  //�ղ���,����������״̬�Ĵ���	 

/*nRF24L01�Ĵ�����ַ*/
#define CONFIG          0x00  //���üĴ�����ַ;bit0:1����ģʽ,0����ģʽ;bit1:��ѡ��;bit2:CRCģʽ;bit3:CRCʹ��;
//bit4:�ж�MAX_RT(�ﵽ����ط������ж�)ʹ��;bit5:�ж�TX_DSʹ��;bit6:�ж�RX_DRʹ��
#define EN_AA           0x01  //ʹ���Զ�Ӧ����  bit0~5,��Ӧͨ��0~5
#define EN_RXADDR       0x02  //���յ�ַ����,bit0~5,��Ӧͨ��0~5
#define SETUP_AW        0x03  //���õ�ַ���(��������ͨ��):bit1,0:00,3�ֽ�;01,4�ֽ�;02,5�ֽ�;
#define SETUP_RETR      0x04  //�����Զ��ط�;bit3:0,�Զ��ط�������;bit7:4,�Զ��ط���ʱ 250*x+86us
#define RF_CH           0x05  //RFͨ��,bit6:0,����ͨ��Ƶ��;
#define RF_SETUP        0x06  //RF�Ĵ���;bit3:��������(0:1Mbps,1:2Mbps);bit2:1,���书��;bit0:�������Ŵ�������
#define STATUS          0x07  //״̬�Ĵ���;bit0:TX FIFO����־;bit3:1,��������ͨ����(���:6);bit4,�ﵽ�����ط�
//bit5:���ݷ�������ж�;bit6:���������ж�;
#define OBSERVE_TX      0x08  //���ͼ��Ĵ���,bit7:4,���ݰ���ʧ������;bit3:0,�ط�������
#define CD              0x09  //�ز����Ĵ���,bit0,�ز����;
#define RX_ADDR_P0      0x0A  //����ͨ��0���յ�ַ,��󳤶�5���ֽ�,���ֽ���ǰ
#define RX_ADDR_P1      0x0B  //����ͨ��1���յ�ַ,��󳤶�5���ֽ�,���ֽ���ǰ
#define RX_ADDR_P2      0x0C  //����ͨ��2���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P3      0x0D  //����ͨ��3���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P4      0x0E  //����ͨ��4���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P5      0x0F  //����ͨ��5���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define TX_ADDR         0x10  //���͵�ַ(���ֽ���ǰ),ShockBurstģʽ��,RX_ADDR_P0��˵�ַ���
#define RX_PW_P0        0x11  //��������ͨ��0��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P1        0x12  //��������ͨ��1��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P2        0x13  //��������ͨ��2��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P3        0x14  //��������ͨ��3��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P4        0x15  //��������ͨ��4��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P5        0x16  //��������ͨ��5��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define NRF_FIFO_STATUS 0x17  //FIFO״̬�Ĵ���;bit0,RX FIFO�Ĵ����ձ�־;bit1,RX FIFO����־;bit2,3,����
//bit4,TX FIFO�ձ�־;bit5,TX FIFO����־;bit6,1,ѭ��������һ���ݰ�.0,��ѭ��;

/*�������״̬*/
#define MAX_TX          0x10  //�ﵽ����ʹ����ж�
#define TX_OK           0x20  //TX��������ж�
#define RX_OK           0x40  //���յ������ж�

/*24L01���ͽ������ݿ�ȶ���*/
#define TX_ADR_WIDTH    5   	//5�ֽڵĵ�ַ���
#define RX_ADR_WIDTH    5   	//5�ֽڵĵ�ַ���
#define TX_PLOAD_WIDTH  32  	//32�ֽڵ��û����ݿ��
#define RX_PLOAD_WIDTH  32  	//32�ֽڵ��û����ݿ��

const uint8_t TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ
const uint8_t RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ

/*д�Ĵ���*/
static uint8_t NRF24L01WriteRegister(NRF24L01ObjectType *nrf,uint8_t reg,uint8_t value);
/*��ȡ�Ĵ���ֵ*/
static uint8_t NRF24L01ReadRegigster(NRF24L01ObjectType *nrf,uint8_t reg);
/*��ָ��λ�ö���ָ�����ȵ�����*/
static uint8_t NRF24L01ReadBuffer(NRF24L01ObjectType *nrf,uint8_t reg,uint8_t *pBuf,uint8_t len);
/*��ָ��λ��дָ�����ȵ�����*/
static uint8_t NRF24L01WriteBuffer(NRF24L01ObjectType *nrf,uint8_t reg, uint8_t *pBuf, uint8_t len);
/*���24L01�Ƿ����,����ֵ:0���ɹ�;1��ʧ��*/
static uint8_t NRF24L01Check(NRF24L01ObjectType *nrf);
/*����nRF24L01��ģʽ*/
static void SetNRF24L01Mode(NRF24L01ObjectType *nrf,NRF24L01ModeType mode);
/*ȱʡƬѡ������*/
static void NRF24L01CSDefault(NRF24L01CSType cs);

/*����NRF24L01����һ�����ݰ�*/
/*������txbuf:�����������׵�ַ*/
/*����ֵ���������״��*/
uint8_t NRF24L01TransmitPacket(NRF24L01ObjectType *nrf,uint8_t *txbuf)
{
    uint8_t status;
    
    SetNRF24L01Mode(nrf,NRF24L01TxMode);
    
    nrf->ChipEnable(NRF24L01CE_Disable);
    NRF24L01WriteBuffer(nrf,WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//д���ݵ�TX BUF 32���ֽ�
    nrf->ChipEnable(NRF24L01CE_Enable);//��������	   
    
    while(nrf->GetIRQ()!=0);//�ȴ��������
    status=NRF24L01ReadRegigster(nrf,STATUS);  //��ȡ״̬�Ĵ�����ֵ	   
    NRF24L01WriteRegister(nrf,WRITE_REG_NRF+STATUS,status); //���TX_DS��MAX_RT�жϱ�־
    if(status&MAX_TX)//�ﵽ����ط�����
    {
        NRF24L01WriteRegister(nrf,FLUSH_TX,0xFF);//���TX FIFO�Ĵ��� 
        return MAX_TX; 
    }
    if(status&TX_OK)//�������
    {
        return TX_OK;
    }
    return 0xFF;//����ԭ����ʧ��
}

/*����NRF24L01����һ�����ݰ�*/
/*������txbuf:�����������׵�ַ*/
/*����ֵ:0��������ɣ��������������*/
uint8_t NRF24L01ReceivePacket(NRF24L01ObjectType *nrf,uint8_t *rxbuf)
{
    uint8_t status;		    							   
    
    SetNRF24L01Mode(nrf,NRF24L01RxMode);
    
    status=NRF24L01ReadRegigster(nrf,STATUS);  //��ȡ״̬�Ĵ�����ֵ    	 
    NRF24L01WriteRegister(nrf,WRITE_REG_NRF+STATUS,status); //���TX_DS��MAX_RT�жϱ�־
    if(status&RX_OK)//���յ�����
    {
        NRF24L01ReadBuffer(nrf,RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
        NRF24L01WriteRegister(nrf,FLUSH_RX,0xFF);//���RX FIFO�Ĵ��� 
        return 0; 
    }	   
    return 1;//û�յ��κ�����
}

/*nRF24L01�����ʼ������*/
NRF24L01ErrorType NRF24L01Initialization(NRF24L01ObjectType *nrf,	//nRF24L01����
                                         NRF24L01ReadWriteByte spiReadWrite,	//SPI��д����ָ��
                                         NRF24L01ChipSelect cs,	//Ƭѡ�źŲ�������ָ��
                                         NRF24L01ChipEnable ce,	//ʹ���źŲ�������ָ��
                                         NRF24L01GetIRQ irq,			//�ж��źŻ�ȡ����ָ��
                                         NRF24L01Delayms delayms	//������ʱ
                                             )
{
    int retry=0;
    
    if((nrf==NULL)||(spiReadWrite==NULL)||(ce==NULL)||(irq==NULL)||(delayms==NULL))
    {
        return NRF24L01_InitError;
    }
    nrf->ReadWriteByte=spiReadWrite;
    nrf->ChipEnable=ce;
    nrf->GetIRQ=irq;
    nrf->Delayms=delayms;
    
    if(cs!=NULL)
    {
        nrf->ChipSelect=cs;
    }
    else
    {
        nrf->ChipSelect=NRF24L01CSDefault;
    }
    
    while(NRF24L01Check(nrf)&&(retry<5))
    {
        nrf->Delayms(300);
        retry++;
    }
    
    if(retry>=5)
    {
        return NRF24L01_Absent;
    }
    
    for(int i=0;i<8;i++)
    {
        nrf->reg[i]=0;
    }
    
    SetNRF24L01Mode(nrf,NRF24L01RxMode);
    
    return NRF24L01_NoError;
}

/*����nRF24L01��ģʽ*/
static void SetNRF24L01Mode(NRF24L01ObjectType *nrf,NRF24L01ModeType mode)
{
    nrf->ChipEnable(NRF24L01CE_Disable);
    
    if(mode==NRF24L01RxMode)
    {
        /*��ʼ��NRF24L01��RXģʽ������RX��ַ,дRX���ݿ��,ѡ��RFƵ��,�����ʺ�LNA HCURR��
        ��CE��ߺ�,������RXģʽ,�����Խ���������*/
        NRF24L01WriteBuffer(nrf,WRITE_REG_NRF+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH);//дRX�ڵ��ַ
        
        NRF24L01WriteRegister(nrf,WRITE_REG_NRF+EN_AA,0x01);    	//ʹ��ͨ��0���Զ�Ӧ��    
        NRF24L01WriteRegister(nrf,WRITE_REG_NRF+EN_RXADDR,0x01);	//ʹ��ͨ��0�Ľ��յ�ַ  	 
        NRF24L01WriteRegister(nrf,WRITE_REG_NRF+RF_CH,40);				//����RFͨ��Ƶ��		  
        NRF24L01WriteRegister(nrf,WRITE_REG_NRF+RX_PW_P0,RX_PLOAD_WIDTH);	//ѡ��ͨ��0����Ч���ݿ�� 	    
        NRF24L01WriteRegister(nrf,WRITE_REG_NRF+RF_SETUP,0x0F);	//����TX�������,0db����,2Mbps,���������濪��   
        NRF24L01WriteRegister(nrf,WRITE_REG_NRF+CONFIG, 0x0F);	//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
    }
    else
    {
        /*��ʼ��NRF24L01��TXģʽ������TX��ַ,дTX���ݿ��,����RX�Զ�Ӧ��ĵ�ַ,
        ���TX��������,ѡ��RFƵ��,�����ʺ�LNA HCURR��PWR_UP,CRCʹ�ܣ���CE��ߺ�,
        ������RXģʽ,�����Խ��������ˣ�CEΪ�ߴ���10us,����������*/
        NRF24L01WriteBuffer(nrf,WRITE_REG_NRF+TX_ADDR,(uint8_t*)TX_ADDRESS,TX_ADR_WIDTH);			//дTX�ڵ��ַ 
        NRF24L01WriteBuffer(nrf,WRITE_REG_NRF+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH);	//����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  
        
        NRF24L01WriteRegister(nrf,WRITE_REG_NRF+EN_AA,0x01);     //ʹ��ͨ��0���Զ�Ӧ��    
        NRF24L01WriteRegister(nrf,WRITE_REG_NRF+EN_RXADDR,0x01); //ʹ��ͨ��0�Ľ��յ�ַ  
        NRF24L01WriteRegister(nrf,WRITE_REG_NRF+SETUP_RETR,0x1A);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
        NRF24L01WriteRegister(nrf,WRITE_REG_NRF+RF_CH,40);       //����RFͨ��Ϊ40
        NRF24L01WriteRegister(nrf,WRITE_REG_NRF+RF_SETUP,0x0F);  //����TX�������,0db����,2Mbps,���������濪��   
        NRF24L01WriteRegister(nrf,WRITE_REG_NRF+CONFIG,0x0E);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
    }
    
    nrf->reg[CONFIG]=NRF24L01ReadRegigster(nrf,CONFIG);
    nrf->reg[EN_AA]=NRF24L01ReadRegigster(nrf,EN_AA);
    nrf->reg[EN_RXADDR]=NRF24L01ReadRegigster(nrf,EN_RXADDR);
    nrf->reg[SETUP_AW]=NRF24L01ReadRegigster(nrf,SETUP_AW);
    nrf->reg[SETUP_RETR]=NRF24L01ReadRegigster(nrf,SETUP_RETR);
    nrf->reg[RF_CH]=NRF24L01ReadRegigster(nrf,RF_CH);
    nrf->reg[RF_SETUP]=NRF24L01ReadRegigster(nrf,RF_SETUP);
    nrf->reg[STATUS]=NRF24L01ReadRegigster(nrf,STATUS);
    
    nrf->ChipEnable(NRF24L01CE_Enable); //CEΪ�ߡ�����RXʱ���������ģʽ������ΪTXʱ,10us����������
}

/*���24L01�Ƿ����,����ֵ:0���ɹ�;1��ʧ��*/
static uint8_t NRF24L01Check(NRF24L01ObjectType *nrf)
{
    uint8_t writeBuf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
    uint8_t readBuf[5]={0XAA,0XAA,0XAA,0XAA,0XAA};
    uint8_t status=0;
    
    NRF24L01WriteBuffer(nrf,WRITE_REG_NRF+TX_ADDR,writeBuf,5);/*д��5���ֽڵĵ�ַ*/
    NRF24L01ReadBuffer(nrf,TX_ADDR,readBuf,5); /*����д��ĵ�ַ*/
    
    for(int i=0;i<5;i++)
    {
        if(readBuf[i]!=0XA5)
        {
            status=1;			//���nRF24L01����
            break;
        }
    }
    
    return status;
}	 	

/*д�Ĵ���*/
/*������reg:ָ���Ĵ�����ַ*/
/*      value:д���ֵ*/
/*����ֵ��״ֵ̬*/
static uint8_t NRF24L01WriteRegister(NRF24L01ObjectType *nrf,uint8_t reg,uint8_t value)
{
    uint8_t status;
    
    nrf->ChipSelect(NRF24L01CS_Enable);             //ʹ��SPI����
    status =nrf->ReadWriteByte(reg);   //���ͼĴ����� 
    nrf->ReadWriteByte(value);         //д��Ĵ�����ֵ
    nrf->ChipSelect(NRF24L01CS_Disable);            //��ֹSPI����
    
    return(status);                       //����״ֵ̬
}

/*��ȡ�Ĵ���ֵ*/
/*������reg:Ҫ���ļĴ���*/
/*����ֵ����ȡ�ļĴ���ֵ*/
static uint8_t NRF24L01ReadRegigster(NRF24L01ObjectType *nrf,uint8_t reg)
{
    uint8_t reg_val;	    
    
    nrf->ChipSelect(NRF24L01CS_Enable);             //ʹ��SPI����		
    
    nrf->ReadWriteByte(reg);           //���ͼĴ�����
    reg_val=nrf->ReadWriteByte(0XFF);  //��ȡ�Ĵ�������
    nrf->ChipSelect(NRF24L01CS_Disable);            //��ֹSPI����
    
    return(reg_val);                      //����״ֵ̬
}

/*��ָ��λ�ö���ָ�����ȵ�����*/
/*������reg:�Ĵ���(λ��)*/
/*      *pBuf:����ָ��*/
/*      len:���ݳ���*/
/*����ֵ,�˴ζ�����״̬�Ĵ���ֵ*/ 
static uint8_t NRF24L01ReadBuffer(NRF24L01ObjectType *nrf,uint8_t reg,uint8_t *pBuf,uint8_t len)
{
    uint8_t status;	       
    
    nrf->ChipSelect(NRF24L01CS_Enable);              //ʹ��SPI����
    
    status=nrf->ReadWriteByte(reg);    //���ͼĴ���ֵ(λ��),����ȡ״ֵ̬ 
    
    for(int i=0;i<len;i++)
    {
        pBuf[i]=nrf->ReadWriteByte(0XFF);//��������
    }
    
    nrf->ChipSelect(NRF24L01CS_Disable);            //�ر�SPI����
    
    return status;                        //���ض�����״ֵ̬
}

/*��ָ��λ��дָ�����ȵ�����*/
/*������reg:�Ĵ���(λ��)*/
/*      *pBuf:����ָ��*/
/*      len:���ݳ���*/
/*����ֵ,�˴ζ�����״̬�Ĵ���ֵ*/
static uint8_t NRF24L01WriteBuffer(NRF24L01ObjectType *nrf,uint8_t reg, uint8_t *pBuf, uint8_t len)
{
    uint8_t status;
    
    nrf->ChipSelect(NRF24L01CS_Enable);             //ʹ��SPI����
    
    status = nrf->ReadWriteByte(reg);  //���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
    
    for(int i=0; i<len; i++)
    {
        nrf->ReadWriteByte(pBuf[i]);     //д������	 
    }
    
    nrf->ChipSelect(NRF24L01CS_Disable);            //�ر�SPI����
    
    return status;                        //���ض�����״ֵ̬
}

/*ȱʡƬѡ������*/
static void NRF24L01CSDefault(NRF24L01CSType cs)
{
    //������SPIͨѶʱ��Ƭѡ�ź�Ӳ����·ѡ�е����
    return;
}

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/
