#ifndef _Common_H_
#define _Common_H_

#include <rtthread.h>
#include "stm32f4xx.h"

typedef struct {
	u8 year;
	u8 month;
	u8 day;
	u8 hour;
	u8 min;
	u8 sec;
}TIME_TYPE;

typedef struct{
	u16  day;
	u8   hour;
	u8   min;
}RUN_TIME_STYP;

typedef struct{
	u16 mbHwVersionW;
	u16 mbARMVersionW;
	u16 mbFPGAVersionW;
	u16 cbHwVersionW;
	u16 cbARMVersionW;
	u16 cbFPGAVersionW;
}Version_STYP;

typedef  struct{
	u16	   cabinetEnN :			1;
	u16    inFuse1N   :  		1;
	u16    inFuse2N   :  		1;
	u16    outFuse1N  : 		1;
	u16    outFuse2N  : 		1;
	u16    outFuse3N  : 		1;
	u16    outFuse4N  : 		1;
	u16    outFuse5N  : 		1;
	u16    preConN  : 		1;
	u16    mainCon1N: 	 	1;
	u16    mainCon2N: 	 	1;
	u16    outCon1N :  		1;
	u16    outCon2N :  		1;
	u16    outCon3N :  		1;
	u16    outCon4N :  		1;
	u16    outCon5N :  		1;
}In_Flags_STYP;

typedef  struct{
	u16	   outCon1N   :			1;
	u16    outCon2N   :  		1;
	u16    outCon3N   :  		1;
	u16    outCon4N   : 		1;
	u16    outCon5N   : 		1;
	u16    res1  	  : 		1;
	u16    preConN    : 		1;
	u16    mainCon1N  : 		1;
	u16    mainCon2N  : 		1;
	u16    res2		  :			7;
}Out_Flags_STYP;

typedef  struct{
	u16		canLostN	:		1;
	u16		AdLost		:		1;
	u16		res1		:		1;
	u16		rail1FailN	:		1;
	u16		rail2FailN	:		1;
	u16		rail3FailN	:		1;
	u16		rail4FailN	:		1;
	u16		rail5FailN	:		1;
	u16		res2		:		8;
}Run_Flags_STYP;





#endif
