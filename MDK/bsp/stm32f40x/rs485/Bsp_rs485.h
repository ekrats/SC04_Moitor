
#ifndef __BSP_EXP_COMM_H__
#define __BSP_EXP_COMM_H__

#include "stm32f4xx.h"
#include "rtthread.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
//----------------------------------------------------
// 使用的USART
//----------------------------------------------------
//#define HMICOMM_USART_PORT         (USART1)
//#define HMICOMM_USART_PORT         (USART2)
#define HMICOMM_USART_PORT           (USART3)
//#define HMICOMM_USART_PORT         (UART4)
//#define HMICOMM_USART_PORT         (UART5)
//-----------------------------------------------------------------------------
// MODBUS 通信超时控制用Timx
//-----------------------------------------------------------------------------
#define HMICOMM_USART_TIM              (TIM1)
//-----------------------------------------------------------------------------
#define Board_ID                       (0x01)
// 本卡地址
//-----------------------------------------------------------------------------
#define HMICOMM_ADR                    (0x01)
#define CMDStateAddr                     (0x00)
#define CMDCtrRegAddr                     (0x100)
#define CMDMonitRegAddr                    (0x110)
#define CMDAdjRegAddr                     (0x120)
#define ERRREG_ADR                     (0x200)
#define SetAck_ADR                    (0x40)



//-----------------------------------------------------------------------------
// 多机接受处理状态机
//-----------------------------------------------------------------------------
#define IDLE_STATE                     (0)
#define RECEIVE_STATE                  (1)
//----------------------------------------------------------------------------------------
// USART外设相关宏定义
//----------------------------------------------------------------------------------------
#define USART_DR_OFFSET                (0x04)
#define USART1_DR_Base                 (USART1_BASE + USART_DR_OFFSET)//0x40013804
#define USART2_DR_Base                 (USART2_BASE + USART_DR_OFFSET)//0x40004404
#define USART3_DR_Base                 (USART3_BASE + USART_DR_OFFSET)//0x40004804
#define UART4_DR_Base                  (UART4_BASE  + USART_DR_OFFSET)//0x40004C04
#define UART5_DR_Base                  (UART5_BASE  + USART_DR_OFFSET)//0x40005004
//---------------------------------------------------
// USARTx GPIO管脚
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
// USARTx DE管脚 GPIO
//----------------------------------------------------
#define HMICOMM_DE485_GPIO             (GPIO_Pin_12)
#define HMICOMM_DE485_GPIOX            (GPIOB)
//----------------------------------------------------
// USARTx DE管脚
//----------------------------------------------------
#define HMICOMM_DE_TX()                (HMICOMM_DE485_GPIOX->BSRRH = HMICOMM_DE485_GPIO)  //set the GPIOA pin1
#define HMICOMM_DE_RX()                (HMICOMM_DE485_GPIOX->BSRRL = HMICOMM_DE485_GPIO)  //reset the GPIOA pin1
//----------------------------------------------------
// USARTx 校验
//----------------------------------------------------
#define HMICOMM_PARITY_MODE            (USART_Parity_Even)//USART_Parity_Odd
#define HMICOMM_PARITY_MODE_MB_INV     (USART_Parity_No)//USART_Parity_Odd  ModBus通信时用的校验模式
//----------------------------------------------------
// USARTx 波特率
//----------------------------------------------------
#define HMICOMM_BAUDRATE               (115200)
//----------------------------------------------------
// 缓冲区大小
//----------------------------------------------------
#define HMICOMM_TX_BUFFER_SIZE_MAX     (256)
#define HMICOMM_RX_BUFFER_SIZE_MAX     (256)
//----------------------------------------------------
// 使用的USART相关配置
//----------------------------------------------------
#define HMICOMM_DR_BASE                (USART3_DR_Base)
//----------------------------------------------------
// 相关RCC
//----------------------------------------------------
#define HMICOMM_RCC                    (RCC_APB1Periph_USART3)
#define HMICOMM_TIM_RCC                (RCC_APB2Periph_TIM1)   
//-----------------------------------------------------------------------------
// 相关中断
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


//HIM命令宏
#define CMD_TIMEADJ                  (0x10)  //时间校准
#define CMD_VOLADJ                   (0x12)  //电压校准
#define CMD_CURRADJ                  (0x14)  //电流校准
#define CMD_TEMPADJ                  (0x16)  //温度校准
#define CMD_CHARGPARA1               (0x18)  //充电参数1
#define CMD_CHARGPARA2               (0x1A)  //充电参数2
#define CMD_PID                      (0x1C)  //PID参数设定
#define CMD_I                        (0x1E)  //充电电流曲线
#define CMD_V                        (0x20)  //充电电压曲线
#define CMD_P                        (0x22)  //充电功率曲线
#define CMD_DEPTH                    (0x24)  //深度充电曲线
#define CMD_SYNVOLT                  (0x26)  //同步电压
#define CMD_CHARGENABLE              (0x28)  //充电允许
#define CMD_SUM                      (0x30)  //深度充电曲线
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
