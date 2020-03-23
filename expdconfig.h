/******************************************************************************/
/** 模块名称：扩展外设驱动                                                   **/
/** 文件名称：expdconfig.h                                                   **/
/** 版    本：V1.1.0                                                         **/
/** 简    介：实现非MCU集成标准外设的驱动处理和外界设备的操作配置            **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2012-07-28          木南              创建文件               **/
/**     V1.0.1  2013-05-21          木南              添加PM25传感器         **/
/**     V1.0.2  2013-09-13          木南              添加EEROM驱动          **/
/**     V1.0.3  2014-08-08          木南              添加显示驱动           **/
/**     V1.0.4  2015-04-22          木南              添加加速度计驱动       **/
/**     V1.0.5  2016-06-12          木南              添加AD7192驱动         **/
/**     V1.0.6  2017-09-17          木南              添加光敏光度计驱动     **/
/**     V1.0.7  2018-08-01          木南              添加MS5536C驱动        **/
/**     V1.0.8  2018-08-28          木南              添加GPIO模拟I2C驱动    **/
/**     V1.0.9  2018-08-28          木南              添加SHT3x传感器驱动    **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __EXDCONFIG_H
#define __EXDCONFIG_H

/* 定义启用AD56xx系列DAC驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_AD56XX_ENABLE
#define EXPD_AD56XX_ENABLE	(1)
#endif

/* 定义启用AD8400系列数字电位器驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_AD8400_ENABLE
#define EXPD_AD8400_ENABLE      (1)
#endif

/* 定义启用AD527x系列数字电位器驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_AD527x_ENABLE
#define EXPD_AD527x_ENABLE      (1)
#endif

/* 定义启用AD9833函数发生器驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_AD9833_ENABLE
#define EXPD_AD9833_ENABLE      (1)
#endif

/* 定义启用AD7705系列ADC驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_AD7705_ENABLE
#define EXPD_AD7705_ENABLE      (1)
#endif

/* 定义启用AD7192系列ADC驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_AD7192_ENABLE
#define EXPD_AD7192_ENABLE      (1)
#endif

/* 定义启用LTC2400系列ADC驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_LTC2400_ENABLE
#define EXPD_LTC2400_ENABLE	(1)
#endif

/* 定义启用GPIO模拟I2C通讯的宏，1：启用；0：禁用*/
#ifndef EXPD_SIMUI2C_ENABLE
#define EXPD_SIMUI2C_ENABLE	(1)
#endif

