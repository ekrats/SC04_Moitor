#ifndef _CAN_TX_QUEUE_H_
#define _CAN_TX_QUEUE_H_

#include "canQueue.h"

typedef struct _Can_Tx_Ram_
{
	CAN_DATA_RAM parent;
	CanTxMsg     data;

}CAN_TX_DATA_RAM, *PCAN_TX_DATA_RAM;

#define canTxQueueSize  20

class TxQueue
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
	uint32_t QueueRam[canTxQueueSize];
	PCAN_TX_DATA_RAM DataFIFO;
public:
	
	struct rt_semaphore can_OPsem;

	TxQueue(void);
	void QueueInit(void);
	void Init(void);
	void pop_Q(uint32_t msg);
	uint16_t GetFifoReg(void);
	void SetFifoReg(uint16_t val);
	uint32_t push_Q(void);
	void reset(void);
	void Push_Msg(uint8_t port, CanTxMsg* msg);
};

#endif
