/* Includes ------------------------------------------------------------------*/
#include "Bsp_rs485.h"
#include "stm32f4xx.h"
#include "rtthread.h"
#include <stdio.h>
#include <string.h>
#include "crc16.h"
//#define usartTransmit
__IO rt_uint8_t TemDataCnter;               //DMARx长度计算，超时中断判断依据

/***************** for debug *****************************/
static rt_uint8_t StartTransmitFlag = 0;    //正常收发标志
static rt_uint32_t mUsartTxCnt = 0;         //发送字节数
static rt_uint32_t mUsartTxNumCnt = 0;      //发送次数

rt_uint8_t HMIFrameType = 0;//发送数据帧类型：状态 0；控制板命令 1；监控板命令 2；校准命令 3；故障 4
char *pFrameType = "StaFrame";
//-----------------------------------------------------------------
// 创建信号量
//-----------------------------------------------------------------
struct rt_semaphore HMICOMM_OPSem;
struct rt_semaphore HMICOMM_ST_OPSem;
struct rt_semaphore HMIGetCmd_OPSem;
//-----------------------------------------------------------------
// HMICOMM 多机通讯状态机
//-----------------------------------------------------------------
//static uint16_t HMICOMMState = 0;
//-----------------------------------------------------------------
// HMICOMM 发送缓冲区
//-----------------------------------------------------------------
static uint8_t HMICOMMTxBuffer[HMICOMM_TX_BUFFER_SIZE_MAX]     = {0};
__IO uint8_t   HMICOMMTxBufferCnt  = 0;
__IO uint8_t   HMICOMMTxBufferSize = 0;
//-----------------------------------------------------------------
// HMICOMM 接收缓冲区
//-----------------------------------------------------------------
__IO uint8_t   HMICOMMRxBuffer[HMICOMM_RX_BUFFER_SIZE_MAX] = {0};
__IO uint8_t   HMIDMARxBuffer[HMICOMM_RX_BUFFER_SIZE_MAX] = {0};
__IO uint8_t   HMICOMMRxBufferCnt  = 0;
 

static void HMICOMM_GPIO_Configuration(void);
static void HMICOMM_UASRT_Configuration(void);
static void HMICOMM_NVIC_Configuration(void);
static void HMICOMM_TIMx_Configration(void);
static void HMICOMM_DMA_Configuration(void);
static void Bsp_StartDmaTx(__IO uint16_t _dma_tx_len);
static void Bsp_StartDmaRx(void);


/*******************************************************************************
* Function Name  : System_HW_HMICOMM3_Init
* Description    : HMI RS485通信初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

int System_HW_HMICOMM_Init(void)
{
	rt_err_t rtv, rtv1,rtv2;
	//----------------------------------------------------------------------------
	// 创建信号量 
	//----------------------------------------------------------------------------
	rtv  = rt_sem_init(&HMICOMM_OPSem,     "SemExpCom",      0,  RT_IPC_FLAG_FIFO);
	rtv1 = rt_sem_init(&HMICOMM_ST_OPSem,  "SemExpComBus",   0,  RT_IPC_FLAG_FIFO);
    rtv2 = rt_sem_init(&HMIGetCmd_OPSem,   "SemGetCmd",   0,  RT_IPC_FLAG_FIFO);
	//-----------------------------------------------------------------------------
	// GPIO 配置
	//-----------------------------------------------------------------------------
	HMICOMM_GPIO_Configuration();
	//-----------------------------------------------------------------------------
	// RS485 Bus to RX status
	//-----------------------------------------------------------------------------
	HMICOMM_DE_RX();
	//-----------------------------------------------------------------------------
	// USARTx 配置
	//-----------------------------------------------------------------------------
	HMICOMM_UASRT_Configuration();
	//-----------------------------------------------------------------------------
	// MODBUS 用的超时控制用的定时器
	//-----------------------------------------------------------------------------
	HMICOMM_TIMx_Configration();
    //-----------------------------------------------------------------------------
    // 485通信DMA 配置
    //-----------------------------------------------------------------------------
    HMICOMM_DMA_Configuration();
	//-----------------------------------------------------------------------------
	// NVIC设定
	//-----------------------------------------------------------------------------
	HMICOMM_NVIC_Configuration();
	//-----------------------------------------------------------------------------
	// 相关外设使能
	//-----------------------------------------------------------------------------
    /* Enable USARTx DMA TX request */
    USART_DMACmd(HMICOMM_USART_PORT, USART_DMAReq_Tx, ENABLE);


  
    USART_Cmd(HMICOMM_USART_PORT,  ENABLE);
	/* enable the Transmit interrupt */
