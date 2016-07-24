/*
* File      : rtc.h
* This file is part of RT-Thread RTOS
* COPYRIGHT (C) 2009, RT-Thread Development Team
*
* The license and distribution terms for this file may be
* found in the file LICENSE in this distribution or at
* http://www.rt-thread.org/license/LICENSE
*
* Change Logs:
* Date           Author       Notes
* 2009-01-05     Bernard      the first version
*/

#ifndef __RTC_H__
#define __RTC_H__

#include <stm32f4xx.h>

#define RTC_RCC_LSE          (0x01)
#define RTC_RCC_LSI          (0x02)

uint8_t rt_hw_rtc_init(void);
void Bsp_ptu_set_date(u8 year, u8 month, u8 day, u8 hour, u8 min, u8 sec);
uint8_t get_rtc_rcc(void);

#endif
