/**
  *************************************************************************
  * This file is part of Platform
  * COPYRIGHT (C) 1999-2015, TongYe R&D Team
  * @file    flash_storage.c
  * @brief   This file provides all the flash storage firmware functions.
  * Change Logs:
  * Date           Author       Notes
  * 2015/01/02     Coase        the first version
  *************************************************************************
  */

#include "flash_storage_at45dbxx.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "hardware_crc32.h"

struct rt_device_blk_geometry flash_geometry;
static storage_map storage_maps[MAX_STORAGE_BLOCK] = {0};
static int current_index = 0;
static int current_page = 0;
static struct flash_storage_device flash_storage = {0};

static rt_thread_t flash_storage_thread = RT_NULL;
static struct rt_mailbox fs_mailbox = {0};
static char fs_mailbox_pool[128] = {0};
extern uint32_t get_unix_time(void);

typedef enum
{
    FLASH_STORAGE_READ,
    FLASH_STORAGE_WRITE,
}OPERATE_TYPE;

typedef struct 
{
    RECORD_TYPE type;
    OPERATE_TYPE cmd;
    uint32_t size;
    uint32_t page;
    void (*callback)(void *parameter); /**< callback function */
    void *parameter;
}flash_storage_operate;
static flash_storage_operate * mail_box_rx = 0;

rt_inline void storage_lock(struct flash_storage_device * storage_device)
{
    rt_mutex_take(&storage_device->lock, RT_WAITING_FOREVER);
}

rt_inline void storage_unlock(struct flash_storage_device * storage_device)
{
    rt_mutex_release(&storage_device->lock);
}

bool scan_flash(rt_device_t device, storage_map* map)
{
    bool result = false;
    const int start_page = map->start_page;
    const int start_search_page = map->current_page;
    const int end_page = map->end_page;
    int search_page = map->current_page;
    int pages = map->page_per_record;
    uint8_t * buffer = rt_malloc(pages * flash_geometry.bytes_per_sector);
    uint32_t * p_id = (uint32_t *)buffer;
    //向前增加查找一个扇区空间
    int forward = 8 / pages;
    if(forward < 2)
    {
        forward = 2;
    }
    
    RT_ASSERT(map != RT_NULL);
    RT_ASSERT(buffer != RT_NULL);
    do
    {
        int id = 0;
        rt_device_read(device, search_page, buffer, pages);
        if (check_crc(buffer, flash_geometry.bytes_per_sector * pages))
        {
            id = *p_id;
            if (id > map->current_id)
            {
                map->current_id = id;
                map->current_page = search_page;
                result = true;
            }
            else
            {
                --forward;
            }
        }
        else
        {
            if (*p_id == ILLEGAL_ID)
            {
                --forward;
            }
        }

        search_page += pages;
        if (search_page > end_page)
        {
            search_page = start_page;
        }
    } while ((search_page != start_search_page) && (forward > 0));

    rt_free(buffer);
    return result;
}

storage_map* get_storage_map(void)
{
    return storage_maps;
}

void set_storage_map(storage_map* map)
{
    rt_memcpy(storage_maps, map, sizeof(storage_maps));
}

static bool repair_map(rt_device_t device, storage_map * map )
{
    bool result = false;
    int i = 0;
    //int map_size = sizeof(storage_map) * MAX_STORAGE_BLOCK;

    //rt_memcpy(map, map, map_size);

    //map[0]为map所在信息,若ID为0,其可能是新的at45,或者map信息丢失,需要重建索引表
    for (i = 1; (i < MAX_STORAGE_BLOCK) && (map[i].end_page != 0); i++)
    {
        result = scan_flash(device, &map[i]);
    }

    return result;
}

