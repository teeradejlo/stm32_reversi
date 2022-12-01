#ifndef __IERG3810_BUZZER_H
#define __IERG3810_BUZZER_H
#include "stm32f10x.h"

extern u8 buzzer_on;

/*
    Init BUZZERs GPIO
        BUZZER = B8 = Push-Pull
*/
void IERG3810_Buzzer_Init(void);

#endif
