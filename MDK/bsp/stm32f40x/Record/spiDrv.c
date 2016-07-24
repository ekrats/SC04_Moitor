/* Includes ------------------------------------------------------------------*/
#include <rtthread.h>
#include "spiDrv.h"
//#include "ds.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static void System_SpiConfiguration(void);
static void RCC_Configuration(void);
static void GPIO_Configuration(void);

/*******************************************************************************
* Function Name  : System_hw_spi_Init
* Description    : 初始化定时器TIMx
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int System_hw_spi_Init(void)
{
	RCC_Configuration();
	GPIO_Configuration();
	System_SpiConfiguration();

	return 1;//jia FUNC_RET_OK
}

/*******************************************************************************
* Function Name  : System_SpiConfiguration
* Description    :  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void System_SpiConfiguration(void)
{
	SPI_InitTypeDef  SPI_InitStructure = {0};

#if(BSP_USE_SPI1 == 1)
	//-----------------------------------------------------------------
	// PCLK2 = 84MHZ, 
	// SPI1接口时钟最高为42MHz
	// AT25访问速度50MHZ
	// 84MHz / 2 = 42MHz
	//-----------------------------------------------------------------
	AD_CS_HIGH();

	/*!< Enable the SPI  */
	SPI_Cmd(SPI1, DISABLE);
	/*!< SPI configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize  = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL      = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS       = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

	SPI_InitStructure.SPI_FirstBit      = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);

	/*!< Enable the  SPI  */
	SPI_Cmd(SPI1, ENABLE);

#endif

#if(BSP_USE_SPI2 == 1)
	//-----------------------------------------------------------------
	// PCLK2 = 84MHZ
	// SPI2接口时钟最高为42MHz
	// TLV45最大访问速度66MHZ
	// 84MHz / 4 = 21MHz
	//-----------------------------------------------------------------
	FLASH_CS_HIGH();
	/*!< Enable the SPI  */
	SPI_Cmd(SPI2, DISABLE);

	/*!< SPI configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize  = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL      = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS       = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;

	SPI_InitStructure.SPI_FirstBit      = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);

	/*!< Enable the  SPI  */
	SPI_Cmd(SPI2, ENABLE);

#endif

#if(BSP_USE_SPI3 == 1)
	//-----------------------------------------------------------------
	// PCLK1 = 42MHZ
	//-----------------------------------------------------------------
	T_CS_HIGH();
	/*!< Enable the SPI  */
	SPI_Cmd(SPI3, DISABLE);

	/*!< SPI configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize  = SPI_DataSize_16b;
	SPI_InitStructure.SPI_CPOL      = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA      = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS       = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;

	SPI_InitStructure.SPI_FirstBit      = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI3, &SPI_InitStructure);

	/*!< Enable the  SPI  */
	SPI_Cmd(SPI3, ENABLE);

#endif

}

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    :  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void RCC_Configuration(void)
{

#if(BSP_USE_SPI1 == 1)
	/* SPI1 clock enable      */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); 
#endif

#if(BSP_USE_SPI2 == 1)
	/* SPI2 clock enable      */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); 
#endif

#if(BSP_USE_SPI3 == 1)
	/* SPI3 clock enable      */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE); 
#endif
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    :  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};

#if(BSP_USE_SPI1 == 1)
	/*!< Enable GPIO clocks */
	RCC_AHB1PeriphClockCmd(SPI1_SCK_GPIO_CLK | SPI1_MISO_GPIO_CLK | 
		SPI1_MOSI_GPIO_CLK, ENABLE);

	/*!< Connect SPI pins to AF5 */  
	GPIO_PinAFConfig(SPI1_SCK_GPIO_PORT,  SPI1_SCK_SOURCE,  GPIO_AF_SPI1);
	GPIO_PinAFConfig(SPI1_MISO_GPIO_PORT, SPI1_MISO_SOURCE, GPIO_AF_SPI1);
	GPIO_PinAFConfig(SPI1_MOSI_GPIO_PORT, SPI1_MOSI_SOURCE, GPIO_AF_SPI1);

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

	/*!< SPI SCK pin configuration */
	GPIO_InitStructure.GPIO_Pin = SPI1_SCK_PIN;
	GPIO_Init(SPI1_SCK_GPIO_PORT, &GPIO_InitStructure);

	/*!< SPI MOSI pin configuration */
	GPIO_InitStructure.GPIO_Pin =  SPI1_MOSI_PIN;
	GPIO_Init(SPI1_MOSI_GPIO_PORT, &GPIO_InitStructure);

	/*!< SPI MISO pin configuration */
	GPIO_InitStructure.GPIO_Pin =  SPI1_MISO_PIN;
	GPIO_Init(SPI1_MISO_GPIO_PORT, &GPIO_InitStructure);

	/*!< Configure sFLASH Card CS pin in output pushpull mode ********************/
	RCC_AHB1PeriphClockCmd(AD_CS_GPIO_CLK, ENABLE);

	GPIO_InitStructure.GPIO_Pin   = AD_CS_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(AD_CS_GPIO_PORT, &GPIO_InitStructure);
