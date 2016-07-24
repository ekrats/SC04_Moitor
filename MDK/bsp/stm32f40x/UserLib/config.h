#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <rtthread.h>
#include "stm32f4xx.h"
#include "MB_DataStruct.h"
#include "common.h"

#define CHARGE_PRIO				10//11
#define CAN_PRIO				11//12
#define CAN_TX_PRIO				12//13
#define CAN_QUEUE_PRIO			12
#define HMI_PRIO				13
#define UPDATE_PRIO				14
#define ADIO_PRIO				14
#define HMIBUS_PRIO				17
#define CYCLE_PRIO				20//最低优先级
#define RECORD_PRIO				18
#define FAULT_PRIO				16

#define MB_HW_VERSION			101
#define MB_ARM_VERSION			101
#define MB_FPGA_VERSION			102
#define CB_HW_VERSION			101
#define CB_ARM_VERSION			102
#define CB_FPGA_VERSION			102

//-----parameter---------------------
#define CF_HW_VERSION				0x1002// 10 ??   02 SC02
#define CF_SW_VERSION				112

#define CF_CHARGE_U_ERR			15000//??1500V???????
#define CF_CHARGE_U_OK_L		500
#define CF_CHARGE_U_OK_H		12000

#define CAN_CB_NUM					5

#define CAN_SINK_ID_MASK	(0x1F<<3)//refer to EXTID_STYP

#define CAN_ID_C0					0x00	
#define CAN_ID_C1					0x01//???1????					
#define CAN_ID_C2					0x02	
#define CAN_ID_C3					0x03
#define CAN_ID_C4					0x04
#define CAN_ID_C5					0x05					
#define CAN_ID_C6					0x06	
#define CAN_ID_C7					0x07
#define CAN_ID_C8					0x08
#define CAN_ID_C9					0x09//???1????					
#define CAN_ID_C10					0x0A	
#define CAN_ID_C11					0x0B
#define CAN_ID_C12					0x0C
#define CAN_ID_C13					0x0D//???1????					
#define CAN_ID_C14					0x0E	
#define CAN_ID_C15					0x0F

#define CAN_ID_M1					0x11//????1????
#define CAN_ID_M2					0x12//????2
#define CAN_ID_Mx					0x13//?????
#define CAN_ID_Cx					0x1C//?????
#define CAN_ID_M1CX					0x1D//?????
#define CAN_ID_M2CX					0x1E//?????
//#define CAN_ID_ALL					0x1F//?????
#define CAN_ID_AD					0x1F

#define 		true		1
#define			false		0
	
#define   ODD(a)	   	(a & 0x01)

#define   B0			0x01
#define   B1			0x02
#define   B2			0x04
#define   B3			0x08
#define   B4			0x10
#define   B5			0x20
#define   B6			0x40
#define   B7			0x80
#define   B8			0x0100
#define   B9			0x0200
#define   B10			0x0400
#define   B11			0x0800
#define   B12			0x1000
#define   B13			0x2000
#define   B14			0x4000
#define   B15			0x8000
#define   B16			0x010000
#define   B17			0x020000
#define   B18			0x040000
#define   B19			0x080000
#define   B20			0x100000
#define   B21			0x200000
#define   B22			0x400000
#define   B23			0x800000
#define   B24			0x01000000
#define   B25			0x02000000
#define   B26			0x04000000
#define   B27			0x08000000
#define   B28			0x10000000
#define   B29			0x20000000
#define   B30			0x40000000
#define   B31			0x80000000

#define   		ODD(a)	    	   		(a & 0x01)
//#define			IWDG_EN					(1)

u16 Merge_fun(u8 TH0, u8 TL0);
uint8_t MemCmp(uint8_t* pstr1, uint8_t* pstr2, uint32_t length);
uint32_t get_unix_time(void);

#define VERSION_STYP_DEFAULT 		{\
													MB_HW_VERSION,\
													MB_ARM_VERSION,\
													MB_FPGA_VERSION,\
													CB_HW_VERSION,\
													CB_ARM_VERSION,\
													CB_FPGA_VERSION,\
													CB_HW_VERSION,\
													CB_ARM_VERSION,\
													CB_FPGA_VERSION,\
													}



#endif

