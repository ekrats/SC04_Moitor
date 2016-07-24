
#ifndef __BSP_EXP_COMM_H__
#define __BSP_EXP_COMM_H__

#include "stm32f4xx.h"
#include "rtthread.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
//----------------------------------------------------
// ʹ�õ�USART
//----------------------------------------------------
//#define HMICOMM_USART_PORT         (USART1)
//#define HMICOMM_USART_PORT         (USART2)
#define HMICOMM_USART_PORT           (USART3)
//#define HMICOMM_USART_PORT         (UART4)
//#define HMICOMM_USART_PORT         (UART5)
//-----------------------------------------------------------------------------
// MODBUS ͨ�ų�ʱ������Timx
//-----------------------------------------------------------------------------
#define HMICOMM_USART_TIM              (TIM1)
//-----------------------------------------------------------------------------
#define Board_ID                       (0x01)
// ������ַ
//-----------------------------------------------------------------------------
#define HMICOMM_ADR                    (0x01)
#define CMDStateAddr                     (0x00)
#define CMDCtrRegAddr                     (0x100)
#define CMDMonitRegAddr                    (0x110)
#define CMDAdjRegAddr                     (0x120)
#define ERRREG_ADR                     (0x200)
#define SetAck_ADR                    (0x40)



//-----------------------------------------------------------------------------
// ������ܴ���״̬��
//-----------------------------------------------------------------------------
#define IDLE_STATE                     (0)
#define RECEIVE_STATE                  (1)
//----------------------------------------------------------------------------------------
// USART������غ궨��
//----------------------------------------------------------------------------------------
#define USART_DR_OFFSET                (0x04)
#define USART1_DR_Base                 (USART1_BASE + USART_DR_OFFSET)//0x40013804
#define USART2_DR_Base                 (USART2_BASE + USART_DR_OFFSET)//0x40004404
#define USART3_DR_Base                 (USART3_BASE + USART_DR_OFFSET)//0x40004804
#define UART4_DR_Base                  (UART4_BASE  + USART_DR_OFFSET)//0x40004C04
#define UART5_DR_Base                  (UART5_BASE  + USART_DR_OFFSET)//0x40005004
//---------------------------------------------------
// USARTx GPIO�ܽ�
//---------------------------------------------------
#define HMICOMM_TX_GPIO                (GPIO_Pin_10)
#define HMICOMM_TX_GPIO_SOURCE         (GPIO_PinSource10)
#define HMICOMM_TX_GPIO_P              (GPIOB)
#define HMICOMM_TX_GPIO_CLK            (RCC_AHB1Periph_GPIOB)
#define HMICOMM_RX_GPIO                (GPIO_Pin_11)
#define HMICOMM_RX_GPIO_SOURCE         (GPIO_PinSource11)
#define HMICOMM_RX_GPIO_P              (GPIOB)
#define HMICOMM_RX_GPIO_CLK            (RCC_AHB1Periph_GPIOB)
#define HMICOMM_USART_AF               (GPIO_AF_USART3)
#define HMICOMM_TX_DMA                 (DMA1_Stream3)
#define HMICOMM_RX_DMA                 (DMA1_Stream1)
#define HMICOMM_DMA_CLK                (RCC_AHB1Periph_DMA1)
#define HMICOMM_RX_DMA_CHANNEL         (DMA_Channel_4)
#define HMICOMM_TX_DMA_CHANNEL         (DMA_Channel_4)
//----------------------------------------------------
// USARTx DE�ܽ� GPIO
//----------------------------------------------------
#define HMICOMM_DE485_GPIO             (GPIO_Pin_12)
#define HMICOMM_DE485_GPIOX            (GPIOB)
//----------------------------------------------------
// USARTx DE�ܽ�
//----------------------------------------------------
#define HMICOMM_DE_TX()                (HMICOMM_DE485_GPIOX->BSRRH = HMICOMM_DE485_GPIO)  //set the GPIOA pin1
#define HMICOMM_DE_RX()                (HMICOMM_DE485_GPIOX->BSRRL = HMICOMM_DE485_GPIO)  //reset the GPIOA pin1
//----------------------------------------------------
// USARTx У��
//----------------------------------------------------
#define HMICOMM_PARITY_MODE            (USART_Parity_Even)//USART_Parity_Odd
#define HMICOMM_PARITY_MODE_MB_INV     (USART_Parity_No)//USART_Parity_Odd  ModBusͨ��ʱ�õ�У��ģʽ
//----------------------------------------------------
// USARTx ������
//----------------------------------------------------
#define HMICOMM_BAUDRATE               (115200)
//----------------------------------------------------
// ��������С
//----------------------------------------------------
#define HMICOMM_TX_BUFFER_SIZE_MAX     (256)
#define HMICOMM_RX_BUFFER_SIZE_MAX     (256)
//----------------------------------------------------
// ʹ�õ�USART�������
//----------------------------------------------------
#define HMICOMM_DR_BASE                (USART3_DR_Base)
//----------------------------------------------------
// ���RCC
//----------------------------------------------------
#define HMICOMM_RCC                    (RCC_APB1Periph_USART3)
#define HMICOMM_TIM_RCC                (RCC_APB2Periph_TIM1)   
//-----------------------------------------------------------------------------
// ����ж�
//-----------------------------------------------------------------------------
#define HMICOMM_EXTI_IRQ               (EXTI15_10_IRQn)
#define HMICOMM_TIM_IRQ                (TIM1_CC_IRQn)
#define HMICOMM_DMA_IRQ                (DMA1_Stream3_IRQn)
#define HMICOMM_EXTI_PrePriority       (1)
#define HMICOMM_EXTI_SubPriority       (2)
#define HMICOMM_TIM_IRQ_PrePriority    (1)
#define HMICOMM_TIM_IRQ_SubPriority    (2)
#define HMICOMM_DMA_IRQ_PrePriority    (1)
#define HMICOMM_DMA_IRQ_SubPriority    (2)

