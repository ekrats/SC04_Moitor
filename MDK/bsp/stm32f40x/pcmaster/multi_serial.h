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
	* @brief ����̨��������Ӧ�����ʼ��������Ӧ������ݲ�ͬͨ��Э���Զ��л����ڷ���
	* ������λ��������̨���Լ����ʾ����
	*
	* @param ��
	*
	* @return ��
	*/
	void serial_services_init(void);

#ifdef CONSOLE_RX_USE_DMA

	/**
	* @brief ����IDLE�����жϺ�����һ����ʱ����������ʱ����ʱ�ж���ȷ��һ�ν����Ƿ���ɡ�
	* �������ݽ��ڸö�ʱ���ж��д���ʹ�øù��ܣ�����ӹ�USARTx_IRQHandlerd���жϵ���
	*
	* @param ָ�����յĴ���
	* @param ��Ҫ�����Ķ�ʱ��
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


