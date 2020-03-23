/******************************************************************************/
/** 模块名称：按键处理驱动                                                   **/
/** 文件名称：keysfunction.c                                                 **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：实现对按键的扫描与读取，对单键和组合键均适用                   **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2012-07-28          木南              创建文件               **/
/******************************************************************************/ 

#include "stddef.h"
#include "keysfunction.h"

KeyStateQueueType keyState;

/*键值出队列程序*/
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
	
/*键值入队列程序*/
void KeyValueEnQueue(uint8_t keyCode)
{
  keyState.queue[keyState.pWrite]=keyCode;

  if(++keyState.pWrite >= KEY_FIFO_SIZE)
  {
    keyState.pWrite=0;
  }
} 

/*清除键值队列程序*/
void ClearKeyValueQueue(void)
{
  keyState.pRead=keyState.pWrite;
}
	
/*按键周期扫描程序*/
void KeyValueDetect(KeyObjectType *pKey)
{
  if (CheckKeyDown(pKey))
  {
    if (pKey->Count < KEY_FILTER_TIME)
    {
      pKey->Count = KEY_FILTER_TIME;
    }
    else if(pKey->Count < 2 * KEY_FILTER_TIME)
    {
      pKey->Count++;
    }
    else
    {
      if (pKey->State == 0)
      {
        pKey->State = 1;

        /*发送按键按下事件消息*/
        KeyValueEnQueue((uint8_t)((pKey->id<<2) + KeyDown));
      }

      if (pKey->LongTime > 0)
      {
        if (pKey->LongCount < pKey->LongTime)
        {
          /* 发送按建持续按下的事件消息 */
          if (++pKey->LongCount == pKey->LongTime)
          {
            /* 键值放入按键FIFO */
            KeyValueEnQueue((uint8_t)((pKey->id<<2) + KeyLong));
          }
        }
        else
        {
          if (pKey->RepeatPeriod > 0)
          {
            if (++pKey->RepeatCount >= pKey->RepeatPeriod)
            {
              pKey->RepeatCount = 0;
              /*长按键后，每隔10ms发送1个按键*/
              KeyValueEnQueue((uint8_t)((pKey->id<<2) + KeyDown));
            }
          }
        }
      }
    }
  }
  else
  {
    if(pKey->Count > KEY_FILTER_TIME)
    {
      pKey->Count = KEY_FILTER_TIME;
    }
    else if(pKey->Count != 0)
    {
      pKey->Count--;
    }
    else
    {
      if (pKey->State == 1)
      {
        pKey->State = 0;

        /*发送按键弹起事件消息*/
        KeyValueEnQueue((uint8_t)((pKey->id<<2)+ KeyUP));
      }
    }

    pKey->LongCount = 0;
    pKey->RepeatCount = 0;
  }
}

/*按键读取初始化*/
void KeysInitialization(KeyObjectType *pKey,uint8_t id,uint16_t longTime, uint8_t repeatPeriod,KeyActiveLevelType level)
{
  if(pKey==NULL)
  {
    return;
  }
	
  pKey->id=id;
  pKey->Count=0;
  pKey->LongCount=0;
  pKey->RepeatCount=0;
  pKey->State=0;
  
  pKey->ActiveLevel=level;
  
  pKey->LongTime=longTime;
  pKey->RepeatPeriod=repeatPeriod;
}

/*检查某个ID的按键（包括组合键）是否按下*/
__weak uint8_t CheckKeyDown(KeyObjectType *pKey)
{
  //与硬件相关，在应用中实现
  return 0;
}
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
