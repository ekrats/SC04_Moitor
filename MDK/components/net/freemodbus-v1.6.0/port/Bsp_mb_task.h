
/**
  *************************************************************************
  * This file is part of modbus
  * COPYRIGHT (C) 1999-2014, TongYe R&D Team
  * @file    Bsp_mb_task.h
  * @brief   This file provides all the modbus task functions.
  *************************************************************************
*/	
#ifndef	BSP_MB_TASK
#define BSP_MB_TASK

#include "stm32f4xx.h"

void Task_MBSlave_create(uint8_t priority);

#endif
