#include "stm32f10x.h"
#include "IERG3810_USART.h"

void IERG3810_USART2_init(u32 pclk1, u32 bound) {
	//USART2
	float temp;
	u16 mantissa;
	u16 fraction;
	temp = (float)(pclk1*1000000)/(bound*16);
	mantissa = temp;
	fraction = (temp - mantissa) * 16;
	mantissa <<= 4;
	mantissa += fraction;
	//ENABLE CLOCK PORTA
	RCC->APB2ENR |= 1 << 2;		//RM008 pg112
	//ENABLE CLOCK USART2
	RCC->APB1ENR |= 1 << 17;	//RM008 pg115
	//Clear PA2, PA3
	GPIOA->CRL &= 0xFFFF00FF;
	//Set PA2, PA3 alternate func.
	//PA2 = 1011 push-pull, output 50MHz
	//PA3 = 1000 push-pull, reserved
	GPIOA->CRL |= 0x00008B00;
	//Reset USART2
	RCC->APB1RSTR |= 1 << 17;	//RM008 pg109
	//Clear val. in register = NO EFFECT
	RCC->APB1RSTR &= ~(1<<17);

	//Set USARTDIV val.
	USART2->BRR = mantissa;		//RM008 pg820
	//bit 13 USART ENABLE
	//bit 3  Transmitter ENABLE
	USART2->CR1 |= 0x2008;		//RM008 pg821
}

void IERG3810_USART1_init(u32 pclk2, u32 bound) {
	//USART1
	float temp;
	u16 mantissa;
	u16 fraction;
	temp = (float)(pclk2*1000000)/(bound*16);
	mantissa = temp;
	fraction = (temp - mantissa) * 16;
	mantissa <<= 4;
	mantissa += fraction;
	RCC->APB2ENR |= 1 << 2;		//RM008 pg112
	RCC->APB2ENR |= 1 << 14;	//USART1 ENABLE (RM008 pg112)
	GPIOA->CRH &= 0xFFFFF00F;	//Clear PA9, PA10
	GPIOA->CRH |= 0x000008B0;	//Set PA9, PA10 alternate func.
	RCC->APB2RSTR |= 1 << 14;	//RM008 pg106
	RCC->APB2RSTR &= ~(1<<14);
	USART1->BRR = mantissa;		//RM008 pg820
	USART1->CR1 |= 0x2008;		//RM008 pg821
}

void USART_print(u8 USARTport, char *st) {
	u8 i = 0;
	while (st[i] != 0x00) {
		if(USARTport == 1) {
			USART1->DR = st[i];
			while(!(USART1->SR & (1 << 7)));
		}
		if(USARTport == 2) {
			USART2->DR = st[i];
			while(!(USART2->SR & (1 << 7)));
		}

		if (i == 255) break;
		i++;
	}
}
