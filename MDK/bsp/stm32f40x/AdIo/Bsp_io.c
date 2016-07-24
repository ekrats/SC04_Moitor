
/***************************************************************************

Copyright (C), 1999-2015, Tongye Tech. Co., Ltd.

* @file           Bsp_io.c
* @author         SCH-Team
* @version        V1.0.0
* @date           2015-03-11
* @brief          IO Task

History:          Revision Records

<Author>          <time>       <version >            <desc>

xiou             	2015-03-11       V1.0.0            input -> logic -> output

***************************************************************************/
#define DSA_IO

#include "Bsp_io.h"
#include "stm32f4xx.h"
#include <rtthread.h>
#include "string.h"
#include "board.h"

static DS_INPUTF sysInputF[INPUT_CHANEL_MAX] = {0};
static __IO uint16_t *FGPA_INPUT_ADDR  = (uint16_t *)(FPGA_BEGIN_ADDR + FPGA_INPUT_OFFSET);
static __IO uint16_t *FGPA_OUTPUT_ADDR = (uint16_t *)(FPGA_BEGIN_ADDR + FPGA_OUTPUT_OFFSET);

/*******************************************************************************
* Function Name  : readInput
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void readInput(void)
{
	uint8_t scanChl;
	uint16_t inputBuff = 0;

	for(scanChl = 0; scanChl < INPUT_CHANEL_MAX; scanChl++)
	{
		if((scanChl % 16) ==0)
		{
			inputBuff = *(uint16_t*)(FGPA_INPUT_ADDR + (scanChl / 16));
		}
		//--------------------------------------------------------
		// 赋值原始数据到滤波区域
		//--------------------------------------------------------
		sysInputF[scanChl].gpioVal = (inputBuff >> (scanChl % 16)) & 0x01;
	}
}

/*******************************************************************************
* Function Name  : processInputF
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void processInputF(void)
{
	uint8_t i;

	for(i = 0; i < INPUT_CHANEL_MAX; i++)
	{
		//---------------------------------------------------------------------------------
		// 根据原始输入数据，进行滤波
		//---------------------------------------------------------------------------------
		if(sysInputF[i].gpioVal)
		{
			if(sysInputF[i].ioHighLevels < sysInputF[i].FlLenth)   sysInputF[i].ioHighLevels++;
			if(sysInputF[i].ioLowLevels > 0)   sysInputF[i].ioLowLevels--;
		}
		else
		{
			if(sysInputF[i].ioLowLevels < sysInputF[i].FlLenth)   sysInputF[i].ioLowLevels++;
			if(sysInputF[i].ioHighLevels > 0)   sysInputF[i].ioHighLevels--;
		}
		//----------------------------------------------------------------------------------
		// 更新输入状态
		//----------------------------------------------------------------------------------
		if(sysInputF[i].ioHighLevels >= sysInputF[i].validCounts)   sysInputF[i].inVal = 1;
		else if(sysInputF[i].ioLowLevels >= sysInputF[i].validCounts)   sysInputF[i].inVal = 0;
	}
}

/*******************************************************************************
* Function Name  : makeSysinput
* Description    :
* Input          : - none.
* Output         : None
* Return         : None
*******************************************************************************/
static void makeSysinput(void)
{
	PDIO(IO_IN)->Chn01 = sysInputF[0].inVal;
	PDIO(IO_IN)->Chn02 = sysInputF[1].inVal;
	PDIO(IO_IN)->Chn03 = sysInputF[2].inVal;
	PDIO(IO_IN)->Chn04 = sysInputF[3].inVal;
	PDIO(IO_IN)->Chn05 = sysInputF[4].inVal;
	PDIO(IO_IN)->Chn06 = sysInputF[5].inVal;
	PDIO(IO_IN)->Chn07 = sysInputF[6].inVal;
	PDIO(IO_IN)->Chn08 = sysInputF[7].inVal;
	PDIO(IO_IN)->Chn09 = sysInputF[8].inVal;
	PDIO(IO_IN)->Chn10 = sysInputF[9].inVal;
	PDIO(IO_IN)->Chn11 = sysInputF[10].inVal;
	PDIO(IO_IN)->Chn12 = sysInputF[11].inVal;
	PDIO(IO_IN)->Chn13 = sysInputF[12].inVal;
	PDIO(IO_IN)->Chn14 = sysInputF[13].inVal;
	PDIO(IO_IN)->Chn15 = sysInputF[14].inVal;
	PDIO(IO_IN)->Chn16 = sysInputF[15].inVal;
	
	PDIO(IO_IN)->Chn17 = sysInputF[16].inVal;
	PDIO(IO_IN)->Chn18 = sysInputF[17].inVal;
	PDIO(IO_IN)->Chn19 = sysInputF[18].inVal;
	PDIO(IO_IN)->Chn20 = sysInputF[19].inVal;
	PDIO(IO_IN)->Chn21 = sysInputF[20].inVal;
	PDIO(IO_IN)->Chn22 = sysInputF[21].inVal;
	PDIO(IO_IN)->Chn23 = sysInputF[22].inVal;
	PDIO(IO_IN)->Chn24 = sysInputF[23].inVal;
	PDIO(IO_IN)->Chn25 = sysInputF[24].inVal;
	PDIO(IO_IN)->Chn26 = sysInputF[25].inVal;
	PDIO(IO_IN)->Chn27 = sysInputF[26].inVal;
	PDIO(IO_IN)->Chn28 = sysInputF[27].inVal;
	PDIO(IO_IN)->Chn29 = sysInputF[28].inVal;
	PDIO(IO_IN)->Chn30 = sysInputF[29].inVal;
	PDIO(IO_IN)->Chn31 = sysInputF[30].inVal;
	PDIO(IO_IN)->Chn32 = sysInputF[31].inVal;	
}

