#include "app_record.h"
#include "rthw.h"
#include <rtthread.h>
#include "stm32f4xx.h"
#include "stdio.h"
#include "string.h"
#include "app_flash.h"



//------------------------------------------------------------------------
// 创建Flash操作任务的信号量,队列
//------------------------------------------------------------------------
struct rt_semaphore sem_record_task;

typedef  struct  _data_queue_    
{
	uint32_t      QEntriesMax;
	uint32_t     *QStart;                          
	uint32_t     *QEnd;                                     /* Ptr to end   of Rx Q data                            */
	uint32_t     *QIn;                                      /* Ptr where next msg will be inserted  in   Rx Q       */
	uint32_t     *QOut;                                     /* Ptr where next msg will be extracted from Rx Q       */
	uint32_t      QSize;                                    /* Size of Rx Q (maximum number of entries)             */
	uint32_t      QEntries;                                 /* Current number of entries in the Rx Q                */
} DATA_Q;


//------------------------------------------------------------------------
// Flash读写操作标志
//------------------------------------------------------------------------
#define READMASK               (0x02)
#define WRITEMASK              (0x01)

#define DRAM_SIZE              (sizeof(DATA_RAM) * FLASH_QUEUE_SIZE)

DATA_Q  _flash_dataQ;
DATA_Q  *pFlashDataQ = &_flash_dataQ; 

//------------------------------------------------------------------------
// 真正的数据FIFO    配合QUEUE 使用 使用外部RAM分配空间
// 在 System_DataRecord_Init() 函数中分配了内存空间
//------------------------------------------------------------------------
//DATA_RAM Flash_Databuff[FLASH_QUEUE_SIZE] = {0};
PDATA_RAM Flash_DataWriteFIFO;
//------------------------------------------------------------------------
// FIFO操作用指针 环形FIFO
//------------------------------------------------------------------------
uint32_t flashRam_Ptr = 0;

//------------------------------------------------------------------------
// 队列消息RAM区
//------------------------------------------------------------------------
uint32_t FLASH_OP_Q[FLASH_QUEUE_SIZE] = {0};

//------------------------------------------------------------------------
// 函数声明区
//------------------------------------------------------------------------
static  void  rt_Q_Add(uint32_t msg);	 
static  uint32_t   rt_Q_Remove(void);
static  void  rt_Q_Reset(void);
static  void  rt_Q_Init(void);

int DataRecord_Init(void);
void  Bsp_DataRecord(void);

/*******************************************************************************
* Function Name  : _FlashWrite
* Description    : This task 
* 
* Input          : pFBuff fBuf  写入缓冲数据块指针
* 
* Output         : None
* Return         : None
*******************************************************************************/

void _FlashWrite(pFBuff fBuf)
{
	uint16_t _size = 0;

	//------------------------------------------------------------------------
	// 将数据导入FIFO
	//------------------------------------------------------------------------
	Flash_DataWriteFIFO[flashRam_Ptr].Info_type = fBuf->Info_type;
	Flash_DataWriteFIFO[flashRam_Ptr].Number = fBuf->Number;
	Flash_DataWriteFIFO[flashRam_Ptr].Index = fBuf->Index;

	if (fBuf->size > FLASh_BUF_SIZE)
	{
		_size = FLASh_BUF_SIZE;
	}
	else
	{
		_size = fBuf->size;
	}

	//------------------------------------------------------------------------
	// 缓冲区数据更新入FIFO
	//------------------------------------------------------------------------
	memcpy((void*)(Flash_DataWriteFIFO[flashRam_Ptr].dataRam), 
		(void*)(fBuf->payload), 
		_size);
	
	Flash_DataWriteFIFO[flashRam_Ptr].Len = fBuf->size;	
	
	//------------------------------------------------------------------------
	// 将FIFO当前数据块地址压入队列
	//------------------------------------------------------------------------
	rt_Q_Add((uint32_t)(&Flash_DataWriteFIFO[flashRam_Ptr]));
	//------------------------------------------------------------------------
	// 维护FIFO区操作指针
	//------------------------------------------------------------------------
	flashRam_Ptr++;
	if(flashRam_Ptr >= FLASH_QUEUE_SIZE) flashRam_Ptr = 0;
	//------------------------------------------------------------------------
	// 弹出信号量,指示FLash任务, 进行数据操作
	//------------------------------------------------------------------------
	rt_sem_release(&sem_record_task);
}

/*******************************************************************************
* Function Name  : BspTaskFlash
* Description    : _FlashRead 
* 
* Input          : 
* 
* Output         : None
* Return         : None
*******************************************************************************/

void _FlashRead(pFBuff fBuf)
{
	DATA_RAM pBufRam;
	uint16_t _size = 0;
	//------------------------------------------------------------------------
	// 将数据导入FIFO
	//------------------------------------------------------------------------
	pBufRam.Info_type = fBuf->Info_type;
	pBufRam.Index = fBuf->Index;
	pBufRam.Len = fBuf->size;
	
	if (fBuf->size > FLASh_BUF_SIZE)
	{
		_size = FLASh_BUF_SIZE;
	}
	else
	{
		_size = fBuf->size;
	}
	
	pBufRam.Len = _size;
		
	flash_DataRead(&pBufRam);
	
	memcpy( (void*)(fBuf->payload), 
			(void*)(pBufRam.dataRam),
		_size);
}

