#ifndef _FLASH_H_
#define _FLASH_H_

#include <rtthread.h>
#include "stm32f4xx.h"
#include "app_record.h"

#define FLASH_READ_AD        (0x01)
#define FLASH_READ_SYS       (0x02)
#define FLASH_READ_ERR		   (0x03)
#define FLASH_WRITE_AD       (0x04)
#define FLASH_WRITE_SYS  	   (0x05)
#define FLASH_WRITE_ERR		   (0x06)

//typedef struct _flash_buffer_
//{
//	uint16_t Info_type;
//	uint16_t size;
//	uint16_t Number;
//	uint8_t  Index;
//	uint8_t  payload[528];
//}FBuff, *pFBuff;

void flash_DataWrite(PDATA_RAM fBuf);
void flash_DataRead(PDATA_RAM fBuf);

#endif