#endif

#if(BSP_USE_SPI2 == 1)
	/*!< Enable GPIO clocks */
	RCC_AHB1PeriphClockCmd(SPI2_SCK_GPIO_CLK | SPI2_MISO_GPIO_CLK | 
		SPI2_MOSI_GPIO_CLK , ENABLE);

	/*!< Connect SPI pins to AF5 */  
	GPIO_PinAFConfig(SPI2_SCK_GPIO_PORT,  SPI2_SCK_SOURCE,  GPIO_AF_SPI2);
	GPIO_PinAFConfig(SPI2_MISO_GPIO_PORT, SPI2_MISO_SOURCE, GPIO_AF_SPI2);
	GPIO_PinAFConfig(SPI2_MOSI_GPIO_PORT, SPI2_MOSI_SOURCE, GPIO_AF_SPI2);

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

	/*!< SPI SCK pin configuration */
	GPIO_InitStructure.GPIO_Pin = SPI2_SCK_PIN;
	GPIO_Init(SPI2_SCK_GPIO_PORT, &GPIO_InitStructure);

	/*!< SPI MOSI pin configuration */
	GPIO_InitStructure.GPIO_Pin =  SPI2_MOSI_PIN;
	GPIO_Init(SPI2_MOSI_GPIO_PORT, &GPIO_InitStructure);

	/*!< SPI MISO pin configuration */
	GPIO_InitStructure.GPIO_Pin =  SPI2_MISO_PIN;
	GPIO_Init(SPI2_MISO_GPIO_PORT, &GPIO_InitStructure);

	/*!< Configure ExAD Card CS pin in output pushpull mode ********************/
	RCC_AHB1PeriphClockCmd(FLASH_CS_GPIO_CLK, ENABLE);	
	GPIO_InitStructure.GPIO_Pin   = FLASH_CS_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(FLASH_CS_GPIO_PORT, &GPIO_InitStructure);
#endif


#if(BSP_USE_SPI3 == 1)
	/*!< Enable GPIO clocks */
	RCC_AHB1PeriphClockCmd(SPI3_SCK_GPIO_CLK | SPI3_MISO_GPIO_CLK | 
		SPI3_MOSI_GPIO_CLK | T_CS_GPIO_CLK, ENABLE);

	/*!< Connect SPI pins to AF5 */  
	GPIO_PinAFConfig(SPI3_SCK_GPIO_PORT,  SPI3_SCK_SOURCE,  GPIO_AF_SPI3);
	GPIO_PinAFConfig(SPI3_MISO_GPIO_PORT, SPI3_MISO_SOURCE, GPIO_AF_SPI3);
	GPIO_PinAFConfig(SPI3_MOSI_GPIO_PORT, SPI3_MOSI_SOURCE, GPIO_AF_SPI3);

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

	/*!< SPI SCK pin configuration */
	GPIO_InitStructure.GPIO_Pin = SPI3_SCK_PIN;
	GPIO_Init(SPI3_SCK_GPIO_PORT, &GPIO_InitStructure);

	/*!< SPI MOSI pin configuration */
	GPIO_InitStructure.GPIO_Pin =  SPI3_MOSI_PIN;
	GPIO_Init(SPI3_MOSI_GPIO_PORT, &GPIO_InitStructure);

	/*!< SPI MISO pin configuration */
	GPIO_InitStructure.GPIO_Pin =  SPI3_MISO_PIN;
	GPIO_Init(SPI3_MISO_GPIO_PORT, &GPIO_InitStructure);

	/*!< Configure Touch CS pin in output pushpull mode ********************/
	RCC_AHB1PeriphClockCmd(T_CS_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin   = T_CS_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(T_CS_GPIO_PORT, &GPIO_InitStructure);
#endif
}


/*******************************************************************************
* Function Name  : SPI_SendByte
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
rt_uint8_t SPI_SendByte(SPI_TypeDef* SPIx, rt_uint8_t byte)
{
	rt_uint32_t i = 0 ;
	rt_uint32_t j = 0 ;
	/* Loop while DR register in not emplty */
	while ((SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET)&&(++i<1000000));
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPIx, byte);
	/* Wait to receive a byte */
	while ((SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET)&&(++j<1000000));
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPIx) ;
}

/*******************************************************************************
* Function Name  : SPI_SendByte
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
rt_uint16_t SPI_SendHalfWord(SPI_TypeDef* SPIx, rt_uint16_t HalfWord)
{
	rt_uint32_t i = 0 ;
	rt_uint32_t j = 0 ;
	/* Loop while DR register in not emplty */
	while ((SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET)&&(++i<1000000));
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPIx, HalfWord);
	/* Wait to receive a byte */
	while ((SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET)&&(++j<1000000));
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPIx) ;
}

/******************* (C) COPYRIGHT 2012 Shenzhen Tongye *****END OF FILE****/


