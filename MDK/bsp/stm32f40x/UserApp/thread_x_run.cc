#include "thread_x_run.h"
#include "stm32f4xx.h"
#include <rtthread.h>
#include "canTask.h"
#include "Bsp_Board_Init.h"
#include "AdIo_task.h"
#include "logicTask.h"
#include "logicApp.h"

struct rt_semaphore sem_reset_cpu;


/*******************************************************************************
* Function Name  : rt_cycle_thread_entry
* Description    : 
* Input          :  .
* Output         : None
* Return         : None
*******************************************************************************/
__IO uint8_t enDog = 1;
static void rt_cycle_thread_entry(void* parameter)
{
	static uint8_t timeDelay = 0;
	//------------------------------------------------------
	// 系统指示灯初始化
	//------------------------------------------------------
	Bsp_led_init();
	
	while(1)
	{
		rt_thread_delay(RT_TICK_PER_SECOND/100);

#if IWDG_EN
		if(enDog)	IWDG_ReloadCounter();
#endif
		
		if(++timeDelay % 50 == 0)
		{
			timeDelay = 0;
			//------------------------------------------------------
			// 获取系统当前RTC时间
			//------------------------------------------------------
			Bsp_get_time();
			//------------------------------------------------------
			// 系统运行灯
			//------------------------------------------------------
			if(GPIOA->ODR & GPIO_Pin_12)
			{
				GPIOA->BSRRH = GPIO_Pin_12;
			}
			else
			{
				GPIOA->BSRRL = GPIO_Pin_12;
			}
		}
	}
}

void rt_init_thread_entry(void* parameter)
{
    rt_sem_init(&sem_reset_cpu, "reset", 0, 0);
    
    hardware_crc32_init();
    at45dbxx_init("flash", "at45");
    storage_distribute(32, 1, RECORD_CATALOG);
    storage_distribute(32, 1, RECORD_PAR);
    storage_distribute(960, 1, RECORD_LOG);
    storage_distribute(3072, 8, RECORD_FAULT);
    flash_storage_init("fs", "flash");
    
    rt_device_init_all();
	rt_thread_delay(200);
	
    //GUI
    if(rt_sem_take(&sem_reset_cpu, RT_WAITING_FOREVER) == RT_EOK)
    {
		rt_thread_delay(1000);
		//关闭所有中断相应
		__set_FAULTMASK(1);
		//系统复位
		NVIC_SystemReset();
    }
}

static void init_thread_create(uint8_t priority)
{
	rt_thread_t tid;

    tid = rt_thread_create("init",
        rt_init_thread_entry, RT_NULL,
        2048, RT_THREAD_PRIORITY_MAX / 3, 20);

    if (tid != RT_NULL)
        rt_thread_startup(tid);
}
/*******************************************************************************
* Function Name  : Task_Cycle_create
* Description    : 
* Input          :  .
* Output         : None
* Return         : None
*******************************************************************************/
static void app_Cycle_create(uint8_t priority) 
{
	rt_thread_t init_thread;

	//------- init thread
	init_thread = rt_thread_create("cycle",
		rt_cycle_thread_entry, RT_NULL,
		512, priority, 20);
	//------- start up thread
	if(init_thread != RT_NULL)
		rt_thread_startup(init_thread);
}

/**
 * 主要线程配置
 */
extern "C" void rt_app_thread_x(void)
{
	init_thread_create(RT_THREAD_PRIORITY_MAX / 3);
	
	app_Cycle_create(18);
	
	Bsp_cantask_create(8);
	
	Task_MBSlave_create(15);
	
	System_AdIoTask_create(10);
	
	Task_MainLogic_create(12);
	
//	/*** pcmaster  task  ***/	
	pcmaster_recordtask_create(20);

	/*** multi serial  task  ***/
	serial_servicestask_create(20);
}
