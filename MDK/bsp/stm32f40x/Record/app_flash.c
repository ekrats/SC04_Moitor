#include "app_flash.h"
#include <rtthread.h>
#include "stm32f4xx.h"
#include "at45Drv.h"
#include "spiDrv.h"
#include "crc16.h"
#include "string.h"
#include "config.h"


#define SIZE sizeof(TEXT_Buffer)
#define F_PAGE_SIZE				528

static u8 FLASH_FLAG = 0x68;
static u8 ERR_FLAG = 0x72;
static u8 _ParMan[528];
static u8 _ErrMan[528];
u8 CRC_DATA[F_PAGE_SIZE];
//监控板参数管理页地址
#define  Monitor_mana_IndexW   	101
#define  Err_mana_IndexW		102
//监控板参数保存页地址
const u8 err_base_IndexW = 103;

static u8 par_IndexW;
DATA_RAM R_SysBuff;

//extern FAULT_TYPE FAULT;

void Flash_Write_ERR(PDATA_RAM fBuf)
{
	u16 k;
	u8 IndexW;
	DATA_RAM * W_ErrBuff;
	W_ErrBuff = fBuf;
	
	for (k=0; k<F_PAGE_SIZE; k++)
	{
		if (k < W_ErrBuff->Number)
		{
			_ErrMan[k] = ERR_FLAG;
		}
		else
		{
			_ErrMan[k] = 0xff;
		}
	}
	
	for(k=0;k<3;k++)//如果写入不成功，则连写入3次，均不成功，不再写
	{
		ICWriteBuf1(0, _ErrMan, F_PAGE_SIZE);
		ICBuf1ToPageN(Err_mana_IndexW);
		//rt_thread_delay(15);
		ICWaitReady();
		
		ICReadPage(CRC_DATA, Err_mana_IndexW, 0, F_PAGE_SIZE);
		if(MemCmp((uint8_t*)(_ErrMan),(uint8_t*)(CRC_DATA),F_PAGE_SIZE))
			break;
	}
	
	for(k=0;k<3;k++)//如果写入不成功，则连写入3次，均不成功，不再写
	{
		ICWriteBuf1(0, W_ErrBuff->dataRam, W_ErrBuff->Len);
		ICBuf1ToPage(IndexW);
		
		ICReadPage(CRC_DATA, IndexW, 0, W_ErrBuff->Len);
		if(MemCmp((uint8_t*)(W_ErrBuff->dataRam), (uint8_t*)(CRC_DATA), W_ErrBuff->Len))
			break;
	}
}

void Flash_Read_SYS(PDATA_RAM fBuf)
{
	u16 valW, crcl = 0x0000, crch = 0x0000, crc;
	u16 i;
	
	//读取故障管理页
	ICReadPage(_ErrMan, Err_mana_IndexW, 0, F_PAGE_SIZE);
	for (i=0; i<F_PAGE_SIZE; i++)
	{
		if (_ErrMan[i] != ERR_FLAG)
		{
			//FAULT.count = i;
			break;
		}
	}
	
	//读取系统参数管理页
	ICReadPage(_ParMan, Monitor_mana_IndexW, 0, F_PAGE_SIZE);
	for (i=0; i<100; i++)
	{
		if (_ParMan[i] != FLASH_FLAG)
		{
			par_IndexW = i;
			break;
		}
	}
	
	//rt_thread_delay(10);
	ICReadPage(R_SysBuff.dataRam, par_IndexW-1, 0, fBuf->Len);
	
	valW = CRC16_Dn_Cal(R_SysBuff.dataRam, fBuf->Len - 2, 0xFFFF);
		
	crcl = 0x0000;
	crch = 0x0000;
	
	crcl |= R_SysBuff.dataRam[fBuf->Len - 2];	
	crch |= R_SysBuff.dataRam[fBuf->Len - 1];	
	
	crc = crcl | (crch << 8);
	
	if (crc == valW)
	{
		rt_memcpy(fBuf->dataRam, R_SysBuff.dataRam, fBuf->Len-2);
	}
	else
	{
		//monitor_par_default();
	}
}

