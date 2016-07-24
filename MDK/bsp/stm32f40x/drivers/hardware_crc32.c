/**
  *************************************************************************
  * This file is part of Platform
  * COPYRIGHT (C) 1999-2015, TongYe R&D Team
  * @file    hardware_crc32.c
  * @brief   This file provides all the hardware crc32 firmware functions.
  * Change Logs:
  * Date           Author       Notes
  * 2015/01/05     Coase        the first version
  *************************************************************************
  */

#include "hardware_crc32.h"

rt_mutex_t hardware_crc_lock = RT_NULL;
void hardware_crc32_init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
    hardware_crc_lock = rt_mutex_create("crc32", RT_IPC_FLAG_FIFO);
    RT_ASSERT(hardware_crc_lock != RT_NULL);
}

u32 get_crc(void* buffer, int size)
{
    u32 crc = 0;
    u32 * tmp = (uint32_t *)buffer;
    int payload_word_length = size / 4;
    if(size < 4)
    {
        return 0;
    }
    
    rt_mutex_take(hardware_crc_lock, RT_WAITING_FOREVER);
    {
        int index = 0;
        CRC->CR = CRC_CR_RESET;

        for(index = 0; index < payload_word_length; index++)
        {
            CRC->DR = tmp[index];
        }
        crc = (CRC->DR);
    }
    rt_mutex_release(hardware_crc_lock);
    return crc;
}

/******************* (C) COPYRIGHT 2015 TongYe **********END OF FILE****/


/*@}*/

