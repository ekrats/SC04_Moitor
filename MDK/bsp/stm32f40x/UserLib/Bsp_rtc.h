/***************************************************************************

Copyright (C), 1999-2015, Tongye Tech. Co., Ltd.

* @file           Bsp_rtc.h
* @author         SCH-Team
* @version        V1.0.0
* @date           2015-03-11
* @brief          Timer manage

History:          // Revision Records

<Author>             <time>       <version >            <desc>

SCH-Team           2015-03-11       V1.0.0            Timer manage

***************************************************************************/


#ifndef __BSP_RTC_H__
#define __BSP_RTC_H__

#include <rtthread.h>
#include "stm32f4xx.h"


//------------------------------------------------------------------------------
// RTCʱ��
//------------------------------------------------------------------------------
typedef struct _SYS_TIME_RTC_
{
	uint8_t  Year;                         //��
	uint8_t  Month;                        //��
	uint8_t  Day;                          //��
	uint8_t  Hour;                         //Сʱ 
	uint8_t  Min;                          //����
	uint8_t  Sec;                          //��
	uint8_t  Msec;                         //����
	uint8_t  Week;                         //����
	uint8_t  RCC_State;                    //����

}SYS_TIME_RTC, *pSYS_TIME_RTC;

extern SYS_TIME_RTC TIME_RTC;		// RTCʱ��

void Bsp_get_time(void);

#endif


/******************* (C) COPYRIGHT 2015 Shenzhen Tongye *****END OF FILE****/