#ifndef _sysInfo_h_
#define _sysInfo_h_
#include "stm32f4xx.h"
#include <rtthread.h>

typedef struct {
	u16 stu[32];
}SYS_INF_TYPE, *PSYS_INF_TYPE;
#define SYS_INFO_SIZE			64

class Sys_Info
{
private:
	uint32_t  FifoRamPtr;
public:
	PSYS_INF_TYPE DataFIFO;
	SYS_INF_TYPE  Data[SYS_INFO_SIZE];
	
	void sysInfInit(void);
	void sysInfoRecord(void);

};

#ifdef _DS_SYS_INFO_
Sys_Info sysInfo;
#else
extern Sys_Info sysInfo;
#endif


#endif
