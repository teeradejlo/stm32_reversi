#ifndef __IERG3810_TFTLCD_H
#define __IERG3810_TFTLCD_H
#include "stm32f10x.h"
#include "global.H"

void IERG3810_TFTLCD_WrReg(u16 regval);

void IERG3810_TFTLCD_WrData(u16 data);

void IERG3810_TFTLCD_SetParameter(void);

void IERG3810_TFTLCD_Init(void);

void IERG3810_TFTLCD_DrawDot(u16 x, u16 y, u16 color);

void IERG3810_TFTLCD_FillRectangle(u16 color, u16 start_x, u16 length_x, u16 start_y, u16 length_y);

void IERG3810_TFTLCD_SevenSegment(u16 color, u16 start_x, u16 start_y, u8 digit);

void IERG3810_TFTLCD_ShowChar(u16 x, u16 y, u8 ascii, u16 color, u16 bcolor);

void IERG3810_TFTLCD_ShowChinChar(u16 x, u16 y, u8 chin_char_num, u16 color, u16 bcolor);

void IERG3810_TFTLCD_OverlayChar(u16 x, u16 y, u8 ascii, u16 color);

void IERG3810_TFTLCD_ShowStart(void);

void IERG3810_TFTLCD_ShowPieces(u16 x, u16 y, Pieces color);

void IERG3810_TFTLCD_ShowPieces_RC(u8 row, u8 col, Pieces color);

void IERG3810_TFTLCD_ShowLegal_RC(u16 row, u16 col, u16 color);

void IERG3810_TFTLCD_ClearLegal_RC(void);

void IERG3810_TFTLCD_UpdateScore(Pieces color);

void IERG3810_TFTLCD_ShowGame(void);

void IERG3810_TFTLCD_ShowGameover(void);

void IERG3810_TFTLCD_ShowTurn(Pieces turn);

void IERG3810_TFTLCD_ShowRules(u8 page);

void IERG3810_TFTLCD_ShowNoLegalTxt(Pieces color);

void IERG3810_TFTLCD_HighlightBorder(u8 row, u8 col, u16 color);

#endif
