/*
 * main.c
 *
 *  Created on: Mar 28, 2025
 *      Author: User
 */

#include "stm32F407xx.h"

int main(void)
{
	return 0;
}

void EXTI0_IRQHandler(void)
{
//	Handle the interrupt
	GPIO_IRQHandling(0);
}
