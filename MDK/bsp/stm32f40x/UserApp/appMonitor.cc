#include "appMonitor.h"

#include "stm32f4xx.h"
#include <rtthread.h>
#include "logicApp.h"
#include "canApp.h"

System_Mode	SysMode;

System_Mode::System_Mode(void)
{
	this->SystemState = SYS_STATE_STOP;
	this->SystemOutput = SYS_OUT_STOP;
}

void System_Mode::ChargeModeManage(void)
{
	switch (this->SystemState)
	{
		case SYS_STATE_STOP :
			//ÐÝÃß
			if (MB_LGA.MB_SYS_INFO.SysModeSet == SLEEP)
			{
				this->SystemState = SYS_STATE_SLEEP;
				this->SystemOutput = SYS_OUT_SLEEP;
			}
			//Æ÷¼þ²âÊÔ
			else if (MB_LGA.MB_SYS_INFO.SysModeSet == DEVICE_TEST)
			{
				this->SystemState = SYS_STATE_TEST;
				this->SystemOutput = SYS_OUT_TEST;
			}
			//ÑÏÖØ¹ÊÕÏ
			else if (MB_LGA.MB_SYS_INFO.sFault == true)
			{
				this->SystemState = SYS_STATE_FAULT;
				this->SystemOutput = SYS_OUT_FAULT;
			}
			//ÆðÍ£°´Å¥ °´ÏÂ
			else if (MB_LGA.MB_IO_IN.cabinetEnN == true)
			{
				this->SystemState = SYS_STATE_PRECHARGE;
				this->SystemOutput = SYS_OUT_PRECHARGE;
			}
			else
			{
				this->SystemState = SYS_STATE_STOP;
				this->SystemOutput = SYS_OUT_STOP;
			}
			break;
			
		case SYS_STATE_PRECHARGE :
			//ÐÝÃß
			if (MB_LGA.MB_SYS_INFO.SysModeSet == SLEEP)
			{
				this->SystemState = SYS_STATE_SLEEP;
				this->SystemOutput = SYS_OUT_SLEEP;
			}
			//Æ÷¼þ²âÊÔ
			else if (MB_LGA.MB_SYS_INFO.SysModeSet == DEVICE_TEST)
			{
				this->SystemState = SYS_STATE_TEST;
				this->SystemOutput = SYS_OUT_TEST;
			}
			//ÑÏÖØ¹ÊÕÏ
			else if (MB_LGA.MB_SYS_INFO.sFault == true)
			{
				this->SystemState = SYS_STATE_FAULT;
				this->SystemOutput = SYS_OUT_FAULT;
			}
			//ÆðÍ£°´Å¥ °´ÏÂ
			else if (MB_LGA.MB_IO_IN.cabinetEnN == false)
			{
				this->PreChargeStep1 = false;
				this->PreChargeStep2 = false;
				this->SystemState = SYS_STATE_STOP;
				this->SystemOutput = SYS_OUT_STOP;
			}
			//Ô¤³äµçÍê³É
			else if (this->PreFinsh)
			{
				this->SystemState = SYS_STATE_STANDBY;
				this->SystemOutput = SYS_OUT_STANDBY;
			}
			//Ô¤³äµçing
			else
			{
				this->SystemState = SYS_STATE_PRECHARGE;
				this->SystemOutput = SYS_OUT_PRECHARGE;
			}
			break;
			//´ý»úÌ¬
		case SYS_STATE_STANDBY :
			//ÐÝÃß
			if (MB_LGA.MB_SYS_INFO.SysModeSet == SLEEP)
			{
				this->SystemState = SYS_STATE_SLEEP;
				this->SystemOutput = SYS_OUT_SLEEP;
			}
			//Æ÷¼þ²âÊÔ
			else if (MB_LGA.MB_SYS_INFO.SysModeSet == DEVICE_TEST)
			{
				this->SystemState = SYS_STATE_TEST;
				this->SystemOutput = SYS_OUT_TEST;
			}
			//ÑÏÖØ¹ÊÕÏ
			else if (MB_LGA.MB_SYS_INFO.sFault == true)
			{
				this->SystemState = SYS_STATE_FAULT;
				this->SystemOutput = SYS_OUT_FAULT;
			}
			//ÆðÍ£°´Å¥ °´ÏÂ
			else if (MB_LGA.MB_IO_IN.cabinetEnN == false)
			{
				this->PreChargeStep1 = false;
				this->PreChargeStep2 = false;
				this->SystemState = SYS_STATE_STOP;
				this->SystemOutput = SYS_OUT_STOP;
			}
			else
			{
				this->SystemState = SYS_STATE_STANDBY;
				this->SystemOutput = SYS_OUT_STANDBY;
			}
			break;
			//¹ÊÕÏÌ¬
		case SYS_STATE_FAULT :
			//ÐÝÃß
			if (MB_LGA.MB_SYS_INFO.SysModeSet == SLEEP)
			{
				this->SystemState = SYS_STATE_SLEEP;
				this->SystemOutput = SYS_OUT_SLEEP;
			}
			//Æ÷¼þ²âÊÔ
			else if (MB_LGA.MB_SYS_INFO.SysModeSet == DEVICE_TEST)
			{
				this->SystemState = SYS_STATE_TEST;
				this->SystemOutput = SYS_OUT_TEST;
			}
			//¹ÊÕÏÏûÊ§
			else if (MB_LGA.MB_SYS_INFO.sFault == false)
			{
				this->SystemState = SYS_STATE_STOP;
				this->SystemOutput = SYS_OUT_STOP;
			}
			else
			{
				this->SystemState = SYS_STATE_FAULT;
				this->SystemOutput = SYS_OUT_FAULT;
			}
			break;
			//Æ÷¼þ²âÊÔ
		case SYS_STATE_TEST:
			//ÐÝÃß
			if (MB_LGA.MB_SYS_INFO.SysModeSet == SLEEP)
			{
				this->SystemState = SYS_STATE_SLEEP;
			}
			//ÍË³öÆ÷¼þ²âÊÔ
			else if (MB_LGA.MB_SYS_INFO.SysModeSet == CHARGE)
			{
				this->SystemState = SYS_STATE_STOP;
				this->SystemOutput = SYS_OUT_STOP;
			}
			else
			{
				this->SystemState = SYS_STATE_TEST;
				this->SystemOutput = SYS_OUT_TEST;
			}
			break;
			//ÐÝÃßÌ¬
		case SYS_STATE_SLEEP:
			//½øÈëÐÝÃß
			if (MB_LGA.CtrCmd.SleepCmd == IN_SLEEP)
			{
				this->SystemState = SYS_STATE_SLEEP;
				this->SystemOutput = SYS_OUT_SLEEP;
			}
			//ÍË³öÐÝÃß
			else if (MB_LGA.CtrCmd.SleepCmd == OUT_SLEEP)
			{
				this->SystemState = SYS_STATE_STOP;
				this->SystemOutput = SYS_OUT_STOP;
			}
			else
			{
				this->SystemState = SYS_STATE_STOP;
				this->SystemOutput = SYS_OUT_STOP;
			}
			break;
			//ÆäËû
		default:
			this->SystemState = SYS_STATE_STOP;
			this->SystemOutput = SYS_OUT_STOP;
			break;
			
	}
}