#ifdef usartTransmit
    USART_ITConfig(HMICOMM_USART_PORT, USART_IT_RXNE, ENABLE);
#else
    USART_ITConfig(HMICOMM_USART_PORT, USART_IT_RXNE, DISABLE);
    USART_ITConfig(HMICOMM_USART_PORT, USART_IT_IDLE, ENABLE);
    /* Enable USARTx DMA RX request */
    USART_DMACmd(HMICOMM_USART_PORT, USART_DMAReq_Rx, ENABLE);
    /* DMA Stream enable */
    DMA_Cmd(HMICOMM_RX_DMA, ENABLE);
#endif

	if ((rtv == RT_EOK) && (rtv1 == RT_EOK) && (rtv2 == RT_EOK))
	{
		return 0;
	} 
	else
	{
		return -1;
	}
}

/*******************************************************************************
* Function Name  : System_HW_TRSFCOM6_Init
* Description    : HMI RS485通信初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

// int System_HW_TRSFCOM6_Init(void)
// {

// }


/*******************************************************************************
* Function Name  : USART_GPIO_Configuration
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void HMICOMM_GPIO_Configuration(void)
{
	GPIO_InitTypeDef       GPIO_InitStructure       = {0};

	/* Enable GPIOx clocks */
	RCC_AHB1PeriphClockCmd( HMICOMM_TX_GPIO_CLK | HMICOMM_RX_GPIO_CLK, ENABLE);

	/* Connect USART pins to AF */
	GPIO_PinAFConfig(HMICOMM_TX_GPIO_P, HMICOMM_TX_GPIO_SOURCE, HMICOMM_USART_AF);
	GPIO_PinAFConfig(HMICOMM_RX_GPIO_P, HMICOMM_RX_GPIO_SOURCE, HMICOMM_USART_AF);

	/* Configure USART Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin   = HMICOMM_RX_GPIO;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(HMICOMM_RX_GPIO_P, &GPIO_InitStructure);

	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin   = HMICOMM_TX_GPIO;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(HMICOMM_TX_GPIO_P, &GPIO_InitStructure);

	/* Configure RS485DE  as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin   = HMICOMM_DE485_GPIO;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(HMICOMM_DE485_GPIOX, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void HMICOMM_UASRT_Configuration(void)
{
	USART_InitTypeDef      USART_InitStructure      = {0};
	USART_ClockInitTypeDef USART_ClockInitStructure = {0};

	//-----------------------------------------------------------------------------
	// Enable USARTx 外设时钟开启
	//-----------------------------------------------------------------------------
	RCC_APB1PeriphClockCmd(HMICOMM_RCC, ENABLE); 
	//------------------------------------------------------------------------------
	//
	//    USART通信端口设定
	//
	//------------------------------------------------------------------------------
	USART_ClockInitStructure.USART_Clock   = USART_Clock_Disable;
	USART_ClockInitStructure.USART_CPOL    = USART_CPOL_Low;
	USART_ClockInitStructure.USART_CPHA    = USART_CPHA_2Edge;
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Enable;
	USART_ClockInit(HMICOMM_USART_PORT, &USART_ClockInitStructure) ;
	//------------------------------------------------------------------------------
	//
	//    USARTx 通信端口设定
	//
	//------------------------------------------------------------------------------
	USART_InitStructure.USART_BaudRate   = HMICOMM_BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits   = USART_StopBits_1 ;
	USART_InitStructure.USART_Parity     = HMICOMM_PARITY_MODE_MB_INV;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(HMICOMM_USART_PORT, &USART_InitStructure);
}

/*******************************************************************************
* Function Name  : HMICOMM_TIMx_Configration 
* Description    : 串口超时判断用定时器
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

static void HMICOMM_TIMx_Configration(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure = {0};
	uint16_t                 overtime;

    //TIM_Load = 单字节位数 * 超时字节数 * 定时器频率 / 设定波特率
    //           整个运算，凡遇到小数，一律进位
	overtime = (uint16_t)((10 * 3.5 * 10000 / HMICOMM_BAUDRATE)+1);

	if(overtime <= 1) 	
        overtime = 1;
	/* TIM1 clock enable [TIM1定时器允许]*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 
	//---------------------------------------------------------------------------
	// 初始化定时器1，TIM1
	//---------------------------------------------------------------------------
	TIM_DeInit( HMICOMM_USART_TIM );//复位TIM1定时器
	/*----------------------------------------------------------------
	TIM1 Configuration: Output Compare Timing Mode:
	TIM1CLK = 168 MHz, Prescaler = 16800, TIM1 counter clock = 10 KHz
	----------------------------------------------------------------*/
	/* TIM1 configuration */
	TIM_TimeBaseStructure.TIM_Period        = overtime ;
	TIM_TimeBaseStructure.TIM_Prescaler     = 16800 - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
	TIM_TimeBaseInit(HMICOMM_USART_TIM, &TIM_TimeBaseStructure);

	/*  */
	TIM_ARRPreloadConfig(HMICOMM_USART_TIM, ENABLE);
	/* Clear TIM  update pending flag[清除TIM 溢出中断标志] */
	TIM_ClearFlag(HMICOMM_USART_TIM, TIM_FLAG_Update);


	/* Enable TIM  Update interrupt [TIM 溢出中断允许]*/
	TIM_ITConfig(HMICOMM_USART_TIM, TIM_IT_Update, ENABLE);  

	TIM_ClearFlag(HMICOMM_USART_TIM, TIM_FLAG_Update);
}
/*******************************************************************************
* Function Name  : USART_DMA_Configuration
* Description    : 配置串口发送DMA
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void HMICOMM_DMA_Configuration(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHB1PeriphClockCmd(HMICOMM_DMA_CLK, ENABLE);  

    /* DMA1 Stream3 Config for TX*/
    DMA_DeInit(HMICOMM_TX_DMA);

    /* fill init structure */
    DMA_InitStructure.DMA_PeripheralBaseAddr = HMICOMM_DR_BASE;
    DMA_InitStructure.DMA_Memory0BaseAddr    = (rt_uint32_t)HMICOMMTxBuffer;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
    DMA_InitStructure.DMA_Channel            = HMICOMM_TX_DMA_CHANNEL;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize         = HMICOMM_TX_BUFFER_SIZE_MAX;

    DMA_Init(HMICOMM_TX_DMA, &DMA_InitStructure);

    DMA_ClearFlag(HMICOMM_TX_DMA, DMA_FLAG_TCIF6);
    /* Enable DMA Stream Transfer Complete interrupt */
    DMA_ITConfig(HMICOMM_TX_DMA, DMA_IT_TC, ENABLE);

    /* DMA1 Stream5 Config for RX */
