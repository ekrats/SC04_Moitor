#ifndef _LOGIC_APP_H_
#define _LOGIC_APP_H_
#include "MB_DataStruct.h"
#include "common.h"
#include "Bsp_io.h"
#include "Bsp_rtc.h"

#define CHARGE_STARTV	2000		//车进站检测电压，10倍系数
#define CHARGE_DEPARTV	2000		//车出站检测电压，10倍系数
#define CHARGE_STOPI	500			//充电终止最小电流50A

enum {
	NONE,
	MODULE1,
	MODULE2,
	MODULE3,
	MODULE4,
	MODULE5,
	MODULE_ALL = 16,
};

enum {
	CHARGE,
	DEVICE_TEST,
	SLEEP,
};

enum {
	NONE_EVENT,
	IN_SLEEP,
	OUT_SLEEP,
};

//------------------------------------------------------
// 系统信息
//------------------------------------------------------
typedef struct _MB_SYS_INFO_
{
	uint16_t 	SystemState;
	uint16_t	OutputMode;
	uint16_t    SysModeSet;
	
	uint8_t		wFault				: 1 ; //警告故障
	uint8_t		sFault				: 1 ; //严重故障
	uint8_t		busConnect			: 1 ;
	uint8_t		busVoltOK			: 1 ; //母线电压OK
	uint8_t		PreFinsh			: 1 ; //预充电完成状态位
	uint8_t		Charging			: 1 ;
	uint8_t		CanFault			: 1 ; //CAN掉线
	uint8_t		HmiFault			: 1 ; //显示屏通信标志位 0：通讯正常 1：异常
	uint8_t		AdFault				: 1 ; //AD板通信标志位 0：通讯正常 1：异常
	uint8_t		wParFlag			: 1 ;
	uint8_t		rParFlag			: 1 ;
	uint8_t		wFaultFlg			: 1 ;
	uint8_t		rFaultFlg			: 1 ;
	
}MB_LGA_INFO;

typedef struct _input_styp
{
	uint16_t		Charge_Mode_Set;
	uint16_t		Charge_Enable : 1 ;
	uint16_t		Instation	  : 1 ;
	uint16_t		Charging	  : 1 ;
	uint16_t		floatChargeN  : 1 ;
	uint16_t		Fault		  : 1 ;
}input_styp;

typedef struct _output_styp
{
	uint16_t		Charge_Mode_Now;
	uint16_t		Pwm_On_Fail	  : 1 ;
	uint16_t		Pwm_Off_Fail  : 1 ;
	uint16_t		chargeComplet : 1 ;
}output_styp;

typedef struct _CHARGE_SYS_STYP_
{
	input_styp 		input;
	output_styp 	output;
}MODULE_SYS_STYP;

typedef struct _AN_VAL_
{
	uint16_t		Ch1_AC;				//1#交流输入电压
	uint16_t		Ch2_AC;				//2#交流输入电压
	uint16_t		BUS_VOLT;			//母线电压
	uint16_t		Ch1_In_Curent;		//1#整流电流
	uint16_t		Ch2_In_Curent;		//2#整流电流
	uint16_t		Module1_Temp;		//1#整流模块温度
	uint16_t		Module2_Temp;		//2#整流模块温度
	uint16_t		FanIn_Temp;			//进风口温度
	uint16_t		FanOut_Temp;		//出风口温度
	uint16_t		Cap1_Temp;			//1#电容模块温度
	uint16_t		Cap2_Temp;			//2#电容模块温度
	uint16_t		Ch1ChargeVolt;		//1#输出电压
	uint16_t		Ch2ChargeVolt;		//2#输出电压
	uint16_t		Ch3ChargeVolt;		//3#输出电压
	uint16_t		Ch4ChargeVolt;		//4#输出电压
	uint16_t		Ch5ChargeVolt;		//5#输出电压
}AN_VAL_TYPE;

typedef struct _LogicApp_
{
	MB_LGA_INFO		MB_SYS_INFO;
	SYS_TIME_RTC    time_rtc;
	CtrCmd_STYP		CtrCmd;
	MODULE_SYS_STYP	cb1_SYS;
	MODULE_SYS_STYP	cb2_SYS;
	MODULE_SYS_STYP	cb3_SYS;
	MODULE_SYS_STYP	cb4_SYS;
	MODULE_SYS_STYP	cb5_SYS;
	HMI_Status_STYP	mbStatus;
	AN_VAL_TYPE		MB_AN_VAL;
	In_Flags_STYP 	MB_IO_IN;
	Out_Flags_STYP	MB_IO_OUT;
	MB_Para_STYP	MB_Para;
	MB_Para_STYP 	MB_RdPara;
	MB_Para_STYP 	MB_WrPara;
	Charge_Cmd_STYP Modue1_ChargeCmd;
	Charge_Cmd_STYP Modue2_ChargeCmd;
	Charge_Cmd_STYP Modue3_ChargeCmd;
	Charge_Cmd_STYP Modue4_ChargeCmd;
	Charge_Cmd_STYP Modue5_ChargeCmd;
	CAN_CB_Status_STYP cb1Status;
	CAN_CB_Status_STYP cb2Status;
	CAN_CB_Status_STYP cb3Status;
	CAN_CB_Status_STYP cb4Status;
	CAN_CB_Status_STYP cb5Status;
	CB_Para_STYP	Modue1_Para;
	CB_Para_STYP	Modue2_Para;
	CB_Para_STYP	Modue3_Para;
	CB_Para_STYP	Modue4_Para;
	CB_Para_STYP	Modue5_Para;
	CB_Para_STYP	Modue1_WPara;
	CB_Para_STYP	Modue2_WPara;
	CB_Para_STYP	Modue3_WPara;
	CB_Para_STYP	Modue4_WPara;
	CB_Para_STYP	Modue5_WPara;
}MB_LGA_TYPE;

#ifdef _DS_LGA_
MB_LGA_TYPE MB_LGA = { 0 };
#else
extern MB_LGA_TYPE MB_LGA;
#endif

void AnalogInputUpdate(void);
void DigitalInputUpdate(void);
void DigitalOutputUpdate(void);
void CanInputUpdate(void);
void MonitorUpdate(void);
void CanOutputUpdate(void);
void HmiInputUpdate(void);
void HmiOutputUpdate(void);
void ParManage(void);
void UserRecord(void);
void FaultManage(void);

#endif

