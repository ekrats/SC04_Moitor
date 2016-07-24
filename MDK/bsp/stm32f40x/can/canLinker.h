#ifndef _CAN_LINKER_H_
#define _CAN_LINKER_H_
#include "canTxQueue.h"
#include "canRxQueue.h"

void CanApp_CycleStream_Server(void);
void CanApp_TriggerStream_Server(void);

void _App_Can_Rx(void* pbuf);
uint8_t _App_Can_Tx(void* pbuf);

void can_Tx_Push_Msg(uint8_t port, CanTxMsg* msg);
extern "C" void can_Rx_Push_Msg(uint8_t port, CanRxMsg* msg);

#endif
