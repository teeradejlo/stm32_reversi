#include "stm32f10x.h"
#include "IERG3810_Exti.h"
#include "IERG3810_Clock.h"

u32 ps2key = 0;
u32 ps2count = 0;
u32 ps2data = 0;
u8 ps2dataReady = 0;
u8 prevF0 = 0;

//Type: Odd Parity (even 1's => parity bit = 1, and vice versa)
u8 parity_lookup[] = { 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1 };
u8 expected_parity;

void IERG3810_NVIC_SetPriorityGroup(u8 prigroup) {
	//Set PRIGROUP AIRCR[10:8]
	u32 temp, temp1;
	temp1 = prigroup & 0x00000007;
	temp1 <<= 8;
	temp = SCB->AIRCR; 		//ARM DDIO337E pg 8-22
	temp &= 0x0000F8FF;		//ARM DDIO337E pg 8-22
	temp |= 0x05FA0000;		//ARM DDIO337E pg 8-22
	temp |= temp1;
	SCB->AIRCR = temp;
}

void IERG3810_key2_ExtiInit(void) {
	//KEY2 at PE2, EXTI-2, IRQ#8
	RCC->APB2ENR |= 1 << 6;			//ENABLE Port E Clock
	GPIOE->CRL &= 0xFFFFF0FF;		//Modify PE2
	GPIOE->CRL |= 0x00000800;
	GPIOE->ODR |= 1 << 2;			//Input Pull Up Mode
	RCC->APB2ENR |= 0x01;			//RM008 pg 146, ENABLE AFIO Clock
	AFIO->EXTICR[0] &= 0xFFFFF0FF;	//RM008 pg 191, AFIO_EXTICR1
	AFIO->EXTICR[0] |= 0x00000400;
	EXTI->IMR |= 1 << 2;			//RM008 pg 211, edge trigger
	EXTI->FTSR |= 1 << 2;			//RM008 pg 212, falling edge
	//EXTI->RTSR |= 1 << 2;			//RM008 pg 212, rising edge

	NVIC->IP[8] = 0x65;				//set priority of this interrupt
	NVIC->ISER[0] &= ~(1<<8);		//set NVIC 'SET ENABLE REGISTER'
									//DDIO337E pg 8-3
	NVIC->ISER[0] |= 1 << 8;		//IRQ8
}

void EXTI2_IRQHandler(void) {
	u8 i;
	for (i = 0; i < 10; i++) {
		GPIOB->BRR = 1 << 5;
		Delay(1000000);
		GPIOB->BSRR = 1 << 5;
		Delay(1000000);
	}
	EXTI->PR = 1 << 2;	//clear this exception pending bit
}

void IERG3810_keyUP_ExtiInit(void) {
	//KEYUP at PA0, EXTI-0, IRQ#6

	//KEY_UP = A0 = Input Pull Down
	RCC->APB2ENR |= 1 << 2;
	GPIOA->CRL &= 0xFFFFFFF0;
	GPIOA->CRL |= 0x00000008;
	GPIOA->BRR = 1 << 0;

	//Configure EXTI0
	RCC->APB2ENR |= 0x01;
	AFIO->EXTICR[0] &= 0xFFFFFFF0;	//RM008 pg 191, AFIO_EXTICR1
	AFIO->EXTICR[0] |= 0x00000000;	//Select Pin A0

	EXTI->IMR |= 1 << 0;			//RM008 pg 211, edge trigger
	//EXTI->FTSR |= 1 << 0;			//RM008 pg 212, falling edge
	EXTI->RTSR |= 1 << 0;			//RM008 pg 212, rising edge

	NVIC->IP[6] = 0x95;				//set priority of this interrupt
	NVIC->ISER[0] &= ~(1<<6);		//set NVIC 'SET ENABLE REGISTER'
									//DDIO337E pg 8-3
	NVIC->ISER[0] |= 1 << 6;		//IRQ8
}

void EXTI0_IRQHandler(void) {
	u8 i;
	for (i = 0; i < 10; i++) {
		GPIOE->BRR = 1 << 5;
		Delay(1000000);
		GPIOE->BSRR = 1 << 5;
		Delay(1000000);
	}
	EXTI->PR = 1 << 0;	//clear this exception pending bit
}

void IERG3810_PS2key_ExtiInit(void) {
	//PC10 = data, PC11 = clock
	RCC->APB2ENR |= 1 << 4;
	//PC10, PC11 to Input Pull Up
	GPIOC->CRH &= 0xFFFF00FF;
	GPIOC->CRH |= 0x00008800;
	GPIOC->BSRR |= 1 << 11;
	GPIOC->BSRR |= 1 << 10;

	//Configure EXTI11
	RCC->APB2ENR |= 0x01;
	AFIO->EXTICR[2] &= 0xFFFF0FFF;	//RM008 pg 191, AFIO_EXTICR1
	AFIO->EXTICR[2] |= 0x00002000;	//Select Pin C11

	EXTI->IMR |= 1 << 11;			//RM008 pg 211, edge trigger
	EXTI->FTSR |= 1 << 11;			//RM008 pg 212, falling edge
	//EXTI->RTSR |= 1 << 11;		//RM008 pg 212, rising edge

	//IRQ#40
	NVIC->IP[40] = 0x05;			//set priority of this interrupt
	NVIC->ISER[1] &= ~(1<<8);		//set NVIC 'SET ENABLE REGISTER'
									//DDIO337E pg 8-3
	NVIC->ISER[1] |= 1 << 8;		//IRQ40
}

void EXTI15_10_IRQHandler(void) {
	//clear bit in data buffer, read input, add info to data buffer, add count, exit handler
	ps2key &= ~(1 << ps2count);
	ps2key |= ((GPIOC->IDR & (1 << 10)) >> 10) << ps2count;
	ps2count++;
	EXTI->PR = 1 << 11;
	EXTI->PR = 1 << 11;
}
