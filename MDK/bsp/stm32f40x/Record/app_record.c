#include "app_record.h"
#include "rthw.h"
#include <rtthread.h>
#include "stm32f4xx.h"
#include "stdio.h"
#include "string.h"
#include "app_flash.h"



//------------------------------------------------------------------------
// ����Flash����������ź���,����
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
// Flash��д������־
//------------------------------------------------------------------------
#define READMASK               (0x02)
#define WRITEMASK              (0x01)

#define DRAM_SIZE              (sizeof(DATA_RAM) * FLASH_QUEUE_SIZE)

DATA_Q  _flash_dataQ;
DATA_Q  *pFlashDataQ = &_flash_dataQ; 

//------------------------------------------------------------------------
// ����������FIFO    ���QUEUE ʹ�� ʹ���ⲿRAM����ռ�
// �� System_DataRecord_Init() �����з������ڴ�ռ�
//------------------------------------------------------------------------
//DATA_RAM Flash_Databuff[FLASH_QUEUE_SIZE] = {0};
PDATA_RAM Flash_DataWriteFIFO;
//------------------------------------------------------------------------
// FIFO������ָ�� ����FIFO
//------------------------------------------------------------------------
uint32_t flashRam_Ptr = 0;

//------------------------------------------------------------------------
// ������ϢRAM��
//------------------------------------------------------------------------
uint32_t FLASH_OP_Q[FLASH_QUEUE_SIZE] = {0};

//------------------------------------------------------------------------
// ����������
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
* Input          : pFBuff fBuf  д�뻺�����ݿ�ָ��
* 
* Output         : None
* Return         : None
*******************************************************************************/

void _FlashWrite(pFBuff fBuf)
{
	uint16_t _size = 0;

	//------------------------------------------------------------------------
	// �����ݵ���FIFO
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
	// ���������ݸ�����FIFO
	//------------------------------------------------------------------------
	memcpy((void*)(Flash_DataWriteFIFO[flashRam_Ptr].dataRam), 
		(void*)(fBuf->payload), 
		_size);
	
	Flash_DataWriteFIFO[flashRam_Ptr].Len = fBuf->size;	
	
	//------------------------------------------------------------------------
	// ��FIFO��ǰ���ݿ��ַѹ�����
	//------------------------------------------------------------------------
	rt_Q_Add((uint32_t)(&Flash_DataWriteFIFO[flashRam_Ptr]));
	//------------------------------------------------------------------------
	// ά��FIFO������ָ��
	//------------------------------------------------------------------------
	flashRam_Ptr++;
	if(flashRam_Ptr >= FLASH_QUEUE_SIZE) flashRam_Ptr = 0;
	//------------------------------------------------------------------------
	// �����ź���,ָʾFLash����, �������ݲ���
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
	// �����ݵ���FIFO
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
	// �����ź���
	//-------------------------------------------------------
	rtv  = rt_sem_init(&sem_record_task,  "record",  1,  RT_IPC_FLAG_FIFO);
	//-------------------------------------------------------
	// ������л����ڴ� 
	//-------------------------------------------------------
	Flash_DataWriteFIFO = rt_malloc(DRAM_SIZE);
	//-------------------------------------------------------
	// ��ʼ��FIFO����ָ��, ��ָ�����㴦�� 
	//-------------------------------------------------------
	flashRam_Ptr = 0;	
	//-------------------------------------------------------
	// ���г�ʼ��
	//-------------------------------------------------------
	rt_Q_Init();

	if((rtv == RT_EOK) && (Flash_DataWriteFIFO != RT_NULL))
	{
		return 0;
	}
}

/*******************************************************************************
* Function Name  : static  void  rt_Q_Add(uint32_t   msg)
* Description    : ����API : ѹ�����  
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


	//ѹ�����
	*pFlashDataQ->QIn++ = msg;                                  /* Insert message into queue.                           */
	//����Ϊ�´�ѹ�����ָ��ά��
	pFlashDataQ->QEntries++;                                    /* Update the nbr of entries in the queue.              */
	if (pFlashDataQ->QIn == pFlashDataQ->QEnd) {              /* Wrap IN ptr when at end of queue.                    */
		pFlashDataQ->QIn  = pFlashDataQ->QStart;
	}
	//���¶���ѹ������ֵ
	entries = pFlashDataQ->QEntries;
	if (pFlashDataQ->QEntriesMax < entries) {
		pFlashDataQ->QEntriesMax = entries;
	}

	/* unlock interrupt */
	rt_hw_interrupt_enable(temp);
}

/*******************************************************************************
* Function Name  : static  uint32_t  rt_Q_Remove(void)
* Description    : ����API : ��������  
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
	//��������
	msg = *pFlashDataQ->QOut++;                                 /* Extract oldest message from the queue.               */
	//����Ϊ�´ε�������ָ��ά��
	pFlashDataQ->QEntries--;                                    /* Update the number of entries in the queue            */
	if (pFlashDataQ->QOut == pFlashDataQ->QEnd) {             /* Wrap OUT ptr when at the end of the queue.           */
		pFlashDataQ->QOut  = pFlashDataQ->QStart;
	}
	/* unlock interrupt */
	rt_hw_interrupt_enable(temp);
	//���ص���������
	return (msg);                                               /* Return message received.                             */
}

/*******************************************************************************
* Function Name  : static  void  rt_Q_Reset(void)
* Description    : ����API : ��λ���� 
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
* Description    : ����API : ��ʼ������
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
* Description    : Flash��������
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


	//�ȴ�д�����ź���
	rt_sem_take(&sem_record_task, RT_WAITING_FOREVER);

	for(;;)
	{
		
		//�Ӷ��е���FIFO�׵�ַ
		_buffRamPtr = rt_Q_Remove();

		//����ָ�벻Ϊ��,���ʾ���в����ɹ�
		if (_buffRamPtr != QUEUE_NULL)
		{
			//------------------------------------------------------------
			// ת��ָ������: RAM�������ݿ�
			//------------------------------------------------------------
			pbufram = (DATA_RAM *)_buffRamPtr;

			//------------------------------------------------------------ 
			// �������ݿ��������, ȷ��д���������
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
