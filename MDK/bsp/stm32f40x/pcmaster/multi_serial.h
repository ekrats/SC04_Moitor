/**
*************************************************************************
* This file is part of Platform
* COPYRIGHT (C) 1999-2014, TongYe Corporation All rights reserved.
* @file    multi_serial.h
* @brief   This file contains all the functions prototypes for the   
*          the multi-function usart firmware library.
* Change Logs:
* Date           Author       Notes
* 2014/12/28     Coase        the first version
*************************************************************************
*/

#ifndef __MULTI_SERIAL__H__
#define __MULTI_SERIAL__H__

#include <rthw.h>
#include <rtthread.h>

/* STM32F40x library definitions */
#include <stm32f4xx.h>
#include "serial.h"

#ifdef __cplusplus
extern "C" {
#endif
	char * get_board_info(void);
	/**
	* @brief 控制台串口自适应服务初始化，自适应服务根据不同通信协议自动切换串口服务
	* 包括上位机，控制台，以及软件示波器
	*
	* @param 无
	*
	* @return 无
	*/
	void serial_services_init(void);

#ifdef CONSOLE_RX_USE_DMA

	/**
	* @brief 串口IDLE产生中断后，启动一个定时器，经过定时器延时中断来确定一次接收是否完成。
	* 接收数据将在该定时器中断中处理。使用该功能，必须接管USARTx_IRQHandlerd的中断调用
	*
	* @param 指定接收的串口
	* @param 需要启动的定时器
	*
	* @return null 
	*/
	rt_inline void rt_hw_multi_serial_isr(USART_TypeDef* usart, TIM_TypeDef* timer)
	{
		if(USART_GetITStatus(usart, USART_IT_IDLE) != RESET)
		{
			USART_ReceiveData(usart);
			TIM_SetCounter(timer, 0);
			TIM_Cmd(timer, ENABLE);
		}	
	}
#else
	void rt_hw_multi_serial_isr(rt_device_t device, TIM_TypeDef* timer);
#endif

#ifdef __cplusplus
}
#endif

#define TIMx_IRQHandler     TIM5_IRQHandler
#define MULTI_SERIAL_TIMx   TIM5

void serial_servicestask_create(char priority);

#endif

/******************* (C) COPYRIGHT 2014 TongYe **********END OF FILE****/


/*@}*/


