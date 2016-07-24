/**
  *************************************************************************
  * This file is part of Platform
  * COPYRIGHT (C) 1999-2015, TongYe R&D Team
  * @file    own_flash.c
  * @brief   This file provides all the own flash firmware
  * functions.
  * Change Logs:
  * Date           Author       Notes
  * 2015/03/15     Coase        the first version
  *************************************************************************
  */


#include "own_flash.h"
#include "stm32f4xx_flash.h"

static const uint16_t flash_sectors[12] = { FLASH_Sector_0, FLASH_Sector_1, FLASH_Sector_2, FLASH_Sector_3,
                    FLASH_Sector_4, FLASH_Sector_5, FLASH_Sector_6, FLASH_Sector_7, FLASH_Sector_8,
                    FLASH_Sector_9, FLASH_Sector_10, FLASH_Sector_11 };

static void own_flash_lock()
{
    /* Unlock the Flash *********************************************************/
    /* Enable the flash control register access */
    FLASH_Unlock();
    /* Erase the user Flash area ************************************************/
    /* area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR */

    /* Clear pending flags (if any) */
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
        FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
}

/**
* @brief  Gets the sector of a given address
* @param  None
* @retval The sector of a given address
*/
static uint32_t get_sector(uint32_t address)
{
    uint32_t address_offset = address & 0xffffff;
    uint32_t sector_index = 0;
    int tmp = (address_offset / 0x20000);
    if (tmp > 0)
    {
        sector_index = tmp + 4;
    }
    else
    {
        tmp = (address_offset / 0x10000);
        if (tmp > 0)
        {
            sector_index = 3 + tmp;
        }
        else
        {
            sector_index = (address_offset / 0x4000);
        }
    }

    return flash_sectors[sector_index];
}

uint32_t own_flash_erase(uint32_t address, uint32_t size)
{
    int start_sector = get_sector(address);
    int end_sector = get_sector(address + size);
    int erase_sector = 0;

    //检测是否已经被操作.
    if ((*((uint32_t *)address) == 0xffffffff)
        && ((*((uint32_t *)(address + size - 4)) == 0xffffffff))
        && ((*((uint32_t *)(address + size / 2)) == 0xffffffff))
        )
    {
        return 0;
    }

    /* Strat the erase operation */
    erase_sector = start_sector;
    while (erase_sector <= end_sector)
    {
        own_flash_lock();
        /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
        be done by word */
        if (FLASH_EraseSector(erase_sector, VoltageRange_3) != FLASH_COMPLETE)
        {
            /* Error occurred while sector erase.
            User can add here some code to deal with this error  */
            return 1;
        }
        erase_sector += 8;
    }

    return 0;
}

uint32_t own_flash_check(uint32_t * scr, int size, uint32_t crc)
{
    uint32_t _file_crc;
    CRC_ResetDR();
    {
        uint32_t index = 0;

        for (index = 0; index < size; index++)
        {
            CRC->DR = scr[index];
        }
    }

    _file_crc = CRC->DR;

    if (_file_crc != crc)
    {
        return 1;
    }
    return 0;
}

uint32_t own_flash_write(uint32_t * dst, uint32_t * scr, int size)
{
    int i = 0;
    uint32_t write_address = (uint32_t)dst;
    uint32_t data = 0;

    for (i = 0; i < size; i++)
    {
        data = scr[i];
        own_flash_lock();
        if ((write_address < USER_FLASH_END_ADDRESS) &&
            (FLASH_ProgramWord(write_address, data) == FLASH_COMPLETE))
        {
            /* Flash content doesn't match SRAM content */
            if (*(uint32_t*)write_address != data)
            {
                return 2;
            }
            write_address += 4;
        }
        else
        {
            /* Error occurred while writing data in Flash memory */
            return 1;
        }
    }

    return 0;
}

uint32_t own_flash_disable_write_protection(uint32_t address)
{
    __IO uint32_t UserStartSector = FLASH_Sector_1, UserWrpSectors = OB_WRP_Sector_1;

    /* Get the sector where start the user flash area */
    UserStartSector = get_sector(address);

    /* Mark all sectors inside the user flash area as non protected */
    UserWrpSectors = 0xFFF - ((1 << (UserStartSector / 8)) - 1);

    /* Unlock the Option Bytes */
    FLASH_OB_Unlock();

    /* Disable the write protection for all sectors inside the user flash area */
    FLASH_OB_WRPConfig(UserWrpSectors, DISABLE);

    /* Start the Option Bytes programming process. */
    if (FLASH_OB_Launch() != FLASH_COMPLETE)
    {
        /* Error: Flash write unprotection failed */
        return (2);
    }

    /* Write Protection successfully disabled */
    return (1);
}

/**
  * @brief  Returns the write protection status of user flash area.
  * @param  None
  * @retval 0: No write protected sectors inside the user flash area
  *         1: Some sectors inside the user flash area are write protected
  */
uint32_t own_flash_get_write_protection_status(uint32_t address)
{
    uint32_t UserStartSector = FLASH_Sector_1;

    /* Get the sector where start the user flash area */
    UserStartSector = get_sector(APPLICATION_ADDRESS);

    /* Check if there are write protected sectors inside the user flash area */
    if ((FLASH_OB_GetWRP() >> (UserStartSector / 8)) == (0xFFF >> (UserStartSector / 8)))
    { /* No write protected sectors inside the user flash area */
        return 0;
    }
    else
    { /* Some sectors inside the user flash area are write protected */
        return 1;
    }
}
uint32_t ob_data;
FlagStatus flash_status;
FLASH_Status status;
uint32_t own_flash_init()
{
    own_flash_lock();
    return 1;
}

void unlock_bootloader_protection()
{
    uint32_t ob_data;
    FLASH_Status status;
    ob_data = FLASH_OB_GetWRP();
    if((ob_data & 0x1) == 0)
    {
        FLASH_OB_GetRDP();
        FLASH_OB_Unlock();
        /* Clear pending flags (if any) */
        FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
            FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
        FLASH_OB_WRPConfig(OB_WRP_Sector_0, DISABLE);
        status = FLASH_OB_Launch();
        FLASH_OB_Lock();
        if(FLASH_COMPLETE != status)
        {
            status = status;
        }
        own_flash_erase(0x8000000, 0x2000);
    }
}

/******************* (C) COPYRIGHT 2015 TongYe **********END OF FILE****/
