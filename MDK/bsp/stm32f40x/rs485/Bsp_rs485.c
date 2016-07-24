/* Includes ------------------------------------------------------------------*/
#include "Bsp_rs485.h"
#include "stm32f4xx.h"
#include "rtthread.h"
#include <stdio.h>
#include <string.h>
#include "crc16.h"
//#define usartTransmit
__IO rt_uint8_t TemDataCnter;               //DMARx���ȼ��㣬��ʱ�ж��ж�����

/***************** for debug *****************************/
static rt_uint8_t StartTransmitFlag = 0;    //�����շ���־
static rt_uint32_t mUsartTxCnt = 0;         //�����ֽ���
static rt_uint32_t mUsartTxNumCnt = 0;      //���ʹ���

rt_uint8_t HMIFrameType = 0;//��������֡���ͣ�״̬ 0�����ư����� 1����ذ����� 2��У׼���� 3������ 4
char *pFrameType = "StaFrame";
//-----------------------------------------------------------------
// �����ź���
//-----------------------------------------------------------------
struct rt_semaphore HMICOMM_OPSem;
struct rt_semaphore HMICOMM_ST_OPSem;
struct rt_semaphore HMIGetCmd_OPSem;
//-----------------------------------------------------------------
// HMICOMM ���ͨѶ״̬��
//-----------------------------------------------------------------
//static uint16_t HMICOMMState = 0;
//-----------------------------------------------------------------
// HMICOMM ���ͻ�����
//-----------------------------------------------------------------
static uint8_t HMICOMMTxBuffer[HMICOMM_TX_BUFFER_SIZE_MAX]     = {0};
__IO uint8_t   HMICOMMTxBufferCnt  = 0;
__IO uint8_t   HMICOMMTxBufferSize = 0;
//-----------------------------------------------------------------
// HMICOMM ���ջ�����
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
* Description    : HMI RS485ͨ�ų�ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