void System_Mode::StopCal(void)
{
	if (MB_LGA.cb1_SYS.input.Charging == false && MB_LGA.cb2_SYS.input.Charging == false
	 && MB_LGA.cb3_SYS.input.Charging == false && MB_LGA.cb4_SYS.input.Charging == false
	 && MB_LGA.cb5_SYS.input.Charging == false)
	 {
		MB_LGA.MB_IO_OUT.mainCon1N = false;
		MB_LGA.MB_IO_OUT.mainCon2N = false;
		MB_LGA.MB_IO_OUT.preConN = false;
		MB_LGA.MB_IO_OUT.outCon1N = false;
		MB_LGA.MB_IO_OUT.outCon2N = false;
		MB_LGA.MB_IO_OUT.outCon3N = false;
		MB_LGA.MB_IO_OUT.outCon4N = false;
		MB_LGA.MB_IO_OUT.outCon5N = false;
	 }
	 
	PreDelayCnt = PRE_CHARGE_DELAY1;
	PreDelay2Cnt = PRE_CHARGE_DELAY2;
}

void System_Mode::PreChargeCal(void)
{
	MB_LGA.MB_IO_OUT.mainCon1N = false;
	MB_LGA.MB_IO_OUT.mainCon2N = false;
	MB_LGA.MB_IO_OUT.preConN = true;
	MB_LGA.MB_IO_OUT.outCon1N = false;
	MB_LGA.MB_IO_OUT.outCon2N = false;
	MB_LGA.MB_IO_OUT.outCon3N = false;
	MB_LGA.MB_IO_OUT.outCon4N = false;
	MB_LGA.MB_IO_OUT.outCon5N = false;
	
	//Ô¤³äµçµÚÒ»½×¶Î¿ªÊ¼
	this->PreChargeStep1 = true;
	
	if(PreDelayCnt > 0)
	{
		PreDelayCnt --;
	}
	else
	{
		this->PreChargeStep2 = true;
		if (MB_LGA.MB_SYS_INFO.busVoltOK)
		{
			MB_LGA.MB_IO_OUT.mainCon1N = true;
			if (PreDelay2Cnt > 0)
			{
				PreDelay2Cnt --;
			}
			else
			{
				MB_LGA.MB_IO_OUT.mainCon2N = true;
				MB_LGA.MB_IO_OUT.preConN = false;
				
				this->PreFinsh = true;
				PreDelayCnt = PRE_CHARGE_DELAY1;
				PreDelay2Cnt = PRE_CHARGE_DELAY2;
			}
		}
	}
}

