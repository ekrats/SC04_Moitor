/**
*************************************************************************
* This file is part of Platform
* COPYRIGHT (C) 1999-2014, TongYe R&D Team
* @file    multi_serial.c
* @brief   This file provides all the multi-function usart firmware 
* functions.
* Change Logs:
* Date           Author       Notes
* 2014/12/28     Coase        the first version
*************************************************************************
*/
#include "multi_serial.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "pcmaster.h"
#include "usart.h"

typedef enum
{
	use_console,
	use_freemaster,
	use_ptu,
}console_type;

console_type function_switch = use_console;
struct rt_semaphore sem_serial_rx;
struct stm32_serial_device* uart;
uint8_t 	is_reconnected = 1;

//===============================================================================
//TIMx isr handler
//===============================================================================
void TIMx_IRQHandler(void)
{
	/* enter interrupt */
	rt_interrupt_enter();
#ifdef CONSOLE_RX_USE_DMA
	uart->int_rx->save_index = UART_RX_BUFFER_SIZE - MULTI_USART_RX_DMA_STREAM->NDTR;
	reset_dma_address();
	memcpy(uart->int_rx->rx_buffer, (void *)MULTI_USART_RX_DMA_STREAM->M0AR, uart->int_rx->save_index);
#endif
	rt_sem_release(&sem_serial_rx);

	TIM_Cmd(MULTI_SERIAL_TIMx, DISABLE);
	TIM_SetCounter(MULTI_SERIAL_TIMx, 0);
	TIM_ClearFlag(MULTI_SERIAL_TIMx, TIM_FLAG_Update);
	/* leave interrupt */
	rt_interrupt_leave();
}


static void Timer_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	/* TIM clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	/* Enable the TIM2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	TIM_DeInit(TIM5);
	TIM_TimeBaseStructure.TIM_Period = 80; 
	TIM_TimeBaseStructure.TIM_Prescaler = (168-1);
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

	TIM_ClearFlag(TIM5, TIM_FLAG_Update);
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
	TIM_SetCounter(MULTI_SERIAL_TIMx, 0);
}

static void serial_services_entry( void* parameter)
{
	rt_device_t uart_device = rt_console_get_device();
	uart = (struct stm32_serial_device*) (uart_device)->user_data;

	rt_sem_init(&sem_serial_rx, "usart_rx", 0, 1);

	Timer_Configuration();

	while(1)
	{
		uint8_t * rx_buffer = uart->int_rx->rx_buffer;
		if(-RT_ETIMEOUT == rt_sem_take(&sem_serial_rx, 1000))
		{
			is_reconnected = 1;
			continue;
		}
		else
		{
			if(is_reconnected)
			{
				is_reconnected = 0;

				if(rx_buffer[0] == 0x2b || (rx_buffer[0] == 0xFC))
				{
					USART_InitTypeDef      USART_InitStructure      = {0};
					USART_InitStructure.USART_BaudRate = 115200;
					USART_InitStructure.USART_WordLength = USART_WordLength_8b;
					USART_InitStructure.USART_StopBits = USART_StopBits_1;
					USART_InitStructure.USART_Parity = USART_Parity_No;
					USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
					USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
					USART_Init(USART1, &USART_InitStructure);	

					function_switch = use_freemaster;

				}
				else if(rx_buffer[1] == 0xcf)
				{
					function_switch = use_ptu;
				}
				else
				{
					USART_InitTypeDef      USART_InitStructure      = {0};
					USART_InitStructure.USART_BaudRate = 115200;
					USART_InitStructure.USART_WordLength = USART_WordLength_8b;
					USART_InitStructure.USART_StopBits = USART_StopBits_1;
					USART_InitStructure.USART_Parity = USART_Parity_No;
					USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
					USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
					USART_Init(USART1, &USART_InitStructure);	
					function_switch = use_console;

				}
			}
		}

		uart->int_rx->read_index = 0;

		switch(function_switch)
		{
		case use_freemaster:
			PutData((char *)rx_buffer, 0, uart->int_rx->save_index);
			break;
		case use_ptu:
			break;
		default:

			if (uart_device->rx_indicate != RT_NULL)
			{
				uart_device->rx_indicate(uart_device, uart->int_rx->save_index);
			}
			break;
		}
	}
}

void serial_servicestask_create(char priority)
{
	struct rt_thread * serial_services_thread = rt_thread_create("usart",
		serial_services_entry, RT_NULL,
		1024, priority, 10);
	if (serial_services_thread != RT_NULL)
	{
		rt_thread_startup(serial_services_thread);
	}
}

#ifndef CONSOLE_RX_USE_DMA
/* ISR for serial interrupt */
void rt_hw_multi_serial_isr(rt_device_t device, TIM_TypeDef* timer)
{
	struct stm32_serial_device* uart = (struct stm32_serial_device*) device->user_data;
	if(USART_GetITStatus(uart->uart_device, USART_IT_RXNE) != RESET)
	{
		/* interrupt mode receive */
		RT_ASSERT(device->flag & RT_DEVICE_FLAG_INT_RX);

		/* save on rx buffer */
		while (uart->uart_device->SR & USART_FLAG_RXNE)
		{
			rt_base_t level;
			/* disable interrupt */
			level = rt_hw_interrupt_disable();
			if(timer->CNT == 0)
			{
				uart->int_rx->save_index = 0;
				TIM_Cmd(timer, ENABLE);
			}
			TIM_SetCounter(timer, 0);

			/* save character */
			uart->int_rx->rx_buffer[uart->int_rx->save_index] = uart->uart_device->DR & 0xff;
			uart->int_rx->save_index ++;
			/* enable interrupt */
			rt_hw_interrupt_enable(level);
		}

		/* clear interrupt */
		USART_ClearITPendingBit(uart->uart_device, USART_IT_RXNE);
	}
}


#endif



/******************* (C) COPYRIGHT 2014 TongYe **********END OF FILE****/


/*@}*/

