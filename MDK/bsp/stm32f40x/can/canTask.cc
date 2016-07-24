#include "canTask.h"
#include "stm32f4xx.h"
#include <rtthread.h>
#include "canTxQueue.h"
#include "canRxQueue.h"
#include "canLinker.h"
#include "canApp.h"
#include "logicApp.h"


RxQueue canRxQueue;
TxQueue canTxQueue;

//------------------------------------------------------------------
// 开始通讯
//------------------------------------------------------------------
uint8_t CanStartComm = 0;

#define BSP_CAN_Q_STACKSIZE              (2 * 1024 / 4)  //2K
//------------------------------------------------------------------
//  Can Rx Queue Task
//------------------------------------------------------------------
static rt_uint32_t bsp_can_rx_queue_stack[BSP_CAN_Q_STACKSIZE];
static struct rt_thread bsp_can_rx_queue_thread;
//------------------------------------------------------------------
//  Can Tx Queue Task
//------------------------------------------------------------------
static rt_uint32_t bsp_can_tx_queue_stack[BSP_CAN_Q_STACKSIZE];
static struct rt_thread bsp_can_tx_queue_thread;
//------------------------------------------------------------------
//  Can Task
//------------------------------------------------------------------
static rt_uint32_t bsp_can_stack[BSP_CAN_Q_STACKSIZE];
static struct rt_thread bsp_can_thread;

//------------------------------------------------------------------
// 控制主逻辑运算的事件变量
//------------------------------------------------------------------
static struct rt_event EventCanSend;
//------------------------------------------------------------------
// 收取事件的变量
//------------------------------------------------------------------
static rt_uint32_t _ev_cansend_flag = 0;
//------------------------------------------------------------------
// 事件变量 初始化结果。只有EventCanSend正确初始化后，才可以Send Event
//------------------------------------------------------------------
static __IO rt_err_t _ev_cansend_init = 0xFF;

static void Bsp_can_event_init(void);
static void Bsp_can_check_event(void);

static void Bsp_can_led(void)
{
	if(can.CanStateWord > 0)
	{
		if(GPIOA->ODR & GPIO_Pin_11)
		{
			GPIOA->BSRRH = GPIO_Pin_11;//LED2
		}
		else
		{
			GPIOA->BSRRL = GPIO_Pin_11;
		}
	}
	else
	{
		GPIOA->BSRRL  = GPIO_Pin_11;
	}
}