/*******************************************************************************
* Function Name  : Bsp_Dio_Init
* Description    :
* Input          : - none.
* Output         : None
* Return         : None
*******************************************************************************/
void Bsp_Dio_Init(void)
{
	uint8_t  i = 0;

	//-------------------------------------------
	// 初始化数字量滤波区域
	//-------------------------------------------
	for(i = 0; i < INPUT_CHANEL_MAX; i++)
	{
		sysInputF[i].gpioVal      = 0;
		sysInputF[i].inVal        = 0;
		sysInputF[i].ioHighLevels = 0;
		sysInputF[i].ioLowLevels  = 0;
		sysInputF[i].FlLenth      = IO_FLT_LENTH;
		sysInputF[i].validCounts  = IO_FLT_VALIDCOUNTS;
	}

	rt_memset((void*)PDIO(IO_OUT),0,2);
	rt_memcpy((void*)FGPA_OUTPUT_ADDR,(void*)PDIO(IO_OUT),2);
}

/*******************************************************************************
* Function Name  : IO_ReadInput
* Description    :
* Input          : - none.
* Output         : None
* Return         : None
*******************************************************************************/
void IO_ReadInput(void)
{
	//----------------------------------------------------------------------
	// IO 输入通道采集
	//----------------------------------------------------------------------
	readInput();
	//----------------------------------------------------------------------
	// IO 输入信号滤波
	//----------------------------------------------------------------------
	processInputF();
	//----------------------------------------------------------------------
	// IO 映射到接口层
	//----------------------------------------------------------------------
	makeSysinput();
}

/*******************************************************************************
* Function Name  : IO_WriteOutput
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void IO_WriteOutput(void)
{
	memcpy((void*)FGPA_OUTPUT_ADDR,(void*)PDIO(IO_OUT),2);
}

/*******************************************************************************
* Function Name  : void iotest(void) 
* Description    :  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static uint8_t testReg = 0;
static void iotest(void)
{	
	if(testReg & OUT_TEST_CMD)
	{
		testReg &= (uint8_t)~OUT_TEST_CMD;
		rt_memset(PDIO(IO_OUT), 0, 2);
		rt_kprintf("\r\n系统未处于强制IO输出模式!\r\n");
		rt_kprintf("\r\n停止测试20路输出通道!");
	}
	else
	{
		testReg |= (uint8_t)OUT_TEST_CMD;
		rt_kprintf("\r\n系统处于强制IO输出模式!\r\n");
		rt_kprintf("\r\n自动测试20路输出通道!");
	}
}

/*******************************************************************************
* Function Name  : io
* Description    :
* Input          : - none.
* Output         : None
* Return         : None
*******************************************************************************/
static void io(void)
{
	uint8_t  i = 0,j = 0;
	uint16_t *pval = (uint16_t *)PDIO(IO_IN);

	rt_kprintf("\r\nDigital In:\r\n");
	for(i=0; i<48; i++)
	{

		if((i % 8) == 0)
		{
			rt_kprintf("    ");
		}
		if((i % 16) == 0)
		{
			rt_kprintf("\r\n");
			for( j=i+1; j<=i+8; j++)
			{
				rt_kprintf("ch%02d|",j);
			}
			rt_kprintf("    ");
			for( ; j<=i+16; j++)
			{
				rt_kprintf("ch%02d|",j);
			}

			rt_kprintf("\r\n");
		}

		rt_kprintf("  %1d |",(*pval >> (i%16)) & 0x01);

		if((i % 16) == 15)
		{
			pval ++;
		}
	}
	rt_kprintf("\r\n\r\n	--------------------------------------------------------------\r\n");

	pval = (uint16_t *)PDIO(IO_OUT);

	rt_kprintf("\r\nDigital Out:\r\n");
	for(i=0; i<16; i++)
	{
		if((i % 8) == 0)
		{
			rt_kprintf("    ");
		}
		if((i % 16) == 0)
		{
			rt_kprintf("\r\n");
			for( j=i+1; j<=i+8; j++)
			{
				rt_kprintf("ch%02d|",j);
			}
			rt_kprintf("    ");
			for( ; j<=i+16; j++)
			{
				rt_kprintf("ch%02d|",j);
			}
			rt_kprintf("\r\n");
		}

		rt_kprintf("  %1d |",(*pval >> (i%16)) & 0x01);

		if((i % 16) == 15)
		{
			pval ++;
		}
	}

	rt_kprintf("\r\n");
}

#include "finsh.h"
FINSH_FUNCTION_EXPORT(io,  printf io  data)
FINSH_FUNCTION_EXPORT(iotest,  test io  out)

/******************* (C) COPYRIGHT 2012 Shenzhen Tongye *****END OF FILE****/
