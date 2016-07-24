#ifndef _BSP_BOARD_INIT_H_
#define _BSP_BOARD_INIT_H_

#include "stm32f4xx.h"
#include <rtthread.h>

#ifndef NULL
#define NULL    ((void *)0)
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

#ifndef TRUE
#define TRUE    (1)
#endif

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;


typedef float          fp32;                    /* 单精度浮点数（32位长度）                 */
typedef double         fp64;                    /* 双精度浮点数（64位长度）                 */

#define   		ODD(a)	    	   		(a & 0x01)
#define RT_USING_RTC

#define USE_FREE_MODBUS					(1)
#define USE_FREE_MASTER					(1)

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

void IWDG_Init(rt_uint8_t ReloadTime);
void Bsp_led_init(void);
int System_fsmc_init(void);

#endif

