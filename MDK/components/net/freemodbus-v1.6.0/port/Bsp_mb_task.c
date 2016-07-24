/**
  *************************************************************************
  * This file is part of modbus
  * COPYRIGHT (C) 1999-2014, TongYe R&D Team
  * @file    Bsp_mb_tsk.c
  * @brief   This file provides all the modbus task functions.
  *************************************************************************
  */
#include "Bsp_mb_task.h"
#include "user_mb_app.h"
#include "mbconfig.h"

#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0
static void thread_entry_ModbusSlavePoll(void* parameter);
#endif
#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0	
static void thread_entry_ModbusMasterPoll(void* parameter);
#endif
static void ModbusMasterTask(void* parameter);

/*******************************************************************************
* Function Name  : void Task_MBSlave_create(uint8_t priority) 
* Description    :  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Task_MBSlave_create(uint8_t priority) 
{
	rt_thread_t init_thread;
#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0
	//------- init thread
	init_thread = rt_thread_create("mbslave",
		thread_entry_ModbusSlavePoll, RT_NULL,
		512, priority, 20);
	//------- start up thread
	if(init_thread != RT_NULL)
		rt_thread_startup(init_thread);
#endif

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0	
	//------- init thread
	init_thread = rt_thread_create("mbmaster",
		thread_entry_ModbusMasterPoll, RT_NULL,
		512, priority, 20);
	//------- start up thread
	if(init_thread != RT_NULL)
		rt_thread_startup(init_thread);
#endif

	//------- init thread
	init_thread = rt_thread_create("mbtask",
		ModbusMasterTask, RT_NULL,
		512, priority, 20);
	//------- start up thread
	if(init_thread != RT_NULL)
		rt_thread_startup(init_thread);
}

/*******************************************************************************
* Function Name  : void thread_entry_ModbusSlavePoll(void) 
* Description    :  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0
static void thread_entry_ModbusSlavePoll(void* parameter)
{
	eMBInit(MB_RTU, 0x01, 1, 115200,  MB_PAR_NONE);
	eMBEnable();
	while (1)
	{
		eMBPoll();
	}
}
#endif

/*******************************************************************************
* Function Name  : void thread_entry_ModbusSlavePoll(void) 
* Description    :  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
static void thread_entry_ModbusMasterPoll(void* parameter)
{
	eMBMasterInit(MB_RTU, 2, 115200,  MB_PAR_NONE);
	eMBMasterEnable();
	while (1)
	{
		eMBMasterPoll();
	}
}
#endif

/*******************************************************************************
* Function Name  : static void ModbusMasterTask(void* parameter)
* Description    :  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void ModbusMasterTask(void* parameter)
{
	while (1)
	{
		rt_thread_delay(RT_TICK_PER_SECOND / 100);		
		Bsp_Modbus_Cycle();
	}
}

