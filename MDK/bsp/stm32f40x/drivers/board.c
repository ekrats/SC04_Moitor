/*
 * File      : board.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009 RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      first implementation
 */

#include <rthw.h>
#include <rtthread.h>

#include "stm32f4xx.h"
#include "board.h"
#include "Bsp_systimer.h"
#include "Bsp_Board_Init.h"
#include "spi_flash.h"
#include "rtc.h"
#include "Bsp_bakram.h"

/**
 * @addtogroup STM32
 */

/*@{*/

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM
	/* Set the Vector Table base location at 0x20000000 */
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
	/* Set the Vector Table base location at 0x08000000 */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

/*******************************************************************************
 * Function Name  : SysTick_Configuration
 * Description    : Configures the SysTick for OS tick.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void  SysTick_Configuration(void)
{
	RCC_ClocksTypeDef  rcc_clocks;
	rt_uint32_t         cnts;

	RCC_GetClocksFreq(&rcc_clocks);

	cnts = (rt_uint32_t)rcc_clocks.HCLK_Frequency / RT_TICK_PER_SECOND;
	cnts = cnts / 8;

	SysTick_Config(cnts);
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
}

/**
 * This is the timer interrupt service routine.
 *
 */
void SysTick_Handler(void)
{
	/* enter interrupt */
	rt_interrupt_enter();

	rt_tick_increase();

	/* leave interrupt */
	rt_interrupt_leave();
}

/**
 * This function will initial STM32 board.
 */
void rt_hw_board_init()
{
	uint8_t rtn = 0;
	
#if IWDG_EN
	IWDG_Init(5);
#endif
	/* NVIC Configuration */
	NVIC_Configuration();

	/* Configure the SysTick */
	SysTick_Configuration();

	rt_hw_usart_init();
#ifdef RT_USING_CONSOLE
	rt_console_set_device(CONSOLE_DEVICE);
#endif
	//-----------------------------------------------------------------------------------------
	// FSMC module
	//-----------------------------------------------------------------------------------------
	rt_kprintf("\r\n\r\n+ System_fsmc_init \r\n");
	System_fsmc_init();
	//-----------------------------------------------------------------------------------------
	// SPI module
	//-----------------------------------------------------------------------------------------
	logic_board_spi_init();
	
#ifdef RT_USING_RTC
	rt_hw_rtc_init();
	
#endif
	//-----------------------------------------------------------------------------------------
	// TIM module
	//-----------------------------------------------------------------------------------------
	if(System_hw_timerInit() != 0)
	{
		rt_kprintf("\r\n+ TIM ERR: Can not create TIM_Sem!\r\n");
		while(1);
	}
	else
	{
		rt_kprintf("\r\n+ TIM INFO: Init success!\r\n");
	}
	
	//-----------------------------------------------------------------------------------------
	// BKP module
	//-----------------------------------------------------------------------------------------
//	if(System_hw_bakramInit() == 0)
//	{
//		rt_kprintf ("\r\n+ BKP SRAM INFO: Init Err! \r\n");
//	}
//	else
//	{
//		rt_kprintf ("\r\n+ BKP SRAM INFO: Init Success! \r\n");
//	}
#if IWDG_EN
	IWDG_Init(1);
#endif
}

/*@}*/
