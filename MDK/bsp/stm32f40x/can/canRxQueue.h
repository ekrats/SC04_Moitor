#ifndef _CAN_RX_QUEUE_H_
#define _CAN_RX_QUEUE_H_

#include "canQueue.h"

typedef struct _Can_Rx_Ram_
{
	CAN_DATA_RAM parent;
	CanRxMsg     data;

}CAN_RX_DATA_RAM, *PCAN_RX_DATA_RAM;

#define canRxQueueSize  20

class RxQueue
{
private:
	uint32_t QEntriesMax;
	uint32_t *QStart;
	uint32_t *QEnd;
	uint32_t *QIn;
	uint32_t *QOut;
	uint32_t  QSize;
	uint32_t  QEntries;
	uint16_t  QInitRegister;			// ¶ÓÁÐ³õÊ¼»¯×´Ì¬¼Ä´æÆ÷
	uint16_t  Register;					// »º³åÇø×´Ì¬¼Ä´æÆ÷
	uint32_t  FifoRamPtr;
	uint32_t QueueRam[canRxQueueSize];
	PCAN_RX_DATA_RAM DataFIFO;
public:
	
	struct rt_semaphore can_OPsem;

	RxQueue(void);
	void QueueInit(void);
	void Init(void);
	void pop_Q(uint32_t msg);
	uint16_t GetFifoReg(void);
	void SetFifoReg(uint16_t val);
	uint32_t push_Q(void);
	void reset(void);
	void Push_Msg(uint8_t port, CanRxMsg* msg);
};

#endif
