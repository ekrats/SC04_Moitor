#define _DS_FAULT_

#include "app_Fault.h"
#include "stm32f4xx.h"
#include <rtthread.h>
#include "logicApp.h"
#include "adApp.h"
#include "config.h"


//故障产生持续时间   单位/S
const double ConTime_default[FT_MAX] = 
{
	3,		3,		3,		0,		5,		5,		2,		2,		2,		2,		2,		2,		5,		5,		5,		5,
	
	5,		5,		5,		5,		3,		3,		3,		0,		0,		3,		3,
	
	2, 	2,		2,		2,		2,		5,		5,		5,		2,		2,		2,		2, 		0.2, 	0.2,	5,		5,
	
	5, 	5, 		0.8,	0.8, 	5, 		0.2, 	3,		3,
};
//故障消除持续时间	 单位/S
const double DisTime_default[FT_MAX] = 
{
	0,		0,		0,		0,		1,		1,		0.5,	0.5,	0.5,	0.5,	0.5,	0.5,	1,		1,		0.2,	0.2,
	
	0.2,	0.2,	0.2,	0.2,	0.2,  	0,		0,		0,		0,		6,  	6,
	
	0, 	0, 		0,		0,		0.2,	180,	180,	0.2,	180,	180,	180,	180,	180,	180,	1,		1,
	
	1,		1,		180,	180,	180,	180,	0.2,	0.2,
};

FBuff ErrInfo_w;
FBuff ErrInfo_r;

