
/***************************************************************************

Copyright (C), 1999-2012, Tongye Tech. Co., Ltd.

* @file           Bsp_sysspi.h
* @author         列供项目部
* @version        V1.0.0
* @date           2012-04-21
* @brief          SPI操作相关

History:          // Revision Records

<Author>             <time>       <version >            <desc>

grape             2012-04-21       V1.0.0             初次建立

***************************************************************************/

#ifndef __BSP_SPI_H__
#define __BSP_SPI_H__
/* Includes ------------------------------------------------------------------*/

#include <stm32f4xx.h>

#define BSP_USE_SPI1    (0)
#define BSP_USE_SPI2    (1)
#define BSP_USE_SPI3    (0)

#define SPI1_SCK_GPIO_CLK   (RCC_AHB1Periph_GPIOA)
#define SPI1_MISO_GPIO_CLK  (RCC_AHB1Periph_GPIOA)
#define SPI1_MOSI_GPIO_CLK  (RCC_AHB1Periph_GPIOA)
#define SPI1_SCK_GPIO_PORT  (GPIOA)
#define SPI1_MISO_GPIO_PORT (GPIOA)
#define SPI1_MOSI_GPIO_PORT (GPIOA)

#define SPI1_SCK_SOURCE     (GPIO_PinSource5)
#define SPI1_SCK_PIN        (GPIO_Pin_5)
#define SPI1_MISO_SOURCE    (GPIO_PinSource6)
#define SPI1_MISO_PIN       (GPIO_Pin_6)
#define SPI1_MOSI_SOURCE    (GPIO_PinSource7)
#define SPI1_MOSI_PIN       (GPIO_Pin_7)

#define SPI2_SCK_GPIO_CLK   (RCC_AHB1Periph_GPIOB)
#define SPI2_MISO_GPIO_CLK  (RCC_AHB1Periph_GPIOB)
#define SPI2_MOSI_GPIO_CLK  (RCC_AHB1Periph_GPIOB)

#define SPI2_SCK_GPIO_PORT  (GPIOB)
#define SPI2_MISO_GPIO_PORT (GPIOB)
#define SPI2_MOSI_GPIO_PORT (GPIOB)

#define SPI2_SCK_SOURCE     (GPIO_PinSource13)
#define SPI2_SCK_PIN        (GPIO_Pin_13)
#define SPI2_MISO_SOURCE    (GPIO_PinSource14)
#define SPI2_MISO_PIN       (GPIO_Pin_14)
#define SPI2_MOSI_SOURCE    (GPIO_PinSource15)
#define SPI2_MOSI_PIN       (GPIO_Pin_15)

#define SPI3_SCK_GPIO_CLK   (RCC_AHB1Periph_GPIOB)
#define SPI3_MISO_GPIO_CLK  (RCC_AHB1Periph_GPIOB)
#define SPI3_MOSI_GPIO_CLK  (RCC_AHB1Periph_GPIOB)

#define SPI3_SCK_GPIO_PORT  (GPIOB)
#define SPI3_MISO_GPIO_PORT (GPIOB)
#define SPI3_MOSI_GPIO_PORT (GPIOB)

#define SPI3_SCK_SOURCE     (GPIO_PinSource3)
#define SPI3_SCK_PIN        (GPIO_Pin_3)
#define SPI3_MISO_SOURCE    (GPIO_PinSource4)
#define SPI3_MISO_PIN       (GPIO_Pin_4)
#define SPI3_MOSI_SOURCE    (GPIO_PinSource5)
#define SPI3_MOSI_PIN       (GPIO_Pin_5)


#define FLASH_CS_GPIO_CLK   (RCC_AHB1Periph_GPIOD)
#define FLASH_CS_GPIO_PORT  (GPIOD)
#define FLASH_CS_PIN        (GPIO_Pin_3)

#define FLASH_CS_HIGH()     (FLASH_CS_GPIO_PORT->BSRRL = FLASH_CS_PIN)
#define FLASH_CS_LOW()      (FLASH_CS_GPIO_PORT->BSRRH = FLASH_CS_PIN)

#define AD_CS_GPIO_CLK      (RCC_AHB1Periph_GPIOA)
#define AD_CS_GPIO_PORT     (GPIOA)
#define AD_CS_PIN           (GPIO_Pin_15)

#define AD_CS_HIGH()        (AD_CS_GPIO_PORT->BSRRL = AD_CS_PIN)
#define AD_CS_LOW()         (AD_CS_GPIO_PORT->BSRRH = AD_CS_PIN)


#define T_CS_GPIO_CLK       (RCC_AHB1Periph_GPIOB)
#define T_CS_GPIO_PORT      (GPIOB)
#define T_CS_PIN            (GPIO_Pin_9)

#define T_CS_HIGH()         (T_CS_GPIO_PORT->BSRRL = T_CS_PIN)
#define T_CS_LOW()          (T_CS_GPIO_PORT->BSRRH = T_CS_PIN)

/* Exported functions ------------------------------------------------------- */

int         System_hw_spi_Init(void);
rt_uint8_t  SPI_SendByte(SPI_TypeDef* SPIx, rt_uint8_t byte);
rt_uint16_t SPI_SendHalfWord(SPI_TypeDef* SPIx, rt_uint16_t HalfWord);

#endif


/******************* (C) COPYRIGHT 2012 Shenzhen Tongye *****END OF FILE****/
