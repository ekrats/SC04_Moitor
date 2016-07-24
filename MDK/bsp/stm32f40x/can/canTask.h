#ifndef _APP_CAN_H_
#define _APP_CAN_H_
#include "canTxQueue.h"
#include "canRxQueue.h"

extern "C" {
	#include "Bsp_can.h"
}

#define CAN_RX_TIME            (200)
//------------------------------------------------------------------
// Event
//------------------------------------------------------------------
#define CanTask_Event_Cycle        ((uint32_t)0x01)
#define CanTask_Event_Trigger      ((uint32_t)0x02)

void Bsp_cantask_create(uint8_t priority);
extern "C" void Bsp_can_send_cycle_event(void);
void Bsp_can_send_trigger_event(void);

extern TxQueue canTxQueue;
extern RxQueue canRxQueue;

#endif
