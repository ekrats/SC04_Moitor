/*
* FreeModbus Libary: STM32 Port
* Copyright (C) 2013 Armink <armink.ztl@gmail.com>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*
* File: $Id: portserial_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions $
*/

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED
/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR(void);
static void prvvUARTRxISR(void);
/* ----------------------- Start implementation -----------------------------*/

void vMBMasterPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
	if (xRxEnable)
	{
		MASTER_RS485_RECEIVE_MODE;
		USART_ITConfig(MODBUS_USE_USART, USART_IT_RXNE, ENABLE);
	}
	else
	{
		MASTER_RS485_SEND_MODE;
		USART_ITConfig(MODBUS_USE_USART, USART_IT_RXNE, DISABLE);
	}
	if (xTxEnable)
	{
		USART_ITConfig(MODBUS_USE_USART, USART_IT_TXE, ENABLE);
	}
	else
	{
		USART_ITConfig(MODBUS_USE_USART, USART_IT_TXE, DISABLE);
	}
}

void vMBMasterPortClose(void)
{
	USART_ITConfig(MODBUS_USE_USART, USART_IT_TXE | USART_IT_RXNE, DISABLE);
	USART_Cmd(MODBUS_USE_USART, DISABLE);
}
//默认一个主机 串口 波特率可设置  奇偶检验可设置
BOOL xMBMasterPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
							 eMBParity eParity)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//======================时钟初始化=======================================
	/* Enable USART3 GPIO clocks */
	RCC_AHB1PeriphClockCmd(MB_GPIO_RCC | MB_GPIO_DE_RCC, ENABLE);
	/* Enable USART3 clock */
	RCC_APB1PeriphClockCmd(MB_USART_RCC, ENABLE);

	/* Connect alternate function */
	GPIO_PinAFConfig(MB_USART_GPIO, MB_GPIO_TX_SOURCE, MB_USART_AF);
	GPIO_PinAFConfig(MB_USART_GPIO, MB_GPIO_RX_SOURCE, MB_USART_AF);
	//======================IO初始化=======================================	
	/* Configure USART Tx and Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;

	GPIO_InitStructure.GPIO_Pin   = MB_GPIO_RX;
	GPIO_Init(MB_USART_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = MB_GPIO_TX;
	GPIO_Init(MB_USART_GPIO, &GPIO_InitStructure);
	//配置485发送和接收模式
	/* Configure RS485DE  as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin   = MB_GPIO_DE;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(MB_DE_GPIO, &GPIO_InitStructure);
	//======================串口初始化=======================================
	USART_InitStructure.USART_BaudRate = ulBaudRate;
	//设置校验模式
	switch (eParity)
	{
	case MB_PAR_NONE: //无校验
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		break;
	case MB_PAR_ODD: //奇校验
		USART_InitStructure.USART_Parity = USART_Parity_Odd;
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;
		break;
	case MB_PAR_EVEN: //偶校验
		USART_InitStructure.USART_Parity = USART_Parity_Even;
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;
		break;
	default:
		return FALSE;
	}

	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_HardwareFlowControl =
		USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	if (ucPORT != 2)
		return FALSE;

	ENTER_CRITICAL_SECTION(); //关全局中断

	USART_Init(MODBUS_USE_USART, &USART_InitStructure);
	USART_ITConfig(MODBUS_USE_USART, USART_IT_RXNE, ENABLE);
	USART_Cmd(MODBUS_USE_USART, ENABLE);

	//=====================中断初始化======================================
	//设置NVIC优先级分组为Group2：0-3抢占式优先级，0-3的响应式优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = MB_USART_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	EXIT_CRITICAL_SECTION(); //开全局中断

	return TRUE;
}

BOOL xMBMasterPortSerialPutByte(CHAR ucByte)
{
	USART_SendData(MODBUS_USE_USART, ucByte);
	return TRUE;
}

BOOL xMBMasterPortSerialGetByte(CHAR * pucByte)
{
	*pucByte = USART_ReceiveData(MODBUS_USE_USART);
	return TRUE;
}

/* 
* Create an interrupt handler for the transmit buffer empty interrupt
* (or an equivalent) for your target processor. This function should then
* call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
* a new character can be sent. The protocol stack will then call 
* xMBPortSerialPutByte( ) to send the character.
*/
static void prvvUARTTxReadyISR(void)
{
	pxMBMasterFrameCBTransmitterEmpty();
}

/* 
* Create an interrupt handler for the receive interrupt for your target
* processor. This function should then call pxMBFrameCBByteReceived( ). The
* protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
* character.
*/
static void prvvUARTRxISR(void)
{
	pxMBMasterFrameCBByteReceived();
}
/*******************************************************************************
* Function Name  : Bsp_Modbus_Irqhandler
* Description    : This function handles USART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Bsp_Modbus_Irqhandler(void)
{
	rt_interrupt_enter();
	//接收中断
	if (USART_GetITStatus(MODBUS_USE_USART, USART_IT_RXNE) == SET)
	{
		USART_ClearITPendingBit(MODBUS_USE_USART, USART_IT_RXNE);
		prvvUARTRxISR();
	}
	//发送中断
	if (USART_GetITStatus(MODBUS_USE_USART, USART_IT_TXE) == SET)
	{
		prvvUARTTxReadyISR();
	}
	rt_interrupt_leave();
}

#endif