static bool write_record(const void* buffer, rt_size_t size, RECORD_TYPE type)
{
    storage_map * map = &storage_maps[type];
    int size_per_record = map->page_per_record * flash_geometry.bytes_per_sector;
    bool result = false;
    RT_ASSERT(size <= (size_per_record - sizeof(record_header)));
    
    if(map->current_id != 0)//从未操作过的区域，从本区域的区首开始操作
    {
        map->current_page += map->page_per_record;
        if(map->current_page > map->end_page)
        {
            map->current_page = map->start_page;
        }
    }
    
    map->current_id++;
    
    {
        uint8_t * tmp = rt_malloc(size_per_record);
        record_header *header = (record_header *)tmp;
        rt_memset(tmp, 0, size_per_record);
        
        //处理头尾信息
        header->id = map->current_id;//添加ID
        header->unix_time = get_unix_time();//添加记录时的时间戳
        rt_memcpy(header->data, buffer, size);//复制数据
        add_crc(tmp, size_per_record);//添加CRC校验
        
        //写入至存储器
        {
            int i = 0;
            for(; i < MAX_RETRY_TIMES; i++)
            {
                rt_device_write(flash_storage.spi_flash_device, map->current_page, tmp, map->page_per_record);
                rt_device_read(flash_storage.spi_flash_device, map->current_page, tmp, map->page_per_record);
                if(RT_TRUE == check_crc(tmp, size_per_record))
                {
                    result = true;
                    break;
                }
                else
                {
                    i = i;
                }
            }
        }
        rt_free(tmp);
    }
    
    return result;
}

static bool try_write_record(const void* buffer, rt_size_t size, RECORD_TYPE type)
{
    bool result = false;
    if(write_record(buffer, size, type))
    {
        result = true;
    }
    return result;
}

static void flash_storage_entry(void * parameter)
{
    while (true)
    {
        if(rt_mb_recv(&fs_mailbox, (rt_uint32_t *)&mail_box_rx, RT_WAITING_FOREVER) == RT_EOK)
        {
            if(mail_box_rx != RT_NULL)
            {
                storage_lock(&flash_storage);
                switch (mail_box_rx->cmd)
                {
                case FLASH_STORAGE_READ:
                    if(mail_box_rx->callback != RT_NULL)
                    {
                        //read form storage
                        rt_device_read(flash_storage.spi_flash_device, mail_box_rx->page, 
                            mail_box_rx->parameter, mail_box_rx->size);
                        mail_box_rx->callback(mail_box_rx->parameter);
                    }
                    break;
                case FLASH_STORAGE_WRITE:
                    //write to storage
                    try_write_record(mail_box_rx->parameter, mail_box_rx->size, mail_box_rx->type);
                    rt_free(mail_box_rx->parameter);
                    break;
                }
                rt_free(mail_box_rx);
                mail_box_rx = RT_NULL;
                storage_unlock(&flash_storage);
            }
        }
    }
}

int storage_distribute( int pages, int page_per_record, RECORD_TYPE type )
{
    RT_ASSERT(MAX_STORAGE_BLOCK >= current_index);//超过分配图大小
    RT_ASSERT(type == current_index);//必须以type为分配顺序索引
    RT_ASSERT((current_page + pages) <= PAGE_NUM_MAX);//超过at45空间大小
    storage_maps[current_index].start_page = current_page;
    storage_maps[current_index].current_page = current_page;
    storage_maps[current_index].current_id = 0;
    storage_maps[current_index].page_per_record = page_per_record;
    storage_maps[current_index].type = (int)type;
    current_page += pages;
    storage_maps[current_index].end_page = current_page - 1;
    current_index++;
    return current_page;
}

static rt_size_t flash_storage_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    flash_storage_operate * optr = (flash_storage_operate*)rt_malloc(sizeof(flash_storage_operate));
    optr->callback = RT_NULL;
    optr->cmd = FLASH_STORAGE_WRITE;
    optr->parameter = (void *)buffer;
    optr->size = size;
    optr->type = (RECORD_TYPE)pos;

    rt_mb_send(&fs_mailbox, (rt_uint32_t)optr);
    return 0;
}

