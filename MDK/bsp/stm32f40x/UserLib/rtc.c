/*
* File      : rtc.c
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
* 2011-11-26     aozima       implementation time.
*/

#include <rtthread.h>
#include <stm32f4xx.h>
#include <time.h>
#include "rtc.h"

__IO uint32_t AsynchPrediv = 0, SynchPrediv = 0;
RTC_TimeTypeDef RTC_TimeStructure;
RTC_InitTypeDef RTC_InitStructure;
RTC_AlarmTypeDef  RTC_AlarmStructure;
RTC_DateTypeDef RTC_DateStructure;

#define MINUTE  (60)
#define HOUR    (60*MINUTE)
#define DAY     (24*HOUR)
#define YEAR    (365*DAY)

static int month[12] =
{
	0,
	DAY*(31),
	DAY*(31+29),
	DAY*(31+29+31),
	DAY*(31+29+31+30),
	DAY*(31+29+31+30+31),
	DAY*(31+29+31+30+31+30),
	DAY*(31+29+31+30+31+30+31),
	DAY*(31+29+31+30+31+30+31+31),
	DAY*(31+29+31+30+31+30+31+31+30),
	DAY*(31+29+31+30+31+30+31+31+30+31),
	DAY*(31+29+31+30+31+30+31+31+30+31+30)
};
static struct rt_device rtc;

static time_t rt_mktime(struct tm *tm)
{
	long res;
	int year;
	year = tm->tm_year - 70;

	res = YEAR * year + DAY * ((year + 1) / 4);
	res += month[tm->tm_mon];

	if (tm->tm_mon > 1 && ((year + 2) % 4))
		res -= DAY;
	res += DAY * (tm->tm_mday - 1);
	res += HOUR * tm->tm_hour;
	res += MINUTE * tm->tm_min;
	res += tm->tm_sec;
	return res;
}
static rt_err_t rt_rtc_open(rt_device_t dev, rt_uint16_t oflag)
{
	if (dev->rx_indicate != RT_NULL)
	{
		/* Open Interrupt */
	}

	return RT_EOK;
}

static rt_size_t rt_rtc_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
	return 0;
}

static rt_err_t rt_rtc_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
	time_t *time;
	struct tm ti,*to;
	RT_ASSERT(dev != RT_NULL);

	switch (cmd)
	{
	case RT_DEVICE_CTRL_RTC_GET_TIME:
		time = (time_t *)args;
		/* read device */
		//RTC_GetTimeStamp(RTC_Format_BIN, &RTC_TimeStructure, &RTC_DateStructure);
		RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
		RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
		ti.tm_sec = RTC_TimeStructure.RTC_Seconds;
		ti.tm_min = RTC_TimeStructure.RTC_Minutes;
		ti.tm_hour = RTC_TimeStructure.RTC_Hours;
		//ti.tm_wday = (RTC_DateStructure.RTC_WeekDay==7)?0:RTC_DateStructure.RTC_WeekDay;
		ti.tm_mon = RTC_DateStructure.RTC_Month -1;
		ti.tm_mday = RTC_DateStructure.RTC_Date;
		ti.tm_year = RTC_DateStructure.RTC_Year + 70;
		*time = rt_mktime(&ti);
		//*time = RTC_GetCounter();

		break;

	case RT_DEVICE_CTRL_RTC_SET_TIME:
		{
			time = (time_t *)args;

			/* Enable the PWR clock */
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

			/* Allow access to RTC */
			PWR_BackupAccessCmd(ENABLE);

			/* Wait until last write operation on RTC registers has finished */
			//RTC_WaitForLastTask();

			/* Change the current time */
			//RTC_SetCounter(*time);

			to = localtime(time);
			RTC_TimeStructure.RTC_Seconds = to->tm_sec;
			RTC_TimeStructure.RTC_Minutes = to->tm_min;
			RTC_TimeStructure.RTC_Hours	= to->tm_hour;
			//RTC_DateStructure.RTC_WeekDay =(ti->tm_wday==0)?7:ti->tm_wday;
			RTC_DateStructure.RTC_Month = to->tm_mon + 1;
			RTC_DateStructure.RTC_Date = to->tm_mday;
			RTC_DateStructure.RTC_Year = to->tm_year - 70;
			RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
			RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);

			/* Wait until last write operation on RTC registers has finished */
			//RTC_WaitForLastTask();

			RTC_WriteBackupRegister(RTC_BKP_DR1, 0xA5A5);
			//BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
		}
		break;
	}

	return RT_EOK;
}