#define SysStatueType                  (0)
#define CmdCtrBoard                    (1)
#define CmdMonitBoard                 (2)
#define CmdAdj                         (3)
#define ErrType                        (4)
#define SetAck                         (5)


//HIM�����
#define CMD_TIMEADJ                  (0x10)  //ʱ��У׼
#define CMD_VOLADJ                   (0x12)  //��ѹУ׼
#define CMD_CURRADJ                  (0x14)  //����У׼
#define CMD_TEMPADJ                  (0x16)  //�¶�У׼
#define CMD_CHARGPARA1               (0x18)  //������1
#define CMD_CHARGPARA2               (0x1A)  //������2
#define CMD_PID                      (0x1C)  //PID�����趨
#define CMD_I                        (0x1E)  //����������
#define CMD_V                        (0x20)  //����ѹ����
#define CMD_P                        (0x22)  //��繦������
#define CMD_DEPTH                    (0x24)  //��ȳ������
#define CMD_SYNVOLT                  (0x26)  //ͬ����ѹ
#define CMD_CHARGENABLE              (0x28)  //�������
#define CMD_SUM                      (0x30)  //��ȳ������
#define UART_RX_BUF_LEN					128//bytes
#define UART_TX_BUF_LEN					600

typedef struct {
	u8 UR_Buffer[HMICOMM_RX_BUFFER_SIZE_MAX];
	u8 UR_SIZE;
}R485_UR_TYPE;

int  System_HW_HMICOMM_Init(void);
void Bsp_HMICOMM_RX_Stream(R485_UR_TYPE * _Rs485);
void Bsp_HMICOMM_Bus_To_RX(void);
void Bsp_HMICOMM_URT_IRQHandler(void);
void Bsp_HMICOMM_TIM_Handler(void);
void Bsp_HMICOMM_DMA_Tx_Irqhandler(void);
void Bsp_HMICOMM_TX_Stream(R485_UR_TYPE * _Rs485);
void HMI_TxDataUpdate(void);
void HMIGet_Dat(void);
#endif
