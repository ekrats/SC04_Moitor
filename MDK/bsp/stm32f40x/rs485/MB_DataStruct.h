#ifndef	 __MB_DATASTRUCT_H__
#define	 __MB_DATASTRUCT_H__
#include "stm32f4xx.h"
//#include "config.h"
#include "Common.h"


enum {
	ADDR_CHARGE_EN,
	ADDR_CHARGE_CMD,
	ADDR_DEV_TEST,
	ADDR_SLEEP,
	ADDR_RESTART,
	ADDR_RESTORE,
	ADDR_W_MB_PARA,
	ADDR_W_CB1_PARA,
	ADDR_W_CB2_PARA,
	ADDR_W_CB3_PARA,
	ADDR_W_CB4_PARA,
	ADDR_W_CB5_PARA,
	ADDR_W_MB_RTC,
	ADDR_MAX,
};

#define HMI_ADDR_W_NUM			ADDR_MAX

//写地址
#define HMI_ADDR_CHARGE_EN		0x0240//控制命令
#define HMI_ADDR_CHARGE_CMD		0x0242//控制命令
#define HMI_ADDR_DEV_TEST		0x0244//控制命令
#define HMI_ADDR_SLEEP			0x0246//控制命令
#define HMI_ADDR_RESTART		0x0248//控制命令
#define HMI_ADDR_RESTORE		0x024A//控制命令
#define HMI_ADDR_W_MB_PARA		0x0260//设置监控板控制参数
#define HMI_ADDR_W_CB1_PARA		0x0280//设置控制板1控制参数
#define HMI_ADDR_W_CB2_PARA   	0x02A0//设置控制板2控制参数
#define HMI_ADDR_W_CB3_PARA		0x02C0//设置控制板3控制参数
#define HMI_ADDR_W_CB4_PARA   	0x02E0//设置控制板4控制参数
#define HMI_ADDR_W_CB5_PARA   	0x0300//设置控制板5控制参数
#define HMI_ADDR_W_MB_RTC   	0x0380//?????????


typedef struct{
	u16 ACinVoltUnderW;				
	u16 ACinVoltOverW;	
	
	u16 busVoltW;
	u16 busVoltUnderW;				
	u16 busVoltOverW;
	u16 CH1chargeVoltOverW;
	u16 CH2chargeVoltOverW;
	u16 CH3chargeVoltOverW;
	u16 CH4chargeVoltOverW;
	u16 CH5chargeVoltOverW;
	u16 ACinCurrUnderW;				
	u16 ACinCurrOverW;	
	u16 LxTemprOver;
	u16 CapxTemprOver;
}MB_Para_STYP;

//typedef struct{
//	u16 inVoltUnderW;				
//	u16 inVoltOverW;					
//	u16 busVoltW;					
//	u16 busVoltUnderW;				
//	u16 busVoltOverW;				
//	u16 chargeVoltOverW;			
//	u16 inCurrOverW;				
//	u16 chargeCurrOverW;			
//	u16 DxTemprOver;				
//	u16 LxTemprOver;				
//}MB_Para_STYP1;

typedef struct{
	u16 res01:  				3;
	u16 fanOnN:  				1;			//??????
	u16 lowLoadN:  			1;			//????
	u16 igbt1OverTemprN: 1;			//IGBT1????
	u16 igbt2OverTemprN: 1;			//IGBT2????
	u16 capVoltFailN:		1;			//?????????
	u16 gridCapVolFailN:	1;			//?????????
	u16 flyCapBalanceN:	1;			//????????
	u16 capVoltLowN:		1;			//??????
	u16 res02:					4;
	u16 sysFailN:				1;	
	u16 chargeOnN:			1;			//????
	u16 floatChargeN:		1;			//????(??)
	u16 chargeIFailN:		1;			//??I?????		
	u16 chargeQFailN:		1;			//??Q?????
	u16 dischargeOnN:		1;			//????
	u16 floatDischargeN:	1;			//????(??)
	u16 dischargeIFailN:	1;			//??I?????
	u16 dischargeQFailN:	1;			//??Q?????
	u16 currDirectN:			1;			//????
	u16 swOverCurrN:		1;			//???			
	u16 hwOverCurrN:		1;			//???
	u16 swGridOverVoltN:	1;			//?????		
	u16 hwGridOverVoltN:	1;			//?????
	u16 swCapOverVoltN:	1;			//?????
	u16 hwCapOverVoltN:	1;			//?????
	u16 syncFailN:			1;
}CB_Flags_STYP;

