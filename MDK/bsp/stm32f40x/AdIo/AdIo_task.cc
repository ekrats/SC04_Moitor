#include "AdIo_task.h"
#include "stm32f4xx.h"
#include <rtthread.h>
#include "adApp.h"

extern struct rt_semaphore sem_adio_task;

/*******************************************************************************
* Function Name  : Bsp_collection_task 
* Description    :  
*                   
*
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

static void Bsp_collection_task(void* parameter)
{
	//-------------------------------------------------------------
	// ADC module
	//-------------------------------------------------------------
	System_hw_ADC_Init();
	//-------------------------------------------------------------
	// IO module
	//-------------------------------------------------------------
	System_fsmc_init();
	Bsp_Dio_Init();

	while(1)
	{
		//----------------------------------------------------------
		// wait for semaphore
		//----------------------------------------------------------
		rt_sem_take(&sem_adio_task, RT_WAITING_FOREVER);
		//-----------  ---------------------------------------------
		// 读取模拟量数据
		//----------------------------------------------------------
		ADC_ReadInput();
		//----------------------------------------------------------
		// 读取数字量数据
		//----------------------------------------------------------
		IO_ReadInput();		
		//----------------------------------------------------------
		// 数字量数据输出
		//----------------------------------------------------------
		IO_WriteOutput();
	}
}

int System_AdIoTask_create(uint8_t priority)
{
	rt_thread_t init_thread;

	//------- init thread
	init_thread = rt_thread_create("Collect",
		Bsp_collection_task, RT_NULL,
		1024, priority, 20);
	rt_thread_startup(init_thread);

	return 0;
}

