/**
  *************************************************************************
  * This file is part of Platform
  * COPYRIGHT (C) 1999-2015, TongYe Corporation All rights reserved.
  * @file    hd_crc32.h
  * @brief   This file contains all the functions prototypes for the   
  *          hardware crc32 firmware library.
  * Change Logs:
  * Date           Author       Notes
  * 2015/01/05     Coase        the first version
  *************************************************************************
  */

#include "stm32f4xx.h"
#include "rtthread.h"
#include "stdint.h"
#ifndef __HD_CRC32__H__
#define __HD_CRC32__H__

#ifdef __cplusplus
extern "C" {
#endif
    /**
     * @brief STM32Fxx硬件CRC功能初始化
     *
     *
     * @return NULL
     */
    void hardware_crc32_init(void);

    /**
     * @brief 获取4字节CRC校验码
     *
     * @param buffer 需要计算CRC校验码的数据缓存
     * @param size  必须不小于4,且为4的整倍数
     *
     * @return 
     */
    u32 get_crc(void* buffer, int size);

    /**
     * @brief 添加CRC校验码于buffer的末四位
     *
     * @param buffer 需要计算CRC校验码的数据缓存
     * @param size 必须大于等于8,且为4的整倍数
     *
     * @return null
     */
    rt_inline void add_crc(void* buffer, int size)
    {
        size -= 4;
        *((u32*)((u8* )buffer + size)) = get_crc(buffer, size);
    }

    /**
     * @brief 添加CRC校验码于buffer之后
     *
     * @param buffer 需要计算CRC校验码的数据缓存, 
     *  缓存必须预留4字节空间以防越界
     * @param size   必须大于等于4,且为4的整倍数
     *
     * @return null
     */
    rt_inline void add_crc_at_end(void* buffer, int size)
    {
        *((u32*)((u8* )buffer + size)) = get_crc(buffer, size);
    }


    /**
     * @brief 校验CRC数据
     *
     * @param buffer 需要计算CRC校验码的数据缓存,其末四位为校验码
     * @param size 必须大于等于8,且为4的整倍数
     *
     * @return null
     */
    rt_inline rt_bool_t check_crc(void * buffer, int size)
    {
        if(size > 4)
        {
            return ( (*(u32 *)((u8*)buffer + size - 4)) == get_crc(buffer, size - 4) );
        }
        else
        {
            return RT_FALSE;
        }
    }
#ifdef __cplusplus
}
#endif

#endif

/******************* (C) COPYRIGHT 2015 TongYe **********END OF FILE****/


/*@}*/