/*******************************************************************************
* Function Name  :  
* Description    :
*
*
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Bsp_can_fault(void)
{
	uint8_t portScan = 0;
	//------------------------------------------------------
	// 持续没有CAN没有接收到数据，判断CAN通讯丢失
	//------------------------------------------------------
	for(portScan = 0; portScan < 6; portScan++)
	{
		can.CanFltCnt[portScan]++;

		if(can.CanFltCnt[portScan] >= CAN_RX_TIME)
		{
			can.CanFltCnt[portScan] = CAN_RX_TIME;
			//-----------------------------------------------------
			// CAN接收1异常
			//-----------------------------------------------------
			can.CanStateWord &= ~(uint16_t)(0x0001 << portScan);
		}
		else
		{
			//-----------------------------------------------------
			// CAN接收1正常
			//-----------------------------------------------------
			can.CanStateWord |= (uint16_t)(0x0001 << portScan);
		}
	}
	
	MB_LGA.MB_SYS_INFO.AdFault = (~can.CanStateWord) & 0x0020;
	can.CanCnt ++;
	
	if (can.CanCnt >= CAN_RX_TIME)
	{
		can.CanCnt = CAN_RX_TIME;
		MB_LGA.MB_SYS_INFO.CanFault = 1; 
	}
	else
	{
		MB_LGA.MB_SYS_INFO.CanFault = 0;
	}
}

uint32_t _rbuffRamPtr;
CAN_RX_DATA_RAM * prbufram;
void CanApp_Rx_Task(void)
{
	
	for (;;)
	{
		//------------------------------------------------------------
		// 从队列弹出 接收数据帧 地址
		//------------------------------------------------------------
		_rbuffRamPtr = canRxQueue.push_Q();
		//------------------------------------------------------------
		// 弹出指针不为空, 则表示队列操作成功
		//------------------------------------------------------------
		if(_rbuffRamPtr != QUEUE_NULL)
		{
			//--------------------------------------------------------
			// 转换指针类型: RAM操作数据块
			//--------------------------------------------------------
			prbufram = (CAN_RX_DATA_RAM *)_rbuffRamPtr;
			//--------------------------------------------------------
			// 接收数据处理 
			//--------------------------------------------------------
			_App_Can_Rx((void*)_rbuffRamPtr);
		}
		else
		{
			canRxQueue.SetFifoReg(FIFO_EMPTY_MASK);
			break;
		}
	}
}

/*******************************************************************************
* Function Name  : CanApp_Tx_Task
* Description    :
*                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static uint32_t _tbuffRamPtr;
static CAN_TX_DATA_RAM * ptbufram;

void CanApp_Tx_Task(void)
{
	uint8_t resendCnt = 0;
	uint8_t mailFull;
	//----------------------------------------------------------------
	// 将发送队列内的CAN数据帧送至CAN控制器发送FIFO
	//----------------------------------------------------------------
	for(;;)
	{
		//------------------------------------------------------------
		// 从队列弹出 接收数据帧 地址
		//------------------------------------------------------------
		_tbuffRamPtr = canTxQueue.push_Q();

		//------------------------------------------------------------
		// 弹出指针不为空,则表示队列操作成功
		//------------------------------------------------------------
		if(_tbuffRamPtr != QUEUE_NULL)
		{
			canTxQueue.SetFifoReg(FIFO_EMPTY_MASK);
			//--------------------------------------------------------
			// 转换指针类型: RAM操作数据块
			//--------------------------------------------------------
			ptbufram = (CAN_TX_DATA_RAM *)_tbuffRamPtr;

			//--------------------------------------------------------
			// 发送数据处理 ，此处可以再添加异常处理，比如：连续12次
			// 发送失败，则补发3次，不成功退出
			//--------------------------------------------------------
			mailFull = 0;

			do 
			{
				mailFull = 0;

				if(_App_Can_Tx((void*)_tbuffRamPtr) == CAN_TxStatus_NoMailBox)
				{
					if(resendCnt < 3)
						mailFull = 1;
					else
						mailFull = 0;
					resendCnt++;
					rt_thread_delay(1);
				}



			} while (mailFull);

			resendCnt = 0;

		}
		else
		{
			break;
		}
	}
}

void  Bsp_can_rx_task(void* paramemter)
{
	//-------------------------------------------------------
	// 创建信号量 
	//-------------------------------------------------------
	rt_err_t rtv    = rt_sem_init(&canRxQueue.can_OPsem,  "SemCanQRx",  1,  RT_IPC_FLAG_FIFO);
	//--------------------------------------------------------------
	// 任务入口
	//--------------------------------------------------------------
	canRxQueue.QueueInit();
	for(;;) 
	{
		//----------------------------------------------------------------
		// 等待Can queue操作信号量
		//----------------------------------------------------------------
		rt_sem_take(&canRxQueue.can_OPsem, RT_WAITING_FOREVER);
		//---------------------------------------------------------
		// 处理队列数据
		//---------------------------------------------------------
		CanApp_Rx_Task();
	}
}

/*******************************************************************************
* Function Name  : void  Bsp_can_task  (void *paramemter)
* Description    :
*
*
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

static void  Bsp_can_tx_task(void* paramemter)
{
	//-------------------------------------------------------
	// 创建信号量 
	//-------------------------------------------------------
	rt_err_t rtv    = rt_sem_init(&canTxQueue.can_OPsem,  "SemCanQTx",  1,  RT_IPC_FLAG_FIFO);

	//--------------------------------------------------------------
	// 任务入口
	//--------------------------------------------------------------
	canTxQueue.QueueInit();
	for(;;) 
	{
		//----------------------------------------------------------
		// 等待Can queue操作信号量
		//----------------------------------------------------------
		rt_sem_take(&canTxQueue.can_OPsem, RT_WAITING_FOREVER);

		//---------------------------------------------------------
		// 处理队列数据
		//---------------------------------------------------------
		CanApp_Tx_Task();
	}
}
/*******************************************************************************
* Function Name  : void  Bsp_can_task  (void *paramemter)
* Description    :
*
*
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Bsp_can_task(void* paramemter)
{
	static uint8_t timeDelay = 0;

	for(;;)
	{
		//------------------------------------------------------
		// 捕获事件信号
		//------------------------------------------------------
		Bsp_can_check_event();

		//------------------------------------------------------
		// 处理事件周期事件
		//------------------------------------------------------
		if (_ev_cansend_flag & CanTask_Event_Cycle)
		{
			//-----------------------------------------------------
			// can周期数据发送服务
			//-----------------------------------------------------
			CanApp_CycleStream_Server();
			//-----------------------------------------------------
			// can故障判断
			//-----------------------------------------------------
			Bsp_can_fault();
			//-----------------------------------------------------
			// can指示灯
			//-----------------------------------------------------
			if(++timeDelay % 50 == 0)
			{
				timeDelay = 0;
				Bsp_can_led();
			}
		}
		//------------------------------------------------------
		// 处理触发事件
		//------------------------------------------------------
		if(_ev_cansend_flag & CanTask_Event_Trigger)
		{
			//-----------------------------------------------------
			// can触发数据发送服务
			//-----------------------------------------------------
			CanApp_TriggerStream_Server();
		}
	}
}
/*******************************************************************************
* Function Name  : Bsp_Cantask_create
* Description    :  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Bsp_cantask_create(uint8_t priority)
{  
	//-------------------------------------------------------------------------
	// CAN 通信配置管理初始化
	//-------------------------------------------------------------------------
	System_HW_Can_Init();
	//-------------------------------------------------------------------------
	// CAN 通信配置管理初始化
	//-------------------------------------------------------------------------
	Bsp_can_event_init();
	//-------------------------------------------------------------------------
	// can rx task
	//-------------------------------------------------------------------------
	rt_thread_init(&bsp_can_rx_queue_thread,
		"CanRx",
		Bsp_can_rx_task,
		RT_NULL,
		&bsp_can_rx_queue_stack[0], 
		sizeof(bsp_can_rx_queue_stack),
		priority-2, 20
		);

		//-------------------------------------------------------------------------
	// can tx task
	//-------------------------------------------------------------------------
	rt_thread_init(&bsp_can_tx_queue_thread,
		"CanTx",
		Bsp_can_tx_task,
		RT_NULL,
		&bsp_can_tx_queue_stack[0], 
		sizeof(bsp_can_tx_queue_stack),
		priority, 15
		);
		//-------------------------------------------------------------------------
	// CAN 发送任务的创建
	//-------------------------------------------------------------------------
	rt_thread_init(&bsp_can_thread,
		"CanTask",
		Bsp_can_task,
		RT_NULL,
		&bsp_can_stack[0], 
		sizeof(bsp_can_stack),
		priority, 10
		);
	//-------------------------------------------------------------------------
	// CAN 任务开启
	//-------------------------------------------------------------------------
	rt_thread_startup(&bsp_can_rx_queue_thread);
	rt_thread_startup(&bsp_can_tx_queue_thread);
	rt_thread_startup(&bsp_can_thread);

	CanStartComm = 1;
}

/*******************************************************************************
* Function Name  :  
* Description    :
*
*
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern "C" void Bsp_can_send_cycle_event(void)
{
	if((_ev_cansend_init == RT_EOK) && (CanStartComm == 1))
	{
		rt_event_send(&EventCanSend, CanTask_Event_Cycle);
	}
}

/*******************************************************************************
* Function Name  :  
* Description    :
*
*
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Bsp_can_send_trigger_event(void)
{
	if((_ev_cansend_init == RT_EOK) && (CanStartComm == 1))
	{
		rt_event_send(&EventCanSend, CanTask_Event_Trigger);
	}
}

/*******************************************************************************
* Function Name  :  
* Description    :         
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Bsp_can_check_event(void)
{
	if(CanStartComm == 0)   return;

	rt_event_recv(&EventCanSend,
		(CanTask_Event_Cycle | CanTask_Event_Trigger),
		(RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR),
		RT_WAITING_FOREVER,
		&_ev_cansend_flag);
}

/*******************************************************************************
* Function Name  :  
* Description    :
*
*
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Bsp_can_event_init(void)
{
	_ev_cansend_init = rt_event_init(&EventCanSend, "E_Can", RT_IPC_FLAG_FIFO);
}
