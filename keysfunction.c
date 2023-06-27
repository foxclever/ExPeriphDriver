/******************************************************************************/
/** ģ�����ƣ�������������                                                   **/
/** �ļ����ƣ�keysfunction.c                                                 **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺ʵ�ֶ԰�����ɨ�����ȡ���Ե�������ϼ�������                   **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2012-07-28          ľ��              �����ļ�               **/
/******************************************************************************/ 

#include "stddef.h"
#include "keysfunction.h"

KeyStateQueueType keyState;

/*��ֵ�����г���*/
uint8_t KeyValueDeQueue(void)
{
    uint8_t result; 
    
    if(keyState.pRead==keyState.pWrite)
    {
        result=0;
    }
    else
    {
        result=keyState.queue[keyState.pRead];
        
        if(++keyState.pRead>=KEY_FIFO_SIZE)
        {
            keyState.pRead=0;
        }
    }
    return result;
}

/*��ֵ����г���*/
void KeyValueEnQueue(uint8_t keyCode)
{
    keyState.queue[keyState.pWrite]=keyCode;
    
    if(++keyState.pWrite >= KEY_FIFO_SIZE)
    {
        keyState.pWrite=0;
    }
} 

/*�����ֵ���г���*/
void ClearKeyValueQueue(void)
{
    keyState.pRead=keyState.pWrite;
}

/*��������ɨ�����*/
void KeyValueDetect(KeyObjectType *pKey)
{
    KeyEventType event=KeyNone;
    
    if (CheckKeyDown(pKey))
    {
        if (pKey->actionCount < KEY_FILTER_TIME)
        {
            pKey->actionCount = KEY_FILTER_TIME;
        }
        else if(pKey->actionCount < 2 * KEY_FILTER_TIME)
        {
            pKey->actionCount++;
        }
        else
        {
            if (pKey->actionState == 0)
            {
                pKey->actionState = 1;
                
                /*���Ͱ��������¼���Ϣ*/
                KeyValueEnQueue((uint8_t)((pKey->id<<2) + KeyDown));
                event=KeyDown;
                pKey->pressCount=KEY_FILTER_TIME;
            }
            
            if (pKey->longTime > KEY_FILTER_TIME)
            {
                if (pKey->pressCount < pKey->longTime)
                {
                    /* ���Ͱ����������µ��¼���Ϣ */
                    if (++pKey->pressCount == pKey->longTime)
                    {
                        /* ��ֵ���밴��FIFO */
                        KeyValueEnQueue((uint8_t)((pKey->id<<2) + KeyLong));
                        event=KeyLong;
                        pKey->pressCount=0;
                    }
                }
                else
                {
                    if (pKey->RepeatPeriod > 0)
                    {
                        if (++pKey->RepeatCount >= pKey->RepeatPeriod)
                        {
                            pKey->RepeatCount = 0;
                            /*��������ÿ��10ms����1������*/
                            KeyValueEnQueue((uint8_t)((pKey->id<<2) + KeyDown));
                        }
                    }
                }
            }
            else
            {
                if((pKey->shortTime > KEY_FILTER_TIME)&&(pKey->pressCount < pKey->shortTime))
                {
                    pKey->pressCount++;
                    
                }
            }
        }
    }
    else
    {
        if(pKey->shortTime > KEY_FILTER_TIME)
        {
            if(pKey->pressCount>=pKey->shortTime)
            {
                /* ��ֵ���밴��FIFO */
                KeyValueEnQueue((uint8_t)((pKey->id<<2) + KeyShort));
                
                event=KeyShort;
            }
        }
        
        if(pKey->actionCount > KEY_FILTER_TIME)
        {
            pKey->actionCount = KEY_FILTER_TIME;
        }
        else if(pKey->actionCount != 0)
        {
            pKey->actionCount--;
        }
        else
        {
            if (pKey->actionState == 1)
            {
                pKey->actionState = 0;
                
                /*���Ͱ��������¼���Ϣ*/
                KeyValueEnQueue((uint8_t)((pKey->id<<2)+ KeyUP));
                
                event=KeyUP;
            }
        }
        
        pKey->pressCount=0;
        pKey->RepeatCount = 0;
    }
    
    pKey->eHandler(event);
}

/*������ȡ��ʼ��*/
void KeysInitialization(KeyObjectType *pKey,            //��������
                        uint8_t id,                     //����ID
                        uint16_t longTime,              //������Чʱ��
                        uint16_t shortTime,             //�̰���Чʱ��
                        uint8_t repeatPeriod,           //�����������
                        KeyActiveLevelType level,       //����������Ч��ƽ
                        KeyeHandlerType handler         //�����¼�������ָ��
                            )
{
    if((pKey==NULL)||(handler==NULL))
    {
        return;
    }
    pKey->eHandler=handler;
    
    pKey->id=id;
    pKey->actionCount=0;
    pKey->actionState=0;
    pKey->pressCount=0;
    pKey->RepeatCount=0;
    
    
    pKey->ActiveLevel=level;
    
    pKey->longTime=longTime;
    pKey->shortTime=shortTime;
    pKey->RepeatPeriod=repeatPeriod;
    
}

/*���ĳ��ID�İ�����������ϼ����Ƿ���*/
__weak uint8_t CheckKeyDown(KeyObjectType *pKey)
{
    //��Ӳ����أ���Ӧ����ʵ��
    return 0;
}
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
