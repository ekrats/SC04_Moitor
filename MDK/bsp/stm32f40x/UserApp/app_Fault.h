#ifndef _APP_FAULT_H_
#define _APP_FAULT_H_

#include "stm32f4xx.h"
#include <rtthread.h>

extern "C" {
	#include "config.h"
#include "app_record.h"
}

#define FAULT_MASK 			0x55

enum {
	WN_CAN_MxCx,//0
	WN_AD_LOST,
	WN_IN_T_OVER,
	WN_OUT_T_OVER,
	WN_IN_T_FAIL,
	WN_OUT_T_FAIL,
	WN_L1_T_FAIL,
	WN_L2_T_FAIL,
	WN_CAP1_T_FAIL,
	WN_CAP2_T_FAIL,
	WN_PRE_CON_FB,
	WN_MAIN_CON1_FB,
	WN_MAIN_CON2_FB,
	WN_OUT1_CON_FB,
	WN_OUT2_CON_FB,
	WN_OUT3_CON_FB,

	WN_OUT4_CON_FB,//16
	WN_OUT5_CON_FB,
	WN_AC1_U_FAIL,
	WN_AC2_U_FAIL,
	WN_BUS_U_FAIL,
	WN_AC1_U_UNDER,
	WN_AC2_U_UNDER,
	WN_PARA_FAIL,
	WN_ADJUST_FAIL,
	WN_OUT1_ON_FAIL,
	WN_OUT2_ON_FAIL,
	WN_OUT3_ON_FAIL,
	WN_OUT4_ON_FAIL,
	WN_OUT5_ON_FAIL,
	res1,
	res2,

	FT_IN_FUSE1,//32
	FT_IN_FUSE2,
	FT_OUT_FUSE1,
	FT_OUT_FUSE2,
	FT_OUT_FUSE3,
	FT_OUT_FUSE4,
	FT_OUT_FUSE5,
	FT_AC1_U_OVER,
	FT_AC2_U_OVER,
	FT_BUS_U_OVER, 
	FT_BUS_U_UNDER,
	FT_OUT1_U_OVER,
	FT_OUT2_U_OVER,
	FT_OUT3_U_OVER,
	FT_OUT4_U_OVER,
	FT_OUT5_U_OVER,

	FT_L1_T_OVER,//48
	FT_L2_T_OVER,
	FT_CAP1_T_OVER,
	FT_CAP2_T_OVER,
	FT_OUT1_U_FAIL,
	FT_OUT2_U_FAIL,
	FT_OUT3_U_FAIL,
	FT_OUT4_U_FAIL,
	FT_OUT5_U_FAIL,
	FT_L1_I_FAIL,
	FT_L2_I_FAIL,
	FT_L1_I_OVER,
	FT_L2_I_OVER,
	FT_OUT1_OFF_FAIL,//PWM1πÿ∂œ ß∞‹
	FT_OUT2_OFF_FAIL,//PWM2πÿ∂œ ß∞‹
	FT_OUT3_OFF_FAIL,//PWM3πÿ∂œ ß∞‹

	FT_OUT4_OFF_FAIL,//64 PWM4πÿ∂œ ß∞‹
	FT_OUT5_OFF_FAIL,//PWM5πÿ∂œ ß∞‹
	FT_MAX,
};

#define Temp_Min	10			//-40+50
#define Temp_Max	170			//120+50

#define faultDelay	160			//π ’œ∫Û1.6√Î—” ±

typedef struct {
	u16 Inx;
	u16 type;
	TIME_TYPE occur_time;
	TIME_TYPE clear_time;
}PACK_TYPE[200];

typedef struct {
	PACK_TYPE PACK;
	u16 count;
	u16 Addr;
	u16 st_record_flag;
	u16 st_record[64][16];
	u16 FaultDisTime[FT_MAX];
	u16 FaultConTime[FT_MAX];
	u16 FaultMask[FT_MAX];
	u16 CountDown[FT_MAX-32];
	u16 FaultAry[5];
	u16 FaultRcdFlg[3];
	u16 Index;
}FAULT_TYPE;

#ifdef _DS_FAULT_
FAULT_TYPE FAULT = {0};
#else
extern FAULT_TYPE FAULT;
#endif

void FaultAnalyse(void);
void fault_Run(void);

#endif

