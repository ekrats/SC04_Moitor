#define _DS_SYS_INFO_

#include "sysInfo.h"
#include "stm32f4xx.h"
#include <rtthread.h>
#include "logicApp.h"
#include "app_Fault.h"

void Sys_Info::sysInfInit(void)
{
	DataFIFO = (SYS_INF_TYPE *)rt_malloc(SYS_INFO_SIZE * sizeof(SYS_INF_TYPE));
}

void Sys_Info::sysInfoRecord(void)
{
	static int count = 10;
	uint8_t temp1;
	uint8_t temp2;
	int i;
	
	if (--count==0)
	{
		count = 10;
		FifoRamPtr = FifoRamPtr >= 64 ? 0 : FifoRamPtr;
		rt_memcpy(&DataFIFO[FifoRamPtr], &MB_LGA.mbStatus, sizeof(SYS_INF_TYPE));
		rt_memcpy(Data, &DataFIFO[FifoRamPtr+1], (SYS_INFO_SIZE - FifoRamPtr - 1) * sizeof(SYS_INF_TYPE));
		rt_memcpy(&Data[SYS_INFO_SIZE - FifoRamPtr - 1], DataFIFO, (FifoRamPtr + 1) * sizeof(SYS_INF_TYPE));
		FifoRamPtr++;
	}
	
	for (i=0; i<FT_MAX-32; i++)
	{
		temp1 = i/16;
		temp2 = i%16;
		if (FAULT.FaultRcdFlg[temp1] & (1 << temp2))
		{
			if (FAULT.CountDown[i] > 0)
			{
				FAULT.CountDown[i] --;
			}
			else
			{
				MB_LGA.MB_SYS_INFO.wFaultFlg = true;
				FAULT.FaultRcdFlg[temp1] &= ~(1 << temp2);
				break;
			}
		}
		else
		{
			FAULT.CountDown[i] = faultDelay;
		}
	}
}
