
/***************************************************************************

Copyright (C), 1999-2015, Tongye Tech. Co., Ltd.

* @file           Bsp_can.c
* @author          
* @version        V1.0.0
* @date           
* @brief          提供提供模拟量信号的采集以及ADC模块初始化
History:          // Revision Records

<Author>              <time>        <version>            <desc>
SCH-Team             2015-03-17       V1.0.0  

***************************************************************************/
#define DSA_AD

#include "Bsp_ad.h"
#include <rtthread.h>
#include <string.h>
#include <math.h>

#define ADC_DR_OFFSET        ((uint32_t)0x4C)
#define ADC1_DR_ADRESS       ((uint32_t)(ADC1_BASE + ADC_DR_OFFSET))
#define ADC3_DR_ADRESS       ((uint32_t)(ADC3_BASE + ADC_DR_OFFSET))

#define ADC1_CHANNELS        (6)
#define ADC3_CHANNELS        (6)

#define ADC_FLT1_BUFFER_SIZE (10)
#define ADC1_BUF_LENGTH      ((uint32_t)(ADC1_CHANNELS * ADC_FLT1_BUFFER_SIZE))
#define ADC3_BUF_LENGTH      ((uint32_t)(ADC3_CHANNELS * ADC_FLT1_BUFFER_SIZE))
//---------------------------------------------------------------------
// 任务支持的最大AD通道数
//---------------------------------------------------------------------
#define ADC_INPUT_CHANEL_MAX (16)
//---------------------------------------------------------------------
// AD滤波算法中，二次滤波缓冲区深度
//---------------------------------------------------------------------
#define ADC_FLT2_BUFFER_SIZE (16)
//---------------------------------------------------------------------
// AD采样原始值
//---------------------------------------------------------------------
rt_uint16_t ADC1_ValTemp[ADC1_BUF_LENGTH] = {0};
rt_uint16_t ADC3_ValTemp[ADC3_BUF_LENGTH] = {0};
//---------------------------------------------------------------------
// 系统所支持的AD_IN的通道数最大为：ADC_INPUT_CHANEL_MAX
// 所有通道的采集原始数据都存于	AD_Origine 数组
//---------------------------------------------------------------------
//---------------------------------------------------------------------
// 按通道顺序排放的各通道AD原始缓冲区数据 
//---------------------------------------------------------------------
rt_uint16_t AD_Origine[ADC_INPUT_CHANEL_MAX][ADC_FLT1_BUFFER_SIZE] = {0};
//---------------------------------------------------------------------
// 按通道顺序排放的各通道AD一次滤波数据区 
//---------------------------------------------------------------------
rt_uint16_t AD_Origine_FL1[ADC_INPUT_CHANEL_MAX] = {0};
//---------------------------------------------------------------------
// 按通道顺序排放的各通道二次滤波数据区 
//---------------------------------------------------------------------
rt_uint16_t AD_Origine_FL2[ADC_INPUT_CHANEL_MAX][ADC_FLT2_BUFFER_SIZE] = {0};
//---------------------------------------------------------------------
// 按通道顺序排放的各通道AD滤波数据区 
//---------------------------------------------------------------------
rt_uint16_t AD_Origine_FL[ADC_INPUT_CHANEL_MAX] = {0};

/*******************************************************************************
* Function Name  : ADC_GPIO_Config
* Description    : Initializes the ADC Input channal pins according to the 
*                  specified  parameters in the KT_InitStruct.
* Input          : - none.
* Output         : None
* Return         : None
*******************************************************************************/

