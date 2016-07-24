#ifndef _ADIO_TASK_H_
#define _ADIO_TASK_H_

#include "stm32f4xx.h"
#include <rtthread.h>

extern "C" {
	#include "Bsp_ad.h"
	#include "Bsp_Io.h"
	#include "Bsp_Board_Init.h"
}

int System_AdIoTask_create(uint8_t priority);

#endif

