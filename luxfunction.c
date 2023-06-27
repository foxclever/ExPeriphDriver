/******************************************************************************/
/** ģ�����ƣ���չ��������                                                   **/
/** �ļ����ƣ�luxfunction.c                                                  **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺�����Ͷ���S1336-5BQ������������Ϊ��ȼƵĸ��ֺ����Ͳ���        **/
/**           S1336-5BQ���������ܵ�ͨ������׼��100lxΪ5΢��                  **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2017-09-01          ľ��              �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#include "luxfunction.h"
#include "stddef.h"

/*�������ǿ��*/
float CalcLxIllumination(LuxObjectType *lm,float mVoltage)
{
    float lux=0.0;
    lux=(mVoltage-lm->vref)*100/(lm->rnf*lm->isc100lux);
    lm->lux=lux;
    return lux;
}

/* ��ȼ������ʼ�� */
void LuxInitialization(LuxObjectType *lm,float isc,float vref,float rnf)
{
    if(lm==NULL)
    {
        return;
    }
    
    lm->lux=0.0;
    lm->isc100lux=isc;
    lm->rnf=rnf;
    lm->vref=vref;
}

/*********** (C) COPYRIGHT 1999-2017 Moonan Technology *********END OF FILE****/
