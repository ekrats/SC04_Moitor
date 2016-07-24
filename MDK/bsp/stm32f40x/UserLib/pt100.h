/*
--------------------------------------------------------------------------------
	Filename:      PT100.h
	Originator:    Jiaqing Lee
	Description:
				   			 PT100 Measurement
	Instance Methods:
				   			 ???
	Method Calling Requirements:
				   			 ???
	Resources:
				   			 ???
	IOtranslate Requirements:
								 ???
	History:
				  *-----*------------*------------*------------------------------
				   		   1.00   04-09-2014     Jia         Initial Code
--------------------------------------------------------------------------------
*/

#ifndef _APP_PT100_H_
#define _APP_PT100_H_

#include <rtthread.h>
#include "stm32f4xx.h"

typedef unsigned short 	word;

#define Mcu_Nop()		__NOP()

typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;


#define PT100_TAB	{\
	209,    222,    238,    255,    269,   283,    294,   310,    325,    340,/*-40 ~ -31*/\
	355,    370,    386,    401,    415,   430,    448,   460,    477,    491,/*-30 ~ -21*/\
	505,    520,    536,    548,    564,   578,    594,   610,    622,    638,/*-20 ~ -11*/\
	650,    668,    685,    699,    714,   726,    740,   755,    772,    786,/*-10 ~ -1*/\
	800,    816,    832,    845,    860,   875,    887,   902,    917,    932,/*0   ~ 9*/\
	947,    963,    979,    991,    1006, 1027, 1036, 1050, 1065,  1078,/*10 ~ 19*/\
	1095,  1100, 1124,  1138, 1153, 1167, 1176, 1195, 1212,  1225,/*20 ~ 29*/\
	1240,  1255, 1270,  1280 ,1298, 1313, 1327, 1345, 1356,  1371,/*30 ~ 39*/\
	1382,  1400, 1414,  1429, 1443, 1458, 1472, 1484, 1458,  1515,/*40 ~ 49*/\
	1529,  1545, 1558,  1573, 1588, 1600, 1615, 1630, 1643,  1660,/*50 ~ 59*/\
	1670,  1685, 1700,  1715, 1729, 1744, 1755, 1770, 1785,  1801,/*60 ~ 69*/\
	1816,  1829, 1843,  1855, 1870, 1885, 1900, 1915, 1927,  1942,/*70 ~ 79*/\
	1958,  1972, 1987,  2000, 2013, 2028, 2041, 2059, 2072,  2085,/*80 ~ 89*/\
	2098,  2116, 2128,  2145, 2156, 2171, 2186, 2196, 2215,  2225,/*90 ~ 99*/\
	2240,  2255, 2270,  2281, 2295, 2310, 2325, 2339, 2350,  2365,/*100 ~ 109*/\
	2379,  2395, 2410,  2420, 2435, 2451, 2465, 2477, 2491,  2505,  2520,/*110 ~ 120*/\
}

#define PT100_TAB_LEN					161//words 

#define PT100_MAX_T						120
#define PT100_MIN_T						-40


#define PT100_INIT_MASK				0x80//128
#define PT100A_APPROACH_CNT		8

#define ERR_T									19999

#define PT100_T(v)			(v+PT100_MIN_T)*10
//--------------------------------------------------------------

typedef struct{
	u16 doneN:	1;
}PT100_Flags_STYP;

typedef struct{
	PT100_Flags_STYP flags;
	u16 *tabP;
	s16 temprSW;
	s16 (*Get)(u16);
}PT100_STYP;


#define PT100_STYP_DEFAULT		{\
										{0},\
										(u16 *)wPT100Tab,\
										0,\
										PT100_GetTempr,\
										}


//s16 PT100_GetTempr(u16,u16);
s16 PT100_GetTempr(word wAdVal);				
										
#endif