#ifdef usartTransmit
    
#else
    DMA_DeInit(HMICOMM_RX_DMA);

    /* fill init structure */
    DMA_InitStructure.DMA_PeripheralBaseAddr = HMICOMM_DR_BASE;
    DMA_InitStructure.DMA_Memory0BaseAddr    = (rt_uint32_t)HMIDMARxBuffer;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
    DMA_InitStructure.DMA_Channel            = HMICOMM_RX_DMA_CHANNEL;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize         = HMICOMM_RX_BUFFER_SIZE_MAX;

    DMA_Init(HMICOMM_RX_DMA, &DMA_InitStructure);
#endif
}
static void DMA_RX_Init(void)//HMI
{
    DMA_InitTypeDef DMA_InitStructure;
    /* DMA1 Stream1 Config */
    DMA_DeInit(HMICOMM_RX_DMA);

    /* fill init structure */
    DMA_InitStructure.DMA_PeripheralBaseAddr = HMICOMM_DR_BASE;
    DMA_InitStructure.DMA_Memory0BaseAddr    = (rt_uint32_t)HMIDMARxBuffer;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
    DMA_InitStructure.DMA_Channel            = HMICOMM_RX_DMA_CHANNEL;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize         = HMICOMM_RX_BUFFER_SIZE_MAX;

    DMA_Init(HMICOMM_RX_DMA, &DMA_InitStructure);
}
/*******************************************************************************
* Function Name  : NVIC_Usart_Configuration
* Description    : 串口接收中断，超时中断，DMA中断中断向量表配置
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void HMICOMM_NVIC_Configuration(void)
{
	NVIC_InitTypeDef     NVIC_InitStructure = {0};

	/* Enable URT IRQChannel */
	NVIC_InitStructure.NVIC_IRQChannel                   = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;  
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	/* Enable TIM1 IRQChannel */
	NVIC_InitStructure.NVIC_IRQChannel                   = TIM1_UP_TIM10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
    /* Enable DMA IRQChannel */
    NVIC_InitStructure.NVIC_IRQChannel                   = HMICOMM_DMA_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 3;  
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* Function Name  : void Bsp_HMICOMM_TIM_Handler(void) 
* Description    : USART_TIM[TIM] 中断处理函数 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Bsp_HMICOMM_TIM_Handler(void)
{
	if (TIM_GetITStatus(HMICOMM_USART_TIM, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(HMICOMM_USART_TIM, TIM_IT_Update);
		TIM_SetCounter(HMICOMM_USART_TIM, 0);
		/* TIM  enable counter */
		TIM_Cmd(HMICOMM_USART_TIM, DISABLE);
		//处理接收到的数据帧
#ifdef usartTransmit
		rt_sem_release(&HMICOMM_OPSem);
#else

        if(TemDataCnter == DMA_GetCurrDataCounter(HMICOMM_RX_DMA))
        {
            Bsp_StartDmaRx();
        }
            /* 重启超时中断 */
//          HMICOMM_USART_TIM->CNT = 0;
//          TIM_Cmd(HMICOMM_USART_TIM, ENABLE);
       
#endif
	}
}

/*******************************************************************************
* Function Name  : Bsp_HMICOMM_URT_IRQHandler
* Description    : 串口中断服务函数
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/

void Bsp_HMICOMM_URT_IRQHandler(void)
{
    static u16 temp       = 0;
	static u16 m_Rx_Data  = 0;
	static u16 m_Tx_Data  = 0;
	
	//-------------------------------------------------------------------------
	// 中断USART接收
	//-------------------------------------------------------------------------
	if (USART_GetITStatus(HMICOMM_USART_PORT, USART_IT_RXNE) == SET)
	{    
		if(USART_GetFlagStatus(HMICOMM_USART_PORT, USART_FLAG_FE)==SET)
		{
            // 帧错误发生后，读取DR寄存器清除
			temp = HMICOMM_USART_PORT->DR;
		}
		else
		{       
			m_Rx_Data = (u16)USART_ReceiveData(HMICOMM_USART_PORT);
            HMICOMMRxBuffer[HMICOMMRxBufferCnt++] = (uint8_t)(m_Rx_Data & 0xFF);  //缓冲区
            HMICOMM_USART_TIM->CNT = 0;
			TIM_Cmd(HMICOMM_USART_TIM, ENABLE);
		}

		USART_ClearITPendingBit(HMICOMM_USART_PORT, USART_IT_RXNE) ;
	} 
    //-------------------------------------------------------------------------
    //  中断DMA接收
    //-------------------------------------------------------------------------
    if (USART_GetITStatus(HMICOMM_USART_PORT, USART_IT_IDLE) == SET)
    {
        temp = HMICOMM_USART_PORT->SR;
        temp = HMICOMM_USART_PORT->DR;


        /* Read the DMA RX Stream receive data number */
       TemDataCnter = DMA_GetCurrDataCounter(HMICOMM_RX_DMA);

        //开启超时中断
        HMICOMM_USART_TIM->CNT = 0;
        TIM_Cmd(HMICOMM_USART_TIM, ENABLE);
        /* 处理接收到的数据帧 */
        //Bsp_StartDmaRx();
    }
	//-------------------------------------------------------------------------
	// 中断USART发送
	//-------------------------------------------------------------------------
	if(USART_GetITStatus(HMICOMM_USART_PORT, USART_IT_TXE) != RESET)
	{   
		
			m_Tx_Data = HMICOMMTxBuffer[HMICOMMTxBufferCnt++];

		/* Write one byte to the transmit data register */
		USART_SendData(HMICOMM_USART_PORT, m_Tx_Data);

		if(HMICOMMTxBufferCnt == HMICOMMTxBufferSize)
		{
			/* Disable the Transmit interrupt */
			USART_ITConfig(HMICOMM_USART_PORT, USART_IT_TXE, DISABLE);
			rt_sem_release(&HMICOMM_ST_OPSem);
		}
	}
}
/*******************************************************************************
* Function Name  : Bsp_HMICOMM_DMA_Tx_Irqhandler
* Description    : DMA中断，当DMA发送完成后，抛出信号量，对485通信DE进行翻转
                   通信转为接收态
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void Bsp_HMICOMM_DMA_Tx_Irqhandler(void)
{
    if (DMA_GetITStatus(HMICOMM_TX_DMA, DMA_IT_TEIF3) == SET)
    {
        DMA_ClearITPendingBit(HMICOMM_TX_DMA, DMA_IT_TEIF3);
        rt_sem_release(&HMICOMM_ST_OPSem);
    }

    if (DMA_GetITStatus(HMICOMM_TX_DMA, DMA_IT_TCIF3) == SET)
    {
        DMA_ClearITPendingBit(HMICOMM_TX_DMA, DMA_IT_TCIF3);

        rt_sem_release(&HMICOMM_ST_OPSem);
    }
}
/*******************************************************************************
* Function Name  : static void Bsp_StartDmaRx(void)
* Description    : 重启DMA 接收数据 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Bsp_StartDmaRx(void)//HMI
{
    /* Disable the USART TX DMA request */
    USART_DMACmd(HMICOMM_USART_PORT, USART_DMAReq_Rx, DISABLE);
    /* Disable the DMA  TX Streams */
    DMA_Cmd(HMICOMM_RX_DMA, DISABLE);

    /* Read the DMA RX Stream receive data number */
    HMICOMMRxBufferCnt = HMICOMM_RX_BUFFER_SIZE_MAX - DMA_GetCurrDataCounter(HMICOMM_RX_DMA);

    if(HMICOMMRxBufferCnt>=3)
    {
        rt_memcpy((void*)HMICOMMRxBuffer,(void*)HMIDMARxBuffer,HMICOMMRxBufferCnt);
        rt_memset((void*)HMIDMARxBuffer,0,HMICOMM_RX_BUFFER_SIZE_MAX);
        rt_sem_release(&HMICOMM_OPSem);
    }
    /* Renew DMA RX */
    DMA_RX_Init();
    /* Enable the USART RX DMA request */
    DMA_Cmd(HMICOMM_RX_DMA, ENABLE);
    USART_DMACmd(HMICOMM_USART_PORT, USART_DMAReq_Rx, ENABLE);
}
/*******************************************************************************
* Function Name  : static void Bsp_StartDmaTx(__IO uint16_t _dma_tx_len)
* Description    : 重启DMA 发送数据 
* Input          : _dma_tx_len : 待发送字节数
* Output         : None
* Return         : None
*******************************************************************************/
static void Bsp_StartDmaTx(__IO uint16_t _dma_tx_len)//HMI
{
    /* Disable the USART TX DMA request */
    USART_DMACmd(HMICOMM_USART_PORT, USART_DMAReq_Tx, DISABLE);
    /* Disable the DMA  TX Streams */
    DMA_Cmd(HMICOMM_TX_DMA, DISABLE);
    //---------------------------------------------------------
    // Clear all falgs of DMA TX Streams
    //---------------------------------------------------------
    DMA_ClearFlag(HMICOMM_TX_DMA, 
        (DMA_FLAG_FEIF6 | DMA_FLAG_DMEIF6 | DMA_FLAG_TEIF6 |  
        DMA_FLAG_HTIF6 | DMA_FLAG_TCIF6 )
        );
    /* Set the DMA TX Stream lenth */
    HMICOMM_TX_DMA->NDTR = _dma_tx_len; //DMA_TxBuffer_Size;

    if(StartTransmitFlag)
    {
        mUsartTxCnt +=_dma_tx_len;
        mUsartTxNumCnt ++;
        StartTransmitFlag = 0;
    }
    /* Clear the TC bit in the SR register by writing 0 to it */
    USART_ClearFlag(HMICOMM_USART_PORT, USART_FLAG_TC);
    /* Enable the DMA  TX Streams */
    DMA_Cmd(HMICOMM_TX_DMA, ENABLE);
    /* Enable the USART TX DMA request */
    USART_DMACmd(HMICOMM_USART_PORT, USART_DMAReq_Tx, ENABLE);
}

