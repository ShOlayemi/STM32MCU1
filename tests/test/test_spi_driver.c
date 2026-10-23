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