static void ADC_GPIO_Config(void)
{
	GPIO_InitTypeDef    GPIO_InitStructure = {0};

	/* Enable the GPIO_A Clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |RCC_AHB1Periph_GPIOB |RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 
		| GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 |GPIO_Pin_1 |GPIO_Pin_2; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : ADC_Init
* Description    : Initializes the KT virtual peripheral according to the 
*                  specified  parameters in the KT_InitStruct.
* Input          :  .
* Output         : None
* Return         : None
*******************************************************************************/
int System_hw_ADC_Init(void)
{
	ADC_InitTypeDef           ADC_InitStructure       = {0};
	DMA_InitTypeDef           DMA_InitStructure       = {0};
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure   = {0};
	TIM_OCInitTypeDef         TIM_OCInitStructure     = {0};
	ADC_CommonInitTypeDef     ADC_CommonInitStructure = {0};

	//--------------------------------------------------------------
	//
	// ADC模块所用到的GPIO管脚的初始化
	//
	//--------------------------------------------------------------
	ADC_GPIO_Config();

	//--------------------------------------------------------------
	// 开启ADC模块时钟，开启DMA1，DMA2时钟，开启TIM1时钟
	// 
	// [注] TIM1用于定时触发ADC采样
	//		DMA 用于传输ADC数据					    
	//--------------------------------------------------------------
	RCC_APB2PeriphClockCmd(ADC_MODULE1
		| ADC_MODULE2,
		ENABLE);

	RCC_AHB1PeriphClockCmd(ADC_DMA,
		ENABLE);

	RCC_APB2PeriphClockCmd(ADC_TIM_RCC, ENABLE);

	//--------------------------------------------------------------
	// 停止ADC工作
	//--------------------------------------------------------------
	ADC_Cmd(ADC1, DISABLE);
	ADC_Cmd(ADC3, DISABLE);

	//==============================================================
	//''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
	//--------------------------------------------------------------
	// TIM8 configuration 用于触发ADC1，ADC3 的规则转换组
	//--------------------------------------------------------------
	// 【注】APB2主频168MHz, TIM8 Clk = 168MHz
	//       TIM8的计数频率1MHz
	//
	//''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
	//==============================================================
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period        = 500;
	TIM_TimeBaseStructure.TIM_Prescaler     = 0x0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
	TIM_TimeBaseInit(ADC_TIM, &TIM_TimeBaseStructure);

	TIM_PrescalerConfig(ADC_TIM, (168 - 1), TIM_PSCReloadMode_Immediate);

	/*-------- TIM8 channel1 configuration in PWM mode ------------*/
	TIM_OCInitStructure.TIM_OCMode          = TIM_OCMode_PWM1; 
	TIM_OCInitStructure.TIM_OutputState     = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse           = 100; 
	TIM_OCInitStructure.TIM_OCPolarity      = TIM_OCPolarity_Low;
	TIM_OC1Init(ADC_TIM, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(ADC_TIM,TIM_OCPreload_Enable);

	//--------------------------------------------------------------
	// 配置DMA通道																 
	//--------------------------------------------------------------
	DMA_DeInit(DMA2_Stream0);
	DMA_InitStructure.DMA_Channel            = DMA_Channel_0;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (rt_uint32_t)ADC1_DR_ADRESS;
	DMA_InitStructure.DMA_Memory0BaseAddr    = (rt_uint32_t)ADC1_ValTemp;
	DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize         = ADC1_BUF_LENGTH;
	DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);

	DMA_DeInit(DMA2_Stream1);
	DMA_InitStructure.DMA_Channel            = DMA_Channel_2; 
	DMA_InitStructure.DMA_PeripheralBaseAddr = (rt_uint32_t)ADC3_DR_ADRESS;
	DMA_InitStructure.DMA_Memory0BaseAddr    = (rt_uint32_t)ADC3_ValTemp;
	DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize         = ADC3_BUF_LENGTH;
	DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream1, &DMA_InitStructure);

	/* ADC Common Init **********************************************************/
	ADC_CommonInitStructure.ADC_Mode             = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler        = ADC_Prescaler_Div4;
	ADC_CommonInitStructure.ADC_DMAAccessMode    = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	//-------------------------------------------------------------------
	// ADC控制器的配置
	//-------------------------------------------------------------------
	/* ADC1 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Resolution           = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode         = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode   = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Falling;
	ADC_InitStructure.ADC_ExternalTrigConv     = ADC_ExternalTrigConv_T8_CC1;
	ADC_InitStructure.ADC_DataAlign            = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion      = ADC1_CHANNELS; 
	ADC_Init(ADC1, &ADC_InitStructure);

	//.......................................................................................
	/* ADC1 regular channel configuration                                                  */ 	
	/* ADC1 采样时间：SampleTime+12  个采样周期，ADC的时钟84/4 = 21MHz                     */ 	
	/* 例如         ：480 + 12 = 492个采样周期                                             */ 	
	/*  转换一个通道的时间 = 492 * 1 / 21 = 23.4 us                                        */
	//.......................................................................................
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4,  1, ADC_SampleTime_480Cycles);//tmp0(1#整流模块温度)
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5,  2, ADC_SampleTime_480Cycles);//tmp1(2#整流模块温度)
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6,  3, ADC_SampleTime_480Cycles);//tmp2(进风口温度)
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7,  4, ADC_SampleTime_480Cycles);//tmp3(出风口温度)
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8,  5, ADC_SampleTime_480Cycles);//tmp4(1#电容模块温度)
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9,  6, ADC_SampleTime_480Cycles);//tmp5(2#电容模块温度)

	/* ADC3 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Resolution           = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode         = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode   = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Falling;
	ADC_InitStructure.ADC_ExternalTrigConv     = ADC_ExternalTrigConv_T8_CC1;
	ADC_InitStructure.ADC_DataAlign            = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion      = ADC3_CHANNELS;	 
	ADC_Init(ADC3, &ADC_InitStructure);
	//.......................................................................................
	/* ADC3 regular channel configuration                                                  */ 	
	/* ADC3 采样时间：SampleTime+12  个采样周期，ADC的时钟84/4 = 21MHz                     */ 	
	/* 例如         ：480 + 12 = 492个采样周期                                             */ 	
	/*  转换一个通道的时间 = 492 * 1 / 21 = 23.4 us                                        */
	//.......................................................................................
	ADC_RegularChannelConfig(ADC3, ADC_Channel_1,  1, ADC_SampleTime_480Cycles);//iAD0(1#整流电流)
	ADC_RegularChannelConfig(ADC3, ADC_Channel_2,  2, ADC_SampleTime_480Cycles);//iAD1(2#整流电流)
	ADC_RegularChannelConfig(ADC3, ADC_Channel_3,  3, ADC_SampleTime_480Cycles);//iAD2(预留)
	ADC_RegularChannelConfig(ADC3, ADC_Channel_10, 4, ADC_SampleTime_480Cycles);//uAD0(1#交流输入电压)
	ADC_RegularChannelConfig(ADC3, ADC_Channel_11, 5, ADC_SampleTime_480Cycles);//uAD1(2#交流输入电压)
	ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 6, ADC_SampleTime_480Cycles);//uAD2(母线电压)	
	
	/* Enable DMA request after last transfer (Single-ADC mode) */
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
	/* Enable DMA request after last transfer (Single-ADC mode) */
	ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);
	//----------------------------------------
	// 启动ADC1 和 与之挂钩的DMA2
	//----------------------------------------
	/* Enable DMA Stream0 */
	DMA_Cmd(DMA2_Stream0, ENABLE);
	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
	//----------------------------------------
	// 启动ADC3 和 与之挂钩的DMA2
	//----------------------------------------
	/* Enable DMA Stream1 */
	DMA_Cmd(DMA2_Stream1, ENABLE);
	/* Enable ADC3 DMA */
	ADC_DMACmd(ADC3, ENABLE);
	/* Enable ADC3 */
	ADC_Cmd(ADC3, ENABLE);
	//-------------------------------------------------------------
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//             启动TIM8，触发ADC1，ADC3的规则组转换
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//-------------------------------------------------------------
	TIM_ARRPreloadConfig(ADC_TIM, ENABLE);
	/* TIM8 counter enable */
	TIM_Cmd(ADC_TIM, ENABLE);
	/* TIM8 main Output Enable */
	TIM_CtrlPWMOutputs(ADC_TIM, ENABLE); 

	return 0;
}


