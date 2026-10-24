#include "unity.h"
#include "fakes.h"
#include "stm32f407xx.h"
#include "stm32f407xx_usart_driver.h"

TEST_SOURCE_FILE("fakes.c")

void setUp(void)    { fakes_reset(); }   /* all-zero RCC = HSI 16 MHz, no prescalers */
void tearDown(void) {}

static USART_Handle_t make_8N1(uint32_t baud)
{
    USART_Handle_t h = {0};
    h.pUSARTx = USART2;
    h.USART_Config.USART_Mode          = USART_MODE_TXRX;
    h.USART_Config.USART_Baud          = baud;
    h.USART_Config.USART_NoOfStopBits  = USART_STOPBITS_1;
    h.USART_Config.USART_WordLength    = USART_WORDLEN_8BITS;
    h.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;
    h.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
    return h;
}

/* Verifies: REQ-USART-001 PCLK1 = 16 MHz on reset clock settings */
void test_RCC_GetPCLK1Value_is_16MHz_after_reset(void)
{
    TEST_ASSERT_EQUAL_UINT32(16000000u, RCC_GetPCLK1Value());
}

/* Verifies: REQ-USART-002 clock enable for USART2 (APB1ENR bit 17) */
void test_PeriClockControl_enables_USART2_clock(void)
{
    USART_PeriClockControl(USART2, ENABLE);
    TEST_ASSERT_BITS_HIGH(1u << 17, fake_RCC.APB1ENR);
}

/* Verifies: REQ-USART-003 115200 baud at 16 MHz -> BRR = 0x8B */
void test_SetBaudRate_115200_at_16MHz(void)
{
    USART_SetBaudRate(USART2, USART_STD_BAUD_115200);
    TEST_ASSERT_EQUAL_HEX32(0x8B, fake_USART2.BRR);
}

/* Verifies: REQ-USART-004 9600 baud at 16 MHz -> BRR = 0x683 */
void test_SetBaudRate_9600_at_16MHz(void)
{
    USART_SetBaudRate(USART2, USART_STD_BAUD_9600);
    TEST_ASSERT_EQUAL_HEX32(0x683, fake_USART2.BRR);
}

/* Verifies: REQ-USART-005 8N1, TX+RX: TE and RE set, M/PCE clear, 1 stop bit */
void test_Init_8N1_txrx(void)
{
    USART_Handle_t h = make_8N1(USART_STD_BAUD_115200);
    USART_Init(&h);
    TEST_ASSERT_BITS_HIGH((1u << USART_CR1_TE) | (1u << USART_CR1_RE), fake_USART2.CR1);
    TEST_ASSERT_BITS_LOW ((1u << USART_CR1_M)  | (1u << USART_CR1_PCE), fake_USART2.CR1);
    TEST_ASSERT_EQUAL_HEX32(0, fake_USART2.CR2 & (0x3u << USART_CR2_STOP));
    TEST_ASSERT_EQUAL_HEX32(0x8B, fake_USART2.BRR);
}

/* Verifies: REQ-USART-006 odd parity: PCE and PS set */
void test_Init_odd_parity(void)
{
    USART_Handle_t h = make_8N1(USART_STD_BAUD_115200);
    h.USART_Config.USART_ParityControl = USART_PARITY_EN_ODD;
    USART_Init(&h);
    TEST_ASSERT_BITS_HIGH((1u << USART_CR1_PCE) | (1u << USART_CR1_PS), fake_USART2.CR1);
}

/* Verifies: REQ-USART-007 2 stop bits -> CR2 STOP = 10 */
void test_Init_two_stop_bits(void)
{
    USART_Handle_t h = make_8N1(USART_STD_BAUD_115200);
    h.USART_Config.USART_NoOfStopBits = USART_STOPBITS_2;
    USART_Init(&h);
    TEST_ASSERT_EQUAL_HEX32(0x2u << USART_CR2_STOP, fake_USART2.CR2 & (0x3u << USART_CR2_STOP));
}

/* Verifies: REQ-USART-008 UE is set only by PeripheralControl */
void test_PeripheralControl_sets_UE(void)
{
    USART_PeripheralControl(USART2, ENABLE);
    TEST_ASSERT_BITS_HIGH(1u << USART_CR1_UE, fake_USART2.CR1);
}

/* Verifies: REQ-USART-009 SendData writes each byte to DR (8-bit, no parity) */
void test_SendData_writes_last_byte_to_DR(void)
{
    uint8_t msg[] = { 'H', 'i' };
    fake_USART2.SR = USART_FLAG_TXE | USART_FLAG_TC;   /* so the wait loops exit */
    USART_SendData(USART2, msg, 2);
    TEST_ASSERT_EQUAL_HEX32('i', fake_USART2.DR);
}
