#include "rs485Task.h"
#include "stm32f4xx.h"
#include "rtthread.h"
#include "Bsp_rs485.h"

static void Bsp_expCommRx_task(void* paramemter);
static void Bsp_expCommTx_task(void* paramemter);
static void Bsp_expComm_Bus_task(void* paramemter);

//extern struct rt_semaphore UsartRx_OPSem; 
//extern struct rt_semaphore BUS_ST_OPSem;
//extern struct rt_semaphore sem_usartTx_task;
/*******************************************************************************
* Function Name  : Task_expcomm_create 
* Description    :  
*                   
*
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Task_expcomm_create(uint8_t priority)
{
	rt_thread_t init_thread;

	System_HW_HMICOMM_Init();

	//------- init thread
	init_thread = rt_thread_create("CommRx",
		Bsp_expCommRx_task, RT_NULL,
		1024, priority, 20);

	rt_thread_startup(init_thread);

	//------- init thread
	init_thread = rt_thread_create("CommTx",
		Bsp_expCommTx_task, RT_NULL,
		1024, priority, 20);

	rt_thread_startup(init_thread);

	//------- init thread
	init_thread = rt_thread_create("CommBus",
		Bsp_expComm_Bus_task, RT_NULL,
		512, priority - 1, 20);

	rt_thread_startup(init_thread);
}

/*******************************************************************************
* Function Name  : void Bsp_expCommRx_task(void *paramemter)
* Description    : 外部通信任务
*                   
*
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

static void Bsp_expCommRx_task(void* paramemter)
{
	//--------------------------------------------------------------
	// 任务入口
	//--------------------------------------------------------------
	for(;;) 
	{
		//-------------------------------------------------------------
		// 等待信号量
		//-------------------------------------------------------------
//		rt_sem_take(&UsartRx_OPSem, RT_WAITING_FOREVER);
		//------------------------------------------------------
		// 处理RS485接收数据
		//------------------------------------------------------
		//HMICOMM_RX_Stream();
	}
}

/*******************************************************************************
* Function Name  : void Bsp_expCommTx_task(void *paramemter)
* Description    : 外部通信任务
*                   
*
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

static void Bsp_expCommTx_task(void* paramemter)
{
	//--------------------------------------------------------------
	// 任务入口
	//--------------------------------------------------------------
	for(;;) 
	{
		//-------------------------------------------------------------
		// 等待信号量
		//-------------------------------------------------------------
//		rt_sem_take(&sem_usartTx_task, RT_WAITING_FOREVER);
		//------------------------------------------------------
		// 处理RS485发送数据
		//------------------------------------------------------
		//HMICOMM_TX_Stream();
	}
}

/*******************************************************************************
* Function Name  : static void Bsp_expComm_Bus_task(void* paramemter)
* Description    : 扩展板通信任务
*                   
*
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

static void Bsp_expComm_Bus_task(void* paramemter)
{
	//--------------------------------------------------------------
	// 任务入口
	//--------------------------------------------------------------
	for(;;) 
	{
		//----------------------------------------------------------
		// 等待BUS操作信号量
		//----------------------------------------------------------
//		rt_sem_take(&BUS_ST_OPSem, RT_WAITING_FOREVER);
		//----------------------------------------------------------
		// RS485总线置为接收态 
		//----------------------------------------------------------
		Bsp_HMICOMM_Bus_To_RX();
	}
}
