#include "canLinker.h"
#include "stm32f4xx.h"
#include <rtthread.h>
#include "canTxQueue.h"
#include "canRxQueue.h"
#include "canTask.h"
#include "canApp.h"


/*******************************************************************************
* Function Name  : void CanApp_CycleStream_Server(void)
* Description    : 负责 应用层与链路层 数据的定时更新
*                   
* Input          : None
* Output         : None
* Return         : None   xiangchuanyu
*******************************************************************************/
void CanApp_CycleStream_Server(void)
{
	static uint8_t TimeCnt = 0;
	
	//-----------------------------------------------------------
	// 更新CAN数据发送接收缓冲区
	//-----------------------------------------------------------
	can.CycleMsgUpdate_data();
	if(++TimeCnt >= 10)
	{
		TimeCnt = 0;
	}
	//-----------------------------------------------------------
	// 管理发送CAN数据帧
	//-----------------------------------------------------------
	can.CycleMsgPush();
}

/*******************************************************************************
* Function Name  : void CanApp_TriggerStream_Server(void)
* Description    : 负责 应用层与链路层 数据的定时更新
*                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CanApp_TriggerStream_Server(void)
{
	can.TriggerMsgPush();
}

//------------------------------------------------------------------
/*******************************************************************************
* Function Name  : static void _SS8X_Rx(CAN_DATA_RAM* pbuf)
* Description    :
*                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void _Can_Rx(CAN_RX_DATA_RAM* pbuf)
{
	CAN_RX_DATA_RAM* _p = pbuf;
	
	if (_p->parent.CanPort == 1)
	{
		can.MsgRx_Manage(_p, _p->data.ExtId);
	}
	else if (_p->parent.CanPort == 2)
	{
		can.MsgRx_Manage(_p, _p->data.ExtId);
	}
}

/*******************************************************************************
* Function Name  : void _App_Rx(void* pbuf)
* Description    :
*                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void _App_Can_Rx(void* pbuf)
{
	_Can_Rx((CAN_RX_DATA_RAM*)pbuf);
}

//////////////////////////////////////////////////////////////////////////
// CAN 物理层发送
//////////////////////////////////////////////////////////////////////////

/*******************************************************************************
* Function Name  : static uint8_t _CAN_HW_Tx(CAN_TX_DATA_RAM* pbuf)
* Description    :
*                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static uint8_t _CAN_HW_Tx(CAN_TX_DATA_RAM* pbuf)
{
	uint8_t retval;

	if (pbuf->parent.CanPort == 1)
	{
		retval = CAN_Transmit(CAN1, &(pbuf->data));
	} 
	else if(pbuf->parent.CanPort == 2)
	{
		retval = CAN_Transmit(CAN2, &(pbuf->data));
	}
	else
	{
		retval = CAN_TxStatus_NoMailBox;
	}

	return retval;
}

/*******************************************************************************
* Function Name  : void _App_Rx(void* pbuf)
* Description    :
*                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t _App_Can_Tx(void* pbuf)
{
	uint8_t retVal;

	retVal = _CAN_HW_Tx((CAN_TX_DATA_RAM*)pbuf);

	return retVal;
}

/*******************************************************************************
* Function Name  : void can_Tx_Push_Msg(uint8_t port, CanRxMsg* msg)
* Description    :
*                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void can_Tx_Push_Msg(uint8_t port, CanTxMsg* msg)
{
	canTxQueue.Push_Msg(port, msg);
}
/*******************************************************************************
* Function Name  : void can_Rx_Push_Msg(uint8_t port, CanRxMsg* msg)
* Description    :
*                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern "C" void can_Rx_Push_Msg(uint8_t port, CanRxMsg* msg)
{
	canRxQueue.Push_Msg(port, msg);
	can.CanCnt++;
}
