#include "stm32f407xx_usart_driver.h"

static const uint16_t AHB_PreScaler[8] = {2, 4, 8, 16, 64, 128, 256, 512};
static const uint8_t  APB_PreScaler[4] = {2, 4, 8, 16};

/* System clock: RCC->CFGR bits 3:2 (SWS). 0 = HSI 16 MHz, 1 = HSE 8 MHz (Discovery crystal).
   PLL is not used in this project. */
static uint32_t RCC_GetSystemClock(void)
{
    /* TODO: read SWS and return 16000000 or 8000000 */
    uint8_t clksrc = (RCC->CFGR >> 2) & 0x3;
    if (clksrc == 1) return 8000000U;   /* HSE */
    return 16000000U;                   /* HSI (default after reset) */
}

/* AHB prescaler: CFGR bits 7:4 (HPRE). Value < 8 -> divide by 1, else AHB_PreScaler[value - 8] */
static uint32_t RCC_GetAHBPrescaler(void)
{
    /* TODO */
    uint8_t temp = (RCC->CFGR >> 4) & 0xF;
    if (temp < 8) return 1;
    return AHB_PreScaler[temp - 8];
}

uint32_t RCC_GetPCLK1Value(void)
{
    /* TODO: APB1 prescaler = CFGR bits 12:10 (PPRE1). < 4 -> 1, else APB_PreScaler[value - 4]
             return SystemClock / AHB prescaler / APB1 prescaler */
    uint32_t ahb = RCC_GetSystemClock() / RCC_GetAHBPrescaler();
    uint8_t temp = (RCC->CFGR >> 10) & 0x7;
    uint16_t apb1 = (temp < 4) ? 1 : APB_PreScaler[temp - 4];
    return ahb / apb1;
}

uint32_t RCC_GetPCLK2Value(void)
{
    /* TODO: same as PCLK1 but with CFGR bits 15:13 (PPRE2) */
    uint32_t ahb = RCC_GetSystemClock() / RCC_GetAHBPrescaler();
    uint8_t temp = (RCC->CFGR >> 13) & 0x7;
    uint16_t apb1 = (temp < 4) ? 1 : APB_PreScaler[temp - 4];
    return ahb / apb1;
}

void USART_PeriClockControl(USART_RegDef_t *pUSARTx, uint8_t EnorDi)
{
    /* TODO: if ENABLE, compare pUSARTx with USART1, USART2, USART3, UART4, UART5, USART6
             and call the matching USARTx_PCLK_EN() macro; else the _PCLK_DI() macro */
    if (EnorDi == ENABLE) {
        if      (pUSARTx == USART1) USART1_PCLK_EN();
        else if (pUSARTx == USART2) USART2_PCLK_EN();
        else if (pUSARTx == USART3) USART3_PCLK_EN();
        else if (pUSARTx == UART4)  UART4_PCLK_EN();
        else if (pUSARTx == UART5)  UART5_PCLK_EN();
        else if (pUSARTx == USART6) USART6_PCLK_EN();
} else {
    /* same ladder with _PCLK_DI() */
    if (EnorDi == ENABLE) pUSARTx->CR1 |=  (1 << USART_CR1_UE);
    else                  pUSARTx->CR1 &= ~(1 << USART_CR1_UE);
}
}

void USART_PeripheralControl(USART_RegDef_t *pUSARTx, uint8_t EnorDi)
{
    /* TODO: set or clear CR1 bit UE */
    if (EnorDi == ENABLE) pUSARTx->CR1 |=  (1 << USART_CR1_UE);
    else                  pUSARTx->CR1 &= ~(1 << USART_CR1_UE);
}

void USART_SetBaudRate(USART_RegDef_t *pUSARTx, uint32_t BaudRate)
{
    /* TODO:
       1. PCLK = PCLK2 for USART1/USART6, PCLK1 for the others
       2. If CR1.OVER8 is set: usartdiv = (25 * PCLK) / (2 * BaudRate)
          else:                usartdiv = (25 * PCLK) / (4 * BaudRate)
          (usartdiv is USARTDIV x 100, so we stay in integers)
       3. mantissa = usartdiv / 100            -> goes in BRR bits 15:4
       4. fraction = usartdiv - mantissa * 100
          OVER8 = 1: fraction = ((fraction * 8)  + 50) / 100 & 0x07
          OVER8 = 0: fraction = ((fraction * 16) + 50) / 100 & 0x0F   -> BRR bits 3:0
       5. Write BRR = (mantissa << 4) | fraction */
    uint32_t PCLKx, usartdiv, M_part, F_part, tempreg = 0;

if (pUSARTx == USART1 || pUSARTx == USART6) PCLKx = RCC_GetPCLK2Value();
else                                        PCLKx = RCC_GetPCLK1Value();

if (pUSARTx->CR1 & (1 << USART_CR1_OVER8))
    usartdiv = (25 * PCLKx) / (2 * BaudRate);   /* over8 */
else
    usartdiv = (25 * PCLKx) / (4 * BaudRate);   /* over16 */

M_part = usartdiv / 100;
tempreg |= M_part << 4;

F_part = usartdiv - (M_part * 100);
if (pUSARTx->CR1 & (1 << USART_CR1_OVER8))
    F_part = (((F_part * 8) + 50) / 100) & 0x07;
else
    F_part = (((F_part * 16) + 50) / 100) & 0x0F;

tempreg |= F_part;
pUSARTx->BRR = tempreg;
}