void Flash_Write_SYS(PDATA_RAM fBuf)
{
	u16 i;
	u8 k;
	u16 valW;
	DATA_RAM * W_SysBuff;
	W_SysBuff = fBuf;
	
	if (par_IndexW == 99)
	{
		par_IndexW = 0;
	}
	par_IndexW++;
	for (i=0; i<F_PAGE_SIZE; i++)
	{
		if (i < par_IndexW)
		{
			_ParMan[i] = FLASH_FLAG;
		}
		else
		{
			_ParMan[i] = 0xff;
		}
	}
	//带擦除
	if (par_IndexW == 1)
	{
		for(k=0;k<3;k++)//如果写入不成功，则连写入3次，均不成功，不再写
		{
			ICWriteBuf1(0, _ParMan, F_PAGE_SIZE);
			ICBuf1ToPage(Monitor_mana_IndexW);
			
			ICReadPage(CRC_DATA, Monitor_mana_IndexW, 0, F_PAGE_SIZE);
			if(MemCmp((uint8_t*)(_ParMan),(uint8_t*)(CRC_DATA),F_PAGE_SIZE))
				break;
		}		
	}
	//不带擦除
	else
	{
		for(k=0;k<3;k++)//如果写入不成功，则连写入3次，均不成功，不再写
		{
			ICWriteBuf1(0, _ParMan, F_PAGE_SIZE);
			ICBuf1ToPageN(Monitor_mana_IndexW);
			
			ICReadPage(CRC_DATA, Monitor_mana_IndexW, 0, F_PAGE_SIZE);
			if(MemCmp((uint8_t*)(_ParMan),(uint8_t*)(CRC_DATA),F_PAGE_SIZE))
				break;
		}
	}
	
	rt_thread_delay(10);
	
	valW = CRC16_Dn_Cal(W_SysBuff->dataRam, W_SysBuff->Len - 2, 0xFFFF);
	W_SysBuff->dataRam[W_SysBuff->Len-2] = valW;
	W_SysBuff->dataRam[W_SysBuff->Len-1] = valW >> 8;

	for(k=0;k<3;k++)//如果写入不成功，则连写入3次，均不成功，不再写
	{
		ICWriteBuf1(0, W_SysBuff->dataRam, W_SysBuff->Len);
		ICBuf1ToPage(par_IndexW-1);
		
		ICReadPage(CRC_DATA, par_IndexW-1, 0, W_SysBuff->Len);
		if(MemCmp((uint8_t*)(W_SysBuff->dataRam),(uint8_t*)(CRC_DATA),W_SysBuff->Len))
			break;
	}
}

void Flash_Read_ERR(PDATA_RAM fBuf)
{
	u8 IndexW;
	DATA_RAM *R_ErrBuff;
	
	IndexW = err_base_IndexW + ((fBuf->Number - 1) * 4) + fBuf->Index;
	
	ICReadPage(R_ErrBuff->dataRam, IndexW, 0, fBuf->Len);
	rt_memcpy(fBuf->dataRam, R_ErrBuff->dataRam, fBuf->Len);
}


void flash_DataWrite(PDATA_RAM fBuf)
{
	switch (fBuf->Info_type)
	{
		case FLASH_WRITE_SYS :
			Flash_Write_SYS(fBuf);
			break;
		case FLASH_WRITE_ERR :
			Flash_Write_ERR(fBuf);
			break;
		case FLASH_WRITE_AD :
			break;
		default:
			break;
	}
}

void flash_DataRead(PDATA_RAM fBuf)
{
	switch (fBuf->Info_type)
	{
		case FLASH_READ_SYS :
			Flash_Read_SYS(fBuf);
			break;
		case FLASH_READ_ERR :
			Flash_Read_ERR(fBuf);
			break;
		case FLASH_WRITE_AD :
			break;
		default:
			break;
	}
}