/*******************************************************************************
* Function Name  : void FltAnalog(uint16_t *psrcdata,uint16_t *PDIOtdata,uint8_t length)
* Description    : 中位值平均滤波法，缓存采集length组，去掉最大最小值，
*                  剩下length - 2组取平均
*                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static rt_uint16_t FltAnalog(rt_uint16_t *psrcdata, rt_uint16_t *pdstdata, rt_uint16_t length)
{
	rt_uint16_t samp_times;
	rt_uint16_t ad_max  = 0;
	rt_uint16_t ad_min  = 0;
	rt_uint16_t ad_data = 0;
	rt_uint32_t ad_sum  = 0;
	rt_uint16_t avg     = 0;

	for(samp_times=0; samp_times < length; samp_times++)
	{
		ad_data = *psrcdata++ ;
		if(samp_times == 0)
		{
			ad_max = ad_data;
			ad_min = ad_data;
		}
		else
		{
			if(ad_data > ad_max)
			{
				ad_max = ad_data;
			}
			else if(ad_data < ad_min)
			{
				ad_min = ad_data;
			}
		}
		ad_sum += ad_data;
	}

	if(length <= 2)
	{
		*pdstdata = (rt_uint16_t)(ad_sum / 2);	
		avg = (rt_uint16_t)(ad_sum / 2);
	}
	else
	{
		ad_sum = ad_sum - ad_max - ad_min;

		*pdstdata = (rt_uint16_t)(ad_sum / (length - 2));	
		avg = (rt_uint16_t)(ad_sum / (length - 2));
	}

	if(avg >= 2048)
	{
		return ad_max;
	}
	else
	{
		return ad_min;
	}
}

/*******************************************************************************
* Function Name  :  
* Description    : 交流均方根计算有效值 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static rt_uint16_t ACFltAnalog(__IO uint16_t* vSrc, uint16_t zData, uint16_t size)
{
	rt_uint16_t i;
	rt_uint32_t val;
	rt_uint32_t sumVal = 0;
	double ss = 0.00;

	for(i = 0; i < size; i++)
	{
		if(vSrc[i] >= zData)
		{
			val = vSrc[i] - zData;
		}
		else
		{
			val = zData - vSrc[i];
		}

		sumVal += val * val;
	}

	ss = (double)sumVal / size;

	return (uint16_t)sqrt(ss);
}


/*******************************************************************************
* Function Name  : uint16_t Filter_JP(uint16_t* pSrc, rt_uint16_t lv, uint16_t length) 
* Description    : 类似移动加权平均滤波，加权效应持续累积 
* Input          : uint16_t* pSrc  ： 滤波数组首地址
*                  uint16_t lv     ： 上次采信值
* Output         : uint16_t length ： 加权平均后的滤波数据
* Return         : None
*******************************************************************************/
static rt_uint16_t Filter_JP(rt_uint16_t* pSrc, rt_uint16_t lv, rt_uint16_t mlength)
{
	s32 dV    = 0;
	s32 adAdj = 0;
	rt_uint32_t adSum = 0;
	rt_uint16_t j     = 0;

	for (j = 0; j < mlength; j++)
	{
		//---------------------------------------------
		//得到更新后的2次滤波缓冲区里，滤波数据和
		//上次采信数据的差值
		//---------------------------------------------
		dV  = pSrc[j] - lv;
		//---------------------------------------------
		// 按照受到干扰程度的不同，进行削峰填谷
		//---------------------------------------------
		if(dV >= 20 || dV <= -20)
		{
			dV = dV / 4;
		}
		else if(dV >= 10 || dV <= -10)
		{
			dV = dV / 2;
		}
		//---------------------------------------------
		// 保证数据校验过后不会小于零, 因为这时处理的
		// 数值都还是ADC采样数据，没有换算成实际温度值
		// 所以，最小值是0，不能是负数
		//---------------------------------------------
		adAdj = lv + dV;
		if(adAdj < 0) adAdj = 0;
		//--------------------------------------------
		// 修正后的数据，再放回数组去
		//--------------------------------------------
		pSrc[j] = adAdj;

		adSum += adAdj;
	}

	return (rt_uint16_t)(adSum / mlength);
}


