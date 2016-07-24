/***************************************************************************

Copyright (C), 1999-2015, Tongye Tech. Co., Ltd.

* @file           Bsp_rtc.c
* @author         SCH-Team
* @version        V1.0.0
* @date           2015-03-11
* @brief          Timer manage

History:          // Revision Records

<Author>             <time>       <version >            <desc>

SCH-Team           2015-03-11       V1.0.0            Timer manage

***************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include <rtthread.h>
#include <stm32f4xx.h>
#include "Bsp_rtc.h"
#include "rtc.h"
#include "time.h"


SYS_TIME_RTC TIME_RTC = {0};

/*******************************************************************************
* Function Name  : void Bsp_get_time(void)
* Description    : 获取系统RTC时钟
*                   
* Input          : - none.
* Output         : None
* Return         : None
*******************************************************************************/

void Bsp_get_time(void)
{
	time_t now;
	struct tm* ti;

	/* get current time */
	time(&now);

	ti = localtime(&now);

	TIME_RTC.Year  = (ti->tm_year + 1900) % 100;
	TIME_RTC.Month = ti->tm_mon + 1;
	TIME_RTC.Day   = ti->tm_mday;
	TIME_RTC.Week  = ti->tm_wday;
	TIME_RTC.Hour  = ti->tm_hour;
	TIME_RTC.Min   = ti->tm_min;
	TIME_RTC.Sec   = ti->tm_sec;
}

/*******************************************************************************
* Function Name  : void Bsp_get_rtc_state(void)
* Description    : 获取系统RTC时钟RCC源
*                   
* Input          : - none.
* Output         : None
* Return         : None
*******************************************************************************/

void Bsp_get_rtc_state(void)
{
	TIME_RTC.RCC_State = get_rtc_rcc();
}

/******************* (C) COPYRIGHT 2012 Shenzhen Tongye *****END OF FILE****/