static rt_size_t flash_storage_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    rt_size_t result = 0;
    int index = pos & 0xffff;
    int map_index = (pos >> 16) & 0xF;
    storage_map * map = &storage_maps[map_index];
    int max_record = (map->end_page - map->start_page) / map->page_per_record;
    int page = 0;
    int page_offset = 0;
    
    RT_ASSERT(map_index < MAX_STORAGE_BLOCK);
    if((map->current_id > index) && (index < max_record))
    {
        page = map->page_per_record;
        page_offset = map->current_page - index * page;
         
        if(page_offset < map->start_page)
        {
            page_offset += (map->end_page - map->start_page);
        }
    }
    else
    {
        return result;
    }
    
    if(size < 65535)
    {
        storage_lock(&flash_storage);
        result = rt_device_read(((struct flash_storage_device *)dev)->spi_flash_device, page_offset, buffer, page);
        if (check_crc(buffer, flash_geometry.bytes_per_sector * page))
        {
            result = flash_geometry.bytes_per_sector * page;
        }
        else
        {
            result = 0;
        }
        storage_unlock(&flash_storage);
    }
    else if((size & 0xFF000000) == 0x08000000)//
    {
        flash_storage_operate * optr = (flash_storage_operate*)rt_malloc(sizeof(flash_storage_operate));
        optr->cmd = FLASH_STORAGE_READ;
        optr->page = page_offset;
        optr->size = map->page_per_record;
        optr->type = (RECORD_TYPE)map_index;
        optr->callback = (void (*)(void *))size;
        optr->parameter = (void *)buffer;

        rt_mb_send(&fs_mailbox, (rt_uint32_t)optr);
    }
    return result;
}

rt_err_t flash_storage_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    return RT_EOK;
}

rt_err_t flash_storage_init(const char * flash_storage_name, const char * flash_device_name)
{
    rt_device_t device_flash = rt_device_find(flash_device_name);

    /* initialize mutex */
    if (rt_mutex_init(&flash_storage.lock, flash_storage_name, RT_IPC_FLAG_FIFO) != RT_EOK)
    {
        rt_kprintf("init flash_storage lock mutex failed\n");
        return -RT_ENOSYS;
    }

    if(device_flash != RT_NULL)
    {
        rt_device_open(device_flash, RT_DEVICE_OFLAG_RDWR);
        rt_device_control(device_flash, RT_DEVICE_CTRL_BLK_GETGEOME, &flash_geometry);
        flash_storage.spi_flash_device = device_flash;
        
        if (storage_maps[0].current_id == 0)
        {
            if (scan_flash(device_flash, storage_maps))
            {
                int length = storage_maps[0].page_per_record * flash_geometry.bytes_per_sector;
                uint8_t *buffer = rt_malloc(length);
                storage_map * p = (storage_map *)(buffer + 8);
                int read_length = flash_storage_read((rt_device_t)&flash_storage, RECORD_CATALOG << 16, buffer, length);
                if(p[3].end_page == storage_maps[3].end_page)
                {
                    rt_memcpy(storage_maps, buffer + 8, sizeof(storage_maps));
                }
                rt_free(buffer);
            }
        }

        repair_map(device_flash, storage_maps);

        try_write_record(storage_maps, sizeof(storage_maps), RECORD_CATALOG);
    }
    else
    {
        return RT_ERROR;
    }

    rt_mb_init(&fs_mailbox, 
        "fsm",
        &fs_mailbox_pool[0],
        sizeof(fs_mailbox_pool) / 4,
        RT_IPC_FLAG_FIFO
        );

    flash_storage_thread = rt_thread_create("fst",
        flash_storage_entry, RT_NULL,
        1024, 18, 15);
    if (flash_storage_thread != RT_NULL)
    {
        rt_thread_startup(flash_storage_thread);
    }

    flash_storage.storage_device.type = RT_Device_Class_Block;
    flash_storage.storage_device.init = RT_NULL;
    flash_storage.storage_device.open = RT_NULL;
    flash_storage.storage_device.close = RT_NULL;
    flash_storage.storage_device.read = flash_storage_read;
    flash_storage.storage_device.write = flash_storage_write;
    flash_storage.storage_device.control = flash_storage_control;
    /* no private */
    flash_storage.storage_device.user_data = RT_NULL;

    rt_device_register(&flash_storage.storage_device, flash_storage_name,
        RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE);
    rt_device_open(&flash_storage.storage_device, RT_DEVICE_OFLAG_RDWR);

    return RT_EOK;
}

/******************* (C) COPYRIGHT 2015 TongYe **********END OF FILE****/


/*@}*/