/*******************************************************************************
* Function Name  : RTC_Configuration
* Description    : Configures the RTC.
* Input          : None
* Output         : None
* Return         : 0 reday,-1 error.
*******************************************************************************/
static uint8_t RTC_LSE_LSI = 0;
uint8_t RTC_Config(void)
{
	u32 count=0x200000;
	/* Enable the PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Allow access to RTC */
	PWR_BackupAccessCmd(ENABLE);

	RCC_LSEConfig(RCC_LSE_ON);

	/* Wait till LSE is ready */
	while ((RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) && (--count));
	if(count == 0)
	{
		RCC_LSEConfig(RCC_LSE_OFF);
		/* LSI used as RTC source clock */
		/* Enable the PWR clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
		/* Allow access to RTC */
		PWR_BackupAccessCmd(ENABLE);
		/* The RTC Clock may varies due to LSI frequency dispersion. */   
		/* Enable the LSI OSC */ 
		RCC_LSICmd(ENABLE);
		/* Wait till LSI is ready */
		count=0x200000;
		while((RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET) && (--count));
		if(count == 0)
		{
			return 0;
		}

		RTC_LSE_LSI = RTC_RCC_LSI;

		/* Select the RTC Clock Source */
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

		/* Enable the RTC Clock */
		RCC_RTCCLKCmd(ENABLE);

		/* Wait for RTC APB registers synchronisation */
		RTC_WaitForSynchro();

		/* Calendar Configuration with LSI supposed at 32KHz */
		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
		RTC_InitStructure.RTC_SynchPrediv  =  0xFF; /* (32KHz / 128) - 1 = 0xFF*/
		RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
		RTC_Init(&RTC_InitStructure); 

		return 1; 
	}

	RTC_LSE_LSI = RTC_RCC_LSE;

	/* Select the RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	SynchPrediv = 0xFF;
	AsynchPrediv = 0x7F;

	/* Enable the RTC Clock */
	RCC_RTCCLKCmd(ENABLE);

	/* Wait for RTC APB registers synchronisation */
	RTC_WaitForSynchro();

	/* Enable The TimeStamp */
	//RTC_TimeStampCmd(RTC_TimeStampEdge_Falling, ENABLE);

	return 1;
}

uint8_t RTC_Configuration(void)
{

	if(RTC_Config() == 0)
		return 0;

	/* Set the Time */
	RTC_TimeStructure.RTC_Hours   = 0;
	RTC_TimeStructure.RTC_Minutes = 0;
	RTC_TimeStructure.RTC_Seconds = 0;

	/* Set the Date */
	RTC_DateStructure.RTC_Month = 1;
	RTC_DateStructure.RTC_Date = 1;
	RTC_DateStructure.RTC_Year = 0;
	RTC_DateStructure.RTC_WeekDay = 4;


	if(RTC_LSE_LSI == RTC_RCC_LSE)
	{
		/* Calendar Configuration */
		RTC_InitStructure.RTC_AsynchPrediv = AsynchPrediv;
		RTC_InitStructure.RTC_SynchPrediv =  SynchPrediv;
		RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
		RTC_Init(&RTC_InitStructure);
	}
	else if(RTC_LSE_LSI == RTC_RCC_LSI)
	{
		/* Calendar Configuration */
		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
		RTC_InitStructure.RTC_SynchPrediv	=  32000/128 - 1;
		RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
		RTC_Init(&RTC_InitStructure);
	}

	/* Set Current Time and Date */
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);
	RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
	if (RTC_Init(&RTC_InitStructure) == ERROR)
		return 0;

	return RTC_LSE_LSI;
}

uint8_t rt_hw_rtc_init(void)
{
	uint8_t rtn = 0;
	rtc.type	= RT_Device_Class_RTC;

	if (RTC_ReadBackupRegister(RTC_BKP_DR1) != 0xA5A5)
	{
		rt_kprintf("\r\n\r\n+ rtc is not configured\n");
		rt_kprintf("+ please configure with set_date and set_time\n");
		rtn = RTC_Configuration();
		
		if(rtn == 0)
		{
			rt_kprintf("+ rtc configure fail...\r\n");
			return 0;
		}
	}
	else
	{
		/* Wait for RTC registers synchronization */
		RTC_WaitForSynchro();
		rtn = RTC_RCC_LSE;
	}

	/* register rtc device */
	rtc.init 	= RT_NULL;
	rtc.open 	= rt_rtc_open;
	rtc.close	= RT_NULL;
	rtc.read 	= rt_rtc_read;
	rtc.write	= RT_NULL;
	rtc.control = rt_rtc_control;

	/* no private */
	rtc.user_data = RT_NULL;

	rt_device_register(&rtc, "rtc", RT_DEVICE_FLAG_RDWR);

	return rtn;
}