/* 定义启用SHT1X温湿度传感器驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_SHT1X_ENABLE
#define EXPD_SHT1X_ENABLE	(1)
#endif

/* 定义启用SHT2X系列温湿度传感器驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_SHT2X_ENABLE
#define EXPD_SHT2X_ENABLE	(1)
#endif

/* 定义启用SHT3X系列数字温湿度传感器驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_SHT3X_ENABLE
#define EXPD_SHT3X_ENABLE	(1)
#endif

/* 定义启用TSEV01CL55红外温度传感器驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_HLPT_ENABLE
#define EXPD_HLPT_ENABLE	(1)
#endif

/* 定义启用MLX90614红外温度传感器驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_MLX_ENABLE
#define EXPD_MLX_ENABLE         (1)
#endif

/* 定义启用DS18B20温度变送器驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_DS18B20_ENABLE
#define EXPD_DS18B20_ENABLE	(1)
#endif

/* 定义启用DHT11温湿度传感器驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_DHT11_ENABLE
#define EXPD_DHT11_ENABLE	(1)
#endif

/* 定义启用MS5536C压力变送器驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_MS5536_ENABLE
#define EXPD_MS5536_ENABLE	(1)
#endif

/* 定义启用MS5803压力变送器驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_MS5803_ENABLE
#define EXPD_MS5803_ENABLE	(1)
#endif

/* 定义启用MS5837压力变送器驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_MS5837_ENABLE
#define EXPD_MS5837_ENABLE	(1)
#endif

/* 定义启用BMP280压力温度传感器驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_BMP280_ENABLE
#define EXPD_BMP280_ENABLE	(1)
#endif

/* 定义启用BME280压力湿度温度传感器驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_BME280_ENABLE
#define EXPD_BME280_ENABLE	(1)
#endif

/* 定义启用BME680环境传感器驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_BME680_ENABLE
#define EXPD_BME680_ENABLE	(1)
#endif

/* 定义启用ADXL345三轴数字加速度计驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_ADXL345_ENABLE
#define EXPD_ADXL345_ENABLE	(1)
#endif

/* 定义启用AT24Cxx外部存储器驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_AT24CXX_ENABLE
#define EXPD_AT24CXX_ENABLE	(1)
#endif

/* 定义启用AT25xxx外部存储器驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_AT25XXX_ENABLE
#define EXPD_AT25XXX_ENABLE	(0)
#endif

/* 定义启用FM24xxxFRAM存储器驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_FM24XXX_ENABLE
#define EXPD_FM24XXX_ENABLE	(1)
#endif

/* 定义启用FM25xxxFRAM存储器驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_FM25XXX_ENABLE
#define EXPD_FM25XXX_ENABLE	(1)
#endif

/* 定义启用nRF24L01无线通讯驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_NRF24L01_ENABLE
#define EXPD_NRF24L01_ENABLE	(1)
#endif

/* 定义启用ESP8266无线通讯驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_ESP8266_ENABLE
#define EXPD_ESP8266_ENABLE	(1)
#endif

/* 定义启用W5500以太网控制器驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_W5500_ENABLE
#define EXPD_W5500_ENABLE	(1)
#endif

/* 定义启用DS17887实时时钟驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_DS17887_ENABLE
#define EXPD_DS17887_ENABLE	(1)
#endif

/* 定义启用宇电AI-BUS通讯驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_AIBUS_ENABLE
#define EXPD_AIBUS_ENABLE	(1)
#endif

/* 定义启用舒茨远红外气体传感器驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_NDIR_ENABLE
#define EXPD_NDIR_ENABLE	(1)
#endif

/* 定义启用HLPM025K3 PM2.5传感器驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_HLPM_ENABLE
#define EXPD_HLPM_ENABLE	(1)
#endif

/* 定义启用LCD1602液晶显示屏驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_LCD1602_ENABLE
#define EXPD_LCD1602_ENABLE	(1)
#endif

/* 定义启用OLED显示屏驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_OLED_ENABLE
#define EXPD_OLED_ENABLE	(1)
#endif

/* 定义启用迪文触摸屏驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_DWLCD_ENABLE
#define EXPD_DWLCD_ENABLE	(1)
#endif

/* 定义启用NTC负温度系数热电阻测温驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_NTC_ENABLE
#define EXPD_NTC_ENABLE		(1)
#endif

/* 定义启用S1336-5BQ光敏二极管作为光度计驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_LUX_ENABLE
#define EXPD_LUX_ENABLE		(1)
#endif

/* 定义启用RTD铂热电阻测温驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_RTD_ENABLE
#define EXPD_RTD_ENABLE         (1)
#endif

/* 定义启用按键操作驱动的宏，1：启用；0：禁用 */
#ifndef EXPD_KEY_ENABLE
#define EXPD_KEY_ENABLE         (1)
#endif


/*AD56xx ADI公司SPI接口系列DAC*/
#if EXPD_AD56XX_ENABLE > (0)
#include "ad56xxfunction.h"
#endif

/*AD8400数字电位器SPI接口*/
#if EXPD_AD8400_ENABLE > (0)
#include "ad840xfunction.h"
#endif

/*AD5270数字电位器SPI接口*/
#if EXPD_AD527x_ENABLE > (0)
#include "ad527xfunction.h"
#endif

/*AD9833函数发生器SPI接口*/
#if EXPD_AD9833_ENABLE > (0)
#include "ad9833function.h"
#endif

/*AD7705双通道SPI接口ADC*/
#if EXPD_AD7705_ENABLE > (0)
#include "ad7705function.h"
#endif

/*AD7192四通道SPI接口ADC*/
#if EXPD_AD7192_ENABLE > (0)
#include "ad7192function.h"
#endif

/*LTC2400单通道SPI接口ADC*/
#if EXPD_LTC2400_ENABLE > (0)
#include "ltc2400function.h"
#endif

/* 启用GPIO模拟I2C通讯*/
#if EXPD_SIMUI2C_ENABLE > (0)
#include "simui2cfunction.h"
#endif

/*SHT1X系列温湿度传感器*/
#if EXPD_SHT1X_ENABLE > (0)
#include "sht1xfunction.h"
#endif

/*SHT2X系列温湿度传感器*/
#if EXPD_SHT2X_ENABLE > (0)
#include "sht2xfunction.h"
#endif