/*******************************************************************************
* Function Name  : rt_uint16_t TemFltAnalog(rt_uint16_t *psrcdata, rt_uint16_t channel)
* Description    : 温度滤波算法
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
rt_uint16_t TemFltAnalog(rt_uint16_t *psrcdata, rt_uint16_t channel)
{
	//---------------------------------------------------------------------
	// 系统复位后，立即更新AD温度滤波区数据，避免温度滤波数据要长时间
	// 才能跟随上真实的温度值
	//---------------------------------------------------------------------
	static rt_uint16_t  AD_Mode_afterSysRst     = 1;
	static rt_uint16_t  AD_Mode_afterSysRst_Cnt = 0;
	//---------------------------------------------------------------------
	// 二次滤波用，通道移位变量
	//---------------------------------------------------------------------
	static rt_uint16_t fltIndex = 0;
	//--------------------------------------------------------------
	// 针对所有通道进行一次滤波
	//--------------------------------------------------------------
	FltAnalog(psrcdata, &AD_Origine_FL1[channel], ADC_FLT1_BUFFER_SIZE);

	if(AD_Mode_afterSysRst)
	{
		//----------------------------------------------------------
		// 更新简单滤波缓冲区数据，至 温度滤波缓冲区，
		// 使得系统复位后，系统能立即响应环境温度的变化
		//----------------------------------------------------------
		AD_Origine_FL2[channel][fltIndex++] = AD_Origine_FL1[channel];

		if(fltIndex >= ADC_FLT2_BUFFER_SIZE)
		{
			fltIndex = 0;
		}

		if(AD_Mode_afterSysRst_Cnt++ >= 50)
		{
			AD_Mode_afterSysRst = 0;
		}

		return AD_Origine_FL2[channel][fltIndex];
	}
	else
	{
		//--------------------------------------------------------------
		// 针对所有通道进行温度特性的二次滤波
		//--------------------------------------------------------------
		AD_Origine_FL2[channel][fltIndex++] = AD_Origine_FL1[channel];

		if(fltIndex >= ADC_FLT2_BUFFER_SIZE)
		{
			fltIndex = 0;
		}

		return Filter_JP((rt_uint16_t*)(AD_Origine_FL2[channel]), AD_Origine_FL[channel], ADC_FLT2_BUFFER_SIZE);
	}
}


/*******************************************************************************
* Function Name  : Ad_GetOrig 
* Description    : AD_Origine[ADC_INPUT_CHANEL_MAX][ADC_FLTBUFFER_SIZE] 
* Input          : - none.
* Output         : None
* Return         : None
*******************************************************************************/
static void Ad_GetOrig(void)
{
	rt_uint8_t i = 0;

	for(i = 0; i < ADC_FLT1_BUFFER_SIZE; i++)
	{
		AD_Origine[0][i] = ADC1_ValTemp[i * ADC1_CHANNELS + 0]; 
		AD_Origine[1][i] = ADC1_ValTemp[i * ADC1_CHANNELS + 1];
		AD_Origine[2][i] = ADC1_ValTemp[i * ADC1_CHANNELS + 2];
		AD_Origine[3][i] = ADC1_ValTemp[i * ADC1_CHANNELS + 3];
		AD_Origine[4][i] = ADC1_ValTemp[i * ADC1_CHANNELS + 4];
		AD_Origine[5][i] = ADC1_ValTemp[i * ADC1_CHANNELS + 5];

		AD_Origine[6][i]  = ADC3_ValTemp[i * ADC3_CHANNELS + 0];
		AD_Origine[7][i]  = ADC3_ValTemp[i * ADC3_CHANNELS + 1];
		AD_Origine[8][i]  = ADC3_ValTemp[i * ADC3_CHANNELS + 2];
		AD_Origine[9][i]  = ADC3_ValTemp[i * ADC3_CHANNELS + 3];
		AD_Origine[10][i] = ADC3_ValTemp[i * ADC3_CHANNELS + 4];
		AD_Origine[11][i] = ADC3_ValTemp[i * ADC3_CHANNELS + 5];
	}
}

