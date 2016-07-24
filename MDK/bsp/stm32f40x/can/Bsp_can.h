
/***************************************************************************

Copyright (C), 1999-2012, Tongye Tech. Co., Ltd.

* @file           Bsp_can.h
* @author          
* @version        V1.0.0
* @date           2013-04-25
* @brief          (1)  
(2) 
(3) 
History:          // Revision Records

<Author>              <time>        <version>            <desc>

life, grape           2013-04-25       V1.0.0             ³õ´Î½¨Á¢

***************************************************************************/

/* Includes ------------------------------------------------------------------*/
#ifndef __BSP_CAN_H__
#define __BSP_CAN_H__


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

#define BSP_USE_CAN1      (1)
#define BSP_USE_CAN2      (1)


#define RCC_CANPeriph_PORT (RCC_AHB1Periph_GPIOB)

#if(BSP_USE_CAN1 == 1)
#define RCC_Periph_CAN1    (RCC_APB1Periph_CAN1)
#define CAN1_Pin_RX        (GPIO_Pin_8)
#define CAN1_Pin_TX        (GPIO_Pin_9)
#define CAN1Port           (GPIOB)
#define CAN1_PinSource_RX  (GPIO_PinSource8)
#define CAN1_PinSource_TX  (GPIO_PinSource9)
#endif

#if(BSP_USE_CAN2 == 1)
#define RCC_Periph_CAN2    (RCC_APB1Periph_CAN2)
#define CAN2_Pin_RX        (GPIO_Pin_5)
#define CAN2_Pin_TX        (GPIO_Pin_6)
#define CAN2Port           (GPIOB)
#define CAN2_PinSource_RX  (GPIO_PinSource5)
#define CAN2_PinSource_TX  (GPIO_PinSource6)
#endif

int  System_HW_Can_Init(void);
void CAN1_RX0_IRQ(void);
void CAN2_RX0_IRQ(void);

#endif


/******************* (C) COPYRIGHT 2013 Group *****END OF FILE****/


