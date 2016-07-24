
/***************************************************************************

Copyright (C), 1999-2015, Tongye Tech. Co., Ltd.

* @file           Bsp_can.c
* @author          
* @version        V1.0.0
* @date           
* @brief          
History:          // Revision Records

<Author>              <time>        <version>            <desc>
SCH-Team             2015-03-17       V1.0.0  

***************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "Bsp_can.h"
#include "stm32f4xx.h"
#include "rtthread.h"
#include <stdio.h>
#include <string.h>
#include "config.h"

//------------------------------------------------------------------------
// ����CAN�жϱ���
//------------------------------------------------------------------------
static CanRxMsg _RxMessage;
//------------------------------------------------------------------------
// ���ձ�������ʱ��RT����ϵͳTick
//------------------------------------------------------------------------
static uint32_t _CanRxTick;

static void Can_GPIO_Configuration(void);
static void Can_Configuration(void);
static void Can_Filter_Configuration(void);
static void Can_NVIC_Configuration(void);

/*******************************************************************************
* Function Name  : static void Can_GPIO_Configuration(void)
* Description    :
*                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

static void Can_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};

	//---------------------------------------------------------------
	// ����GPIOʱ�ӣ�CAN1ʱ�Ӻ�CAN2ʱ��
	//---------------------------------------------------------------
	RCC_AHB1PeriphClockCmd(RCC_CANPeriph_PORT, ENABLE);

#if(BSP_USE_CAN1 == 1)
	RCC_APB1PeriphClockCmd(RCC_Periph_CAN1, ENABLE);

	//---------------------------------------------------------------
	// ����CAN1�շ�GPIO�ܽ�
	//---------------------------------------------------------------
	GPIO_InitStructure.GPIO_Pin   = CAN1_Pin_RX | CAN1_Pin_TX;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(CAN1Port, &GPIO_InitStructure);

	GPIO_PinAFConfig(CAN1Port, CAN1_PinSource_RX, GPIO_AF_CAN1);
	GPIO_PinAFConfig(CAN1Port, CAN1_PinSource_TX, GPIO_AF_CAN1); 
#endif

#if(BSP_USE_CAN2 == 1)
	RCC_APB1PeriphClockCmd(RCC_Periph_CAN2, ENABLE);

	//---------------------------------------------------------------
	// ����CAN2�շ�GPIO�ܽ�
	//---------------------------------------------------------------
	GPIO_InitStructure.GPIO_Pin   = CAN2_Pin_RX | CAN2_Pin_TX;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(CAN2Port, &GPIO_InitStructure);

	GPIO_PinAFConfig(CAN2Port, CAN2_PinSource_RX, GPIO_AF_CAN2);
	GPIO_PinAFConfig(CAN2Port, CAN2_PinSource_TX, GPIO_AF_CAN2); 
#endif
}

/*******************************************************************************
* Function Name  : static void Can_Configuration(void)
* Description    : 
*                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

static void Can_Configuration(void)
{
	CAN_InitTypeDef CAN_InitStructure = {0};

#if(BSP_USE_CAN1 == 1)
	//---------------------------------------------------------------
	// ����ʼ��CAN1
	//---------------------------------------------------------------
	CAN_DeInit(CAN1);
	//---------------------------------------------------------------
	// ��ȱʡֵ����ؼĴ���
	//---------------------------------------------------------------
	CAN_StructInit(&CAN_InitStructure);
	//---------------------------------------------------------------
	// ��ʼ��CAN, ����ؼĴ�����
	//---------------------------------------------------------------
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = ENABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	//---------------------------------------------------------------
	// CAN clocked at 42 MHz
	// 42M/(7*(1+4+1)) = 500Kbps
	// CAN_InitStructure.CAN_SJW  = CAN_SJW_1tq;
	// CAN_InitStructure.CAN_BS1  = CAN_BS1_12tq;
	// CAN_InitStructure.CAN_BS2  = CAN_BS2_1tq;
	// CAN_InitStructure.CAN_Prescaler = 6;  
	//---------------------------------------------------------------
	CAN_InitStructure.CAN_SJW  = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1  = CAN_BS1_4tq;
	CAN_InitStructure.CAN_BS2  = CAN_BS2_1tq;
	CAN_InitStructure.CAN_Prescaler = 7;

	CAN_Init(CAN1, &CAN_InitStructure);
#endif

#if(BSP_USE_CAN2 == 1)
	//---------------------------------------------------------------
	// ����ʼ��CAN2
	//---------------------------------------------------------------
	CAN_DeInit(CAN2);
	//---------------------------------------------------------------
	// ��ȱʡֵ����ؼĴ���
	//---------------------------------------------------------------
	CAN_StructInit(&CAN_InitStructure);
	//---------------------------------------------------------------
	// ��ʼ��CAN, ����ؼĴ�����
	//---------------------------------------------------------------
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = ENABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	//---------------------------------------------------------------
	// CAN clocked at 42 MHz
	// 42M/(7*(1+4+1)) = 1Mbps
	// CAN_InitStructure.CAN_SJW  = CAN_SJW_1tq;
	// CAN_InitStructure.CAN_BS1  = CAN_BS1_4tq;
	// CAN_InitStructure.CAN_BS2  = CAN_BS2_1tq;
	// CAN_InitStructure.CAN_Prescaler = 7;
	//---------------------------------------------------------------
	CAN_InitStructure.CAN_SJW  = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1  = CAN_BS1_4tq;
	CAN_InitStructure.CAN_BS2  = CAN_BS2_1tq;
	CAN_InitStructure.CAN_Prescaler = 7;

	CAN_Init(CAN2, &CAN_InitStructure);
#endif
}

/*******************************************************************************
* Function Name  : static void Can_Filter_Configuration(void)
* Description    : 
*                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

static void Can_Filter_Configuration(void)
{
	CAN_FilterInitTypeDef CAN_FilterInitStructure = {0};

	CAN_FilterInitStructure.CAN_FilterMode           = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale          = CAN_FilterScale_32bit;
#if(BSP_USE_CAN1 == 1)
	//----------------------------------------------------------------
	// CAN1����ȫ������
	//----------------------------------------------------------------
//	CAN_FilterInitStructure.CAN_FilterNumber         = 0;
//	CAN_FilterInitStructure.CAN_FilterIdHigh         = 0x0000;
//	CAN_FilterInitStructure.CAN_FilterIdLow          = 0x0000;
//	CAN_FilterInitStructure.CAN_FilterMaskIdHigh     = 0x0000;
//	CAN_FilterInitStructure.CAN_FilterMaskIdLow      = 0x0000; 
//	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
//	CAN_FilterInitStructure.CAN_FilterActivation     = ENABLE;
//	CAN_FilterInit(&CAN_FilterInitStructure);

	////---------------------------------------------------------------
	//// CAN���˲����趨
	////---------------------------------------------------------------
	CAN_FilterInitStructure.CAN_FilterMode           = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale          = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh     = CAN_SINK_ID_MASK;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow      = 0x0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation     = ENABLE;
	//-------------------------------------------------------------
	// CAN1�˲����趨   
	//-------------------------------------------------------------
	//��һ���˲���
	CAN_FilterInitStructure.CAN_FilterNumber         = 0;
	CAN_FilterInitStructure.CAN_FilterIdHigh         = (CAN_ID_M1<<3);
	CAN_FilterInitStructure.CAN_FilterIdLow          = 0x0;
	CAN_FilterInit(&CAN_FilterInitStructure);
	////�ڶ����˲���
	CAN_FilterInitStructure.CAN_FilterNumber         = 1;
	CAN_FilterInitStructure.CAN_FilterIdHigh         = (CAN_ID_M1CX<<3);
	CAN_FilterInitStructure.CAN_FilterIdLow          = 0x0;
	CAN_FilterInit(&CAN_FilterInitStructure);
	////�������˲���
//	CAN_FilterInitStructure.CAN_FilterNumber         = 2;
//	CAN_FilterInitStructure.CAN_FilterIdHigh         = 0x0000;
//	CAN_FilterInitStructure.CAN_FilterIdLow          = 0x0;
//	CAN_FilterInit(&CAN_FilterInitStructure);
	////���ĸ��˲���
	//CAN_FilterInitStructure.CAN_FilterNumber         = 3;
	//CAN_FilterInitStructure.CAN_FilterIdHigh         = 0x0000;
	//CAN_FilterInitStructure.CAN_FilterIdLow          = 0x0020;
	//CAN_FilterInit(&CAN_FilterInitStructure);
	////������˲���
//	CAN_FilterInitStructure.CAN_FilterNumber         = 4;
//	CAN_FilterInitStructure.CAN_FilterIdHigh         = 0x0000;
//	CAN_FilterInitStructure.CAN_FilterIdLow          = 0x0080;
//	CAN_FilterInit(&CAN_FilterInitStructure);
#endif

#if(BSP_USE_CAN2 == 1)    
	//----------------------------------------------------------------
	// CAN2����ȫ������
	//----------------------------------------------------------------
//	CAN_FilterInitStructure.CAN_FilterNumber         = 14;
//	CAN_FilterInitStructure.CAN_FilterIdHigh         = 0x0000;
//	CAN_FilterInitStructure.CAN_FilterIdLow          = 0x0000;
//	CAN_FilterInitStructure.CAN_FilterMaskIdHigh     = 0x0000;
//	CAN_FilterInitStructure.CAN_FilterMaskIdLow      = 0x0000; 
//	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
//	CAN_FilterInitStructure.CAN_FilterActivation     = ENABLE;
//	CAN_FilterInit(&CAN_FilterInitStructure);

	////---------------------------------------------------------------
	//// CAN���˲����趨
	////---------------------------------------------------------------
	CAN_FilterInitStructure.CAN_FilterMode           = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale          = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh     = CAN_SINK_ID_MASK;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow      = 0x0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation     = ENABLE;
	////-------------------------------------------------------------
	//// CAN2�˲����趨   
	////-------------------------------------------------------------
	////��һ���˲���
	CAN_FilterInitStructure.CAN_FilterNumber         = 14;
	CAN_FilterInitStructure.CAN_FilterIdHigh         = (CAN_ID_M1<<3);
	CAN_FilterInitStructure.CAN_FilterIdLow          = 0x0;
	CAN_FilterInit(&CAN_FilterInitStructure); 
	////�ڶ����˲���
	CAN_FilterInitStructure.CAN_FilterNumber         = 15;
	CAN_FilterInitStructure.CAN_FilterIdHigh         = (CAN_ID_M1CX<<3);
	CAN_FilterInitStructure.CAN_FilterIdLow          = 0x0;
	CAN_FilterInit(&CAN_FilterInitStructure);
	////�������˲���
//	CAN_FilterInitStructure.CAN_FilterNumber         = 16;
//	CAN_FilterInitStructure.CAN_FilterIdHigh         = 0x0000;
//	CAN_FilterInitStructure.CAN_FilterIdLow          = 0x0070;
//	CAN_FilterInit(&CAN_FilterInitStructure);
	////���ĸ��˲���
	//CAN_FilterInitStructure.CAN_FilterNumber         = 17;
	//CAN_FilterInitStructure.CAN_FilterIdHigh         = 0x0000;
	//CAN_FilterInitStructure.CAN_FilterIdLow          = 0x0020;
	//CAN_FilterInit(&CAN_FilterInitStructure);
	////������˲���
	//CAN_FilterInitStructure.CAN_FilterNumber         = 18;
	//CAN_FilterInitStructure.CAN_FilterIdHigh         = 0x0000;
	//CAN_FilterInitStructure.CAN_FilterIdLow          = 0x0080;
	//CAN_FilterInit(&CAN_FilterInitStructure);
#endif
}

/*******************************************************************************
* Function Name  : Can_NVIC_Configuration
* Description    : 
*                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

static void Can_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure = {0};

#if(BSP_USE_CAN1 == 1)
	//-------------------------------------------------------------
	// ����CAN1�ж�������   
	//-------------------------------------------------------------
	NVIC_InitStructure.NVIC_IRQChannel                   = CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

#if(BSP_USE_CAN2 == 1)
	//-------------------------------------------------------------
	// ����CAN2�ж�������   
	//-------------------------------------------------------------
	NVIC_InitStructure.NVIC_IRQChannel                   = CAN2_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
}

/*******************************************************************************
* Function Name  : CAN1_RX0_IRQ
* Description    : 
*                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void can_Rx_Push_Msg(uint8_t port, CanRxMsg* msg);
#if(BSP_USE_CAN1 == 1)

void CAN1_RX0_IRQ(void)
{
	if(CAN_GetITStatus(CAN1, CAN_IT_FMP0))
	{
		//------------------------------------------------------------
		// ��ȡCAN1���������ձ���
		//------------------------------------------------------------
		CAN_Receive(CAN1,  CAN_FIFO0, &_RxMessage);
		//------------------------------------------------------------
		// ��ȡ��ǰϵͳrt_tickʱ���
		//------------------------------------------------------------
		_CanRxTick = rt_tick_get();
		//------------------------------------------------------------
		// �����յı���ѹ�����
		//------------------------------------------------------------
		//CanQ_Push_Rx_Msg(1, _CanRxTick, &_RxMessage);
		can_Rx_Push_Msg(1, &_RxMessage);
	}
}

#endif

/*******************************************************************************
* Function Name  : CAN2_RX0_IRQ
* Description    : 
*                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

#if(BSP_USE_CAN2 == 1)

void CAN2_RX0_IRQ(void)
{
	if(CAN_GetITStatus(CAN2, CAN_IT_FMP0))
	{
		//------------------------------------------------------------
		// ��ȡCAN2���������ձ���
		//------------------------------------------------------------
		CAN_Receive(CAN2,  CAN_FIFO0, &_RxMessage);
		//------------------------------------------------------------
		// ��ȡ��ǰϵͳrt_tickʱ���
		//------------------------------------------------------------
		_CanRxTick = rt_tick_get();
		//------------------------------------------------------------
		// �����յı���ѹ�����
		//------------------------------------------------------------
		//CanQ_Push_Rx_Msg(2, _CanRxTick, &_RxMessage);
		can_Rx_Push_Msg(2, &_RxMessage);
	}
}

#endif

/*******************************************************************************
* Function Name  : System_HW_Can_Init
* Description    :
*                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

int System_HW_Can_Init(void)
{
	//---------------------------------------------------------------
	// CAN GPIO�ܽ�����
	//---------------------------------------------------------------
	Can_GPIO_Configuration();

	//---------------------------------------------------------------
	// CAN����������
	//---------------------------------------------------------------
	Can_Configuration();

	//---------------------------------------------------------------
	// CAN�˲�������
	//---------------------------------------------------------------
	Can_Filter_Configuration();

	//---------------------------------------------------------------
	// CAN�ж�����������
	//---------------------------------------------------------------
	Can_NVIC_Configuration();

	//---------------------------------------------------------------
	// ��CAN�������ж�
	//---------------------------------------------------------------
#if(BSP_USE_CAN1 == 1)
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
#endif

#if(BSP_USE_CAN2 == 1)
	CAN_ITConfig(CAN2, CAN_IT_FMP0, ENABLE);
#endif

	return RT_TRUE;
}

/******************* (C) COPYRIGHT 2013 Group *****END OF FILE****/




