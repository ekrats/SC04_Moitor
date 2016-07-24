/*
--------------------------------------------------------------------------------
	  Filename :	  PT100.c
	  Originator:	  Sleepy Lee
--------------------------------------------------------------------------------
*/
#include "Board.h"
//extern Config_STYP config;
//#include "STM32_Usart.h"
#include <rtthread.h>
#include "stm32f4xx.h"
#include "pt100.h"

const u16 wPT100Tab[PT100_TAB_LEN] = PT100_TAB;

/*
--------------------------------------------------------------------------------
    Method:       PT100_GetTempr
    Description:  approach method 
--------------------------------------------------------------------------------
*/
//s16 PT100_GetTempr(word wAdVal,u16 idW)
s16 PT100_GetTempr(word wAdVal)
{
	word wIndex = 0;
	word wMask,wTemp;
	
	s16 temprSW;
	
	if((wAdVal<wPT100Tab[0])
			|| (wAdVal>wPT100Tab[PT100_TAB_LEN-1])){
		return ERR_T;
	}

	wMask = PT100_INIT_MASK;
	
	while(wMask){

		if((wIndex|wMask)<=PT100_TAB_LEN-1){
			
			wIndex |= wMask;//set mask
			
			if(wAdVal==wPT100Tab[wIndex]){//jia need to confirm
				return (PT100_T(wIndex));
			}
			else if(wAdVal>wPT100Tab[wIndex]){
				Mcu_Nop();//continue
			}
			else{
				wIndex &= ~wMask;//clear mask
			}
		}
		wMask>>=1;
		
	}

	if(wAdVal>wPT100Tab[wIndex]){
		wTemp = (10*(wAdVal-wPT100Tab[wIndex]))/(wPT100Tab[wIndex+1]-wPT100Tab[wIndex]);
	}
	else{//wIndex will  >0 here
		wTemp = (10*(wPT100Tab[wIndex])-wAdVal)/(wPT100Tab[wIndex]-wPT100Tab[wIndex-1]);
	}
	temprSW = PT100_T(wIndex)+wTemp;	
	//-----º∆»Î≤π≥•÷µ-------------------
	//temprSW = (s32)(config.mbAdjust.ary[idW].a*temprSW)/100+(s16)10*config.mbAdjust.ary[idW].b;
	
	return temprSW;
}