/*******************************************************************************
* Function Name  : AD_FL1 
* Description    : 对所有通道进行去极值的平均滤波运算  
*                   
* Input          : - none.
* Output         : None
* Return         : None
*******************************************************************************/
static void AD_FL(void)
{
	// 温度通道
	//--------------------------------------------------------------
	AD_Origine_FL[0] = TemFltAnalog(AD_Origine[0], 0);   //环境温度
	AD_Origine_FL[1] = TemFltAnalog(AD_Origine[1], 1);   //进线电抗器温度
	AD_Origine_FL[2] = TemFltAnalog(AD_Origine[2], 2);   //1#电抗器温度
	AD_Origine_FL[3] = TemFltAnalog(AD_Origine[3], 3);   //2#电抗器温度
	AD_Origine_FL[4] = TemFltAnalog(AD_Origine[4], 4);   //3#电抗器温度
	AD_Origine_FL[5] = TemFltAnalog(AD_Origine[5], 5);   //4#电抗器温度
	//--------------------------------------------------------------
	// 直流通道
	//--------------------------------------------------------------
	FltAnalog(AD_Origine[6],  &AD_Origine_FL[6],  ADC_FLT1_BUFFER_SIZE);  //直流母线电压
	FltAnalog(AD_Origine[7],  &AD_Origine_FL[7],  ADC_FLT1_BUFFER_SIZE);  //高压侧电压 
	FltAnalog(AD_Origine[8],  &AD_Origine_FL[8],  ADC_FLT1_BUFFER_SIZE);  //超级电容侧电压
	FltAnalog(AD_Origine[11], &AD_Origine_FL[11], ADC_FLT1_BUFFER_SIZE);  //电容侧电流
	//--------------------------------------------------------------
	// 交流通道
	//--------------------------------------------------------------
	ACFltAnalog(AD_Origine[9],  AD_Origine_FL[9],  ADC_FLT1_BUFFER_SIZE);  //逆变器电流
	ACFltAnalog(AD_Origine[10], AD_Origine_FL[10], ADC_FLT1_BUFFER_SIZE);  //高压侧电流
	
}