int System_HW_HMICOMM_Init(void)
{
	rt_err_t rtv, rtv1,rtv2;
	//----------------------------------------------------------------------------
	// �����ź��� 
	//----------------------------------------------------------------------------
	rtv  = rt_sem_init(&HMICOMM_OPSem,     "SemExpCom",      0,  RT_IPC_FLAG_FIFO);
	rtv1 = rt_sem_init(&HMICOMM_ST_OPSem,  "SemExpComBus",   0,  RT_IPC_FLAG_FIFO);
    rtv2 = rt_sem_init(&HMIGetCmd_OPSem,   "SemGetCmd",   0,  RT_IPC_FLAG_FIFO);
	//-----------------------------------------------------------------------------
	// GPIO ����
	//-----------------------------------------------------------------------------
	HMICOMM_GPIO_Configuration();
	//-----------------------------------------------------------------------------
	// RS485 Bus to RX status
	//-----------------------------------------------------------------------------
	HMICOMM_DE_RX();
	//-----------------------------------------------------------------------------
	// USARTx ����
	//-----------------------------------------------------------------------------
	HMICOMM_UASRT_Configuration();
	//-----------------------------------------------------------------------------
	// MODBUS �õĳ�ʱ�����õĶ�ʱ��
	//-----------------------------------------------------------------------------
	HMICOMM_TIMx_Configration();
    //-----------------------------------------------------------------------------
    // 485ͨ��DMA ����
    //-----------------------------------------------------------------------------
    HMICOMM_DMA_Configuration();
	//-----------------------------------------------------------------------------
	// NVIC�趨
	//-----------------------------------------------------------------------------
	HMICOMM_NVIC_Configuration();
	//-----------------------------------------------------------------------------
	// �������ʹ��
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
* Description    : HMI RS485ͨ�ų�ʼ��
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
	// Enable USARTx ����ʱ�ӿ���
	//-----------------------------------------------------------------------------
	RCC_APB1PeriphClockCmd(HMICOMM_RCC, ENABLE); 
	//------------------------------------------------------------------------------
	//
	//    USARTͨ�Ŷ˿��趨
	//
	//------------------------------------------------------------------------------
	USART_ClockInitStructure.USART_Clock   = USART_Clock_Disable;
	USART_ClockInitStructure.USART_CPOL    = USART_CPOL_Low;
	USART_ClockInitStructure.USART_CPHA    = USART_CPHA_2Edge;
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Enable;
	USART_ClockInit(HMICOMM_USART_PORT, &USART_ClockInitStructure) ;
	//------------------------------------------------------------------------------
	//
	//    USARTx ͨ�Ŷ˿��趨
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
* Description    : ���ڳ�ʱ�ж��ö�ʱ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

static void HMICOMM_TIMx_Configration(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure = {0};
	uint16_t                 overtime;

    //TIM_Load = ���ֽ�λ�� * ��ʱ�ֽ��� * ��ʱ��Ƶ�� / �趨������
    //           �������㣬������С����һ�ɽ�λ
	overtime = (uint16_t)((10 * 3.5 * 10000 / HMICOMM_BAUDRATE)+1);

	if(overtime <= 1) 	
        overtime = 1;
	/* TIM1 clock enable [TIM1��ʱ������]*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 
	//---------------------------------------------------------------------------
	// ��ʼ����ʱ��1��TIM1
	//---------------------------------------------------------------------------
	TIM_DeInit( HMICOMM_USART_TIM );//��λTIM1��ʱ��
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
	/* Clear TIM  update pending flag[���TIM ����жϱ�־] */
	TIM_ClearFlag(HMICOMM_USART_TIM, TIM_FLAG_Update);


	/* Enable TIM  Update interrupt [TIM ����ж�����]*/
	TIM_ITConfig(HMICOMM_USART_TIM, TIM_IT_Update, ENABLE);  

	TIM_ClearFlag(HMICOMM_USART_TIM, TIM_FLAG_Update);
}
/*******************************************************************************
* Function Name  : USART_DMA_Configuration
* Description    : ���ô��ڷ���DMA
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
* Description    : ���ڽ����жϣ���ʱ�жϣ�DMA�ж��ж�����������
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
* Description    : USART_TIM[TIM] �жϴ����� 
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
		//������յ�������֡
#ifdef usartTransmit
		rt_sem_release(&HMICOMM_OPSem);
#else

        if(TemDataCnter == DMA_GetCurrDataCounter(HMICOMM_RX_DMA))
        {
            Bsp_StartDmaRx();
        }
            /* ������ʱ�ж� */
//          HMICOMM_USART_TIM->CNT = 0;
//          TIM_Cmd(HMICOMM_USART_TIM, ENABLE);
       
#endif
	}
}

