/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：rtdfunction.c                                                  **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现铂热电阻温度测量的相关函数和操作                       **/
/**           所面向的热电阻遵循GB/T 30121-2013(IEC 60751:2008)标准          **/
/**           适用于温度系数为0.003851的铂热电阻                             **/
/**           温度与电阻的关系表达式如下：                                   **/
/**               -200到0摄氏度：Rt=R0*[1+A*t+B*t^2+C*(t-100)*t^3]           **/
/**               0到850摄氏度：Rt=R0*(1+A*t+B*t^2)                          **/
/**           上述表达式中温度单位均为摄氏度,式中各常数的取值如下：          **/
/**               A=3.9083*10^-3                                             **/
/**               B=-5.775*10^-7                                             **/
/**               C=-4.183*10^-13                                            **/
/**           计算0度以上的温度需要解一个一元二次方程，对于二次项系数大于0的 **/
/**           抛物线，当其与西周相切时两根重合，其它有两根的情况均分布在周两 **/
/**           侧。我们计算两根重合时，值为3383.81，由于取值范围的限制，实际永**/
/**           远只能由一个根符合要求，所以只需计算一个根。                   **/
/**           计算0度以下的温度实际需要解一个一元四次方程。                  **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2018-01-11          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stddef.h"
#include "rtdfunction.h"

/* 计算铂电阻温度 */
static float CalcWzpTemperature(RTDObjectType *rtd,float rt);
/* 计算铂电阻温度 */
static float CalcWzcTemperature(RTDObjectType *rtd,float rt);
/* 计算镍电阻温度 */
static float CalcWznTemperature(RTDObjectType *rtd,float rt);

float (*CalcTemperature[])(RTDObjectType *rtd,float rt)={CalcWzpTemperature,CalcWzcTemperature,CalcWznTemperature};

/*计算热电阻测得的温度*/
float CalcRTDTemperature(RTDObjectType *rtd,float rt)
{
    float temp=0.0;
    
    temp=CalcTemperature[rtd->type](rtd,rt);
    
    return temp;
}

/* 计算铂电阻温度 */
static float CalcWzpTemperature(RTDObjectType *rtd,float rt)
{
    float temp=0.0;
    float fr=0.0;
    float dfr=1.0;
    float a=0.0039083;
    float b=-0.0000005775;
    float c=-0.000000000004183;
    
    rtd->temperature=((rt/rtd->nominalR)-1.0)/a;
    
    if(rt<rtd->nominalR)    //温度小于0度
    {
        while(fabs(rtd->temperature-temp)>0.001)
        {
            temp=rtd->temperature;
            fr=rt-rtd->nominalR*(1+a*temp+b*temp*temp+c*(temp-100)*temp*temp*temp);
            dfr=rtd->nominalR*(a+2*b*temp-300*c*temp*temp+4*c*temp*temp*temp);
            
            rtd->temperature=temp+fr/dfr;
        }
        
        return rtd->temperature;
    }
    
    while(fabs(rtd->temperature-temp)>0.001)
    {
        temp=rtd->temperature;
        fr=rt-rtd->nominalR*(1+a*temp+b*temp*temp);
        dfr=rtd->nominalR*(a+2*b*temp);
        
        rtd->temperature=temp+fr/dfr;
    }
    
    return rtd->temperature;
}

/* 计算铜电阻温度 */
static float CalcWzcTemperature(RTDObjectType *rtd,float rt)
{
    float temp=0.0;
    float alpha=0.00428;
    
    temp=((rt/rtd->nominalR)-1.0)/alpha;
    rtd->temperature=temp;
    
    return temp;
}

/* 计算镍电阻温度 */
static float CalcWznTemperature(RTDObjectType *rtd,float rt)
{
    float temp=0.0;
    float a=0.005485;
    
    temp=((rt/rtd->nominalR)-1.0)/a;
    rtd->temperature=temp;
    
    return temp;
}

/* 初始化RTD对象 */
void RTDInitialization(RTDObjectType *rtd,RTDCategoryType type,float nr)
{
    if(rtd==NULL)
    {
        return;
    }
    
    rtd->temperature=0.0;
    rtd->type=type;
    rtd->nominalR=nr;
}
/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/
