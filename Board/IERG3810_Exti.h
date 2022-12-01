#ifndef __IERG3810_EXTI_H
#define __IERG3810_EXTI_H
#include "stm32f10x.h"

extern u32 timeout;
extern u32 ps2key;
extern u32 ps2count;
extern u32 ps2data;
extern u8 ps2dataReady;
extern u8 prevF0;

//Type: Odd Parity (even 1's => parity bit = 1, and vice versa)
extern u8 parity_lookup[16];
extern u8 expected_parity;

void IERG3810_NVIC_SetPriorityGroup(u8 prigroup);

void IERG3810_key2_ExtiInit(void);

void EXTI2_IRQHandler(void);

void IERG3810_keyUP_ExtiInit(void);

void EXTI0_IRQHandler(void);

void IERG3810_PS2key_ExtiInit(void);

void EXTI15_10_IRQHandler(void);

#endif