/*******************************************************************************
* Function Name  : Bsp_HMICOMM_URT_IRQHandler
* Description    : �����жϷ�����
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
	// �ж�USART����
	//-------------------------------------------------------------------------
	if (USART_GetITStatus(HMICOMM_USART_PORT, USART_IT_RXNE) == SET)
	{    
		if(USART_GetFlagStatus(HMICOMM_USART_PORT, USART_FLAG_FE)==SET)
		{
            // ֡�������󣬶�ȡDR�Ĵ������
			temp = HMICOMM_USART_PORT->DR;
		}
		else
		{       
			m_Rx_Data = (u16)USART_ReceiveData(HMICOMM_USART_PORT);
            HMICOMMRxBuffer[HMICOMMRxBufferCnt++] = (uint8_t)(m_Rx_Data & 0xFF);  //������
            HMICOMM_USART_TIM->CNT = 0;
			TIM_Cmd(HMICOMM_USART_TIM, ENABLE);
		}

		USART_ClearITPendingBit(HMICOMM_USART_PORT, USART_IT_RXNE) ;
	} 
    //-------------------------------------------------------------------------
    //  �ж�DMA����
    //-------------------------------------------------------------------------
    if (USART_GetITStatus(HMICOMM_USART_PORT, USART_IT_IDLE) == SET)
    {
        temp = HMICOMM_USART_PORT->SR;
        temp = HMICOMM_USART_PORT->DR;


        /* Read the DMA RX Stream receive data number */
       TemDataCnter = DMA_GetCurrDataCounter(HMICOMM_RX_DMA);

        //������ʱ�ж�
        HMICOMM_USART_TIM->CNT = 0;
        TIM_Cmd(HMICOMM_USART_TIM, ENABLE);
        /* ������յ�������֡ */
        //Bsp_StartDmaRx();
    }
	//-------------------------------------------------------------------------
	// �ж�USART����
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
* Description    : DMA�жϣ���DMA������ɺ��׳��ź�������485ͨ��DE���з�ת
                   ͨ��תΪ����̬
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
* Description    : ����DMA �������� 
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
* Description    : ����DMA �������� 
* Input          : _dma_tx_len : �������ֽ���
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
* Description    : �������ݽӿ�
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void Bsp_HMICOMM_TX_Stream(R485_UR_TYPE * _rs485Tx)////////HMI
{
	//---------------------------------------------------------
	// RS485����ת����̬
	//---------------------------------------------------------
	HMICOMM_DE_TX();

	//---------------------------------------------------------
    // ���ɷ������ݣ������ط��ͳ���
	//---------------------------------------------------------
	rt_memcpy(HMICOMMTxBuffer, _rs485Tx->UR_Buffer, _rs485Tx->UR_SIZE);
    HMICOMMTxBufferSize = _rs485Tx->UR_SIZE;
	//---------------------------------------------------------
    // ������������
    //---------------------------------------------------------
    if(HMICOMMTxBufferSize > 0)
    {
       HMICOMMTxBufferCnt = 0;
	   /* enable the Transmit interrupt */
	   //USART_ITConfig(HMICOMM_USART_PORT, USART_IT_TXE, ENABLE);
       //--------------------------------------------------------------------
       //  ����DMA����
       //--------------------------------------------------------------------
       Bsp_StartDmaTx(HMICOMMTxBufferSize);
    }
    else
    {
        //---------------------------------------------------------
        // RS485����ת����̬
        //---------------------------------------------------------
        //HMICOMM_DE_RX();
        rt_sem_release(&HMICOMM_ST_OPSem);
    }
}

/*******************************************************************************
* Function Name  : Bsp_HMICOMM_RX_Stream
* Description    : ���� [ ���� ] ����  
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/


void Bsp_HMICOMM_RX_Stream(R485_UR_TYPE * _rs485Rx)//HMI
{
	uint16_t CrcResult;
	uint8_t  crc_l, crc_h;

	//----------------------------------------------------------------
	// ���ݽ�����ɣ����յ������ݽ���CRCУ��
	//----------------------------------------------------------------
	CrcResult = CRC16_Dn_Cal((uint8_t*)HMICOMMRxBuffer, (HMICOMMRxBufferCnt - 2), 0xFFFF);
	crc_l     = CrcResult & 0x00FF;
	crc_h     = (CrcResult >> 8) & 0x00FF;
	//----------------------------------------------------------------
	// CRCУ��ͨ��, ׼������Ӧ������
	//----------------------------------------------------------------
	if((HMICOMMRxBuffer[HMICOMMRxBufferCnt-1]==crc_h) && (HMICOMMRxBuffer[HMICOMMRxBufferCnt-2]==crc_l))
	{
		memcpy((void *)_rs485Rx->UR_Buffer, (void *)HMICOMMRxBuffer, HMICOMMRxBufferCnt-2);
		_rs485Rx->UR_SIZE = HMICOMMRxBufferCnt-2;
		
	}
	
	//--------------------------------------------------------------------
	// �������ݳ�������
	//--------------------------------------------------------------------
	HMICOMMRxBufferCnt  = 0;
	//--------------------------------------------------------------------
	// ��ս���RAM������
	//--------------------------------------------------------------------
	rt_memset((void*)HMICOMMRxBuffer, 0, HMICOMM_RX_BUFFER_SIZE_MAX);
}

/*******************************************************************************
* Function Name  : Bsp_HMICOMM_Bus_To_RX
* Description    : RS485������Ϊ����̬ 
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
	// ��ʱ����֤���ݷ������
	//--------------------------------------------------------------------
	//cnt = 10;
	cnt = 1000;
	while(cnt--);
	//----------------------------------------------------------------
	// ��RS485����Ϊ������̬
	//----------------------------------------------------------------
	if(firstfalg == 0)
		firstfalg = 1;
	HMICOMM_DE_RX();
}
