/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：w5500function.c                                                **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于使用W5500实现以太网通讯的相关处理                          **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2015-07-30          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include <string.h>
#include "w5500function.h"

#define DATA_BUFFER_SIZE        2048

/*函数注册，首先,应由用户实现SPI注册回调函数来访问WIZCHIP*/
static void RegisterFunction(W5500CSCrisType cris_en,
                             W5500CSCrisType cris_ex,
                             W5500CSCrisType cs_sel,
                             W5500CSCrisType cs_desel,
                             W5500SPIReadByteType spi_rb,
                             W5500SPIWriteByteType spi_wb);
/*初始化芯片参数*/
static W5500ErrorType ChipParametersConfiguration(void);
/*初始化WIZCHIP中的网络参数信息*/
static void NetworkParameterConfiguration(wiz_NetInfo gWIZNETINFO);
/*数据回环处理*/
static uint16_t LoopBackDataHandle(uint8_t *rxBuffer,uint16_t rxSize,uint8_t *txBuffer);
/*默认测试请求*/
static uint16_t DefaultRequest(uint8_t *rqBuffer);

/*W5500对象初始化*/
void W5500Initialization(W5500ObjectType *w5500,            //W5500对象
                         uint8_t mac[6],                   //本地Mac地址
                         uint8_t ip[4],                    //本地IP地址
                         uint8_t sn[4],                    //子网掩码
                         uint8_t gw[4],                    //网关地址
                         uint8_t dns[4],                   //DNS服务器地址
                         dhcp_mode dhcp,                    //DHCP类型
                         W5500CSCrisType cris_en,           //进入临界区
                         W5500CSCrisType cris_ex,           //退出临界区
                         W5500CSCrisType cs_sel,            //片选使能
                         W5500CSCrisType cs_desel,          //片选失能
                         W5500SPIReadByteType spi_rb,       //SPI读字节
                         W5500SPIWriteByteType spi_wb,      //SPI写字节
                         W5500DataParsingType dataParse,    //报文解析函数
                         W5500RequestDataType requst        //数据请求函数
                             )
{
    w5500->w5500Error=W5500_NONE_ERROR;
    
    if((w5500==NULL)||(cris_en==NULL)||(cris_ex==NULL)||(cs_sel==NULL)||(cs_desel==NULL)||(spi_rb==NULL)||(spi_wb==NULL))
    {
        w5500->w5500Error=W5500_INIT_ERROR;
        return;
    }
    
    for(int i=0;i<6;i++)
    {
        w5500->gWIZNETINFO.mac[i]=mac[i];
    }
    
    for(int i=0;i<4;i++)
    {
        w5500->gWIZNETINFO.ip[i]=ip[i];
        w5500->gWIZNETINFO.sn[i]=sn[i];
        w5500->gWIZNETINFO.gw[i]=gw[i];
        w5500->gWIZNETINFO.dns[i]=dns[i];
    }
    
    w5500->gWIZNETINFO.dhcp=dhcp;
    
    /*注册TCP通讯相关的回调函数*/
    RegisterFunction(cris_en,cris_ex,cs_sel,cs_desel,spi_rb,spi_wb);
    
    /*初始化芯片参数*/
    w5500->w5500Error=ChipParametersConfiguration();
    
    /*初始化网络通讯参数*/
    NetworkParameterConfiguration(w5500->gWIZNETINFO);
    
    if(dataParse!=NULL)
    {
        w5500->DataParsing=dataParse;
    }
    else
    {
        w5500->DataParsing=LoopBackDataHandle;
    }
    
    if(requst!=NULL)
    {
        w5500->RequestData=requst;
    }
    else
    {
        w5500->RequestData=DefaultRequest;
    }
}

/*函数注册，首先,应由用户实现SPI注册回调函数来访问WIZCHIP*/
static void RegisterFunction(W5500CSCrisType cris_en,
                             W5500CSCrisType cris_ex,
                             W5500CSCrisType cs_sel,
                             W5500CSCrisType cs_desel,
                             W5500SPIReadByteType spi_rb,
                             W5500SPIWriteByteType spi_wb)
{  
    //临界区回调函数
    reg_wizchip_cris_cbfunc(cris_en, cris_ex);	//注册临界区函数
    
    //片选回调函数
#if   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
    reg_wizchip_cs_cbfunc(cs_sel, cs_desel);//注册SPI片选信号函数
#elif _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_
    reg_wizchip_cs_cbfunc(cs_sel, cs_desel);  // CS必须为低电平.
#else
    
#if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SIP_) != _WIZCHIP_IO_MODE_SIP_
#error "Unknown _WIZCHIP_IO_MODE_"
#else
    reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
#endif
#endif
    //SPI的读写回调函数
    reg_wizchip_spi_cbfunc(spi_rb, spi_wb);	//注册读写函数
}