void System_Mode::StandbyCal(void)
{
	MB_LGA.MB_IO_OUT.mainCon1N = true;
	MB_LGA.MB_IO_OUT.mainCon2N = true;
	MB_LGA.MB_IO_OUT.preConN = false;
	
	PreDelayCnt = PRE_CHARGE_DELAY1;
	PreDelay2Cnt = PRE_CHARGE_DELAY2;
}

void System_Mode::FaultCal(void)
{
	if (MB_LGA.cb1_SYS.input.Charging == false && MB_LGA.cb2_SYS.input.Charging == false
	 && MB_LGA.cb3_SYS.input.Charging == false && MB_LGA.cb4_SYS.input.Charging == false
	 && MB_LGA.cb5_SYS.input.Charging == false)
	 {
		MB_LGA.MB_IO_OUT.mainCon1N = false;
		MB_LGA.MB_IO_OUT.mainCon2N = false;
		MB_LGA.MB_IO_OUT.preConN = false;
		MB_LGA.MB_IO_OUT.outCon1N = false;
		MB_LGA.MB_IO_OUT.outCon2N = false;
		MB_LGA.MB_IO_OUT.outCon3N = false;
		MB_LGA.MB_IO_OUT.outCon4N = false;
		MB_LGA.MB_IO_OUT.outCon5N = false;
	 }
	 
	PreDelayCnt = PRE_CHARGE_DELAY1;
	PreDelay2Cnt = PRE_CHARGE_DELAY2;
}

void System_Mode::DevTestCal(void)
{
	uint16_t DevTestCmd;
	DevTestCmd = MB_LGA.CtrCmd.DevTestCmd;
	
	if (DevTestCmd & (0x0001 << 0))
	{
		MB_LGA.MB_IO_OUT.preConN = true;
	}
	else
	{
		MB_LGA.MB_IO_OUT.preConN = false;
	}
	if (DevTestCmd & (0x0001 << 1))
	{
		MB_LGA.MB_IO_OUT.mainCon1N = true;
	}
	else
	{
		MB_LGA.MB_IO_OUT.mainCon1N = false;
	}
	if (DevTestCmd & (0x0001 << 2))
	{
		MB_LGA.MB_IO_OUT.mainCon2N = true;
	}
	else
	{
		MB_LGA.MB_IO_OUT.mainCon2N = false;
	}
	if (DevTestCmd & (0x0001 << 3))
	{
		MB_LGA.MB_IO_OUT.outCon1N = true;
	}
	else
	{
		MB_LGA.MB_IO_OUT.outCon1N = false;
	}
	if (DevTestCmd & (0x0001 << 4))
	{
		MB_LGA.MB_IO_OUT.outCon2N = true;
	}
	else
	{
		MB_LGA.MB_IO_OUT.outCon2N = false;
	}
	if (DevTestCmd & (0x0001 << 5))
	{
		MB_LGA.MB_IO_OUT.outCon3N = true;
	}
	else
	{
		MB_LGA.MB_IO_OUT.outCon3N = false;
	}
	if (DevTestCmd & (0x0001 << 6))
	{
		MB_LGA.MB_IO_OUT.outCon4N = true;
	}
	else
	{
		MB_LGA.MB_IO_OUT.outCon4N = false;
	}
	if (DevTestCmd & (0x0001 << 7))
	{
		MB_LGA.MB_IO_OUT.outCon5N = true;
	}
	else
	{
		MB_LGA.MB_IO_OUT.outCon5N = false;
	}
	PreDelayCnt = PRE_CHARGE_DELAY1;
	PreDelay2Cnt = PRE_CHARGE_DELAY2;
}

void System_Mode::SleepCal(void)
{
	__NOP();
	PreDelayCnt = PRE_CHARGE_DELAY1;
	PreDelay2Cnt = PRE_CHARGE_DELAY2;
}

void System_Mode::System_Output(void)
{
	switch (this->SystemOutput)
	{
		//Í£»úÌ¬
		case SYS_OUT_STOP:
			StopCal();
			break;
		//Ô¤³äµç
		case SYS_OUT_PRECHARGE:
			PreChargeCal();
			break;
		//´ý»úÌ¬
		case SYS_OUT_STANDBY:
			StandbyCal();
			break;
		//¹ÊÕÏÌ¬
		case SYS_OUT_FAULT:
			FaultCal();
			break;
		//Æ÷¼þ²âÊÔ
		case SYS_OUT_TEST:
			DevTestCal();
			break;
		//ÐÝÃß
		case SYS_OUT_SLEEP:
			SleepCal();
			break;
		default:
			break;
	}
}
