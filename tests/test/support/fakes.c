#include <string.h>
#include "fakes.h"
GPIO_RegDef_t   fake_GPIOA, fake_GPIOB, fake_GPIOC, fake_GPIOD, fake_GPIOE,
                fake_GPIOF, fake_GPIOG, fake_GPIOH, fake_GPIOI;
RCC_RegDef_t    fake_RCC;
EXTI_RegDef_t   fake_EXTI;
SYSCFG_RegDef_t fake_SYSCFG;
SPI_RegDef_t    fake_SPI1, fake_SPI2, fake_SPI3;
uint32_t        fake_NVIC_ISER[4], fake_NVIC_ICER[4], fake_NVIC_IPR[60];

void fakes_reset(void)
{
    memset(&fake_GPIOA, 0, sizeof fake_GPIOA);  memset(&fake_GPIOB, 0, sizeof fake_GPIOB);
    memset(&fake_GPIOC, 0, sizeof fake_GPIOC);  memset(&fake_GPIOD, 0, sizeof fake_GPIOD);
    memset(&fake_GPIOE, 0, sizeof fake_GPIOE);  memset(&fake_GPIOF, 0, sizeof fake_GPIOF);
    memset(&fake_GPIOG, 0, sizeof fake_GPIOG);  memset(&fake_GPIOH, 0, sizeof fake_GPIOH);
    memset(&fake_GPIOI, 0, sizeof fake_GPIOI);
    memset(&fake_RCC, 0, sizeof fake_RCC);      memset(&fake_EXTI, 0, sizeof fake_EXTI);
    memset(&fake_SYSCFG, 0, sizeof fake_SYSCFG);
    memset(&fake_SPI1, 0, sizeof fake_SPI1);    memset(&fake_SPI2, 0, sizeof fake_SPI2);
    memset(&fake_SPI3, 0, sizeof fake_SPI3);
    memset(fake_NVIC_ISER, 0, sizeof fake_NVIC_ISER);
    memset(fake_NVIC_ICER, 0, sizeof fake_NVIC_ICER);
    memset(fake_NVIC_IPR, 0, sizeof fake_NVIC_IPR);
}