#include <time.h>
#if defined (__IAR_SYSTEMS_ICC__) &&  (__VER__) >= 6020000   /* for IAR 6.2 later Compiler */
#pragma module_name = "?time"
time_t (__time32)(time_t *t)                                 /* Only supports 32-bit timestamp */
#else
time_t time(time_t* t)
#endif
{
	rt_device_t device;
	time_t time=0;

	device = rt_device_find("rtc");
	if (device != RT_NULL)
	{
		rt_device_control(device, RT_DEVICE_CTRL_RTC_GET_TIME, &time);
		if (t != RT_NULL) *t = time;
	}

	return time;
}

#ifdef RT_USING_FINSH
#include <finsh.h>

void set_date(rt_uint32_t year, rt_uint32_t month, rt_uint32_t day)
{
	time_t now;
	struct tm* ti;
	rt_device_t device;
	ti = RT_NULL;
	/* get current time */
	time(&now);

	ti = localtime(&now);
	if (ti != RT_NULL)
	{
		ti->tm_year = year - 1900;
		ti->tm_mon 	= month - 1; /* ti->tm_mon 	= month; */
		ti->tm_mday = day;
	}

	now = mktime(ti);

	device = rt_device_find("rtc");
	if (device != RT_NULL)
	{
		rt_rtc_control(device, RT_DEVICE_CTRL_RTC_SET_TIME, &now);
	}
}
FINSH_FUNCTION_EXPORT(set_date, set date. e.g: set_date(2010,2,28))

void set_time(rt_uint32_t hour, rt_uint32_t minute, rt_uint32_t second)
{
	time_t now;
	struct tm* ti;
	rt_device_t device;

	ti = RT_NULL;
	/* get current time */
	time(&now);

	ti = localtime(&now);
	if (ti != RT_NULL)
	{
		ti->tm_hour = hour;
		ti->tm_min 	= minute;
		ti->tm_sec 	= second;
	}

	now = mktime(ti);
	device = rt_device_find("rtc");
	if (device != RT_NULL)
	{
		rt_rtc_control(device, RT_DEVICE_CTRL_RTC_SET_TIME, &now);
	}
}
FINSH_FUNCTION_EXPORT(set_time, set time. e.g: set_time(23,59,59))

void list_date()
{
	time_t now;

	time(&now);
	rt_kprintf("%s\n", ctime(&now));
}
FINSH_FUNCTION_EXPORT(list_date, show date and time.)
#endif

void get_linux_time(int _ptr, struct tm* t)
{
	struct tm* ti;
	time_t now;
	/* get current time */
	time(&now);
	now = now + _ptr;
	ti = localtime(&now);
	t->tm_year = ti->tm_year;
	t->tm_mon  = ti->tm_mon ;
	t->tm_mday = ti->tm_mday;
	t->tm_hour = ti->tm_hour;
	t->tm_min  = ti->tm_min ;
	t->tm_sec  = ti->tm_sec ;
	t->tm_wday = ti->tm_wday;
}

void Bsp_set_time(u8 year, u8 month, u8 day, u8 hour, u8 min, u8 sec)
{
	time_t now;
	time_t det;
	struct tm* ti;
	rt_device_t device;

	ti = RT_NULL;

	if( ((year == 0x00) && (month == 0x00) && (day == 0x00) && (hour == 0x00) && (min == 0x00) && (sec == 0x00))
		|| ((year == 0xff) && (month == 0xff) && (day == 0xff) && (hour == 0xff) && (min == 0xff) && (sec == 0xff)))
	{
		return;
	}

	if(year < 14)  return;

	/* get current time */
	time(&now);

	ti = localtime(&now);

	if((ti->tm_year ==  ((year + 2000) - 1900))
		&& (ti->tm_mon ==  month) && (ti->tm_mday ==  day)
		&& (ti->tm_hour ==  hour) && (ti->tm_min ==  min))
	{
		if(ti->tm_sec > sec)    det = ti->tm_sec - sec;
		else                    det = sec - ti->tm_sec;

		if(det < 5)  return;
	}

	if(ti != RT_NULL)
	{
		ti->tm_year = (year + 2000) - 1900;
		ti->tm_mon 	= month - 1;
		ti->tm_mday = day;
		ti->tm_hour = hour;
		ti->tm_min 	= min;
		ti->tm_sec 	= sec;

		det = mktime(ti);

		if(det >= 5)
		{
			device = rt_device_find("rtc");
			if (device != RT_NULL)
			{
				rt_rtc_control(device, RT_DEVICE_CTRL_RTC_SET_TIME, &det);
			}
		}
	}
}

uint8_t get_rtc_rcc(void)
{
	return RTC_LSE_LSI;
}
