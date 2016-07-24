/**
  *************************************************************************
  * This file is part of Platform
  * COPYRIGHT (C) 1999-2014, TongYe R&D Team
  * @file    spi_flash.c
  * @brief   This file provides all the spi_flash firmware functions.
  * Change Logs:
  * Date           Author       Notes
  * 2014/12/23     Coase        the first version
  *************************************************************************
  */

#include "spi_flash.h"
#include "stm32f20x_40x_spi.h"
//#include "hd_version.h"

void logic_board_spi_init()
{
    /* register at45 spi bus */
    {
        static struct stm32_spi_bus at45_spi;
        GPIO_InitTypeDef GPIO_InitStructure;

        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

        /*!< SPI SCK pin configuration */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        /* Connect alternate function */
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);

        stm32_spi_register(SPI2, &at45_spi, "spi2");
    }
    
    /* attach cs */
    {
        static struct rt_spi_device spi_device_at45;
        static struct stm32_spi_cs  spi_cs_at45;

        GPIO_InitTypeDef GPIO_InitStructure;

        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        
        /* spi2: PB12 */
        spi_cs_at45.GPIOx = GPIOD;
        spi_cs_at45.GPIO_Pin = GPIO_Pin_3;
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = spi_cs_at45.GPIO_Pin;
        GPIO_SetBits(spi_cs_at45.GPIOx, spi_cs_at45.GPIO_Pin);
        GPIO_Init(spi_cs_at45.GPIOx, &GPIO_InitStructure);
        
        spi_cs_at45.GPIOx->BSRRL = spi_cs_at45.GPIO_Pin;

        rt_spi_bus_attach_device(&spi_device_at45, "at45", "spi2", (void*)&spi_cs_at45);
    }
}

/******************* (C) COPYRIGHT 2014 TongYe **********END OF FILE****/


/*@}*/

