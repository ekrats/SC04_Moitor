/**
  *************************************************************************
  * This file is part of Platform
  * COPYRIGHT (C) 1999-2015, TongYe R&D Team
  * @file    own_flash.h
  * @brief   This file contains all the functions prototypes for the
  *  own_flash firmware library.
  * Change Logs:
  * Date           Author       Notes
  * 2015/03/15     Coase        the first version
  *************************************************************************
  */


#ifndef __OWN_FLASH__H__
#define __OWN_FLASH__H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
    /* End of the Flash address */
    #define USER_FLASH_END_ADDRESS  (0x080FFFFF)
    /* Define the address from where user application will be loaded.
       Note: the 1st sector 0x08000000-0x08003FFF is reserved for the IAP code */
    #define APPLICATION_OFFSET      (0X80)
    #define APPLICATION_ADDRESS     ((uint32_t)0x08004000)
    #define UPDATE_ADDRESS          ((uint32_t)0x08080000)

    /**
      * @brief  Unlocks Flash for write access
      * @param  None
      * @retval None
      */
    uint32_t own_flash_init(void);
    /**
      * @brief  This function does an erase of all user flash area
      * @retval 0: user flash area successfully erased
      *         1: error occurred
      */
    uint32_t own_flash_erase(uint32_t address, uint32_t size);
    
    /**
      * @brief  This function check flash data  (data are 32-bit aligned).
      * @param  scr: start address for checking data buffer
      * @param  size: length of data buffer (unit is 32-bit word)   
      * @retval 0: crc check successfully with the given crc
      *         1: crc check unsuccessfully with the given crc
      */
    uint32_t own_flash_check(uint32_t * scr, int size, uint32_t crc);
    
    /**
      * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
      * @note   After writing data buffer, the flash content is checked.
      * @param  dst: start address for writing data buffer
      * @param  scr: pointer on data buffer
      * @param  size: length of data buffer (unit is 32-bit word)   
      * @retval 0: Data successfully written to Flash memory
      *         1: Error occurred while writing data in Flash memory
      *         2: Written Data in flash memory is different from expected one
      */
    uint32_t own_flash_write(uint32_t * dst, uint32_t * scr, int size);
    
    /**
      * @brief  Returns the write protection status of user flash area.
      * @param  address: start address for query
      * @retval 0: No write protected sectors inside the user flash area
      *         1: Some sectors inside the user flash area are write protected
      */
    uint32_t own_flash_get_write_protection_status(uint32_t address);
    
    /**
      * @brief  Disables the write protection of user flash area.
      * @param  address: start address for operating
      * @retval 1: Write Protection successfully disabled
      *         2: Error: Flash write unprotection failed
      */
    uint32_t own_flash_disable_write_protection(uint32_t address);

    /**
      * @brief  Disables the write protection of bootloader area.
      * @retval none
      */
    void unlock_bootloader_protection(void);

#ifdef __cplusplus
}
#endif

#endif


/******************* (C) COPYRIGHT 2015 TongYe **********END OF FILE****/


/*@}*/