/*初始化芯片参数*/
static W5500ErrorType ChipParametersConfiguration(void)
{
    uint8_t tmp;
    uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};
    uint8_t tryTimes=0;
    
    //WIZCHIP SOCKET缓存区初始化
    if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1)
    {
//        while(1);
        return W5500_SOCKET_ERROR;
    }
    
    //PHY物理层连接状态检查
    do
    {
        if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1)
        {
            //无网络连接，输出信息
            return W5500_LINK_ERROR;
        }
        
        tryTimes++;
        if((tryTimes>=5)&&(tmp==PHY_LINK_OFF))
        {
            //无网络连接，输出信息
            return W5500_LINK_ERROR;
        }
        
    }while(tmp == PHY_LINK_OFF);

    return W5500_NONE_ERROR;
}

/*初始化WIZCHIP中的网络参数信息*/
static void NetworkParameterConfiguration(wiz_NetInfo gWIZNETINFO)
{
    uint8_t tmpstr[6];
    ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
    ctlnetwork(CN_GET_NETINFO, (void*)&gWIZNETINFO);
    
    ctlwizchip(CW_GET_ID,(void*)tmpstr);
}

/*数据回环处理*/
static uint16_t LoopBackDataHandle(uint8_t *rxBuffer,uint16_t rxSize,uint8_t *txBuffer)
{
    uint16_t txSize = 0;
    
    txSize=(uint16_t)rxSize;
    
    for(int i=0;i<txSize;i++)
    {
        txBuffer[i]=rxBuffer[i];
    }
    
    return txSize;
}

/*默认测试请求*/
static uint16_t DefaultRequest(uint8_t *rqBuffer)
{
    uint16_t rSize=0;
    
    char requstString[]="This is a new client connection.\r\n";
    
    rSize=strlen(requstString);
    
    for(int i=0;i<rSize;i++)
    {
        rqBuffer[i]=requstString[i];
    }
    
    return rSize;
}

/*TCP服务器数据通讯*/
int32_t W5500TCPServer(W5500ObjectType *w5500,W5500SocketType sn,uint16_t lPort)
{
    int32_t ret;
    
    switch(getSn_SR(sn))
    {
    case SOCK_ESTABLISHED:
        {
            if(getSn_IR(sn) & Sn_IR_CON)
            {
                setSn_IR(sn,Sn_IR_CON);
            }
            uint16_t size=0;
            if((size = getSn_RX_RSR(sn)) > 0)
            {
                if(size > DATA_BUFFER_SIZE)
                {
                    size = DATA_BUFFER_SIZE;
                }
                
                uint8_t rxBuffer[DATA_BUFFER_SIZE];
                ret = recv(sn,rxBuffer,size);
                if(ret <= 0)
                {
                    return ret;
                }
                
                //添加数据解析及响应的函数
                uint8_t txBuffer[DATA_BUFFER_SIZE];
                uint16_t length=w5500->DataParsing(rxBuffer,ret,txBuffer);
                
                uint16_t sentsize=0;
                while(length != sentsize)
                {
                    ret = send(sn,txBuffer+sentsize,length-sentsize);
                    if(ret < 0)
                    {
                        close(sn);
                        return ret;
                    }
                    sentsize += ret; // 不用管SOCKERR_BUSY, 因为它是零.
                }
            }
            break;
        }
    case SOCK_CLOSE_WAIT:
        {
            if((ret=disconnect(sn)) != SOCK_OK)
            {
                return ret;
            }
            break;
        }
    case SOCK_INIT:
        {
            if( (ret = listen(sn)) != SOCK_OK)
            {
                return ret;
            }
            break;
        }
    case SOCK_CLOSED:
        {
            if((ret=socket(sn,Sn_MR_TCP,lPort,0x00)) != sn)
            {
                return ret;
            }
            break;
        }
    default:
        {
            break;
        }
    }
    return 1;
}