void USART_Init(USART_Handle_t *pUSARTHandle)
{
    uint32_t tempreg = 0;
    uint32_t reg = 0;

    /* Enable the peripheral clock */
    USART_PeriClockControl(pUSARTHandle->pUSARTx, ENABLE);

    /* TODO CR1:
       - Mode: ONLY_RX -> RE, ONLY_TX -> TE, TXRX -> both
       - Word length -> bit M
       - Parity: EN_EVEN -> PCE only; EN_ODD -> PCE and PS
       write reg to CR1 */
    if (pUSARTHandle->USART_Config.USART_Mode == USART_MODE_ONLY_RX)
    tempreg |= (1 << USART_CR1_RE);
else if (pUSARTHandle->USART_Config.USART_Mode == USART_MODE_ONLY_TX)
    tempreg |= (1 << USART_CR1_TE);
else if (pUSARTHandle->USART_Config.USART_Mode == USART_MODE_TXRX)
    tempreg |= (1 << USART_CR1_RE) | (1 << USART_CR1_TE);

/* CR1: word length */
tempreg |= pUSARTHandle->USART_Config.USART_WordLength << USART_CR1_M;

/* CR1: parity */
if (pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_EN_EVEN)
    tempreg |= (1 << USART_CR1_PCE);                      /* PS = 0 is even */
else if (pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_EN_ODD)
    tempreg |= (1 << USART_CR1_PCE) | (1 << USART_CR1_PS);

pUSARTHandle->pUSARTx->CR1 = tempreg;

    /* TODO CR2: stop bits value << USART_CR2_STOP, write to CR2 */
    tempreg = 0;
    tempreg |= pUSARTHandle->USART_Config.USART_NoOfStopBits << USART_CR2_STOP;
    pUSARTHandle->pUSARTx->CR2 = tempreg;
    /* TODO CR3: CTS -> CTSE, RTS -> RTSE, CTS_RTS -> both, write to CR3 */
    tempreg = 0;
    if (pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_CTS)
        tempreg |= (1 << USART_CR3_CTSE);
    else if (pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_RTS)
        tempreg |= (1 << USART_CR3_RTSE);
    else if (pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_CTS_RTS)
        tempreg |= (1 << USART_CR3_CTSE) | (1 << USART_CR3_RTSE);
    pUSARTHandle->pUSARTx->CR3 = tempreg;
    /* Baud rate */
    USART_SetBaudRate(pUSARTHandle->pUSARTx, pUSARTHandle->USART_Config.USART_Baud);
    (void)reg;
}

uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSARTx, uint32_t FlagName)
{
    /* TODO: return FLAG_SET if (SR & FlagName) else FLAG_RESET */
    if (pUSARTx->SR & FlagName) return FLAG_SET;
    return FLAG_RESET;
}

void USART_SendData(USART_RegDef_t *pUSARTx, uint8_t *pTxBuffer, uint32_t Len)
{
    /* TODO: for each byte:
         wait until TXE is set
         8-bit, no parity : DR = *pTxBuffer & 0xFF;  pTxBuffer++
         (9-bit: you may add later - not needed for the tests)
       after the loop: wait until TC is set */
    for (uint32_t i = 0; i < Len; i++) {
        while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == FLAG_RESET);
        pUSARTx->DR = (*pTxBuffer & 0xFF);
        pTxBuffer++;
    }
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == FLAG_RESET);
}

void USART_ReceiveData(USART_RegDef_t *pUSARTx, uint8_t *pRxBuffer, uint32_t Len)
{
    /* TODO: for each byte: wait until RXNE is set, then *pRxBuffer = (uint8_t)(DR & 0xFF); pRxBuffer++ */
    for (uint32_t i = 0; i < Len; i++) {
        while (USART_GetFlagStatus(pUSARTx, USART_FLAG_RXNE) == FLAG_RESET);
        *pRxBuffer = (uint8_t)(pUSARTx->DR & 0xFF);
        pRxBuffer++;
    }
}
