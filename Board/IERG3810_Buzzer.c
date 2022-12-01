#include "stm32f10x.h"
#include "IERG3810_Buzzer.h"

u8 buzzer_on = 0;

void IERG3810_Buzzer_Init(void) {
	//Clock ENABLE = Port B
	RCC->APB2ENR |= 1 << 3;

    //Buzzer = B8 = PP
	GPIOB->CRH &= 0xFFFFFFF0;
	GPIOB->CRH |= 0x00000003;
	//Set output to low = turn buzzer off
	GPIOB->BRR = 1 << 8;
	buzzer_on = 0;
}