/*******************************************************************************
* Function Name  : Bsp_HMICOMM_TX_Stream
* Description    : 发送数据接口
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void Bsp_HMICOMM_TX_Stream(R485_UR_TYPE * _rs485Tx)////////HMI
{
	//---------------------------------------------------------
	// RS485总线转发送态
	//---------------------------------------------------------
	HMICOMM_DE_TX();

	//---------------------------------------------------------
    // 生成发送数据，并返回发送长度
	//---------------------------------------------------------
	rt_memcpy(HMICOMMTxBuffer, _rs485Tx->UR_Buffer, _rs485Tx->UR_SIZE);
    HMICOMMTxBufferSize = _rs485Tx->UR_SIZE;
	//---------------------------------------------------------
    // 重新启动发送
    //---------------------------------------------------------
    if(HMICOMMTxBufferSize > 0)
    {
       HMICOMMTxBufferCnt = 0;
	   /* enable the Transmit interrupt */
	   //USART_ITConfig(HMICOMM_USART_PORT, USART_IT_TXE, ENABLE);
       //--------------------------------------------------------------------
       //  启动DMA发送
       //--------------------------------------------------------------------
       Bsp_StartDmaTx(HMICOMMTxBufferSize);
    }
    else
    {
        //---------------------------------------------------------
        // RS485总线转接收态
        //---------------------------------------------------------
        //HMICOMM_DE_RX();
        rt_sem_release(&HMICOMM_ST_OPSem);
    }
}

