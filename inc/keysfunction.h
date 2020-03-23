/******************************************************************************/
/** 模块名称：按键处理驱动                                                   **/
/** 文件名称：keysfunction.h                                                 **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：实现对按键的扫描与读取                                         **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2012-07-28          木南              创建文件               **/
/******************************************************************************/ 

#ifndef __KEYSFUNCTION_H
#define __KEYSFUNCTION_H

#include "stdint.h"

#ifndef KEY_FIFO_SIZE
#define KEY_FIFO_SIZE 10        //键值队列的默认大小
#endif

#define KEY_FILTER_TIME   5     //按键铝箔时间50ms，无论按下或弹起
#define KEY_LONG_TIME     100   //长按键侧时间1s

/*定义按键的激活电平*/
typedef enum KeyActiveLevel {
  KeyLowLevel,
  KeyHighLevel,
  KeyNoDefine
}KeyActiveLevelType;

/*定义键值存储队列的类型*/
typedef struct KeyStateQueue{
  uint8_t queue[KEY_FIFO_SIZE]; //键值存储队列
  uint8_t pRead;		//读队列指针
  uint8_t pWrite;		//写队列指针
}KeyStateQueueType;

/*定义按键对象类型*/
typedef struct KeyObject {
  uint8_t id;                   //按键的ID
  uint8_t Count;                //滤波器计数器
  uint16_t LongCount;           //长按计数器
  uint16_t LongTime;            //按键按下持续时间, 0 表示不检测长按
  uint8_t  State;               //按键当前状态（按下还是弹起）
  uint8_t  RepeatPeriod;        //连续按键周期
  uint8_t  RepeatCount;         //连续按键计数器
  uint8_t ActiveLevel;          //激活电平
}KeyObjectType;

/*定义按键取值枚举*/
typedef enum KeyValue {
  KeyNone,
  KeyDown,
  KeyUP,
  KeyLong
}KeyValueType;

/*按键周期扫描程序*/
void KeyValueDetect(KeyObjectType *pKey);

/*键值出队列程序*/
uint8_t KeyValueDeQueue(void);

/*清除键值队列程序*/
void ClearKeyValueQueue(void);

/*键值入队列程序*/
void KeyValueEnQueue(uint8_t keyCode);

/*按键读取初始化*/
void KeysInitialization(KeyObjectType *pKey,            //按键对象
                        uint8_t id,                     //按键ID
                        uint16_t longTime,              //长按有效时间
                        uint8_t repeatPeriod,           //连按间隔周期
                        KeyActiveLevelType level        //按键按下有效电平
                       );

/*检查某个ID的按键（包括组合键）是否按下*/
uint8_t CheckKeyDown(KeyObjectType *pKey);

#endif
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