/* 启用SHT3X系列数字温湿度传感器*/
#if EXPD_SHT3X_ENABLE > (0)
#include "sht3xfunction.h"	
#endif

/* 启用TSEV01CL55红外温度传感器*/
#if EXPD_HLPT_ENABLE > (0)
#include "hlptfunction.h"	
#endif

/* 启用MLX90614红外温度传感器*/
#if EXPD_MLX_ENABLE > (0)
#include "mlx90614function.h"	
#endif

/*DS18B20温度变送器*/
#if EXPD_DS18B20_ENABLE > (0)
#include "ds18b20function.h"
#endif

/*DHT11温湿度传感器*/
#if EXPD_DHT11_ENABLE > (0)
#include "dht11function.h"
#endif

/*MS5536C压力变送器SPI接口*/
#if EXPD_MS5536_ENABLE > (0)
#include "ms5536function.h"
#endif

/*MS5803压力变送器I2C接口*/
#if EXPD_MS5803_ENABLE > (0)
#include "ms5803function.h"
#endif

/*MS5837压力变送器I2C接口*/
#if EXPD_MS5837_ENABLE > (0)
#include "ms5837function.h"
#endif

/* 启用BMP280压力温度传感器 */
#if EXPD_BMP280_ENABLE > (0)
#include "bmp280function.h"
#endif

/* 启用BME280压力湿度温度传感器 */
#if EXPD_BME280_ENABLE > (0)
#include "bme280function.h"
#endif

/* 启用BME680环境传感器 */
#if EXPD_BME680_ENABLE > (0)
#include "bme680function.h"
#endif

/*ADXL345三轴数字加速度计*/
#if EXPD_ADXL345_ENABLE
#include "adxl345function.h"
#endif

/*AT24Cxx外部存储器*/
#if EXPD_AT24CXX_ENABLE > (0)
#include "at24cxxfunction.h"
#endif

/*AT25xxx外部存储器*/
#if EXPD_AT25XXX_ENABLE > (0)
#include "at25xxxfunction.h"
#endif

/*FM24xxx外部存储器*/
#if EXPD_FM24XXX_ENABLE > (0)
#include "fm24xxxfunction.h"
#endif

/*FM25xxx外部存储器*/
#if EXPD_FM25XXX_ENABLE > (0)
#include "fm25xxxfunction.h"
#endif

/* 启用nRF24L01无线通讯*/
#if EXPD_NRF24L01_ENABLE > (0)
#include "nrf24l01function.h"
#endif

/* 启用ESP8266无线通讯*/
#if EXPD_ESP8266_ENABLE > (0)
#include "esp8266function.h"
#endif

/*W5500以太网控制器*/
#if EXPD_W5500_ENABLE > (0)
#include "w5500function.h"
#endif

/*DS17887实时时钟*/
#if EXPD_DS17887_ENABLE > (0)
#include "ds17887function.h"
#endif

/*厦门宇电AI-BUS通讯*/
#if EXPD_AIBUS_ENABLE > (0)
#include "aibusfunction.h"
#endif

/*舒茨红外气体传感器*/
#if EXPD_NDIR_ENABLE > (0)
#include "ndirfunction.h"
#endif

/*HLPM025K3 PM25传感器*/
#if EXPD_HLPM_ENABLE > (0)
#include "hlpmfunction.h"
#endif

/*LCD1602液晶显示屏*/
#if EXPD_LCD1602_ENABLE > (0)
#include "lcd1602function.h"
#endif

/*OLED显示屏*/
#if EXPD_OLED_ENABLE > (0)
#include "oledfunction.h"
#endif

/*迪文串口触摸屏*/
#if EXPD_DWLCD_ENABLE > (0)
#include "dwinlcdfunction.h"
#endif

/* 启用NTC负温度系数热电阻测温*/
#if EXPD_NTC_ENABLE > (0)
#include "ntcfunction.h"
#endif

/* 启用S1336-5BQ光敏二极管作为光度计*/
#if EXPD_LUX_ENABLE > (0)
#include "luxfunction.h"
#endif

/* 启用RTD铂热电阻测温*/
#if EXPD_RTD_ENABLE > (0)
#include "rtdfunction.h"
#endif

/* 定义启用按键操作驱动的宏，1：启用；0：禁用*/
#if EXPD_KEY_ENABLE > (0)
#include "keysfunction.h"
#endif

#endif
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/