/*******************************************************************************
* Function Name  : static void System_record_inittask(void* parameter)
* Description    :  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void System_RecordQueue_init(void* parameter)
{
	DataRecord_Init();

	rt_kprintf("\r\n\r\n");
	for(;;)
	{
		Bsp_DataRecord();
	}
}

int DataRecord_Init(void)
{
	rt_err_t rtv;
	//-------------------------------------------------------
	// 创建信号量
	//-------------------------------------------------------
	rtv  = rt_sem_init(&sem_record_task,  "record",  1,  RT_IPC_FLAG_FIFO);
	//-------------------------------------------------------
	// 申请队列缓冲内存 
	//-------------------------------------------------------
	Flash_DataWriteFIFO = rt_malloc(DRAM_SIZE);
	//-------------------------------------------------------
	// 初始化FIFO操作指针, 该指针清零处理 
	//-------------------------------------------------------
	flashRam_Ptr = 0;	
	//-------------------------------------------------------
	// 队列初始化
	//-------------------------------------------------------
	rt_Q_Init();

	if((rtv == RT_EOK) && (Flash_DataWriteFIFO != RT_NULL))
	{
		return 0;
	}
}

/*******************************************************************************
* Function Name  : static  void  rt_Q_Add(uint32_t   msg)
* Description    : 队列API : 压入操作  
*                   
*
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

static  void  rt_Q_Add(uint32_t   msg)
{
	uint32_t entries;
	register rt_base_t temp;

	/* lock interrupt */
	temp = rt_hw_interrupt_disable();

	if (pFlashDataQ->QEntries >= pFlashDataQ->QSize) {          /* Make sure queue is not full.                         */
		/* unlock interrupt */
		rt_hw_interrupt_enable(temp);

		return;
	}


	//压入队列
	*pFlashDataQ->QIn++ = msg;                                  /* Insert message into queue.                           */
	//队列为下次压入进行指针维护
	pFlashDataQ->QEntries++;                                    /* Update the nbr of entries in the queue.              */
	if (pFlashDataQ->QIn == pFlashDataQ->QEnd) {              /* Wrap IN ptr when at end of queue.                    */
		pFlashDataQ->QIn  = pFlashDataQ->QStart;
	}
	//更新队列压入的最大值
	entries = pFlashDataQ->QEntries;
	if (pFlashDataQ->QEntriesMax < entries) {
		pFlashDataQ->QEntriesMax = entries;
	}

	/* unlock interrupt */
	rt_hw_interrupt_enable(temp);
}

/*******************************************************************************
* Function Name  : static  uint32_t  rt_Q_Remove(void)
* Description    : 队列API : 弹出操作  
*                   
*
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

static  uint32_t  rt_Q_Remove(void)
{
	uint32_t     msg;
	register rt_base_t temp;

	/* lock interrupt */
	temp = rt_hw_interrupt_disable();

	if (pFlashDataQ->QEntries == 0) {                           /* Chk for queue empty.                                 */
		msg  = QUEUE_NULL;
		/* unlock interrupt */
		rt_hw_interrupt_enable(temp);

		return(msg);
	}
	//弹出数据
	msg = *pFlashDataQ->QOut++;                                 /* Extract oldest message from the queue.               */
	//队列为下次弹出进行指针维护
	pFlashDataQ->QEntries--;                                    /* Update the number of entries in the queue            */
	if (pFlashDataQ->QOut == pFlashDataQ->QEnd) {             /* Wrap OUT ptr when at the end of the queue.           */
		pFlashDataQ->QOut  = pFlashDataQ->QStart;
	}
	/* unlock interrupt */
	rt_hw_interrupt_enable(temp);
	//返回弹出的数据
	return (msg);                                               /* Return message received.                             */
}

/*******************************************************************************
* Function Name  : static  void  rt_Q_Reset(void)
* Description    : 队列API : 复位操作 
*                   
*
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

static  void  rt_Q_Reset(void)
{
	pFlashDataQ->QIn      = pFlashDataQ->QStart;
	pFlashDataQ->QOut     = pFlashDataQ->QStart;
	pFlashDataQ->QEntries = 0;
}

/*******************************************************************************
* Function Name  : static  void  rt_Q_Init(void)
* Description    : 队列API : 初始化操作
*                   
*
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

static  void  rt_Q_Init(void)
{
	pFlashDataQ->QStart      =  FLASH_OP_Q;     /*  Initialize Rx Q.   */
	pFlashDataQ->QEnd        = &FLASH_OP_Q[FLASH_QUEUE_SIZE - 1];
	pFlashDataQ->QSize       =  FLASH_QUEUE_SIZE;
	pFlashDataQ->QEntriesMax = 0;
	rt_Q_Reset();    /* Reset Rx status words queue.    */
}


/*******************************************************************************
* Function Name  : void  mBsp_DataRecord(void)
* Description    : Flash操作任务
*                   
*
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void  Bsp_DataRecord(void)
{
	uint32_t _buffRamPtr;
	DATA_RAM * pbufram;


	//等待写操作信号量
	rt_sem_take(&sem_record_task, RT_WAITING_FOREVER);

	for(;;)
	{
		
		//从队列弹出FIFO首地址
		_buffRamPtr = rt_Q_Remove();

		//弹出指针不为空,则表示队列操作成功
		if (_buffRamPtr != QUEUE_NULL)
		{
			//------------------------------------------------------------
			// 转换指针类型: RAM操作数据块
			//------------------------------------------------------------
			pbufram = (DATA_RAM *)_buffRamPtr;

			//------------------------------------------------------------ 
			// 根据数据块操作类型, 确定写入操作类型
			//------------------------------------------------------------
		
			flash_DataWrite(pbufram);
		}
		else
		{
			break;
		}
	}
}

/*******************************************************************************
* Function Name  : int Record_Task_create(uint8_t priority)
* Description    :  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int Record_Task_create(uint8_t priority)
{
	rt_thread_t tid;

	tid = rt_thread_create("RecordQ",
		System_RecordQueue_init, RT_NULL,
		2048, priority, 20);

	if (tid != RT_NULL)
		rt_thread_startup(tid);

	return 0;
}
