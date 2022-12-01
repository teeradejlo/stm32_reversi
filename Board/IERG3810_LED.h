#ifndef __IERG3810_LED_H
#define __IERG3810_LED_H
#include "stm32f10x.h"

extern u8 led1_on, led0_on;

/*
    Init LEDs GPIO
        DSO = LED0 = B5 = Push-Pull
        DS1 = LED1 = E5 = Push-Pull
*/
void IERG3810_LED_Init(void);

#endif
