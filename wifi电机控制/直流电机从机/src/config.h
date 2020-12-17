#ifndef _CONFIG_H
#define _CONFIG_H

#include <STC12C5A60S2.h>
#include <intrins.h>
#include <math.h>

#include "EEPROM.h"
#include "CC1101.h"
#include "AM2321.h"
#include "crc.h"
#include "ADC.h"
#include "ADXL345.h"
#include "GF3658.h"
#include "SHT10.h"



sbit Sensor_SDA = P1^1;
sbit  P1_0 = P1^0;//定义P1.0端口
sbit shuchu = P1^1;


sbit  COM1 = P2^6;
sbit  COM2 = P1^4;
sbit  COM3 = P1^1;
sbit  COM4 = P2^5;
sbit  COM5 = P2^4;

sbit  set1 = P1^0;
sbit  set2 = P3^2;
sbit  set3 = P0^5;
sbit  set4 = P0^4;
sbit  set5 = P0^3;
sbit  set6 = P0^2;
sbit  set7 = P0^1;
sbit  set8 = P0^0;



/*************** 用户定义参数 *****************************/

#define MAIN_Fosc		11059200UL	
#define Baudrate0		9600UL		
									
/**********************************************************/
/****************** 编译器自动生成，用户请勿修改 ************************************/

#define BRT_Reload			(256 - MAIN_Fosc / 16 / Baudrate0)		//Calculate the timer1 reload value ar 1T mode

/**********************************************************/
#define		BUF_LENTH	255		//定义串口接收缓冲长度
#define	uchar	unsigned char
#define uint	unsigned int
#define ulong	unsigned long




void uart1_init(void);
void Uart1_TxByte(unsigned char dat);
void Uart1_String(unsigned char code *puts);
void delay_ms(unsigned char ms);

#endif