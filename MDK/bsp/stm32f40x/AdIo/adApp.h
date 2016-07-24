#ifndef _AD_APP_H_
#define _AD_APP_H_

#include "stm32f4xx.h"
#include "rtthread.h"

extern "C" {
	#include "Bsp_ad.h"
	#include "pt100.h"
}

#define ADC_VREF					3.00

#define ADC_GET_V(v)			(float)(v*ADC_VREF/4096)//get the channel volt
#define ADC_GET_T(t)			(float)(4000*t/(3-t))
#define ADC_AC_KA				(30000/20)//20欧采样电阻，1500V-->50mA
#define ADC_DC_KA				(30000.0/(20*2))//20欧采样电阻，2倍运放，2100V-->50mA	

#define FanIn_Temp_Over					85
#define FanOut_Temp_Over				85


class AppAd {
private:
	u16 BaseData01;
	u16 BaseData02;
	u16 BaseData03;
	u16 BaseData04;
	u16 BaseData05;
	u16 BaseData06;
	u16 BaseData07;
	u16 BaseData08;
	u16 BaseData09;
	u16 BaseData10;
	u16 BaseData11;
	u16 BaseData12;

public:
	uint16_t		Ch1_In_Curent;		//1#整流电流
	uint16_t		Ch2_In_Curent;		//2#整流电流
	uint16_t		Module1_Temp;		//1#整流模块温度
	uint16_t		Module2_Temp;		//2#整流模块温度
	uint16_t		FanIn_Temp;			//进风口温度
	uint16_t		FanOut_Temp;		//出风口温度
	uint16_t		Cap1_Temp;			//1#电容模块温度
	uint16_t		Cap2_Temp;			//2#电容模块温度
	uint16_t		Ch1_AC;				//1#交流输入电压
	uint16_t		Ch2_AC;				//2#交流输入电压
	uint16_t		BUS_VOLT;			//母线电压
	

	void Calculate(void);
	void GetVal(void);

};


extern AppAd AD;


void ADC_ReadInput(void);

#endif

