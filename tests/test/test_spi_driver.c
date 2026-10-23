#include "unity.h"
#include "fakes.h"
#include "stm32f407xx.h"
#include "stm32f407xx_spi_driver.h"

TEST_SOURCE_FILE("fakes.c")

void setUp(void)    { fakes_reset(); }
void tearDown(void) {}

/* Verifies: REQ-SPI-010 regression for the NVIC fix: IRQ 64..95 -> ISER2 */
void test_IRQ71_enable_lands_in_ISER2_bit7(void)
{
    SPI_IRQInterruptConfig(71, ENABLE);
    TEST_ASSERT_BITS_HIGH(1u << 7, fake_NVIC_ISER[2]);
    TEST_ASSERT_EQUAL_HEX32(0, fake_NVIC_ISER[3]);
}

/* Verifies: REQ-SPI-011 regression for the disable path (>= 64, not >= 6) */
void test_IRQ71_disable_lands_in_ICER2_bit7(void)
{
    SPI_IRQInterruptConfig(71, DISABLE);
    TEST_ASSERT_BITS_HIGH(1u << 7, fake_NVIC_ICER[2]);
    TEST_ASSERT_EQUAL_HEX32(0, fake_NVIC_ICER[0]);
}

/* Verifies: REQ-SPI-012 IRQ 40 -> ISER1 bit 8 */
void test_IRQ40_enable_lands_in_ISER1_bit8(void)
{
    SPI_IRQInterruptConfig(40, ENABLE);
    TEST_ASSERT_BITS_HIGH(1u << 8, fake_NVIC_ISER[1]);
}

/* Verifies: REQ-SPI-013 regression for the 16-bit buffer fix:
   in 16-bit mode the buffer must advance 2 bytes per frame */
void test_SPI_SendData_16bit_sends_second_word_correctly(void)
{
    uint16_t data[2] = {0x1234, 0xABCD};

    fake_SPI2.CR1 |= (1u << 11);        /* DFF = 1: 16-bit frames          */
    fake_SPI2.SR  |= SPI_TXE_FLAG;      /* TXE set so the wait loop exits  */

    SPI_SendData(SPI2, (uint8_t *)data, 4);   /* 2 frames = 4 bytes */

    /* DR keeps the last word written. With the old bug it would be 0xCD12. */
    TEST_ASSERT_EQUAL_HEX16(0xABCD, (uint16_t)fake_SPI2.DR);
}
