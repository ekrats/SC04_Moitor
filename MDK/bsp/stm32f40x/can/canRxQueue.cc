#include "canRxQueue.h"
#include "stm32f4xx.h"
#include <rtthread.h>
#include "rthw.h"

#define canRxRamSize          (sizeof(CAN_RX_DATA_RAM) * canRxQueueSize)

RxQueue::RxQueue(void)
{
	
}
//------------------------------------------------------------------
// 接收发送队列信号量
//------------------------------------------------------------------

void RxQueue::QueueInit(void)
{
	this->DataFIFO = (CAN_RX_DATA_RAM *)rt_malloc(canRxRamSize);
	this->FifoRamPtr = 0;
	this->Register = FIFO_EMPTY_MASK;
	this->Init();
	//-------------------------------------------------------
	// 队列初始化完成，置位初始化状态寄存器
	//-------------------------------------------------------
	this->QInitRegister = INIT_OK;
	rt_kprintf("Queue init OK!");
}

void RxQueue::Push_Msg(uint8_t port, CanRxMsg* msg)
{
	if (this->Register != FIFO_FULL_MASK)
	{
		this->DataFIFO[this->FifoRamPtr].parent.CanPort = port;
		this->DataFIFO[this->FifoRamPtr].data.StdId = msg->StdId;
		this->DataFIFO[this->FifoRamPtr].data.ExtId = msg->ExtId;
		this->DataFIFO[this->FifoRamPtr].data.IDE = msg->IDE;
		this->DataFIFO[this->FifoRamPtr].data.RTR = msg->RTR;
		this->DataFIFO[this->FifoRamPtr].data.DLC = msg->DLC;
		this->DataFIFO[this->FifoRamPtr].data.Data[0] = msg->Data[0];
		this->DataFIFO[this->FifoRamPtr].data.Data[1] = msg->Data[1];
		this->DataFIFO[this->FifoRamPtr].data.Data[2] = msg->Data[2];
		this->DataFIFO[this->FifoRamPtr].data.Data[3] = msg->Data[3];
		this->DataFIFO[this->FifoRamPtr].data.Data[4] = msg->Data[4];
		this->DataFIFO[this->FifoRamPtr].data.Data[5] = msg->Data[5];
		this->DataFIFO[this->FifoRamPtr].data.Data[6] = msg->Data[6];
		this->DataFIFO[this->FifoRamPtr].data.Data[7] = msg->Data[7];
		this->DataFIFO[this->FifoRamPtr].data.FMI = msg->FMI;
		
		//------------------------------------------------------------------------
		// 将FIFO当前数据块地址压入队列
		//------------------------------------------------------------------------
		this->pop_Q((uint32_t)(&this->DataFIFO[this->FifoRamPtr]));
		//------------------------------------------------------------------------
		// 维护FIFO区操作指针
		//------------------------------------------------------------------------
		this->FifoRamPtr++;
		if (this->FifoRamPtr >= canRxQueueSize)
			this->FifoRamPtr = 0;
		//------------------------------------------------------------------------
		// 弹出信号量, 进行数据操作
		//------------------------------------------------------------------------
		rt_sem_release(&this->can_OPsem);
	}
}



void RxQueue::pop_Q(uint32_t msg)
{
	register rt_base_t temp;
	
	//队列未初始化，不进行压栈操作
	if(this->QInitRegister  != INIT_OK) 
		return;
	
	/* lock interrupt */
	temp = rt_hw_interrupt_disable();

	if (this->QEntries >= this->QSize) 
	{
		/* unlock interrupt */
		rt_hw_interrupt_enable(temp);
		
		//队列已满，置位寄存器
		this->Register = FIFO_FULL_MASK;
		return;
	}
	//压入队列
	*this->QIn++ = msg;
	//队列为下次压入进行指针维护
	this->QEntries++;
	if (this->QIn == this->QEnd)
	{
		this->QIn = this->QStart;
	}
	if (this->QEntriesMax < this->QEntries)
	{
		this->QEntriesMax = this->QEntries;
	}
	
	if (this->QEntriesMax == this->QSize)
	{
		for(;;)
			break;
	}
	/* unlock interrupt */
	rt_hw_interrupt_enable(temp);
}
/*******************************************************************************
* Function Name  : static uint32_t rt_Q_Remove(void)
* Description    : 队列API : 弹出操作  
*                   
*
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

uint32_t RxQueue::push_Q(void)
{
	uint32_t  msg;
	register rt_base_t temp;
	/* lock interrupt */
	temp = rt_hw_interrupt_disable();
	
	if (this->QEntries == 0) 
	{
		msg = QUEUE_NULL;
		/* unlock interrupt */
		rt_hw_interrupt_enable(temp);
		
		return (msg);
	}
	//弹出数据
	msg = *this->QOut++;
	//队列为下次弹出进行指针维护
	this->QEntries--;
	if (this->QOut == this->QEnd)
	{
		this->QOut = this->QStart;
	}
	
	/* unlock interrupt */
	rt_hw_interrupt_enable(temp);
	return (msg);
}
/*******************************************************************************
* Function Name  : static void rt_Q_Reset(void)
* Description    : 队列API : 复位操作 
*                   
*
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void RxQueue::reset(void)
{
	this->QIn = this->QStart;
	this->QOut = this->QStart;
	this->QEntries = 0;
}
/*******************************************************************************
* Function Name  : static void rt_Q_Init(void)
* Description    : 队列API : 初始化操作
*                   
*
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void RxQueue::Init(void)
{
	this->QStart 	  = this->QueueRam;
	this->QEnd 	 	  = &this->QueueRam[canRxQueueSize-1];
	this->QSize  	  = canRxQueueSize;
	this->QEntriesMax = 0;
	
	this->reset();
}

/*******************************************************************************
* Function Name  : uint16_t CanQ_Rx_GetFifoReg(void)
* Description    : 获取队列状态寄存器 
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t RxQueue::GetFifoReg(void)
{
	return this->Register;
}

/*******************************************************************************
* Function Name  : void CanQ_SetFifoReg(uint16_t val)
* Description    : 设置队列状态寄存器 
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void RxQueue::SetFifoReg(uint16_t val)
{
	this->Register = val;
}

