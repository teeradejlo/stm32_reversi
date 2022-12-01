#include "stm32f10x.h"
#include "IERG3810_Timer.h"

void IERG3810_TIM3_Init(u16 arr, u16 psc) {
	//TIM3, IRQ#29
	RCC->APB1ENR |= 1 << 1;			//RM0008 pg 115
	TIM3->ARR = arr;						//RM0008 pg 419
	TIM3->PSC = psc;						//RM0008 pg 418
	TIM3->DIER |= 1 << 0;				//RM0008 pg 409, UIE (Update Interrupt Enable)
	TIM3->CR1 |= 0x01;					//RM0008 pg 404, CEN (Counter Enable)
	NVIC->IP[29] = 0x45;				//Refer to lab-4
	NVIC->ISER[0] |= (1 << 29);	//Refer to lab-4
}

void IERG3810_TIM3_PwmInit(u16 arr, u16 psc) {
	RCC->APB2ENR |= 1 << 3;			//clock enable port B
	GPIOB->CRL &= 0xFF0FFFFF;		//clear pin B5
	GPIOB->CRL |= 0x00B00000;		//alternate function push-pull, 50MHz
	RCC->APB2ENR |= 1 << 0;			//ENABLE AFIO Clock
	AFIO->MAPR &= 0xFFFFF3FF;		//clear TIM3_REMAP (bit[11:10])
	AFIO->MAPR |= 1 << 11;			//set bit 11 (TIM3_REMAP = 10)
	RCC->APB1ENR |= 1 << 1;			//RM0008 pg 115, ENABLE TIM3 clock
	TIM3->ARR = arr;						//RM0008 pg 419
	TIM3->PSC = psc;						//RM0008 pg 418
	TIM3->CCMR1 |= 7<<12;				//RM0008 pg 413
	TIM3->CCMR1 |= 1<<11;
	TIM3->CCER |= 1<<4;					//RM0008 pg 417
	TIM3->CR1 = 0x0080;					//RM0008 pg 404
	TIM3->CR1 |= 0x01;					
}

void TIM3_IRQHandler(void) {
	if (TIM3->SR & 1 << 0) {		//check UIF, RM0008 pg 410
		GPIOB->ODR ^= 1 << 5;			//toggle DS0 with read-modify-write
	}
	TIM3->SR &= ~(1 << 0);			//UIF, RM0008 pg 410
}

void IERG3810_TIM4_Init(u16 arr, u16 psc) {
	//TIM4, IRQ#30
	RCC->APB1ENR |= 1 << 2;			//RM0008 pg 115
	TIM4->ARR = arr;						//RM0008 pg 419
	TIM4->PSC = psc;						//RM0008 pg 418
	TIM4->DIER |= 1 << 0;				//RM0008 pg 409, UIE (Update Interrupt Enable)
	TIM4->CR1 |= 0x01;					//RM0008 pg 404, CEN (Counter Enable)
	NVIC->IP[30] = 0x45;				//Refer to lab-4
	NVIC->ISER[0] |= (1 << 30);	//Refer to lab-4
}

void TIM4_IRQHandler(void) {
	if (TIM4->SR & 1 << 0) {		//check UIF, RM0008 pg 410
		flip_delay++;
	}
	TIM4->SR &= ~(1 << 0);			//UIF, RM0008 pg 410
}

void IERG3810_SYSTICK_Init10ms(void) {
	//SYSTICK
	SysTick->CTRL = 0;					//Clear
	SysTick->LOAD = 90909;			//DDI 0337E pg 8-9
															//CLK freq = 9 MHz (72M / 8)
															//Clock pulse period = 1 / 9M = 110 ns
															//RELOAD = 1ms / 110ns = 90909
	SysTick->CTRL |= 0x03;			//CLKSOURCE = 0, TICKINT = 1 (pend handler), ENABLE = 1
}
