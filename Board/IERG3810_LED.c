#include "stm32f10x.h"
#include "IERG3810_LED.h"

u8 led1_on = 0, led0_on = 0;

void IERG3810_LED_Init(void) {
	//Clock ENABLE = Port B, E respectively
	RCC->APB2ENR |= 1 << 3;
	RCC->APB2ENR |= 1 << 6;

    //LED0 = B5 = PP
	GPIOB->CRL &= 0xFF0FFFFF;
	GPIOB->CRL |= 0x00300000;
	GPIOB->BSRR = 1 << 5;

	//LED1 = E5 = PP
	GPIOE->CRL &= 0xFF0FFFFF;
	GPIOE->CRL |= 0x00300000;
	GPIOE->BSRR = 1 << 5;

	led1_on = 0;
	led0_on = 0;
}
