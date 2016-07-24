#include "config.h"
#include <rtthread.h>
#include "stm32f4xx.h"


/*********************************************************************************************************
** Function name:     uint8_t StrCompare(uint8_t* pstr1,uint8_t* pstr2,uint32_t length)   
** Descriptions:      ������������бȽ�
** input parameters:  pstr1  ��һ������ָ��
pstr2  �ڶ�������ָ��
length ����
** output parameters: ��
** Returned value:    ����������ͬ�򷵻�1
�������鲻ͬ�򷵻�0
*********************************************************************************************************/

uint8_t MemCmp(uint8_t* pstr1, uint8_t* pstr2, uint32_t length)
{
	uint32_t i;
	for(i=0;i<length;i++)
	{
		if(*pstr1 != *pstr2)
			return 0;
		else
		{
			pstr1++;
			pstr2++;
		}
	}
	return 1;
}

u16 Merge_fun(u8 TH0, u8 TL0)
{
	u16 temp = 0;
	u16 LOW = 0x0000, HIGH = 0x0000;
	
	HIGH = TH0;
	LOW  = TL0;
	
	temp = LOW | (HIGH << 8);
	
	return temp;
}

uint32_t get_unix_time(void)
{
    return 1465555555;
}
