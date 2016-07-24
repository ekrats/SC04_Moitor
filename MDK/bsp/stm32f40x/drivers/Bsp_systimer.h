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

#ifndef __BSP_TIM_H__
#define __BSP_TIM_H__

#define  BSP_TIM_RCC           (RCC_APB1Periph_TIM4)
#define  BSP_TIM               (TIM4) 
#define  BSP_TIM_IRQ           (TIM4_IRQn)

int  System_hw_timerInit(void);
void BSP_TIM_IRQHandler(void);
#endif


/******************* (C) COPYRIGHT 2015 Shenzhen Tongye *****END OF FILE****/