/*TCP客户端数据通讯*/
int32_t W5500TCPClient(W5500ObjectType *w5500,W5500SocketType sn,uint16_t lPort,uint8_t *rIP,uint16_t rPort)
{
    int32_t ret;
    uint16_t size = 0;
    uint16_t sentsize=0;
    uint8_t rxBuffer[DATA_BUFFER_SIZE];
    uint8_t txBuffer[DATA_BUFFER_SIZE];
    
    switch(getSn_SR(sn))
    {
    case SOCK_INIT:
        {
            if( (ret = connect(sn,rIP,rPort)) != SOCK_OK)
            {
                return ret;
            }
            break;
        }
    case SOCK_ESTABLISHED:
        {
            if(getSn_IR(sn) & Sn_IR_CON)
            {
                setSn_IR(sn,Sn_IR_CON);
            }
            
            if((size = getSn_RX_RSR(sn)) > 0)
            {
                if(size > DATA_BUFFER_SIZE)
                {
                    size = DATA_BUFFER_SIZE;
                }
                
                ret = recv(sn,rxBuffer,size);
                if(ret <= 0)
                {
                    return ret;
                }
                
                size=w5500->DataParsing(rxBuffer,ret,txBuffer);
                
                sentsize = 0;
                while(size != sentsize)
                {
                    ret = send(sn,txBuffer+sentsize,size-sentsize);
                    if(ret < 0)
                    {
                        close(sn);
                        return ret;
                    }
                    sentsize += ret; 
                }
            }
            else
            {
                size=w5500->RequestData(txBuffer);
                
                sentsize = 0;
                while(size != sentsize)
                {
                    ret = send(sn,txBuffer+sentsize,size-sentsize);
                    if(ret < 0)
                    {
                        close(sn);
                        return ret;
                    }
                    sentsize += ret; 
                }
            }
            break;
        }
    case SOCK_CLOSE_WAIT:
        {
            if((ret=disconnect(sn)) != SOCK_OK)
            {
                return ret;
            }
            break;
        }
    case SOCK_CLOSED:
        {
            if((ret=socket(sn,Sn_MR_TCP,lPort,0x00)) != sn)
            {
                return ret;
            }
            break;
        }
    default:
        {
            break;
        }
    }
    return 1;
}

/*UDP服务器数据通讯*/
int32_t W5500UDPServer(W5500ObjectType *w5500,W5500SocketType sn, uint16_t lPort,uint8_t *rIP,uint16_t rPort)
{
    int32_t ret;
    uint8_t rxBuffer[DATA_BUFFER_SIZE];
    uint8_t txBuffer[DATA_BUFFER_SIZE];
    
    switch(getSn_SR(sn))
    {
    case SOCK_UDP:
        {
            uint16_t size=0;
            if((size = getSn_RX_RSR(sn)) > 0)
            {
                if(size > DATA_BUFFER_SIZE)
                {
                    size = DATA_BUFFER_SIZE;
                }
                
                ret = recvfrom(sn,rxBuffer,size,rIP,(uint16_t*)&rPort);
                
                if(ret <= 0)
                {
                    return ret;
                }
                
                //添加数据解析及响应的函数
                size = w5500->DataParsing(rxBuffer,ret,txBuffer);
                
                uint16_t sentsize = 0;
                while(sentsize != size)
                {
                    ret = sendto(sn,txBuffer+sentsize,size-sentsize,rIP,rPort);
                    if(ret < 0)
                    {
                        return ret;
                    }
                    sentsize += ret; // 不用管SOCKERR_BUSY,因为它是零
                }
            }
            break;
        }
    case SOCK_CLOSED:
        {
            if((ret=socket(sn,Sn_MR_UDP,lPort,0x00)) != sn)
            {
                return ret;
            }
            break;
        }
    default:
        {
            break;
        }
    }
    return 1;
}

/*UDP客户端数据通讯*/
int32_t W5500UDPClient(W5500ObjectType *w5500,W5500SocketType sn, uint16_t lPort,uint8_t *rIP,uint16_t rPort)
{
    int32_t ret;
    uint8_t rxBuffer[DATA_BUFFER_SIZE];
    uint8_t txBuffer[DATA_BUFFER_SIZE];
    
    switch(getSn_SR(sn))
    {
    case SOCK_UDP:
        {
            uint16_t sentsize;
            uint16_t size=0;
            if((size = getSn_RX_RSR(sn)) > 0)
            {
                if(size > DATA_BUFFER_SIZE)
                {
                    size = DATA_BUFFER_SIZE;
                }
                
                ret = recvfrom(sn,rxBuffer,size,rIP,(uint16_t*)&rPort);
                
                if(ret <= 0)
                {
                    return ret;
                }
                
                //添加数据解析及响应的函数
                size = w5500->DataParsing(rxBuffer,ret,txBuffer);
                
                sentsize = 0;
                while(sentsize != size)
                {
                    ret = sendto(sn,txBuffer+sentsize,size-sentsize,rIP,rPort);
                    if(ret < 0)
                    {
                        return ret;
                    }
                    sentsize += ret; // 不用管SOCKERR_BUSY,因为它是零
                }
            }
            else
            {
                size=w5500->RequestData(txBuffer);
                
                sentsize = 0;
                while(sentsize != size)
                {
                    ret = sendto(sn,txBuffer+sentsize,size-sentsize,rIP,rPort);
                    if(ret < 0)
                    {
                        return ret;
                    }
                    sentsize += ret; // 不用管SOCKERR_BUSY,因为它是零
                }
            }
            break;
        }
    case SOCK_CLOSED:
        {
            if((ret=socket(sn,Sn_MR_UDP,lPort,0x00)) != sn)
            {
                return ret;
            }
            break;
        }
    default:
        {
            break;
        }
    }
    
    return 1;
}
/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
