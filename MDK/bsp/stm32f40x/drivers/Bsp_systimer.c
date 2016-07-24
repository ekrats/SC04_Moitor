/***************************************************************************

Copyright (C), 1999-2015, Tongye Tech. Co., Ltd.

* @file           Bsp_ad.c
* @author         SCH-Team
* @version        V1.0.0
* @date           2015-03-11
* @brief          Timer manage

History:          // Revision Records

<Author>             <time>       <version >            <desc>

xiou             	2015-03-11       V1.0.0            Timer manage

***************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include <rtthread.h>
#include <stm32f4xx.h>
#include "Bsp_systimer.h"

/* Private variables ---------------------------------------------------------*/
struct rt_semaphore sem_adio_task;
struct rt_semaphore sem_logic_task;
struct rt_semaphore sem_usartTx_task;
struct rt_semaphore sem_cantask_5ms;

/* Private function prototypes -----------------------------------------------*/

static void System_TimerConfiguration(void);
static void RCC_Configuration(void);
static void NVIC_Configuration(void);

extern struct rt_semaphore sem_fmrecord_10ms;
/*******************************************************************************
* Function Name  : BSP_TIM4_IRQHandler
* Description    :  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Bsp_can_send_cycle_event(void);
void BSP_TIM_IRQHandler(void)
{
	static uint8_t cnt1 = 0;
	static uint8_t cnt2 = 0;
	
	TIM_ClearITPendingBit(BSP_TIM, TIM_IT_Update);

	if((cnt1++ % 5) == 0)
	{
		rt_sem_release(&sem_adio_task);
		Bsp_can_send_cycle_event();
	}
	
	if((cnt2++ % 10) == 0)
	{
		rt_sem_release(&sem_logic_task);
		rt_sem_release(&sem_usartTx_task);
		rt_sem_release(&sem_fmrecord_10ms);
	}
		
//	Bsp_can_send_cycle_event();
}

/*******************************************************************************
* Function Name  : System_hw_timerInit
* Description    : 初始化定时器TIMx
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int System_hw_timerInit(void)
{
	rt_err_t rtv1,rtv2,rtv3,rtv4;

	rtv1 = rt_sem_init(&sem_adio_task,     "adio",    0,  RT_IPC_FLAG_FIFO);
	rtv2  = rt_sem_init(&sem_logic_task,   "logic",   0,  RT_IPC_FLAG_FIFO);
	rtv3  = rt_sem_init(&sem_usartTx_task, "commTx",  0,  RT_IPC_FLAG_FIFO);
	rtv4  = rt_sem_init(&sem_cantask_5ms,  "can",     0,  RT_IPC_FLAG_FIFO);

	RCC_Configuration(); 
	NVIC_Configuration();
	System_TimerConfiguration();

	if ((rtv1 == RT_EOK) && (rtv1 == RT_EOK) && (rtv2 == RT_EOK) && (rtv3 == RT_EOK) && (rtv4 == RT_EOK))
	{
		return 0;
	} 
	else
	{
		return -1;
	}
}

/*******************************************************************************
* Function Name  : System_TimerConfiguration
* Description    :  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void System_TimerConfiguration(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure = {0};
	//---------------------------------------------------------------------------
	// 初始化定时器TIM
	//---------------------------------------------------------------------------
	TIM_DeInit( BSP_TIM );//复位TIM定时器
	/* ---------------------------------------------------------------
	TIM Configuration: Output Compare Timing Mode:
	TIMCLK = 84 MHz, Prescaler = 8400, TIM counter clock = 10 KHz
	--------------------------------------------------------------- */
	/* TIM configuration */
	TIM_TimeBaseStructure.TIM_Period        = 9;
	TIM_TimeBaseStructure.TIM_Prescaler     = 0;      
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;    
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;   
	TIM_TimeBaseInit(BSP_TIM, &TIM_TimeBaseStructure);

	/*--------------------------------------------------------------------- */
	/* Prescaler configuration TIM 的时钟设定为10KHz，计数器的计量单位：1ms */
	/*   注意：分频数不能超过65535 （0xFFFF）                               */
	/*--------------------------------------------------------------------- */
	TIM_PrescalerConfig(BSP_TIM, (8400 - 1), TIM_PSCReloadMode_Immediate);

	/*  */
	TIM_ARRPreloadConfig(BSP_TIM, ENABLE);
	/* Clear TIM update pending flag[清除TIM 溢出中断标志] */
	TIM_ClearFlag(BSP_TIM, TIM_FLAG_Update);

	/* Enable TIM Update interrupt [TIM 溢出中断允许]*/
	TIM_ITConfig(BSP_TIM, TIM_IT_Update, ENABLE);  

	/* TIM enable counter [允许tim计数]*/
	TIM_Cmd(BSP_TIM, ENABLE);
}

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    :  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void RCC_Configuration(void)
{
	/* TIM clock enable [TIM定时器允许]*/
	RCC_APB1PeriphClockCmd(BSP_TIM_RCC, ENABLE); 
}


/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    :  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the TIM Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel                   = BSP_TIM_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


/******************* (C) COPYRIGHT 2012 Shenzhen Tongye *****END OF FILE****/