/*******************************************************************************
* Function Name  : Bsp_HMICOMM_RX_Stream
* Description    : 处理 [ 接收 ] 数据  
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/


void Bsp_HMICOMM_RX_Stream(R485_UR_TYPE * _rs485Rx)//HMI
{
	uint16_t CrcResult;
	uint8_t  crc_l, crc_h;

	//----------------------------------------------------------------
	// 数据接收完成，对收到的数据进行CRC校验
	//----------------------------------------------------------------
	CrcResult = CRC16_Dn_Cal((uint8_t*)HMICOMMRxBuffer, (HMICOMMRxBufferCnt - 2), 0xFFFF);
	crc_l     = CrcResult & 0x00FF;
	crc_h     = (CrcResult >> 8) & 0x00FF;
	//----------------------------------------------------------------
	// CRC校验通过, 准备数据应答主卡
	//----------------------------------------------------------------
	if((HMICOMMRxBuffer[HMICOMMRxBufferCnt-1]==crc_h) && (HMICOMMRxBuffer[HMICOMMRxBufferCnt-2]==crc_l))
	{
		memcpy((void *)_rs485Rx->UR_Buffer, (void *)HMICOMMRxBuffer, HMICOMMRxBufferCnt-2);
		_rs485Rx->UR_SIZE = HMICOMMRxBufferCnt-2;
		
	}
	
	//--------------------------------------------------------------------
	// 接收数据长度清零
	//--------------------------------------------------------------------
	HMICOMMRxBufferCnt  = 0;
	//--------------------------------------------------------------------
	// 清空接收RAM区数据
	//--------------------------------------------------------------------
	rt_memset((void*)HMICOMMRxBuffer, 0, HMICOMM_RX_BUFFER_SIZE_MAX);
}

/*******************************************************************************
* Function Name  : Bsp_HMICOMM_Bus_To_RX
* Description    : RS485总线置为接收态 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void Bsp_HMICOMM_Bus_To_RX(void)
{
	static uint8_t firstfalg = 1;
	u32 cnt = 3000;

	while(USART_GetFlagStatus(HMICOMM_USART_PORT, USART_FLAG_TC) == RESET);
	//--------------------------------------------------------------------
	// 延时，保证数据发送完成
	//--------------------------------------------------------------------
	//cnt = 10;
	cnt = 1000;
	while(cnt--);
	//----------------------------------------------------------------
	// 置RS485总线为：接收态
	//----------------------------------------------------------------
	if(firstfalg == 0)
		firstfalg = 1;
	HMICOMM_DE_RX();
}
