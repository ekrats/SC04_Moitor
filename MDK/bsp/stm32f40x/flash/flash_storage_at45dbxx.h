/**
  *************************************************************************
  * This file is part of Platform
  * COPYRIGHT (C) 1999-2015, TongYe Corporation All rights reserved.
  * @file    flash_storage.h
  * @brief   This file contains all the functions prototypes for the   
  *          flash storage firmware library.
  * Change Logs:
  * Date           Author       Notes
  * 2015/01/02     Coase        the first version
  *************************************************************************
  */

#ifndef __FLASH_STORAGE__H__
#define __FLASH_STORAGE__H__
#include "stdint.h"
#include "stdbool.h"
#include "spi_flash_at45dbxx.h"

#ifdef __cplusplus
extern "C" {
#endif
#define MAX_STORAGE_BLOCK 5
#define MAX_RETRY_TIMES 3
#define ILLEGAL_ID 0xffffffff
#define MAX_PAGE_ADDR               (0x3FFF00)
#define PAGE_NUM_MAX                (0x4000)
#define MIN_RW_SIZE                 512

    typedef enum
    {
        RECORD_CATALOG,
        RECORD_PAR,
        RECORD_LOG,
        RECORD_FAULT,
        RECORD_SYSTEM,
    }RECORD_TYPE;

    typedef struct 
    {
        int start_page;
        int end_page;
        int page_per_record;
        int type;
        int current_page;
        int current_id;
    }storage_map;

    typedef struct
    {
        uint32_t id;
        uint32_t unix_time;
        uint8_t data[4];
    }record_header;

    struct flash_storage_device
    {
        struct rt_device    storage_device;
        rt_device_t         spi_flash_device;
        struct rt_mutex     lock;
    };
    
    int storage_distribute(int pages, int page_per_record, RECORD_TYPE type);
    storage_map* get_storage_map(void);
    void set_storage_map(storage_map* map);
    rt_err_t flash_storage_init(const char * flash_storage_name, const char * flash_device_name);
    void flash_resetup(void);
    void flash_setup(void);
#ifdef __cplusplus
}
#endif

#endif

/******************* (C) COPYRIGHT 2015 TongYe **********END OF FILE****/


/*@}*/

