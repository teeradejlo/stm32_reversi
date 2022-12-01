#include "stm32f10x.h"
#include "IERG3810_KEY.h"

#define BOUNCE_TIME 50000

u8 key_2_in = 0, prev_key_2_in = 0;
u8 key_1_in = 0, prev_key_1_in = 0;
u8 key_0_in = 0, prev_key_0_in = 0;
u8 key_up_in = 0, prev_key_up_in = 0;

u8 key_2_start_count = 0, key_1_start_count = 0, key_0_start_count = 0, key_up_start_count = 0;
int key_2_countdown = 0, key_1_countdown = 0, key_0_countdown = 0, key_up_countdown = 0;

void IERG3810_KEY_Init(void) {
	//Clock ENABLE = Port A, E respectively
	RCC->APB2ENR |= 1 << 2;
	RCC->APB2ENR |= 1 << 6;

    //KEY2 = E2 = Input Pull Up
	GPIOE->CRL &= 0xFFFFF0FF;
	GPIOE->CRL |= 0x00000800;
	GPIOE->BSRR = 1 << 2;

	//KEY1 = E3 = Input Pull Up
	GPIOE->CRL &= 0xFFFF0FFF;
	GPIOE->CRL |= 0x00008000;
	GPIOE->BSRR = 1 << 3;

	//KEY0 = E4 = Input Pull Up
	GPIOE->CRL &= 0xFFFF0FFF;
	GPIOE->CRL |= 0x00008000;
	GPIOE->BSRR = 1 << 4;

	//KEY_UP = A0 = Input Pull Down
	GPIOA->CRL &= 0xFFFFFFF0;
	GPIOA->CRL |= 0x00000008;
	GPIOA->BRR = 1 << 0;
}

void keyInputVal_Init() {
	//Init Input KEY2
	if ((GPIOE->IDR & (1 << 2)) == 0) {
		key_2_in = 0;
	} else {
		key_2_in = 1;
	}
	prev_key_2_in = key_2_in;

	//Init Input KEY1
	if ((GPIOE->IDR & (1 << 3)) == 0) {
		key_1_in = 0;
	} else {
		key_1_in = 1;
	}
	prev_key_1_in = key_1_in;

	//Init Input KEY0
	if ((GPIOE->IDR & (1 << 4)) == 0) {
		key_0_in = 0;
	} else {
		key_0_in = 1;
	}
	prev_key_0_in = key_0_in;

	//Init Input KEY_UP
	if ((GPIOA->IDR & 1) == 0) {
		key_up_in = 0;
	} else {
		key_up_in = 1;
	}
	prev_key_up_in = key_up_in;
}

void debouncingVar_Init() {
	key_2_start_count = 0;
	key_2_countdown = BOUNCE_TIME;
	key_1_start_count = 0;
	key_1_countdown = BOUNCE_TIME;
	key_0_start_count = 0;
	key_0_countdown = BOUNCE_TIME;
	key_up_start_count = 0;
	key_up_countdown = BOUNCE_TIME;
}

u8 isDebouncing(u8* start_count, int* countdown) {
	if (*start_count) {
		if (*countdown < 0) {
			*start_count = 0;
			*countdown = BOUNCE_TIME;
		} else {
			(*countdown)--;
		}
		return 1;
	} else {
		return 0;
	}
}
