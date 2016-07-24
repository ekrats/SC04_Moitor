/*
** File:		crc8.h
** Version:		V1.00
** Author:		JianruiLe
** Create:		2006.4.12
** Description: CRC8 check.
**
** History:
** V1.00    2006.04.12   JianruiLe   First preliminary finished.
*/

/*
** 定义文件版本
*/
#undef __THIS_FILE_VERSION__
#define __THIS_FILE_VERSION__ 100U
#include "stm32f4xx.h"
/*
** 公开函数功能调用的声明
*/

//校验CRC值
u8 Crc8_CheckResult( u8 *p, u8 n );

//计算CRC值
unsigned short getCRC16(volatile unsigned char *ptr,unsigned char len);
unsigned short CRC16_Dn_Cal(unsigned char * dat, unsigned long len, unsigned short startCrc);
unsigned short CRC16_Up_Cal(unsigned char * dat, unsigned char len, unsigned short startCrc); 

/* End Source */
