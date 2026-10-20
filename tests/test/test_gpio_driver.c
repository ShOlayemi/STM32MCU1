#include "unity.h"
#include "fakes.h"
#include "stm32f407xx.h"
#include "stm32f407xx_gpio_driver.h"

TEST_SOURCE_FILE("fakes.c")

void setUp(void)    { fakes_reset(); }
void tearDown(void) {}

/* Verifies: REQ-GPIO-001 output mode */
void test_GPIO_Init_sets_pin5_as_output(void)
{
    GPIO_Handle_t h = {0};
    h.pGPIOx = GPIOA;
    h.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
    h.GPIO_PinConfig.GPIO_PinMode   = GPIO_MODE_OUT;

    GPIO_Init(&h);

    /* MODER bits 11:10 must be 01 (general-purpose output) */
    TEST_ASSERT_EQUAL_HEX32(0x1u << (2 * 5), fake_GPIOA.MODER & (0x3u << (2 * 5)));
}

/* Verifies: REQ-GPIO-002 falling-edge interrupt on PD6 */
void test_GPIO_Init_falling_edge_interrupt_on_PD6(void)
{
    GPIO_Handle_t h = {0};
    h.pGPIOx = GPIOD;
    h.GPIO_PinConfig.GPIO_PinNumber = 6;
    h.GPIO_PinConfig.GPIO_PinMode   = GPIO_MODE_IT_FT;

    GPIO_Init(&h);

    TEST_ASSERT_BITS_HIGH(1u << 6, fake_EXTI.FTSR);   /* falling edge on   */
    TEST_ASSERT_BITS_LOW (1u << 6, fake_EXTI.RTSR);   /* rising edge off   */
    TEST_ASSERT_BITS_HIGH(1u << 6, fake_EXTI.IMR);    /* line unmasked (your fix) */
    /* EXTICR[1] covers pins 4..7; pin 6 uses bits 11:8; port D code = 3 */
    TEST_ASSERT_EQUAL_HEX32(0x3u << 8, fake_SYSCFG.EXTICR[1] & (0xFu << 8));
}