typedef struct{
	u8 CMD;
	u8 Rate;
	u8 res[6];
}Charge_Cmd_STYP, *PCharge_Cmd_STYP;

typedef struct{
	u8 res01[2];
	CB_Flags_STYP flags;
	s16 currW;								//?????
	u16 flyingCapvoltW;				//?????
	u8 igbt1Tempr;						
	u8 igbt2Tempr;
	u16 gridCapvoltW;					//?????
	u16 capvoltW;						//?????
	u8 res02[8];
}CAN_CB_Status_STYP, *PCAN_CB_Status_STYP;

typedef struct{
	u16 chargeVoltW;
	u16 chargeCurrW;
	u16 chargeVoltOverW;
	u16 chargeCurrOverW;
	u16 dischargeVoltW;
	u16 dischargeCurrW;
	u16 dischargeVoltOverW;		
	u16 dischargeCurrOverW;		
	u16 flyingCapVoltOverW;
	u8 IGBT1TemprOver;
	u8 IGBT2TemprOver;
	u8 res[4];
}CB_Para_STYP, *PCB_Para_STYP;

typedef struct{
	u16 CMD1;
	u16 CMD2;
}ChargeCmd_STYP;

typedef struct{
	u16 CMD1;
	u16 CMD2;
}ChargeEn_STYP;

typedef struct{
	ChargeEn_STYP	ChargeEn;
	ChargeCmd_STYP  ChargeCmd;
	u16				DevTestCmd;
	u16				SleepCmd;
	u16				RestartCmd;
	u16				RestoreParCmd;
}CtrCmd_STYP;

typedef struct{
	u16 year;//offset 1900
	u16 mon;
	u16 mday;
	u16 hour;
	u16 min;
	u16 sec;
}TIME_STYP;

typedef struct _CAN_AN_VAL_       //数据来源于AD板
{
	uint16_t		Ch1ChargeVolt;		//1#输出电压
	uint16_t		Ch2ChargeVolt;		//2#输出电压
	uint16_t		Ch3ChargeVolt;		//3#输出电压
	uint16_t		Ch4ChargeVolt;		//4#输出电压
	uint16_t		Ch5ChargeVolt;		//5#输出电压
	uint16_t		res[3];

}DS_CAN_AN_VAL,*PDS_CAN_AN_VAL;

typedef struct _mb_statu_
{
/* 0x000 */	uint16_t		sys_mode;
/* 0x001 */	uint16_t		dcdc1_mode;
			uint16_t		dcdc1_step;
			uint16_t		dcdc2_mode;
			uint16_t		dcdc2_step;
			uint16_t		dcdc3_mode;
			uint16_t		dcdc3_step;
			uint16_t		dcdc4_mode;
			uint16_t		dcdc4_step;
			uint16_t		dcdc5_mode;
			uint16_t		dcdc5_step;
/* 0x002 */	In_Flags_STYP	In_Flags;      
/* 0x003 */	Run_Flags_STYP	Run_Flags;
/* 0x004 */	uint16_t  		In1_ACVolt;
/* 0x005 */ uint16_t  		In2_ACVolt;
/* 0x006 */	uint16_t		BusVolt;
/* 0x007 */	uint16_t		DC1_OutVolt;
/* 0x008 */	uint16_t		DC2_OutVolt;	
/* 0x009 */	uint16_t		DC3_OutVolt;
/* 0x00A */ uint16_t		DC4_OutVolt;
/* 0x00B */ uint16_t		DC5_OutVolt;
/* 0x00C */	uint16_t		In1_Curr;
/* 0x00D */	uint16_t		In2_Curr;
/* 0x00E */	uint16_t		Modue1_Temp;
/* 0x00F */	uint16_t		Modue2_Temp;
/* 0x010 */	uint16_t		FanIn_Temp;
/* 0x011 */	uint16_t		FanOut_Temp;
/* 0x012 */	uint16_t		Cap1_Temp;
/* 0x013 */	uint16_t		Cap2_Temp;
/* 0x014-0x016 */	TIME_TYPE	Time;
/* 0x017 */ uint16_t		Warn1;
/* 0x018 */ uint16_t		Warn2;
/* 0x019 */ uint16_t		Fault1;
/* 0x01A */ uint16_t		Fault2;
/* 0x01B */ uint16_t		Fault3;
/* 0x01C */ uint16_t		Event;
/* 0x01D */ uint16_t		signalCnt;
/* 0x01E */ uint16_t		chargeCntW;
/* 0x01F */ uint16_t		logNumW;
/* 0x020~0x021 */ RUN_TIME_STYP	RunTime;
/* 0x022~0x028 */ uint16_t		res1[7];
			CAN_CB_Status_STYP cb1Status;
			CAN_CB_Status_STYP cb2Status;
			CAN_CB_Status_STYP cb3Status;
			CAN_CB_Status_STYP cb4Status;
			CAN_CB_Status_STYP cb5Status;
}HMI_Status_STYP;

