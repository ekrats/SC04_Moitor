#ifndef _APP_MODULE_H_
#define _APP_MODULE_H_

#include "stm32f4xx.h"
#include <rtthread.h>
#include "MB_DataStruct.h"
#include "logicApp.h"

#define CB_STATE_IDLE				0x0000
#define CB_STATE_MANUAL				0x0010
#define CB_STATE_AUTO				0x0020
#define CB_STATE_FAULT				0x0030

#define CB_OUT_IDLE					0x0001
#define CB_OUT_MANUAL				0x0002
#define CB_OUT_STANDY				0x0003
#define CB_OUT_AUTO					0x0004
#define CB_OUT_FAULT				0x0005

#define CHARGE_BASE_T						10//ms

#define CHARGE_START_T						(2000/CHARGE_BASE_T)//启动时间
#define CHARGE_FLOAT_T						(5000/CHARGE_BASE_T)//恒压时间

enum{
	CB_STANDBY,
	CB_MANUAL,
	CB_AUTO,
	CB_FAULT,
};

class CbMode
{
private:
	u8 canIndex;
	u16 State;
	u16 OutputMode;
	u16	startDelay;
	u16 floatDelay;
public:
	Charge_Cmd_STYP *pcb_cmd;
	MODULE_SYS_STYP	*pcb_sys;

	CbMode();
	CbMode(MODULE_SYS_STYP *sys, Charge_Cmd_STYP *cmd, u8 Index);
	void CB_Idle_Cal(void);
	void CB_Standby_Cal(void);
	void CB_Manual_Cal(void);
	void CB_Auto_Cal(void);
	void CB_Fault_Cal(void);
	void ModuleManage(void);
	void OutputManage(void);
	void CbCtrl(void);
};



extern CbMode Moudle1Charge;
extern CbMode Moudle2Charge;
extern CbMode Moudle3Charge;
extern CbMode Moudle4Charge;
extern CbMode Moudle5Charge;

void DcDcManage(void);

#endif
