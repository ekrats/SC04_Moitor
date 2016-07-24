
/***************************************************************************

Copyright (C), 1999-2014, Tongye Tech. Co., Ltd.

* @file           Bsp_bakram.c
* @author      
* @version        V1.0.0
* @date           2014-04-28
* @brief          备份寄存器操作相关

History:          // Revision Records

<Author>             <time>       <version >            <desc>

Tongye BCU Team    2014-09-02       V1.0.0             初次建立

***************************************************************************/

/* Includes ------------------------------------------------------------------*/

#define DSA_BKP

#include <stm32f4xx.h>
#include <rtthread.h>
#include "Bsp_bakram.h"

uint8_t*  _pbram;  //访问BakRAM指针

static void BKP_RCC_Configuration(void);

/*******************************************************************************
* Function Name  : System_hw_bakramInit
* Description    : 初始化后备SRAM
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

int System_hw_bakramInit(void)
{
  uint16_t  i = 0;
	uint16_t  errorindex = 0;
	
	if(RTC_ReadBackupRegister(RTC_BKP_DR2) != 0x1975)
	{	
		BKP_RCC_Configuration();
		//---------------------------------------------------------
		// 测试备份寄存器区域
		//---------------------------------------------------------
		for (i = 0x0000; i < 0x1000; i += 4)
		{
			*(__IO uint32_t *) (BKPSRAM_BASE + i) = i;
		}
		
		for (i = 0x0000; i < 0x1000; i += 4)
		{
			if ((*(__IO uint32_t *) (BKPSRAM_BASE + i)) != i)
      {
         errorindex++;
      }
    }
		
		if(errorindex)
		{
			rt_kprintf ("\r\n+ BKP SRAM Number of errors = %d \r\n", errorindex);
		}
		else
		{
			rt_kprintf ("\r\n+ BKP SRAM write OK! \r\n");
		}
		//---------------------------------------------------------
		// 初始化访问BakRAM指针
		//---------------------------------------------------------
		_pbram = (u8 *)BKPSRAM_BASE;
		//---------------------------------------------------------
		// 清0备份寄存区域
		//---------------------------------------------------------
		rt_memset(_pbram, 0, 4096);

		/* Enable the Backup SRAM low power Regulator to retain it's content in VBAT mode */
		PWR_BackupRegulatorCmd(ENABLE);

		/* Wait until the Backup SRAM low power Regulator is ready */
		while(PWR_GetFlagStatus(PWR_FLAG_BRR) == RESET)
		{
			
		}
		RTC_WriteBackupRegister(RTC_BKP_DR2, 0x1975);
	}
	else
	{	
		BKP_RCC_Configuration();
		//---------------------------------------------------------
		// 初始化访问BakRAM指针
		//---------------------------------------------------------
		_pbram = (u8 *)BKPSRAM_BASE;
		
//		PDLA(CTR_PAR)->DisChargeEnergyTotal = RTC_ReadBackupRegister(RTC_BKP_DR3);
//		PDLA(CTR_PAR)->EnergyRecEnergyTotal = RTC_ReadBackupRegister(RTC_BKP_DR4);
	}

	return 1;
}

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    :  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

static void BKP_RCC_Configuration(void)
{
	/* Enable the PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Allow access to RTC */
	PWR_BackupAccessCmd(ENABLE);

	/* Enable BKPRAM Clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);
}

/*******************************************************************************
* Function Name  : void pbram(void)
* Description    :  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void clrbram(u16 addr,u16 size)
{
	//---------------------------------------------------------
	// 清0备份寄存区域
	//---------------------------------------------------------
	rt_memset(_pbram+addr, 0, size);
}

#include "finsh.h"
FINSH_FUNCTION_EXPORT(clrbram, clrbram(u16 addr,u16 size))
///******************* (C) COPYRIGHT 2014 Shenzhen Tongye **********END OF FILE****/

