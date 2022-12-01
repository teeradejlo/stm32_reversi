#ifndef __IERG3810_KEY_H
#define __IERG3810_KEY_H
#include "stm32f10x.h"

//Debouncing Components Var
extern u8 key_2_start_count, key_1_start_count, key_0_start_count, key_up_start_count;
extern int key_2_countdown, key_1_countdown, key_0_countdown, key_up_countdown;

//Status Check Var
extern u8 key_2_in, prev_key_2_in;
extern u8 key_1_in, prev_key_1_in;
extern u8 key_0_in, prev_key_0_in;
extern u8 key_up_in, prev_key_up_in;

/*
    Init KEYs GPIO
        KEY_UP = A0 = Pull-Down
        KEY_0 = E4 = Pull-Up
        KEY_1 = E3 = Pull-Up
        KEY_2 = E2 = Pull-Up
*/
void IERG3810_KEY_Init(void);

void keyInputVal_Init(void);

void debouncingVar_Init(void);

u8 isDebouncing(u8 * start_count, int* countdown);

#endif
