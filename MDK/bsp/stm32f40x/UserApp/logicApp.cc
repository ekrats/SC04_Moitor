#define _DS_LGA_

#include "logicApp.h"
#include "stm32f4xx.h"
#include <rtthread.h>
#include "MB_DataStruct.h"
#include <string.h>
#include "canApp.h"
#include "adApp.h"
#include "appCharge.h"
#include "flash_storage_at45dbxx.h"
#include "appMonitor.h"
#include "appModule.h"
#include "app_Fault.h"
#include "sysInfo.h"

/*******************************************************************************
* Function Name  : DigitalInputUpdate
* Description    : 数字量输入更新
*                   
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void DigitalInputUpdate(void)
{
	//--------------------------------------------------------------
	// 数字量输入数据更新
	//--------------------------------------------------------------
	MB_LGA.MB_IO_IN.cabinetEnN      =  PDIO(IO_IN)->Chn01; //常开触点
	MB_LGA.MB_IO_IN.inFuse1N		=  PDIO(IO_IN)->Chn02; //常开触点
	MB_LGA.MB_IO_IN.inFuse2N		=  PDIO(IO_IN)->Chn03; //常开触点
	MB_LGA.MB_IO_IN.outFuse1N		=  PDIO(IO_IN)->Chn04; //常开触点
	MB_LGA.MB_IO_IN.outFuse2N		=  PDIO(IO_IN)->Chn05; //常开触点
	MB_LGA.MB_IO_IN.outFuse3N		=  PDIO(IO_IN)->Chn06; //常开触点
	MB_LGA.MB_IO_IN.outFuse4N		=  PDIO(IO_IN)->Chn07; //常开触点
	MB_LGA.MB_IO_IN.outFuse5N		=  PDIO(IO_IN)->Chn08; //常开触点
	MB_LGA.MB_IO_IN.preConN		=  PDIO(IO_IN)->Chn14; //常开触点
	MB_LGA.MB_IO_IN.mainCon1N		=  PDIO(IO_IN)->Chn15; //常开触点
	MB_LGA.MB_IO_IN.mainCon2N		=  PDIO(IO_IN)->Chn16; //常开触点
	MB_LGA.MB_IO_IN.outCon1N		=  PDIO(IO_IN)->Chn19; //常开触点
	MB_LGA.MB_IO_IN.outCon2N		=  PDIO(IO_IN)->Chn20; //常开触点
	MB_LGA.MB_IO_IN.outCon3N		=  PDIO(IO_IN)->Chn21; //常开触点
	MB_LGA.MB_IO_IN.outCon4N		=  PDIO(IO_IN)->Chn22; //常开触点
	MB_LGA.MB_IO_IN.outCon5N		=  PDIO(IO_IN)->Chn23; //常开触点
}

/*******************************************************************************
* Function Name  : AnalogInputUpdate
* Description    : 模拟量输入更新
*                   
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void AnalogInputUpdate(void)
{	
	//--------------------------------------------------------------
	// 模拟量:电流、电压、温度数据更新
	//--------------------------------------------------------------
	MB_LGA.MB_AN_VAL.Ch1_AC = AD.Ch1_AC;
	MB_LGA.MB_AN_VAL.Ch2_AC = AD.Ch2_AC;
	MB_LGA.MB_AN_VAL.BUS_VOLT = AD.BUS_VOLT;
	MB_LGA.MB_AN_VAL.Ch1_In_Curent = AD.Ch1_In_Curent;
	MB_LGA.MB_AN_VAL.Ch2_In_Curent = AD.Ch2_In_Curent;
	MB_LGA.MB_AN_VAL.Module1_Temp = AD.Module1_Temp;
	MB_LGA.MB_AN_VAL.Module2_Temp = AD.Module2_Temp;
	MB_LGA.MB_AN_VAL.FanIn_Temp = AD.FanIn_Temp;
	MB_LGA.MB_AN_VAL.FanOut_Temp = AD.FanOut_Temp;
	MB_LGA.MB_AN_VAL.Cap1_Temp = AD.Cap1_Temp;
	MB_LGA.MB_AN_VAL.Cap2_Temp = AD.Cap2_Temp;
	//----------------------------------------------------
	// 监控板采集数据
	//----------------------------------------------------	
}
/*******************************************************************************
* Function Name  : DigitalOutputUpdate
* Description    : 数字量输出更新
*                   
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void DigitalOutputUpdate(void)
{	
	//--------------------------------------------------------------
	// 数字量输出数据更新
	//--------------------------------------------------------------
	PDIO(IO_OUT)->Chn01 = MB_LGA.MB_IO_OUT.outCon1N;
	PDIO(IO_OUT)->Chn02 = MB_LGA.MB_IO_OUT.outCon2N;
	PDIO(IO_OUT)->Chn03 = MB_LGA.MB_IO_OUT.outCon3N;
	PDIO(IO_OUT)->Chn04 = MB_LGA.MB_IO_OUT.outCon4N;
	PDIO(IO_OUT)->Chn05 = MB_LGA.MB_IO_OUT.outCon5N;
	
	PDIO(IO_OUT)->Chn07 = MB_LGA.MB_IO_OUT.preConN;
	PDIO(IO_OUT)->Chn08 = MB_LGA.MB_IO_OUT.mainCon1N;
	PDIO(IO_OUT)->Chn09 = MB_LGA.MB_IO_OUT.mainCon2N;

}
/*******************************************************************************
* Function Name  : CanInputUpdate
* Description    : Can输入(接收)更新
*                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CanInputUpdate(void)
{	
	//--------------------------------------------------------------
	// Can输入(接收)数据更新
	//--------------------------------------------------------------
	
	MB_LGA.cb1Status = can.AppBuf.Cb1Status;
	MB_LGA.cb2Status = can.AppBuf.Cb2Status;
	MB_LGA.cb3Status = can.AppBuf.Cb3Status;
	MB_LGA.cb4Status = can.AppBuf.Cb4Status;
	MB_LGA.cb5Status = can.AppBuf.Cb5Status;
	
	MB_LGA.Modue1_Para = can.AppBuf.Cb1Para;
	MB_LGA.Modue2_Para = can.AppBuf.Cb2Para;
	MB_LGA.Modue3_Para = can.AppBuf.Cb3Para;
	MB_LGA.Modue4_Para = can.AppBuf.Cb4Para;
	MB_LGA.Modue5_Para = can.AppBuf.Cb5Para;
	
	MB_LGA.MB_AN_VAL.Ch1ChargeVolt = can.AppBuf.AD_RxChnn.Ch1ChargeVolt;
	MB_LGA.MB_AN_VAL.Ch2ChargeVolt = can.AppBuf.AD_RxChnn.Ch2ChargeVolt;
	MB_LGA.MB_AN_VAL.Ch3ChargeVolt = can.AppBuf.AD_RxChnn.Ch3ChargeVolt;
	MB_LGA.MB_AN_VAL.Ch4ChargeVolt = can.AppBuf.AD_RxChnn.Ch4ChargeVolt;
	MB_LGA.MB_AN_VAL.Ch5ChargeVolt = can.AppBuf.AD_RxChnn.Ch5ChargeVolt;
}
/*******************************************************************************
* Function Name  : HmiINputUpdate
* Description    : HMI输入(接收)更新
*                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void HmiInputUpdate(void)
{
	uint8_t i;
	uint16_t	HMI_Cmd1;
	uint16_t	HMI_Cmd2;
	
	for (i=0; i<HMI_ADDR_W_NUM; i++)
	{
		if (HMI.HMI_WAddr_flag[i])
			break;
	}
	
	if (i==ADDR_CHARGE_EN)
	{
		HMI_Cmd1 = HMI.CtrCmd.ChargeEn.CMD1;
		HMI_Cmd2 = HMI.CtrCmd.ChargeEn.CMD2;
		switch (HMI_Cmd1){
			case MODULE1:
				if (HMI_Cmd2 == 1)
				{
					MB_LGA.cb1_SYS.input.Charge_Enable = true;
				}
				else if (HMI_Cmd2 == 2)
				{
					MB_LGA.cb1_SYS.input.Charge_Enable = false;
				}
				break;
			case MODULE2:
				if (HMI_Cmd2 == 1)
				{
					MB_LGA.cb2_SYS.input.Charge_Enable = true;
				}
				else if (HMI_Cmd2 == 2)
				{
					MB_LGA.cb2_SYS.input.Charge_Enable = false;
				}
				break;
			case MODULE3:
				if (HMI_Cmd2 == 1)
				{
					MB_LGA.cb3_SYS.input.Charge_Enable = true;
				}
				else if (HMI_Cmd2 == 2)
				{
					MB_LGA.cb3_SYS.input.Charge_Enable = false;
				}
				break;
			case MODULE4:
				if (HMI_Cmd2 == 1)
				{
					MB_LGA.cb4_SYS.input.Charge_Enable = true;
				}
				else if (HMI_Cmd2 == 2)
				{
					MB_LGA.cb4_SYS.input.Charge_Enable = false;
				}
				break;
			case MODULE5:
				if (HMI_Cmd2 == 1)
				{
					MB_LGA.cb5_SYS.input.Charge_Enable = true;
				}
				else if (HMI_Cmd2 == 2)
				{
					MB_LGA.cb5_SYS.input.Charge_Enable = false;
				}
				break;
			default:
				break;
				
		}
		HMI.HMI_WAddr_flag[i] = false;
	}
	else if (i==ADDR_CHARGE_CMD)
	{
		MB_LGA.MB_SYS_INFO.SysModeSet = CHARGE;
		HMI_Cmd1 = HMI.CtrCmd.ChargeCmd.CMD1;
		HMI_Cmd2 = HMI.CtrCmd.ChargeCmd.CMD2;
		switch (HMI_Cmd1){
			case MODULE1:
				MB_LGA.cb1_SYS.input.Charge_Mode_Set = HMI_Cmd2;
				break;
			case MODULE2:
				MB_LGA.cb2_SYS.input.Charge_Mode_Set = HMI_Cmd2;
				break;
			case MODULE3:
				MB_LGA.cb3_SYS.input.Charge_Mode_Set = HMI_Cmd2;
				break;
			case MODULE4:
				MB_LGA.cb4_SYS.input.Charge_Mode_Set = HMI_Cmd2;
				break;
			case MODULE5:
				MB_LGA.cb5_SYS.input.Charge_Mode_Set = HMI_Cmd2;
				break;
			default:
				break;
				
		}
		HMI.HMI_WAddr_flag[i] = false;
	}
		
	else if (i==ADDR_DEV_TEST)
	{
		MB_LGA.MB_SYS_INFO.SysModeSet = DEVICE_TEST;
		MB_LGA.CtrCmd.DevTestCmd = HMI.CtrCmd.DevTestCmd;
		HMI.HMI_WAddr_flag[i] = false;
	}
	
	else if (i==ADDR_SLEEP)
	{
		MB_LGA.MB_SYS_INFO.SysModeSet = SLEEP;
		MB_LGA.CtrCmd.SleepCmd = HMI.CtrCmd.SleepCmd;
		HMI.HMI_WAddr_flag[i] = false;
	}
	
	else if (i==ADDR_RESTART)
	{
		//发送停止充电指令
		
		//给控制板发送重启指令
		can.SendCan(Reboot);
		
		//延时
		
		//监控板重启
		HMI.HMI_WAddr_flag[i] = false;
	}
	
	else if (i==ADDR_RESTORE)
	{
		HMI.HMI_WAddr_flag[i] = false;
	}
	
	else if (i==ADDR_W_MB_PARA)
	{
		MB_LGA.MB_WrPara = HMI.mbWrPara;
		//HMI.HMI_WAddr_flag[i] = false;
	}
		
	else if (i==ADDR_W_MB_RTC)	
	{
		//监控板时间设置
		HMI.HMI_WAddr_flag[i] = false;
	}
}
/*******************************************************************************
* Function Name  : MonitorUpdate
* Description    : 监控板状态数据更新
*                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MonitorUpdate(void)
{
	HMI_Status_STYP *p = (HMI_Status_STYP *)&MB_LGA.mbStatus;
	
	MB_LGA.cb1_SYS.input.Charging = MB_LGA.cb1Status.flags.chargeOnN;
	MB_LGA.cb1_SYS.input.floatChargeN = MB_LGA.cb1Status.flags.floatChargeN;
	MB_LGA.cb1_SYS.input.Fault = MB_LGA.cb1Status.flags.sysFailN;
	MB_LGA.cb1_SYS.input.Instation = MB_LGA.MB_AN_VAL.Ch1ChargeVolt > CHARGE_STARTV ? true : false;
	
	MB_LGA.cb2_SYS.input.Charging = MB_LGA.cb2Status.flags.chargeOnN;
	MB_LGA.cb2_SYS.input.floatChargeN = MB_LGA.cb2Status.flags.floatChargeN;
	MB_LGA.cb2_SYS.input.Fault = MB_LGA.cb2Status.flags.sysFailN;
	MB_LGA.cb2_SYS.input.Instation = MB_LGA.MB_AN_VAL.Ch2ChargeVolt > CHARGE_STARTV ? true : false;
	
	MB_LGA.cb3_SYS.input.Charging = MB_LGA.cb3Status.flags.chargeOnN;
	MB_LGA.cb3_SYS.input.floatChargeN = MB_LGA.cb3Status.flags.floatChargeN;
	MB_LGA.cb3_SYS.input.Fault = MB_LGA.cb3Status.flags.sysFailN;
	MB_LGA.cb3_SYS.input.Instation = MB_LGA.MB_AN_VAL.Ch3ChargeVolt > CHARGE_STARTV ? true : false;
	
	MB_LGA.cb4_SYS.input.Charging = MB_LGA.cb4Status.flags.chargeOnN;
	MB_LGA.cb4_SYS.input.floatChargeN = MB_LGA.cb4Status.flags.floatChargeN;
	MB_LGA.cb4_SYS.input.Fault = MB_LGA.cb4Status.flags.sysFailN;
	MB_LGA.cb4_SYS.input.Instation = MB_LGA.MB_AN_VAL.Ch4ChargeVolt > CHARGE_STARTV ? true : false;
	
	MB_LGA.cb5_SYS.input.Charging = MB_LGA.cb5Status.flags.chargeOnN;
	MB_LGA.cb5_SYS.input.floatChargeN = MB_LGA.cb5Status.flags.floatChargeN;
	MB_LGA.cb5_SYS.input.Fault = MB_LGA.cb5Status.flags.sysFailN;
	MB_LGA.cb5_SYS.input.Instation = MB_LGA.MB_AN_VAL.Ch5ChargeVolt > CHARGE_STARTV ? true : false;
	
	p->sys_mode = SysMode.SystemState / 0x100;
	p->dcdc1_mode = MB_LGA.cb1_SYS.output.Charge_Mode_Now / 0x10;
	p->dcdc2_mode = MB_LGA.cb1_SYS.output.Charge_Mode_Now / 0x10;
	p->dcdc3_mode = MB_LGA.cb1_SYS.output.Charge_Mode_Now / 0x10;
	p->dcdc4_mode = MB_LGA.cb1_SYS.output.Charge_Mode_Now / 0x10;
	p->dcdc5_mode = MB_LGA.cb1_SYS.output.Charge_Mode_Now / 0x10;
	
	//待机、恒流
	p->dcdc1_step = MB_LGA.cb1Status.flags.chargeOnN + MB_LGA.cb1Status.flags.floatChargeN;
	p->dcdc2_step = MB_LGA.cb2Status.flags.chargeOnN + MB_LGA.cb1Status.flags.floatChargeN;
	p->dcdc3_step = MB_LGA.cb3Status.flags.chargeOnN + MB_LGA.cb1Status.flags.floatChargeN;
	p->dcdc4_step = MB_LGA.cb4Status.flags.chargeOnN + MB_LGA.cb1Status.flags.floatChargeN;
	p->dcdc5_step = MB_LGA.cb5Status.flags.chargeOnN + MB_LGA.cb1Status.flags.floatChargeN;
	
	p->In_Flags.cabinetEnN = MB_LGA.MB_IO_IN.cabinetEnN;
	p->In_Flags.inFuse1N = MB_LGA.MB_IO_IN.inFuse1N;
	p->In_Flags.inFuse2N = MB_LGA.MB_IO_IN.inFuse2N;
	p->In_Flags.outFuse1N = MB_LGA.MB_IO_IN.outFuse1N;
	p->In_Flags.outFuse2N = MB_LGA.MB_IO_IN.outFuse2N;
	p->In_Flags.outFuse3N = MB_LGA.MB_IO_IN.outFuse3N;
	p->In_Flags.outFuse4N = MB_LGA.MB_IO_IN.outFuse4N;
	p->In_Flags.outFuse5N = MB_LGA.MB_IO_IN.outFuse5N;
	p->In_Flags.mainCon1N = MB_LGA.MB_IO_IN.mainCon1N;
	p->In_Flags.mainCon2N = MB_LGA.MB_IO_IN.mainCon2N;
	p->In_Flags.outCon1N = MB_LGA.MB_IO_IN.outCon1N;
	p->In_Flags.outCon2N = MB_LGA.MB_IO_IN.outCon2N;
	p->In_Flags.outCon3N = MB_LGA.MB_IO_IN.outCon3N;
	p->In_Flags.outCon4N = MB_LGA.MB_IO_IN.outCon4N;
	p->In_Flags.outCon5N = MB_LGA.MB_IO_IN.outCon5N;
	
	p->Run_Flags.AdLost = MB_LGA.MB_SYS_INFO.AdFault;
	p->Run_Flags.canLostN = MB_LGA.MB_SYS_INFO.CanFault;
	p->Run_Flags.rail1FailN = (p->Run_Flags.canLostN || ((~can.CanStateWord) & 0x0001));
	p->Run_Flags.rail2FailN = (p->Run_Flags.canLostN || ((~can.CanStateWord) & 0x0002));
	p->Run_Flags.rail3FailN = (p->Run_Flags.canLostN || ((~can.CanStateWord) & 0x0004));
	p->Run_Flags.rail4FailN = (p->Run_Flags.canLostN || ((~can.CanStateWord) & 0x0008));
	p->Run_Flags.rail5FailN = (p->Run_Flags.canLostN || ((~can.CanStateWord) & 0x0010));
	p->In1_ACVolt = MB_LGA.MB_AN_VAL.Ch1_AC;
	p->In2_ACVolt = MB_LGA.MB_AN_VAL.Ch2_AC;
	p->BusVolt = MB_LGA.MB_AN_VAL.BUS_VOLT;
	p->DC1_OutVolt = MB_LGA.MB_AN_VAL.Ch1ChargeVolt;
	p->DC2_OutVolt = MB_LGA.MB_AN_VAL.Ch2ChargeVolt;
	p->DC3_OutVolt = MB_LGA.MB_AN_VAL.Ch3ChargeVolt;
	p->DC4_OutVolt = MB_LGA.MB_AN_VAL.Ch4ChargeVolt;
	p->DC5_OutVolt = MB_LGA.MB_AN_VAL.Ch5ChargeVolt;
	p->In1_Curr = MB_LGA.MB_AN_VAL.Ch1_In_Curent;
	p->In2_Curr = MB_LGA.MB_AN_VAL.Ch2_In_Curent;
	p->Modue1_Temp = MB_LGA.MB_AN_VAL.Module1_Temp;
	p->Modue2_Temp = MB_LGA.MB_AN_VAL.Module2_Temp;
	p->FanIn_Temp = MB_LGA.MB_AN_VAL.FanIn_Temp;
	p->FanOut_Temp = MB_LGA.MB_AN_VAL.FanOut_Temp;
	p->Cap1_Temp = MB_LGA.MB_AN_VAL.Cap1_Temp;
	p->Cap2_Temp = MB_LGA.MB_AN_VAL.Cap2_Temp;
	
	p->Time.year = TIME_RTC.Year;
	p->Time.month = TIME_RTC.Month;
	p->Time.day = TIME_RTC.Day;
	p->Time.hour = TIME_RTC.Hour;
	p->Time.min = TIME_RTC.Min;
	p->Time.sec = TIME_RTC.Sec;
	
	p->Warn1 = FAULT.FaultAry[0];
	p->Warn2 = FAULT.FaultAry[1];
	p->Fault1 = FAULT.FaultAry[2];
	p->Fault2 = FAULT.FaultAry[3];
	p->Fault3 = FAULT.FaultAry[4];
	
	//p->chargeCntW
	//p->Event = 
	
	p->cb1Status = MB_LGA.cb1Status;
	p->cb2Status = MB_LGA.cb2Status;
	p->cb3Status = MB_LGA.cb3Status;
	p->cb4Status = MB_LGA.cb4Status;
	p->cb5Status = MB_LGA.cb5Status;
}

/*******************************************************************************
* Function Name  : FaultManage
* Description    : 故障保护逻辑
*                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FaultManage(void)
{
	FaultAnalyse();
	fault_Run();
	sysInfo.sysInfoRecord();
}
/*******************************************************************************
* Function Name  : CanOutputUpdate
* Description    : Can输出(发送)更新
*                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CanOutputUpdate(void)
{
	can.AppBuf.Cb1WPara = MB_LGA.Modue1_WPara;
	can.AppBuf.Cb2WPara = MB_LGA.Modue2_WPara;
	can.AppBuf.Cb3WPara = MB_LGA.Modue3_WPara;
	can.AppBuf.Cb4WPara = MB_LGA.Modue4_WPara;
	can.AppBuf.Cb5WPara = MB_LGA.Modue5_WPara;
	
	can.AppBuf.Cb1ChargeCmd = MB_LGA.Modue1_ChargeCmd;
	can.AppBuf.Cb2ChargeCmd = MB_LGA.Modue2_ChargeCmd;
	can.AppBuf.Cb3ChargeCmd = MB_LGA.Modue3_ChargeCmd;
	can.AppBuf.Cb4ChargeCmd = MB_LGA.Modue4_ChargeCmd;
	can.AppBuf.Cb5ChargeCmd = MB_LGA.Modue5_ChargeCmd;
}

/*******************************************************************************
* Function Name  : HmiOutputUpdate
* Description    : HMI输出(接收)更新
*                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HmiOutputUpdate(void)
{
//	HMI.mbStatus = MB_LGA.mbStatus;
//	HMI.mbRdPara = MB_LGA.MB_RdPara;
//	HMI.Modue1_Para = MB_LGA.Modue1_Para;
//	HMI.Modue2_Para = MB_LGA.Modue2_Para;
//	HMI.Modue3_Para = MB_LGA.Modue3_Para;
//	HMI.Modue4_Para = MB_LGA.Modue4_Para;
//	HMI.Modue5_Para = MB_LGA.Modue5_Para;
}

void ParManage(void)
{
	uint8_t i;
	static uint8_t cnt1 = 0;
	static uint8_t cnt2 = 0;
	static uint8_t cnt3 = 0;
	static uint8_t cnt4 = 0;
	static uint8_t cnt5 = 0;
	
	for (i=ADDR_W_MB_PARA; i<HMI_ADDR_W_NUM-1; i++)
	{
		if (HMI.HMI_WAddr_flag[i])
			break;
	}
	switch (i)
	{
		case ADDR_W_MB_PARA:
			if(memcmp((void *)(&MB_LGA.MB_WrPara), (void *)(&MB_LGA.MB_RdPara), sizeof(MB_Para_STYP)) != 0)
			{
				//参数更新
				MB_LGA.MB_RdPara = MB_LGA.MB_WrPara;
				MB_LGA.MB_Para = MB_LGA.MB_WrPara;
				//参数写入Flash标志位置1
				MB_LGA.MB_SYS_INFO.wParFlag = true;
				HMI.HMI_WAddr_flag[i] = false;
			}
			break;
		
		case ADDR_W_CB1_PARA:
			if(memcmp((void *)(&HMI.Modue1_WPara), (void *)(&MB_LGA.Modue1_Para), sizeof(MB_Para_STYP)) != 0)
			{
				//参数更新
				MB_LGA.Modue1_WPara = HMI.Modue1_WPara;
				can.AppBuf.Cb1WPara = MB_LGA.Modue1_WPara;
				if (cnt1 == 0)
				{
					can.SendCan(Module1_Set_Par);
					cnt1++;
				}
				else
				{
					can.SendCan(Module1_Read_Par);
					HMI.HMI_WAddr_flag[i] = false;
					cnt1 = 0;
				}
			}
			break;
			
		case ADDR_W_CB2_PARA:	
			if(memcmp((void *)(&HMI.Modue2_WPara), (void *)(&MB_LGA.Modue2_Para), sizeof(MB_Para_STYP)) != 0)
			{
				//参数更新
				MB_LGA.Modue2_WPara = HMI.Modue2_WPara;
				can.AppBuf.Cb2WPara = MB_LGA.Modue2_WPara;
				if (cnt2 == 0)
				{
					can.SendCan(Module2_Set_Par);
					cnt2++;
				}
				else
				{
					can.SendCan(Module2_Read_Par);
					HMI.HMI_WAddr_flag[i] = false;
					cnt2 = 0;
				}
			}
			break;
		
		case ADDR_W_CB3_PARA:	
			if(memcmp((void *)(&HMI.Modue3_WPara), (void *)(&MB_LGA.Modue3_Para), sizeof(MB_Para_STYP)) != 0)
			{
				//参数更新
				MB_LGA.Modue3_WPara = HMI.Modue3_WPara;
				can.AppBuf.Cb3WPara = MB_LGA.Modue3_WPara;
				if (cnt3 == 0)
				{
					can.SendCan(Module3_Set_Par);
					cnt3++;
				}
				else
				{
					can.SendCan(Module3_Read_Par);
					HMI.HMI_WAddr_flag[i] = false;
					cnt3 = 0;
				}
			}
			break;
			
		case ADDR_W_CB4_PARA:	
			if(memcmp((void *)(&HMI.Modue4_WPara), (void *)(&MB_LGA.Modue4_Para), sizeof(MB_Para_STYP)) != 0)
			{
				//参数更新
				MB_LGA.Modue4_WPara = HMI.Modue4_WPara;
				can.AppBuf.Cb4WPara = MB_LGA.Modue4_WPara;
				if (cnt4 == 0)
				{
					can.SendCan(Module4_Set_Par);
					cnt4++;
				}
				else
				{
					can.SendCan(Module4_Read_Par);
					HMI.HMI_WAddr_flag[i] = false;
					cnt4 = 0;
				}
			}
			break;
			
		case ADDR_W_CB5_PARA:	
			if(memcmp((void *)(&HMI.Modue5_WPara), (void *)(&MB_LGA.Modue5_Para), sizeof(MB_Para_STYP)) != 0)
			{
				//参数更新
				MB_LGA.Modue5_WPara = HMI.Modue5_WPara;
				can.AppBuf.Cb5WPara = MB_LGA.Modue5_WPara;
				if (cnt5 == 0)
				{
					can.SendCan(Module5_Set_Par);
					cnt5++;
				}
				else
				{
					can.SendCan(Module5_Read_Par);
					HMI.HMI_WAddr_flag[i] = false;
					cnt5 = 0;
				}
			}
			break;
			
		default:
			break;
	}
}


void sys_par_write(MB_Para_STYP *par)
{
	int bch_record_size = sizeof(MB_Para_STYP);
	
	rt_device_t fs = rt_device_find("fs");
	rt_uint8_t *tmp = (rt_uint8_t *)rt_malloc(bch_record_size);
	
	rt_memcpy(tmp, par, bch_record_size);
	rt_device_write(fs, RECORD_PAR, tmp, bch_record_size);
	
	rt_free(tmp);
}

void sys_par_read(void)
{
	MB_Para_STYP * p = (MB_Para_STYP *)&MB_LGA.MB_Para;
	int bch_record_size = sizeof(MB_Para_STYP);
	rt_uint8_t * read_buf = (rt_uint8_t *)rt_malloc(512);
	
	rt_device_t fs = rt_device_find("fs");
	rt_uint8_t *tmp = (rt_uint8_t *)rt_malloc(bch_record_size);
	
	rt_device_read(fs, (RECORD_PAR << 16) ,&read_buf[0], 512);
	
	rt_memcpy(tmp, &read_buf[8], bch_record_size);
	
	rt_memcpy(p, tmp, bch_record_size);
	
	MB_LGA.MB_WrPara = *p;
	
	rt_free(tmp);
	
}

void UserRecord(void)
{
	if (MB_LGA.MB_SYS_INFO.wParFlag)
	{
		MB_LGA.MB_SYS_INFO.wParFlag = false;
		sys_par_write(&MB_LGA.MB_Para);
	}
	else if (MB_LGA.MB_SYS_INFO.rParFlag)
	{
		MB_LGA.MB_SYS_INFO.rParFlag = false;
		sys_par_read();
	}
}

