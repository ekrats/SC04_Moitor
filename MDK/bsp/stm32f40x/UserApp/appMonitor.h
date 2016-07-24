#ifndef _APP_MONITOR_H_
#define _APP_MONITOR_H_

#include "stm32f4xx.h"
#include <rtthread.h>

#define CHARGE_BASE_T						10//ms
#define PRE_CHARGE_DELAY1			(60000/CHARGE_BASE_T)//预充电时间
#define PRE_CHARGE_DELAY2			(500/CHARGE_BASE_T)

#define SYS_STATE_STOP			    0x0000
#define SYS_STATE_PRECHARGE			0x0100
#define SYS_STATE_STANDBY			0x0200
#define SYS_STATE_FAULT				0x0300
#define SYS_STATE_TEST				0x0400
#define SYS_STATE_SLEEP				0x0500

#define SYS_OUT_STOP			    0x1000
#define SYS_OUT_PRECHARGE			0x2000
#define SYS_OUT_STANDBY				0x3000
#define SYS_OUT_FAULT				0x4000
#define SYS_OUT_TEST				0x5000
#define SYS_OUT_SLEEP 				0x6000

enum{
	CHARGE_NONE,
	CHARGE_MODE_AUTO,
	CHARGE_MODE_MANUAL,
	CHARGE_MODE_STANDBY,
};

class System_Mode
{
private:
	uint16_t 	PreDelayCnt;
	uint16_t 	PreDelay2Cnt;
public:
	u16	SystemState;
	u16 SystemOutput;
	uint8_t		PreChargeStep1		: 1 ; //预充电接触器闭合
	uint8_t		PreChargeStep2		: 1 ; //预充电时间之后
	uint8_t		PreFinsh			: 1 ; //预充电完成状态位

	System_Mode(void);
	void StopCal(void);
	void PreChargeCal(void);
	void StandbyCal(void);
	void FaultCal(void);
	void DevTestCal(void);
	void SleepCal(void);
	void System_Output(void);
	void ChargeModeManage(void);
};

extern System_Mode	SysMode;

#endif
