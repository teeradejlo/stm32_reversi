#include "stm32f10x.h"
#include "IERG3810_Clock.h"

void Delay(u32 count) {
	u32 i;
	for (i=0; i<count;i++);
}

void IERG3810_clock_tree_init(void) {
	//board's crystal generate 8MHz clk
	u8 PLL = 7;
	unsigned char temp = 0;
	RCC->CFGR &= 0xF8FF0000;	//RCC->CFGR = RM008 pg101
								//0b 1111 1000 1111 1111 0000 0000 0000 0000
								//RCC->CR   = RM008 pg99
								//0b 1111 1110 1111 0110 1111 1111 1111 1111
	RCC->CR &= 0xFEF6FFFF;		//PLLON = 0 [24], CSSON = 0 [19], HSEON = 0 [16]
	RCC->CR |= 0x00010000;		//HSEON = 1 [16]
	while(!(RCC->CR>>17));		//Check HSERDY
	RCC->CFGR = 0x00000400;		//PPRE1 = 100 [10:8], set APB1 (HCLK divided by 2)
	RCC->CFGR &= 0xFFFFC7FF;  	//PPRE2 = 000 [13:11], set APB2 (HCLK not divided)
	RCC->CFGR |= PLL<<18;		//PLLMUL = 0111 [21:18], set PLL input clock 9x
	RCC->CFGR |= 1 << 16;		//PLLSRC = 1 [16]
								//FLASH->ACR = RM008 pg60
	FLASH->ACR |= 0x30;			//72MHz should set FLASH with 2 wait states,
	RCC->CR |= 0x01000000;		//PLLON = 1 [24]
	while(!(RCC->CR>>25));		//Check PLLRDY [25]
	RCC->CFGR|= 0x00000002;		//SW = 10 [1:0]
	while(temp != 0x02) {		//check SWS[1:0] (RM008 pg101)
		temp = RCC->CFGR>>2;	//check SWS[1:0] status
		temp &= 0x03;			//keep last two bits
	}
}
