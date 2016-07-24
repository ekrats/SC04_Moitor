#ifndef _APP_QUEUE_H_
#define _APP_QUEUE_H_

/* Includes ------------------------------------------------------------------*/
#include <rtthread.h>
#include "stm32f4xx.h"

/* Exported macro ------------------------------------------------------------*/
#define FLASH_READ_AD        (0x01)
#define FLASH_READ_SYS       (0x02)
#define FLASH_READ_ERR		   (0x03)
#define FLASH_WRITE_AD       (0x04)
#define FLASH_WRITE_SYS  	   (0x05)
#define FLASH_WRITE_ERR		   (0x06)

/* Private typedef -----------------------------------------------------------*/
#define  FLASH_QUEUE_SIZE  (20)
#define  FLASh_BUF_SIZE     (528)
#define  QUEUE_NULL        (0)

//------------------------------------------------------------------------
// Queue队列每条信息数据结构体
//------------------------------------------------------------------------

typedef struct _Data_Ram_
{
	uint16_t   Info_type;
	uint16_t   Len;
	uint16_t   Number;
	uint8_t    Index;
	uint8_t    dataRam[FLASh_BUF_SIZE];
}DATA_RAM, *PDATA_RAM;


typedef struct _flash_buffer_
{
	uint16_t Info_type;
	uint16_t size;
	uint16_t Number;
	uint8_t  Index;
	uint8_t  payload[528];
}FBuff, *pFBuff;

/* Exported functions ------------------------------------------------------- */

void _FlashWrite(pFBuff fBuf);
int Record_Task_create(uint8_t priority);
void _FlashRead(pFBuff fBuf);

#endif
