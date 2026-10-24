#ifndef INC_STM32F407XX_USART_DRIVER_H_
#define INC_STM32F407XX_USART_DRIVER_H_

#include "stm32f407xx.h"

typedef struct
{
    uint8_t  USART_Mode;            /* @USART_Mode          */
    uint32_t USART_Baud;            /* @USART_Baud          */
    uint8_t  USART_NoOfStopBits;    /* @USART_NoOfStopBits  */
    uint8_t  USART_WordLength;      /* @USART_WordLength    */
    uint8_t  USART_ParityControl;   /* @USART_ParityControl */
    uint8_t  USART_HWFlowControl;   /* @USART_HWFlowControl */
} USART_Config_t;

typedef struct
{
    USART_RegDef_t *pUSARTx;
    USART_Config_t  USART_Config;
} USART_Handle_t;

/* @USART_Mode */
#define USART_MODE_ONLY_TX   0
#define USART_MODE_ONLY_RX   1
#define USART_MODE_TXRX      2
/* @USART_Baud */
#define USART_STD_BAUD_9600     9600
#define USART_STD_BAUD_115200   115200
/* @USART_ParityControl */
#define USART_PARITY_DISABLE   0
#define USART_PARITY_EN_EVEN   1
#define USART_PARITY_EN_ODD    2
/* @USART_WordLength */
#define USART_WORDLEN_8BITS  0
#define USART_WORDLEN_9BITS  1
/* @USART_NoOfStopBits */
#define USART_STOPBITS_1     0
#define USART_STOPBITS_0_5   1
#define USART_STOPBITS_2     2
#define USART_STOPBITS_1_5   3
/* @USART_HWFlowControl */
#define USART_HW_FLOW_CTRL_NONE     0
#define USART_HW_FLOW_CTRL_CTS      1
#define USART_HW_FLOW_CTRL_RTS      2
#define USART_HW_FLOW_CTRL_CTS_RTS  3

/* Status flags */
#define USART_FLAG_TXE    (1 << USART_SR_TXE)
#define USART_FLAG_RXNE   (1 << USART_SR_RXNE)
#define USART_FLAG_TC     (1 << USART_SR_TC)

/* Clock helpers (read the clock tree from RCC) */
uint32_t RCC_GetPCLK1Value(void);
uint32_t RCC_GetPCLK2Value(void);

/* API */
void    USART_PeriClockControl(USART_RegDef_t *pUSARTx, uint8_t EnorDi);
void    USART_PeripheralControl(USART_RegDef_t *pUSARTx, uint8_t EnorDi);
void    USART_Init(USART_Handle_t *pUSARTHandle);
void    USART_SetBaudRate(USART_RegDef_t *pUSARTx, uint32_t BaudRate);
uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSARTx, uint32_t FlagName);
void    USART_SendData(USART_RegDef_t *pUSARTx, uint8_t *pTxBuffer, uint32_t Len);
void    USART_ReceiveData(USART_RegDef_t *pUSARTx, uint8_t *pRxBuffer, uint32_t Len);

#endif /* INC_STM32F407XX_USART_DRIVER_H_ */
