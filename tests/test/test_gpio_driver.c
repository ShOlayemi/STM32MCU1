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

/* Verifies: REQ-GPIO-003 pull-up */
void test_GPIO_Init_enables_pullup_on_pin3(void)
{
    GPIO_Handle_t h = {0};
    h.pGPIOx = GPIOA;
    h.GPIO_PinConfig.GPIO_PinNumber      = 3;
    h.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_IN;
    h.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;

    GPIO_Init(&h);

    /* PUPDR bits 7:6 must be 01 (pull-up) */
    TEST_ASSERT_EQUAL_HEX32(0x1u << (2 * 3), fake_GPIOA.PUPDR & (0x3u << (2 * 3)));
}

/* Verifies: REQ-GPIO-004 open-drain output */
void test_GPIO_Init_sets_open_drain_on_pin2(void)
{
    GPIO_Handle_t h = {0};
    h.pGPIOx = GPIOA;
    h.GPIO_PinConfig.GPIO_PinNumber = 2;
    h.GPIO_PinConfig.GPIO_PinMode   = GPIO_MODE_OUT;
    h.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;

    GPIO_Init(&h);

    TEST_ASSERT_BITS_HIGH(1u << 2, fake_GPIOA.OTYPER);
}

/* Verifies: REQ-GPIO-005 regression for the EXTICR fix:
   configuring a second pin must not wipe the first pin's port */
void test_EXTICR_second_pin_keeps_first_pin_port(void)
{
    GPIO_Handle_t pd0 = {0}, pa1 = {0};
    pd0.pGPIOx = GPIOD;
    pd0.GPIO_PinConfig.GPIO_PinNumber = 0;
    pd0.GPIO_PinConfig.GPIO_PinMode   = GPIO_MODE_IT_FT;
    pa1.pGPIOx = GPIOA;
    pa1.GPIO_PinConfig.GPIO_PinNumber = 1;
    pa1.GPIO_PinConfig.GPIO_PinMode   = GPIO_MODE_IT_FT;

    GPIO_Init(&pd0);
    GPIO_Init(&pa1);

    /* EXTICR[0]: pin 0 = bits 3:0 must still be port D (3); pin 1 = bits 7:4 = port A (0) */
    TEST_ASSERT_EQUAL_HEX32(0x3u, fake_SYSCFG.EXTICR[0] & 0xFu);
    TEST_ASSERT_EQUAL_HEX32(0x0u, fake_SYSCFG.EXTICR[0] & 0xF0u);
}
