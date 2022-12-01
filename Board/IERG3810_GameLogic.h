#ifndef __IERG3810_GAMELOGIC_H
#define __IERG3810_GAMELOGIC_H
#include "stm32f10x.h"
#include "global.H"

void IEGR3810_GameLogic_ClearGBoard(void);

void IEGR3810_GameLogic_ClearLegal(void);

void IEGR3810_GameLogic_ClearFlipping(void);

u8 IEGR3810_GameLogic_UpdateLegal(Pieces turn);

void IEGR3810_GameLogic_UpdateCursorPos(signed char dirX, signed char dirY);

void IEGR3810_GameLogic_EnqueueFlippingNeeded(u8 row, u8 col, Pieces turn);

#endif