/*******************************************************************************
* Function Name  : AD_Interface_DataInteract  
* Description    : 向用户接口交换数据  
*                   
*
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void AD_Interface_DataInteract(void)
{
	//--------------------------------------------------------------
	// 滤波处理后结果，用于逻辑运算用
	//--------------------------------------------------------------
	PDAD(AD_IN)->inVal01_AD = AD_Origine_FL[0];  //环境温度   
	PDAD(AD_IN)->inVal02_AD = AD_Origine_FL[1];  //进线电抗器温度
	PDAD(AD_IN)->inVal03_AD = AD_Origine_FL[2];  //1#电抗器温度
	PDAD(AD_IN)->inVal04_AD = AD_Origine_FL[3];  //2#电抗器温度
	PDAD(AD_IN)->inVal05_AD = AD_Origine_FL[4];  //3#电抗器温度
	PDAD(AD_IN)->inVal06_AD = AD_Origine_FL[5];  //4#电抗器温度
	PDAD(AD_IN)->inVal07_AD = AD_Origine_FL[6];  //直流母线电压
	PDAD(AD_IN)->inVal08_AD = AD_Origine_FL[7];  //高压侧电压 
	PDAD(AD_IN)->inVal09_AD = AD_Origine_FL[8];  //超级电容侧电压
	PDAD(AD_IN)->inVal10_AD = AD_Origine_FL[9];  //逆变器电流
	PDAD(AD_IN)->inVal11_AD = AD_Origine_FL[10]; //电容侧电流
	PDAD(AD_IN)->inVal12_AD = AD_Origine_FL[11]; //高压侧电流
	PDAD(AD_IN)->inVal13_AD = AD_Origine_FL[12]; 
	PDAD(AD_IN)->inVal14_AD = AD_Origine_FL[13];
	PDAD(AD_IN)->inVal15_AD = AD_Origine_FL[14];
	PDAD(AD_IN)->inVal16_AD = AD_Origine_FL[15];
}


/*******************************************************************************
* Function Name  : AD_Collection 
* Description    : 系统底层AD数据处理任务.   
*                   
*
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void AD_Collection(void)
{
	//--------------------------------------------------------------
	// 拆分AD通道数据
	//--------------------------------------------------------------
	Ad_GetOrig();
	//--------------------------------------------------------------
	// 针对所有通道进行滤波
	//--------------------------------------------------------------
	AD_FL();
	//--------------------------------------------------------------
	// 模拟量通道值保存
	//--------------------------------------------------------------
	AD_Interface_DataInteract();
}

/******************* (C) COPYRIGHT 2012 Shenzhen Tongye *****END OF FILE****/

