
/***************************************************************************

Copyright (C), 1999-2015, Tongye Tech. Co., Ltd.

* @file           Bsp_can.c
* @author          
* @version        V1.0.0
* @date           
* @brief          提供提供模拟量信号的采集以及ADC模块初始化
History:          // Revision Records

<Author>              <time>        <version>            <desc>
SCH-Team             2015-03-17       V1.0.0  

***************************************************************************/

#ifndef __BSP_AD_H__
#define __BSP_AD_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

#define ADC_TIM            (TIM8)
#define ADC_TIM_RCC        (RCC_APB2Periph_TIM8)
#define ADC_MODULE1        (RCC_APB2Periph_ADC1)
#define ADC_MODULE2        (RCC_APB2Periph_ADC3)
#define ADC_DMA            (RCC_AHB1Periph_DMA2)

#define ADC_AC_DATA        (0x800)//过零点标幺值
//------------------------------------------------------------------------------
//  AD_IN  数据映射接口
//------------------------------------------------------------------------------
typedef struct _DS_AD_IN_
{
	/* Input 01 */ 	uint16_t  inVal01_AD;	//模拟量输入通道 01
	/* Input 02 */	uint16_t  inVal02_AD;	//模拟量输入通道 02
	/* Input 03 */	uint16_t  inVal03_AD;	//模拟量输入通道 03
	/* Input 04 */	uint16_t  inVal04_AD;	//模拟量输入通道 04
	/* Input 05 */	uint16_t  inVal05_AD;	//模拟量输入通道 05
	/* Input 06 */	uint16_t  inVal06_AD;	//模拟量输入通道 06
	/* Input 07 */	uint16_t  inVal07_AD;	//模拟量输入通道 07
	/* Input 08 */	uint16_t  inVal08_AD;	//模拟量输入通道 08
	/* Input 09 */	uint16_t  inVal09_AD;	//模拟量输入通道 09
	/* Input 10 */	uint16_t  inVal10_AD;	//模拟量输入通道 10
	/* Input 11 */	uint16_t  inVal11_AD;	//模拟量输入通道 11
	/* Input 12 */	uint16_t  inVal12_AD;	//模拟量输入通道 12
	/* Input 13 */	uint16_t  inVal13_AD;	//模拟量输入通道 13
	/* Input 14 */	uint16_t  inVal14_AD;	//模拟量输入通道 14
	/* Input 15 */	uint16_t  inVal15_AD;	//模拟量输入通道 15
	/* Input 16 */	uint16_t  inVal16_AD;	//模拟量输入通道 16

}DS_AD_IN, *PDS_AD_IN;

/*=============================================================================*/
/*******************************************************************************
*                                                                              *
*                       工程数据集  (Drive Layer)                              *
*                                                                              *
*                                                                              *
********************************************************************************/
/*=============================================================================*/

typedef struct _DS_AD_
{
	DS_AD_IN           theDS_AD_IN;

}DS_AD, *PDS_AD;

/*******************************************************************************
*                                                                              *
*                       工程数据集: '引用'宏定义                               *
*                                                                              *
*                                                                              *
********************************************************************************/
/*=============================================================================*/
#define PDAD(name)             (&(theDAD.theDS_##name))
#define SDAD(name)             (sizeof(DS_##name))

/*=============================================================================*/
/*******************************************************************************
*                                                                              *
*                       工程数据集: 结构体变量定义                             *
*                                                                              *
*                                                                              *
********************************************************************************/
/*=============================================================================*/

#ifdef DSA_AD
DS_AD	theDAD = { 0 };	
#else
extern	DS_AD	theDAD;
#endif

int  System_hw_ADC_Init(void);
void AD_Collection(void);

#endif


/******************* (C) COPYRIGHT 2015 Shenzhen Tongye *****END OF FILE****/