void FaultAnalyse(void)
{
	//can掉线
	if (MB_LGA.MB_SYS_INFO.CanFault)
	{
		FAULT.FaultMask[WN_CAN_MxCx] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_CAN_MxCx] = false;
	}
	//AD板掉线
	if (MB_LGA.MB_SYS_INFO.AdFault)
	{
		FAULT.FaultMask[WN_CAN_MxCx] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_CAN_MxCx] = false;
	}
	//进风口温度
	if (AD.FanIn_Temp > FanIn_Temp_Over)
	{
		FAULT.FaultMask[WN_IN_T_OVER] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_IN_T_OVER] = false;
	}
	//出风口温度
	if (AD.FanOut_Temp > FanOut_Temp_Over)
	{
		FAULT.FaultMask[WN_OUT_T_OVER] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_OUT_T_OVER] = false;
	}
	//进风口温度传感器故障
	if (AD.FanIn_Temp <= Temp_Min || AD.FanIn_Temp >= Temp_Max)
	{
		FAULT.FaultMask[WN_IN_T_FAIL] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_IN_T_FAIL] = false;
	}
	//出风口温度传感器故障
	if (AD.FanOut_Temp <= Temp_Min || AD.FanOut_Temp >= Temp_Max)
	{
		FAULT.FaultMask[WN_OUT_T_FAIL] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_OUT_T_FAIL] = false;
	}
	//1#整流模块传感器故障
	if (AD.Module1_Temp <= Temp_Min || AD.Module1_Temp >= Temp_Max)
	{
		FAULT.FaultMask[WN_L1_T_FAIL] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_L1_T_FAIL] = false;
	}
	//2#整流模块传感器故障
	if (AD.Module2_Temp <= Temp_Min || AD.Module2_Temp >= Temp_Max)
	{
		FAULT.FaultMask[WN_L2_T_FAIL] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_L2_T_FAIL] = false;
	}
	//1#电容模块温度传感器故障
	if (AD.Cap1_Temp <= Temp_Min || AD.Cap1_Temp >= Temp_Max)
	{
		FAULT.FaultMask[WN_CAP1_T_FAIL] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_CAP1_T_FAIL] = false;
	}
	//2#电容模块温度传感器故障
	if (AD.Cap2_Temp <= Temp_Min || AD.Cap2_Temp >= Temp_Max)
	{
		FAULT.FaultMask[WN_CAP2_T_FAIL] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_CAP2_T_FAIL] = false;
	}
	//预充电接触器反馈故障
	if (MB_LGA.MB_IO_IN.preConN != MB_LGA.MB_IO_OUT.preConN)
	{
		FAULT.FaultMask[WN_PRE_CON_FB] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_PRE_CON_FB] = false;
	}
	//1#主接触器反馈故障
	if (MB_LGA.MB_IO_IN.mainCon1N != MB_LGA.MB_IO_OUT.mainCon1N)
	{
		FAULT.FaultMask[WN_MAIN_CON1_FB] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_MAIN_CON1_FB] = false;
	}
	//2#主接触器反馈故障
	if (MB_LGA.MB_IO_IN.mainCon2N != MB_LGA.MB_IO_OUT.mainCon2N)
	{
		FAULT.FaultMask[WN_MAIN_CON2_FB] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_MAIN_CON2_FB] = false;
	}
	//1#输出接触器反馈故障
	if (MB_LGA.MB_IO_IN.outCon1N != MB_LGA.MB_IO_OUT.outCon1N)
	{
		FAULT.FaultMask[WN_OUT1_CON_FB] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_OUT1_CON_FB] = false;
	}
	//2#输出接触器反馈故障
	if (MB_LGA.MB_IO_IN.outCon2N != MB_LGA.MB_IO_OUT.outCon2N)
	{
		FAULT.FaultMask[WN_OUT2_CON_FB] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_OUT2_CON_FB] = false;
	}
	//3#输出接触器反馈故障
	if (MB_LGA.MB_IO_IN.outCon3N != MB_LGA.MB_IO_OUT.outCon3N)
	{
		FAULT.FaultMask[WN_OUT3_CON_FB] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_OUT3_CON_FB] = false;
	}
	//4#输出接触器反馈故障
	if (MB_LGA.MB_IO_IN.outCon4N != MB_LGA.MB_IO_OUT.outCon4N)
	{
		FAULT.FaultMask[WN_OUT4_CON_FB] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_OUT4_CON_FB] = false;
	}
	//5#输出接触器反馈故障
	if (MB_LGA.MB_IO_IN.outCon5N != MB_LGA.MB_IO_OUT.outCon5N)
	{
		FAULT.FaultMask[WN_OUT5_CON_FB] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_OUT5_CON_FB] = false;
	}
	//1#交流电压传感器故障
	if (MB_LGA.MB_AN_VAL.Ch1_AC > 20000
		|| (MB_LGA.MB_SYS_INFO.busConnect && (MB_LGA.MB_AN_VAL.Ch1_AC < 500) 
	    && (MB_LGA.MB_AN_VAL.BUS_VOLT > 1000)))
	{
		FAULT.FaultMask[WN_AC1_U_FAIL] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_AC1_U_FAIL] = false;
	}
	//2#交流电压传感器故障
	if (MB_LGA.MB_AN_VAL.Ch2_AC > 20000
		|| (MB_LGA.MB_SYS_INFO.busConnect && (MB_LGA.MB_AN_VAL.Ch2_AC < 500) 
	    && (MB_LGA.MB_AN_VAL.BUS_VOLT > 1000)))
	{
		FAULT.FaultMask[WN_AC2_U_FAIL] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_AC2_U_FAIL] = false;
	}
	//母线电压传感器故障
	if (MB_LGA.MB_AN_VAL.BUS_VOLT > 20000
		|| (MB_LGA.MB_SYS_INFO.busConnect && (MB_LGA.MB_AN_VAL.BUS_VOLT < 500) 
	    && (MB_LGA.MB_AN_VAL.Ch1_AC > 1000)))
	{
		FAULT.FaultMask[WN_BUS_U_FAIL] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_BUS_U_FAIL] = false;
	}
	
	
	
	
	//1#交流输入欠压
	if (MB_LGA.MB_AN_VAL.Ch1_AC < MB_LGA.MB_Para.ACinVoltUnderW)
	{
		FAULT.FaultMask[WN_AC1_U_UNDER] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_AC1_U_UNDER] = false;
	}
	//2#交流输入欠压
	if (MB_LGA.MB_AN_VAL.Ch2_AC < MB_LGA.MB_Para.ACinVoltUnderW)
	{
		FAULT.FaultMask[WN_AC2_U_UNDER] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_AC2_U_UNDER] = false;
	}
	//控制参数异常
	
	//校准参数异常
	
	//1#斩波模块启动失败
	if (MB_LGA.cb1_SYS.output.Pwm_On_Fail == true)
	{
		FAULT.FaultMask[WN_OUT1_ON_FAIL] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_OUT1_ON_FAIL] = false;
	}
	//2#斩波模块启动失败
	if (MB_LGA.cb2_SYS.output.Pwm_On_Fail == true)
	{
		FAULT.FaultMask[WN_OUT2_ON_FAIL] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_OUT2_ON_FAIL] = false;
	}
	//3#斩波模块启动失败
	if (MB_LGA.cb3_SYS.output.Pwm_On_Fail == true)
	{
		FAULT.FaultMask[WN_OUT3_ON_FAIL] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_OUT3_ON_FAIL] = false;
	}
	//4#斩波模块启动失败
	if (MB_LGA.cb4_SYS.output.Pwm_On_Fail == true)
	{
		FAULT.FaultMask[WN_OUT4_ON_FAIL] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_OUT4_ON_FAIL] = false;
	}
	//5#斩波模块启动失败
	if (MB_LGA.cb5_SYS.output.Pwm_On_Fail == true)
	{
		FAULT.FaultMask[WN_OUT5_ON_FAIL] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[WN_OUT5_ON_FAIL] = false;
	}
	//1#输入熔断器故障
	if (MB_LGA.MB_IO_IN.inFuse1N == false)
	{
		FAULT.FaultMask[FT_IN_FUSE1] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[FT_IN_FUSE1] = false;
	}
	//2#输入熔断器故障
	if (MB_LGA.MB_IO_IN.inFuse2N == false)
	{
		FAULT.FaultMask[FT_IN_FUSE2] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[FT_IN_FUSE2] = false;
	}
	//1#输出熔断器故障
	if (MB_LGA.MB_IO_IN.outFuse1N == false)
	{
		FAULT.FaultMask[FT_OUT_FUSE1] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[FT_OUT_FUSE1] = false;
	}
	//2#输出熔断器故障
	if (MB_LGA.MB_IO_IN.outFuse2N == false)
	{
		FAULT.FaultMask[FT_OUT_FUSE2] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[FT_OUT_FUSE2] = false;
	}
	//3#输出熔断器故障
	if (MB_LGA.MB_IO_IN.outFuse3N == false)
	{
		FAULT.FaultMask[FT_OUT_FUSE3] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[FT_OUT_FUSE3] = false;
	}
	//4#输出熔断器故障
	if (MB_LGA.MB_IO_IN.outFuse4N == false)
	{
		FAULT.FaultMask[FT_OUT_FUSE4] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[FT_OUT_FUSE4] = false;
	}
	//5#输出熔断器故障
	if (MB_LGA.MB_IO_IN.outFuse5N == false)
	{
		FAULT.FaultMask[FT_OUT_FUSE5] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[FT_OUT_FUSE5] = false;
	}
	//1#交流输入过压
	if (MB_LGA.MB_AN_VAL.Ch1_AC >= MB_LGA.MB_Para.ACinVoltOverW)
	{
		FAULT.FaultMask[FT_AC1_U_OVER] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[FT_AC1_U_OVER] = false;
	}
	//2#交流输入过压
	if (MB_LGA.MB_AN_VAL.Ch2_AC >= MB_LGA.MB_Para.ACinVoltOverW)
	{
		FAULT.FaultMask[FT_AC2_U_OVER] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[FT_AC2_U_OVER] = false;
	}
	//母线过压
	if (MB_LGA.MB_AN_VAL.BUS_VOLT >= MB_LGA.MB_Para.busVoltOverW)
	{
		FAULT.FaultMask[FT_BUS_U_OVER] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[FT_BUS_U_OVER] = false;
	}
	//母线欠压
	if (MB_LGA.MB_AN_VAL.BUS_VOLT <= MB_LGA.MB_Para.busVoltUnderW)
	{
		FAULT.FaultMask[FT_BUS_U_UNDER] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[FT_BUS_U_UNDER] = false;
	}
	//1#斩波模块充电过压
	if (MB_LGA.MB_AN_VAL.Ch1ChargeVolt >= MB_LGA.MB_Para.CH1chargeVoltOverW)
	{
		FAULT.FaultMask[FT_OUT1_U_OVER] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[FT_OUT1_U_OVER] = false;
	}
	//2#斩波模块充电过压
	if (MB_LGA.MB_AN_VAL.Ch2ChargeVolt >= MB_LGA.MB_Para.CH2chargeVoltOverW)
	{
		FAULT.FaultMask[FT_OUT2_U_OVER] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[FT_OUT2_U_OVER] = false;
	}
	//3#斩波模块充电过压
	if (MB_LGA.MB_AN_VAL.Ch3ChargeVolt >= MB_LGA.MB_Para.CH3chargeVoltOverW)
	{
		FAULT.FaultMask[FT_OUT3_U_OVER] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[FT_OUT3_U_OVER] = false;
	}
	//4#斩波模块充电过压
	if (MB_LGA.MB_AN_VAL.Ch4ChargeVolt >= MB_LGA.MB_Para.CH4chargeVoltOverW)
	{
		FAULT.FaultMask[FT_OUT4_U_OVER] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[FT_OUT4_U_OVER] = false;
	}
	//5#斩波模块充电过压
	if (MB_LGA.MB_AN_VAL.Ch5ChargeVolt >= MB_LGA.MB_Para.CH5chargeVoltOverW)
	{
		FAULT.FaultMask[FT_OUT5_U_OVER] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[FT_OUT5_U_OVER] = false;
	}
	//1#整流模块过温
	if (MB_LGA.MB_AN_VAL.Module1_Temp >= MB_LGA.MB_Para.LxTemprOver)
	{
		FAULT.FaultMask[FT_L1_T_OVER] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[FT_L1_T_OVER] = false;
	}
	//2#整流模块过温
	if (MB_LGA.MB_AN_VAL.Module2_Temp >= MB_LGA.MB_Para.LxTemprOver)
	{
		FAULT.FaultMask[FT_L2_T_OVER] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[FT_L2_T_OVER] = false;
	}
	//1#电容模块过温
	if (MB_LGA.MB_AN_VAL.Cap1_Temp >= MB_LGA.MB_Para.CapxTemprOver)
	{
		FAULT.FaultMask[FT_CAP1_T_OVER] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[FT_CAP1_T_OVER] = false;
	}
	//2#电容模块过温
	if (MB_LGA.MB_AN_VAL.Cap2_Temp >= MB_LGA.MB_Para.CapxTemprOver)
	{
		FAULT.FaultMask[FT_CAP2_T_OVER] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[FT_CAP2_T_OVER] = false;
	}
	//1#输出电压传感器故障
	if (!MB_LGA.MB_SYS_INFO.AdFault && MB_LGA.MB_AN_VAL.Ch1ChargeVolt > CF_CHARGE_U_ERR)
	{
		FAULT.FaultMask[FT_OUT1_U_FAIL] = FAULT_MASK;
	}
	else if (MB_LGA.cb1_SYS.input.Charging 
		&& (MB_LGA.MB_AN_VAL.Ch1ChargeVolt > CF_CHARGE_U_OK_L) 
	    && (MB_LGA.MB_AN_VAL.Ch1ChargeVolt < CF_CHARGE_U_OK_H))
	{
		FAULT.FaultMask[FT_OUT1_U_FAIL] = false;
	}
	//2#输出电压传感器故障
	if (!MB_LGA.MB_SYS_INFO.AdFault && MB_LGA.MB_AN_VAL.Ch2ChargeVolt > CF_CHARGE_U_ERR)
	{
		FAULT.FaultMask[FT_OUT2_U_FAIL] = FAULT_MASK;
	}
	else if (MB_LGA.cb2_SYS.input.Charging 
		&& (MB_LGA.MB_AN_VAL.Ch2ChargeVolt > CF_CHARGE_U_OK_L) 
	    && (MB_LGA.MB_AN_VAL.Ch2ChargeVolt < CF_CHARGE_U_OK_H))
	{
		FAULT.FaultMask[FT_OUT2_U_FAIL] = false;
	}
	//3#输出电压传感器故障
	if (!MB_LGA.MB_SYS_INFO.AdFault && MB_LGA.MB_AN_VAL.Ch3ChargeVolt > CF_CHARGE_U_ERR)
	{
		FAULT.FaultMask[FT_OUT3_U_FAIL] = FAULT_MASK;
	}
	else if (MB_LGA.cb3_SYS.input.Charging 
		&& (MB_LGA.MB_AN_VAL.Ch3ChargeVolt > CF_CHARGE_U_OK_L) 
	    && (MB_LGA.MB_AN_VAL.Ch3ChargeVolt < CF_CHARGE_U_OK_H))
	{
		FAULT.FaultMask[FT_OUT3_U_FAIL] = false;
	}
	//4#输出电压传感器故障
	if (!MB_LGA.MB_SYS_INFO.AdFault && MB_LGA.MB_AN_VAL.Ch4ChargeVolt > CF_CHARGE_U_ERR)
	{
		FAULT.FaultMask[FT_OUT4_U_FAIL] = FAULT_MASK;
	}
	else if (MB_LGA.cb4_SYS.input.Charging 
		&& (MB_LGA.MB_AN_VAL.Ch4ChargeVolt > CF_CHARGE_U_OK_L) 
	    && (MB_LGA.MB_AN_VAL.Ch4ChargeVolt < CF_CHARGE_U_OK_H))
	{
		FAULT.FaultMask[FT_OUT4_U_FAIL] = false;
	}
	//5#输出电压传感器故障
	if (!MB_LGA.MB_SYS_INFO.AdFault && MB_LGA.MB_AN_VAL.Ch5ChargeVolt > CF_CHARGE_U_ERR)
	{
		FAULT.FaultMask[FT_OUT5_U_FAIL] = FAULT_MASK;
	}
	else if (MB_LGA.cb5_SYS.input.Charging 
		&& (MB_LGA.MB_AN_VAL.Ch5ChargeVolt > CF_CHARGE_U_OK_L) 
	    && (MB_LGA.MB_AN_VAL.Ch5ChargeVolt < CF_CHARGE_U_OK_H))
	{
		FAULT.FaultMask[FT_OUT5_U_FAIL] = false;
	}

	if (!MB_LGA.MB_SYS_INFO.Charging && !MB_LGA.MB_SYS_INFO.busConnect)
	{
		//1#整流模块电流传感器故障
		if (MB_LGA.MB_AN_VAL.Ch1_In_Curent > 1000)
		{
			FAULT.FaultMask[FT_L1_I_FAIL] = FAULT_MASK;
		}
		else if (MB_LGA.MB_AN_VAL.Ch1_In_Curent < 200)
		{
			FAULT.FaultMask[FT_L1_I_FAIL] = false;
		}
		//2#整流模块电流传感器故障
		if (MB_LGA.MB_AN_VAL.Ch2_In_Curent > 1000)
		{
			FAULT.FaultMask[FT_L2_I_FAIL] = FAULT_MASK;
		}
		else if (MB_LGA.MB_AN_VAL.Ch2_In_Curent < 200)
		{
			FAULT.FaultMask[FT_L2_I_FAIL] = false;
		}
	}
	
	
	//1#整流模块电流过流
	if (MB_LGA.MB_SYS_INFO.busVoltOK  && (MB_LGA.MB_AN_VAL.Ch1_In_Curent > MB_LGA.MB_Para.ACinCurrOverW))
	{
		FAULT.FaultMask[FT_L1_I_OVER] = FAULT_MASK;
	}
	else if (MB_LGA.MB_SYS_INFO.busVoltOK  && (MB_LGA.MB_AN_VAL.Ch1_In_Curent < MB_LGA.MB_Para.ACinCurrOverW))
	{
		FAULT.FaultMask[FT_L1_I_OVER] = false;
	}
	//2#整流模块电流过流
	if (MB_LGA.MB_SYS_INFO.busVoltOK  && (MB_LGA.MB_AN_VAL.Ch2_In_Curent > MB_LGA.MB_Para.ACinCurrOverW))
	{
		FAULT.FaultMask[FT_L2_I_OVER] = FAULT_MASK;
	}
	else if (MB_LGA.MB_SYS_INFO.busVoltOK  && (MB_LGA.MB_AN_VAL.Ch2_In_Curent < MB_LGA.MB_Para.ACinCurrOverW))
	{
		FAULT.FaultMask[FT_L2_I_OVER] = false;
	}
	//1#斩波模块关断失败
	if (MB_LGA.cb1_SYS.output.Pwm_Off_Fail == true)
	{
		FAULT.FaultMask[FT_OUT1_OFF_FAIL] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[FT_OUT1_OFF_FAIL] = false;
	}
	//2#斩波模块关断失败
	if (MB_LGA.cb2_SYS.output.Pwm_Off_Fail == true)
	{
		FAULT.FaultMask[FT_OUT2_OFF_FAIL] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[FT_OUT2_OFF_FAIL] = false;
	}
	//3#斩波模块关断失败
	if (MB_LGA.cb3_SYS.output.Pwm_Off_Fail == true)
	{
		FAULT.FaultMask[FT_OUT3_OFF_FAIL] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[FT_OUT3_OFF_FAIL] = false;
	}
	//4#斩波模块关断失败
	if (MB_LGA.cb4_SYS.output.Pwm_Off_Fail == true)
	{
		FAULT.FaultMask[FT_OUT4_OFF_FAIL] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[FT_OUT4_OFF_FAIL] = false;
	}
	//5#斩波模块关断失败
	if (MB_LGA.cb5_SYS.output.Pwm_Off_Fail == true)
	{
		FAULT.FaultMask[FT_OUT5_OFF_FAIL] = FAULT_MASK;
	}
	else
	{
		FAULT.FaultMask[FT_OUT5_OFF_FAIL] = false;
	}
	
}

