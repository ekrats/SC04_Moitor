#include "adApp.h"
#include "stm32f4xx.h"
#include "rtthread.h"

AppAd AD;

void AppAd::GetVal(void)
{
	this->BaseData01 = PDAD(AD_IN)->inVal01_AD;
	this->BaseData02 = PDAD(AD_IN)->inVal02_AD;
	this->BaseData03 = PDAD(AD_IN)->inVal03_AD;
	this->BaseData04 = PDAD(AD_IN)->inVal04_AD;
	this->BaseData05 = PDAD(AD_IN)->inVal05_AD;
	this->BaseData06 = PDAD(AD_IN)->inVal06_AD;
	this->BaseData07 = PDAD(AD_IN)->inVal07_AD;
	this->BaseData08 = PDAD(AD_IN)->inVal08_AD;
	this->BaseData09 = PDAD(AD_IN)->inVal09_AD;
	this->BaseData10 = PDAD(AD_IN)->inVal10_AD;
	this->BaseData11 = PDAD(AD_IN)->inVal11_AD;
	this->BaseData12 = PDAD(AD_IN)->inVal12_AD;
}

void AppAd::Calculate()
{
	float fTemp;
	s16 tp;
	
	fTemp = 10*ADC_DC_KA*ADC_GET_V(this->BaseData01);
	this->Ch1_In_Curent = (u16)fTemp;
	
	fTemp = 10*ADC_DC_KA*ADC_GET_V(this->BaseData02);
	this->Ch2_In_Curent = (u16)fTemp;
	
	tp = PT100_GetTempr(this->BaseData04);
	this->Module1_Temp = (tp + 500) / 10; 
	
	tp = PT100_GetTempr(this->BaseData05);
	this->Module2_Temp = (tp + 500) / 10;
	
	tp = PT100_GetTempr(this->BaseData06);
	this->FanIn_Temp = (tp + 500) / 10;
	
	tp = PT100_GetTempr(this->BaseData07);
	this->FanOut_Temp = (tp + 500) / 10;
	
	tp = PT100_GetTempr(this->BaseData08);
	this->Cap1_Temp = (tp + 500) / 10;
	
	tp = PT100_GetTempr(this->BaseData09);
	this->Cap2_Temp = (tp + 500) / 10;
	
	fTemp = 10*ADC_DC_KA*ADC_GET_V(this->BaseData10);
	this->Ch1_AC = (u16)fTemp;
	
	fTemp = 10*ADC_DC_KA*ADC_GET_V(this->BaseData11);
	this->Ch2_AC = (u16)fTemp;
	
	fTemp = 10*ADC_DC_KA*ADC_GET_V(this->BaseData12);
	this->BUS_VOLT = (u16)fTemp;
}

void ADC_ReadInput(void)
{
	AD_Collection();
	
	AD.GetVal();
	
	AD.Calculate();
}