typedef struct _HmiBuffApp_
{
//--------------------------------------------------------
//Monitor向触摸屏输出
//--------------------------------------------------------
	HMI_Status_STYP	mbStatus;
	MB_Para_STYP    mbRdPara;
	CB_Para_STYP	Modue1_Para;
	CB_Para_STYP	Modue2_Para;
	CB_Para_STYP	Modue3_Para;
	CB_Para_STYP	Modue4_Para;
	CB_Para_STYP	Modue5_Para;
	Version_STYP    version;
//--------------------------------------------------------
// 触摸屏向Monitor输出
//--------------------------------------------------------
	uint8_t 		HMI_WAddr_flag[HMI_ADDR_W_NUM];
	CtrCmd_STYP     CtrCmd;
	MB_Para_STYP    mbWrPara;
	CB_Para_STYP	Modue1_WPara;
	CB_Para_STYP	Modue2_WPara;
	CB_Para_STYP	Modue3_WPara;
	CB_Para_STYP	Modue4_WPara;
	CB_Para_STYP	Modue5_WPara;
	TIME_STYP       Time;
}HMI_TYPE;

typedef struct _mb_RegData
{
	
		    HMI_Status_STYP	mbStatus;
			uint16_t			res1[146];
			//uint16_t        res1[256];
	
			MB_Para_STYP    mbRdPara;
			uint16_t        res2[18];
			CB_Para_STYP	Modue1_Para;
			uint16_t        res3[20];
			CB_Para_STYP	Modue2_Para;
			uint16_t        res4[20];
			CB_Para_STYP	Modue3_Para;
			uint16_t        res5[20];
			CB_Para_STYP	Modue4_Para;
			uint16_t        res6[20];
			CB_Para_STYP	Modue5_Para;
			uint16_t        res7[20];
			CB_Para_STYP	Modue6_Para;
			uint16_t        res8[20];
			CB_Para_STYP	Modue7_Para;
			uint16_t        res9[20];
			CB_Para_STYP	Modue8_Para;
			uint16_t        res10[20];
			
			Version_STYP    version;
			uint16_t        res11[26];
//--------------------------------------------------------
// 触摸屏向DSP输出
//--------------------------------------------------------			
			CtrCmd_STYP     CtrCmd;
			uint16_t		res12[24];
			MB_Para_STYP    mbWrPara;
			uint16_t        res13[18];
			CB_Para_STYP	Modue1_WPara;
			uint16_t        res14[20];
			CB_Para_STYP	Modue2_WPara;
			uint16_t        res15[20];
			CB_Para_STYP	Modue3_WPara;
			uint16_t        res16[20];
			CB_Para_STYP	Modue4_WPara;
			uint16_t        res17[20];
			CB_Para_STYP	Modue5_WPara;
			uint16_t        res18[20];
			CB_Para_STYP	Modue6_WPara;
			uint16_t        res19[20];
			CB_Para_STYP	Modue7_WPara;
			uint16_t        res20[20];
			CB_Para_STYP	Modue8_WPara;
			uint16_t        res21[20];
			TIME_STYP       Time;

}MB_RegData;


/*=============================================================================*/
/*******************************************************************************
*                                                                              *
*                       工程数据集  (Drive Layer)                              *
*                                                                              *
*                                                                              *
********************************************************************************/
/*=============================================================================*/

typedef struct _DS_HMI_
{
	MB_RegData	theDS_Reg;

}DS_HMI, *PDS_HMI;


/*******************************************************************************
*                                                                              *
*                       工程数据集: '引用'宏定义                               *
*                                                                              *
*                                                                              *
********************************************************************************/
/*=============================================================================*/
#define PDHMI(name)             (&(theDHMI.theDS_##name))
#define SDHMI(name)             (sizeof(##name))

/*=============================================================================*/
/*******************************************************************************
*                                                                              *
*                       工程数据集: 结构体变量定义                             *
*                                                                              *
*                                                                              *
********************************************************************************/
/*=============================================================================*/

#ifdef DSA_HMI
DS_HMI	theDHMI = { 0 };
HMI_TYPE HMI = { 0 };
#else
extern HMI_TYPE HMI;
extern	DS_HMI	theDHMI;
#endif



#endif