void fault_Run(void)
{
	uint8_t i;
	uint8_t temp1;
	uint8_t temp2;
	
	for (i=0; i<FT_MAX; i++)
	{
		temp1 = i/16;
		temp2 = i%16;
		if (FAULT.FaultMask[i] == FAULT_MASK)
		{
			FAULT.FaultDisTime[i] = DisTime_default[i] * 100;
			if (!(FAULT.FaultAry[temp1] & (0x0001 << temp2)))
			{
				if (FAULT.FaultConTime[i] < ConTime_default[i] * 100)
				{
					FAULT.FaultConTime[i]++;
				}
				else
				{
					FAULT.FaultAry[temp1] |= 1 << temp2;
					if (temp1 >= 2)
					{
						FAULT.FaultRcdFlg[temp1 - 2] |= 0x0001 << temp2;
						FAULT.count++;
					}
				}
			}
		}
		else
		{
			FAULT.FaultConTime[i] = 0;
			if (FAULT.FaultAry[temp1] & (1 << temp2))
			{
				if (FAULT.FaultDisTime[i] > 0)
				{
					FAULT.FaultDisTime[i] --;
				}
				else
				{
					FAULT.FaultAry[temp1] &= ~(0x0001 << temp2);
				}
			}
		}
	}
	
	if (FAULT.FaultAry[2] == 0 && FAULT.FaultAry[3] == 0 && FAULT.FaultAry[4] == 0)
	{
		MB_LGA.MB_SYS_INFO.sFault = false;
	}
	else
	{
		MB_LGA.MB_SYS_INFO.sFault = true;
	}
}

void FaultWrite(void)
{
//	ErrInfo_w.Info_type = FLASH_WRITE_ERR;
//	ErrInfo_w.Number = FAULT.count;
//	_FlashWrite(&ErrInfo_w);
}


