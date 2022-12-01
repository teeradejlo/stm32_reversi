#include "stm32f10x.h"
#include "IERG3810_TFTLCD.h"
#include "IERG3810_Clock.h"
#include "IERG3810_USART.h"
#include "FONT.H"
#include "CFONT.H"
#include "REVERSIOBJ.H"
#include <string.h>
#include <stdlib.h>

typedef struct {
	u16 LCD_REG;
	u16 LCD_RAM;
}	LCD_TypeDef;

#define LCD_BASE			((u32) (0x6C000000 | 0x000007FE))
#define LCD					((LCD_TypeDef *) LCD_BASE)

//Write Command
void IERG3810_TFTLCD_WrReg(u16 regval) {
	LCD->LCD_REG = regval;
}

//Write Data
void IERG3810_TFTLCD_WrData(u16 data) {
	LCD->LCD_RAM = data;
}

//ILI9341 settings
void IERG3810_TFTLCD_SetParameter(void) {
	IERG3810_TFTLCD_WrReg(0x01);	//Software reset
	IERG3810_TFTLCD_WrReg(0x11);	//Exit_sleep_mode

	IERG3810_TFTLCD_WrReg(0x3A);	//Set_pixel_format
	IERG3810_TFTLCD_WrData(0x55);	//65536 colors

	IERG3810_TFTLCD_WrReg(0x29);	//Display ON

	IERG3810_TFTLCD_WrReg(0x36);	//Memory Access Control (sec. 8.2.29, pg127)
	IERG3810_TFTLCD_WrData(0xC8);	//Control display direction
}

//set FSMC
void IERG3810_TFTLCD_Init(void){
	RCC->AHBENR |= 1<<8;	//FSMC CLK ENABLE
	RCC->APB2ENR |= 1<<3;	//PORTB
	RCC->APB2ENR |= 1<<5; 	//PORTD
	RCC->APB2ENR |= 1<<6;	//PORTE
	RCC->APB2ENR |= 1<<8;	//PORTG

	//PB0
	GPIOB->CRL &= 0xFFFFFFF0;
	GPIOB->CRL |= 0x00000003; //gpio, output, push-pull, 50MHz

	//PORTD
	//alternate output, push-pull, 50MHz
	GPIOD->CRH &= 0x00FFF000;
	GPIOD->CRH |= 0xBB000BBB;
	GPIOD->CRL &= 0xFF00FF00;
	GPIOD->CRL |= 0x00BB00BB;

	//PORTE
	//alternate output, push-pull, 50MHz
	GPIOE->CRH &= 0x00000000;
	GPIOE->CRH |= 0xBBBBBBBB;
	GPIOE->CRL &= 0x0FFFFFFF;
	GPIOE->CRL |= 0xB0000000;

	//PORTG
	//alternate output, push-pull, 50MHz
	GPIOG->CRH &= 0xFFF0FFFF;
	GPIOG->CRH |= 0x000B0000;
	GPIOG->CRL &= 0xFFFFFFF0;	//PG0->RS
	GPIOG->CRL |= 0x0000000B;

	//LCD uses FSMC Bank 4 memory bank
	//Use Mode A
	FSMC_Bank1->BTCR[6]  = 0x00000000;	//FSMC_BCR4 (reset)
	FSMC_Bank1->BTCR[7]  = 0x00000000;	//FSMC_BTR4 (reset)
	FSMC_Bank1E->BWTR[6] = 0x00000000;	//FSMC_BWTR4 (reset)
	FSMC_Bank1->BTCR[6] |= 1<<12;		//FSMC_BCR4 -> WREN
	FSMC_Bank1->BTCR[6] |= 1<<14;		//FSMC_BCR4 -> EXTMOD
	FSMC_Bank1->BTCR[6] |= 1<<4;		//FSMC_BCR4 -> MWID
	FSMC_Bank1->BTCR[7] |= 0<<28;		//FSMC_BTR4 -> ACCMOD
	FSMC_Bank1->BTCR[7] |= 1<<0;		//FSMC_BTR4 -> ADDSET
	FSMC_Bank1->BTCR[7] |= 0xF<<8;		//FSMC_BTR4 -> DATAST
	FSMC_Bank1E->BWTR[6]|= 0<<28;		//FSMC_BWTR4 -> ACCMOD
	FSMC_Bank1E->BWTR[6]|= 0<<0;		//FSMC_BWTR4 -> ADDSET
	FSMC_Bank1E->BWTR[6]|= 3<<8;		//FSMC_BWTR4 -> DATAST
	FSMC_Bank1->BTCR[6] |= 1<<0;		//FSMC_BCR4 -> FACCEN
	IERG3810_TFTLCD_SetParameter();		//special settings for LCD module

	//LCD_LIGHT_ON
	GPIOB->BSRR = 1 << 0;
}

void IERG3810_TFTLCD_DrawDot(u16 x, u16 y, u16 color) {
	//set x pos.
	IERG3810_TFTLCD_WrReg(0x2A);
		IERG3810_TFTLCD_WrData(x >> 8);			//first 8-bit
		IERG3810_TFTLCD_WrData(x & 0xFF);		//last 8-bit
		IERG3810_TFTLCD_WrData(0x01);
		IERG3810_TFTLCD_WrData(0x3F);

	//set y pos.
	IERG3810_TFTLCD_WrReg(0x2B);
		IERG3810_TFTLCD_WrData(y >> 8);			//first 8-bit
		IERG3810_TFTLCD_WrData(y & 0xFF);		//last 8-bit
		IERG3810_TFTLCD_WrData(0x01);
		IERG3810_TFTLCD_WrData(0xDF);

	//set point with color
	IERG3810_TFTLCD_WrReg(0x2C);
		IERG3810_TFTLCD_WrData(color);
}

void IERG3810_TFTLCD_FillRectangle(u16 color, u16 start_x, u16 length_x, u16 start_y, u16 length_y) {
	u32 index = 0;
	//set x range
	IERG3810_TFTLCD_WrReg(0x2A);
		IERG3810_TFTLCD_WrData(start_x >> 8);
		IERG3810_TFTLCD_WrData(start_x & 0xFF);
		IERG3810_TFTLCD_WrData((start_x + length_x - 1) >> 8);
		IERG3810_TFTLCD_WrData((start_x + length_x - 1) & 0xFF);

	//set y range
	IERG3810_TFTLCD_WrReg(0x2B);
		IERG3810_TFTLCD_WrData(start_y >> 8);
		IERG3810_TFTLCD_WrData(start_y & 0xFF);
		IERG3810_TFTLCD_WrData((start_y + length_y - 1) >> 8);
		IERG3810_TFTLCD_WrData((start_y + length_y - 1) & 0xFF);

	//input colors
	IERG3810_TFTLCD_WrReg(0x2C);
	for (index = 0; index < length_x * length_y; index++) {
		IERG3810_TFTLCD_WrData(color);
	}
}

void IERG3810_TFTLCD_SevenSegment(u16 color, u16 start_x, u16 start_y, u8 digit) {
	/* Segment:
		a: IERG3810_TFTLCD_FillRectangle(color, start_x + vert_width, hor_width, start_y + total_height - hor_height, hor_height);
		b: IERG3810_TFTLCD_FillRectangle(color, start_x + hor_width + vert_width, vert_width, start_y + vert_height + 2 * hor_height, vert_height);
		c: IERG3810_TFTLCD_FillRectangle(color, start_x + hor_width + vert_width, vert_width, start_y + hor_height, vert_height);
		d: IERG3810_TFTLCD_FillRectangle(color, start_x + vert_width, hor_width, start_y, hor_height);
		e: IERG3810_TFTLCD_FillRectangle(color, start_x, vert_width, start_y + hor_height, vert_height);
		f: IERG3810_TFTLCD_FillRectangle(color, start_x, vert_width, start_y + vert_height + 2 * hor_height, vert_height);
		g: IERG3810_TFTLCD_FillRectangle(color, start_x + vert_width, hor_width, start_y + hor_height + vert_height, hor_height);
	*/
	u8 total_height = 160;
	u8 total_width = 80;

	u8 hor_height = 10;
	u8 hor_width = 60;
	u8 vert_height = (total_height - 3 * hor_height) / 2;
	u8 vert_width = (total_width - hor_width) / 2;

	//Write Digit
	switch(digit) {
		case 0:
			//a, b, c, d, e, f
			IERG3810_TFTLCD_FillRectangle(color, start_x + vert_width, hor_width, start_y + total_height - hor_height, hor_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + hor_width + vert_width, vert_width, start_y + vert_height + 2 * hor_height, vert_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + hor_width + vert_width, vert_width, start_y + hor_height, vert_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + vert_width, hor_width, start_y, hor_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x, vert_width, start_y + hor_height, vert_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x, vert_width, start_y + vert_height + 2 * hor_height, vert_height);
			break;
		case 1:
			//b, c
			IERG3810_TFTLCD_FillRectangle(color, start_x + hor_width + vert_width, vert_width, start_y + vert_height + 2 * hor_height, vert_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + hor_width + vert_width, vert_width, start_y + hor_height, vert_height);
			break;
		case 2:
			//a, b, d, e, g
			IERG3810_TFTLCD_FillRectangle(color, start_x + vert_width, hor_width, start_y + total_height - hor_height, hor_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + hor_width + vert_width, vert_width, start_y + vert_height + 2 * hor_height, vert_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + vert_width, hor_width, start_y, hor_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x, vert_width, start_y + hor_height, vert_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + vert_width, hor_width, start_y + hor_height + vert_height, hor_height);
			break;
		case 3:
			//a, b, c, d, g
			IERG3810_TFTLCD_FillRectangle(color, start_x + vert_width, hor_width, start_y + total_height - hor_height, hor_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + hor_width + vert_width, vert_width, start_y + vert_height + 2 * hor_height, vert_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + hor_width + vert_width, vert_width, start_y + hor_height, vert_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + vert_width, hor_width, start_y, hor_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + vert_width, hor_width, start_y + hor_height + vert_height, hor_height);
			break;
		case 4:
			//b, c, f, g
			IERG3810_TFTLCD_FillRectangle(color, start_x + hor_width + vert_width, vert_width, start_y + vert_height + 2 * hor_height, vert_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + hor_width + vert_width, vert_width, start_y + hor_height, vert_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x, vert_width, start_y + vert_height + 2 * hor_height, vert_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + vert_width, hor_width, start_y + hor_height + vert_height, hor_height);
			break;
		case 5:
			//a, c, d, f, g
			IERG3810_TFTLCD_FillRectangle(color, start_x + vert_width, hor_width, start_y + total_height - hor_height, hor_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + hor_width + vert_width, vert_width, start_y + hor_height, vert_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + vert_width, hor_width, start_y, hor_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x, vert_width, start_y + vert_height + 2 * hor_height, vert_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + vert_width, hor_width, start_y + hor_height + vert_height, hor_height);
			break;
		case 6:
			//a, c, d, e, f, g
			IERG3810_TFTLCD_FillRectangle(color, start_x + vert_width, hor_width, start_y + total_height - hor_height, hor_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + hor_width + vert_width, vert_width, start_y + hor_height, vert_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + vert_width, hor_width, start_y, hor_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x, vert_width, start_y + hor_height, vert_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x, vert_width, start_y + vert_height + 2 * hor_height, vert_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + vert_width, hor_width, start_y + hor_height + vert_height, hor_height);
			break;
		case 7:
			//a, b, c
			IERG3810_TFTLCD_FillRectangle(color, start_x + vert_width, hor_width, start_y + total_height - hor_height, hor_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + hor_width + vert_width, vert_width, start_y + vert_height + 2 * hor_height, vert_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + hor_width + vert_width, vert_width, start_y + hor_height, vert_height);
			break;
		case 8:
			//a, b, c, d, e, f, g
			IERG3810_TFTLCD_FillRectangle(color, start_x + vert_width, hor_width, start_y + total_height - hor_height, hor_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + hor_width + vert_width, vert_width, start_y + vert_height + 2 * hor_height, vert_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + hor_width + vert_width, vert_width, start_y + hor_height, vert_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + vert_width, hor_width, start_y, hor_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x, vert_width, start_y + hor_height, vert_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x, vert_width, start_y + vert_height + 2 * hor_height, vert_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + vert_width, hor_width, start_y + hor_height + vert_height, hor_height);
			break;
		case 9:
			//a, b, c, d, f, g
			IERG3810_TFTLCD_FillRectangle(color, start_x + vert_width, hor_width, start_y + total_height - hor_height, hor_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + hor_width + vert_width, vert_width, start_y + vert_height + 2 * hor_height, vert_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + hor_width + vert_width, vert_width, start_y + hor_height, vert_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + vert_width, hor_width, start_y, hor_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x, vert_width, start_y + vert_height + 2 * hor_height, vert_height);
			IERG3810_TFTLCD_FillRectangle(color, start_x + vert_width, hor_width, start_y + hor_height + vert_height, hor_height);
			break;
	}
}

void IERG3810_TFTLCD_ShowChar(u16 x, u16 y, u8 ascii, u16 color, u16 bcolor) {
	u8 i, j;
	u8 index;
	u8 height = 16, length = 8;
	if (ascii < 32 || ascii >= 127) return;

	ascii -= 32;
	//set x range
	IERG3810_TFTLCD_WrReg(0x2A);
		IERG3810_TFTLCD_WrData(x >> 8);
		IERG3810_TFTLCD_WrData(x & 0xFF);
		IERG3810_TFTLCD_WrData((x + length - 1) >> 8);
		IERG3810_TFTLCD_WrData((x + length - 1) & 0xFF);

	//set y range
	IERG3810_TFTLCD_WrReg(0x2B);
		IERG3810_TFTLCD_WrData(y >> 8);
		IERG3810_TFTLCD_WrData(y & 0xFF);
		IERG3810_TFTLCD_WrData((y + height - 1) >> 8);
		IERG3810_TFTLCD_WrData((y + height - 1) & 0xFF);

	IERG3810_TFTLCD_WrReg(0x2C);
	//j => lower half then upper half
	for(j = 0; j < height/8; j++) {
		//i => bit location
		for(i = 0; i < height/2; i++) {
			//index => column num.
			for(index = 0; index < length; index++) {
				if(asc2_1608[ascii][index * 2 + 1 - j] >> i & 0x01) {
					IERG3810_TFTLCD_WrData(color);
				} else {
					IERG3810_TFTLCD_WrData(bcolor);
				}
			}
		}
	}
}

void IERG3810_TFTLCD_ShowChinChar(u16 x, u16 y, u8 chin_char_num, u16 color, u16 bcolor) {
	u8 i, j;
	u8 index;
	u8 height = 16, length = 16;
	if (chin_char_num >= 6) return;

	//set x range
	IERG3810_TFTLCD_WrReg(0x2A);
		IERG3810_TFTLCD_WrData(x >> 8);
		IERG3810_TFTLCD_WrData(x & 0xFF);
		IERG3810_TFTLCD_WrData((x + length - 1) >> 8);
		IERG3810_TFTLCD_WrData((x + length - 1) & 0xFF);

	//set y range
	IERG3810_TFTLCD_WrReg(0x2B);
		IERG3810_TFTLCD_WrData(y >> 8);
		IERG3810_TFTLCD_WrData(y & 0xFF);
		IERG3810_TFTLCD_WrData((y + height - 1) >> 8);
		IERG3810_TFTLCD_WrData((y + height - 1) & 0xFF);

	IERG3810_TFTLCD_WrReg(0x2C);
	//j => lower half then upper half
	for(j = 0; j < height/8; j++) {
		//i => bit location
		for(i = 0; i < height/2; i++) {
			//index => column num.
			for(index = 0; index < length; index++) {
				if(chi_1616[chin_char_num][index * 2 + 1 - j] >> i & 0x01) {
					IERG3810_TFTLCD_WrData(color);
				} else {
					IERG3810_TFTLCD_WrData(bcolor);
				}
			}
		}
	}
}

void IERG3810_TFTLCD_OverlayChar(u16 x, u16 y, u8 ascii, u16 color) {
	u8 i, j;
	u8 index;
	u8 height = 16, length = 8;
	if (ascii < 32 || ascii >= 127) return;

	ascii -= 32;

	//j => lower half then upper half
	for(j = 0; j < height/8; j++) {
		//i => bit location
		for(i = 0; i < height/2; i++) {
			//index => column num.
			for(index = 0; index < length; index++) {
				if(asc2_1608[ascii][index * 2 + 1 - j] >> i & 0x01) {
					IERG3810_TFTLCD_DrawDot(x + index, y + 8 * j + i, color);
				}
			}
		}
	}
}

void IERG3810_TFTLCD_ShowStart(void) {
	u8 i, j, index;
	u8 height = 48, length = 116;
	u8 tot_row = height / 8;
	u16 x, y;
	
	char bas_sid[] = "1155147487";
	char andy_sid[] = "1155149524";
	char start_prompt[] = "START press 7";
	char rules_prompt[] = "RULES press 8";
	
	//GAME NAME
	x = 94;
	y = 200;
	//j => row num.
	for(j = 0; j < tot_row; j++) {
		//i => bit location
		for(i = 0; i < 8; i++) {
			//index => column num.
			for(index = 0; index < length; index++) {
				if(title[index * tot_row + (tot_row - 1) - j] >> i & 0x01) {
					IERG3810_TFTLCD_DrawDot(x + index, y + 8 * j + i, 0x0000);
				}
			}
		}
	}
	
	x = 64;
	y = 150;
	//BY:
	IERG3810_TFTLCD_ShowChar(x - 34, y, 'B', 0x0000, BG_COLOR);
	IERG3810_TFTLCD_ShowChar(x - 26, y, 'Y', 0x0000, BG_COLOR);
	IERG3810_TFTLCD_ShowChar(x - 18, y, ':', 0x0000, BG_COLOR);
	
	//Wong Ming Ho, Lo Si Ji
	for(i = 0; i < 3; i++) {
		IERG3810_TFTLCD_ShowChinChar(x + 16 * i, y, i, 0x0000, BG_COLOR);
		IERG3810_TFTLCD_ShowChinChar(x + 16 * i, y - 24, i + 3, 0x0000, BG_COLOR);
	}
	
	//1155149524, 1155147487
	x += 64;
	for(i = 0; i < 10; i++) {
		IERG3810_TFTLCD_ShowChar(x + 8 * i, y, andy_sid[i], 0x0000, BG_COLOR);
		IERG3810_TFTLCD_ShowChar(x + 8 * i, y - 24, bas_sid[i], 0x0000, BG_COLOR);
	}
	
	x = 68;
	y = 90;
	//START and RULES
	for(i = 0; i < 12; i++) {
		IERG3810_TFTLCD_ShowChar(x + 8 * i, y, start_prompt[i], 0x0000, BG_COLOR);
		IERG3810_TFTLCD_ShowChar(x + 8 * i, y - 32, rules_prompt[i], 0x0000, BG_COLOR);
	}
	IERG3810_TFTLCD_ShowChar(x + 8 * 12, y, start_prompt[12], 0xF800, BG_COLOR);
	IERG3810_TFTLCD_ShowChar(x + 8 * 12, y - 32, rules_prompt[12], 0x001F, BG_COLOR);
	
	//ICON
	x = 30;
	y = 200;
	//0000 0111 1110 1111
	IERG3810_TFTLCD_FillRectangle(0x0000, x, 48, y, 48);
	IERG3810_TFTLCD_FillRectangle(BOARD_COLOR, x+4, 40, y+4, 40);
	IERG3810_TFTLCD_FillRectangle(BOARD_COLOR, x+4, 40, y+4, 40);
	IERG3810_TFTLCD_FillRectangle(0x0000, x+22, 4, y+4, 40);
	IERG3810_TFTLCD_FillRectangle(0x0000, x+4, 40, y+22, 4);
	IERG3810_TFTLCD_ShowPieces(x+5, y+5, BLACK);
	IERG3810_TFTLCD_ShowPieces(x+27, y+27, BLACK);
	IERG3810_TFTLCD_ShowPieces(x+5, y+27, WHITE);
	IERG3810_TFTLCD_ShowPieces(x+27, y+5, WHITE);
}

void IERG3810_TFTLCD_UpdateScore(Pieces color) {
	u16 x = 144, y;
	
	if (color == BLACK) {
		//BLACK
		y = 284;
		IERG3810_TFTLCD_ShowChar(x, y, (u8)(black_score / 10) + 48, 0x0000, BG_COLOR);
		IERG3810_TFTLCD_ShowChar(x+8, y, (u8)(black_score % 10) + 48, 0x0000, BG_COLOR);
	} else if (color == WHITE) {
		//WHITE
		y = 20;
		IERG3810_TFTLCD_ShowChar(x, y, (u8)(white_score / 10) + 48, 0x0000, BG_COLOR);
		IERG3810_TFTLCD_ShowChar(x+8, y, (u8)(white_score % 10) + 48, 0x0000, BG_COLOR);
	}
}

void IERG3810_TFTLCD_ShowPieces(u16 x, u16 y, Pieces color) {
	u8 i, j, index;
	u8 height = 16, length = 16;
	u8 tot_row = height / 8;
	
	//j => row num.
	for(j = 0; j < tot_row; j++) {
		//i => bit location
		for(i = 0; i < 8; i++) {
			//index => column num.
			for(index = 0; index < length; index++) {
				if(black_piece[index * tot_row + (tot_row - 1) - j] >> i & 0x01) {
						IERG3810_TFTLCD_DrawDot(x + index, y + 8 * j + i, 0x0000);
				}
				if (color == WHITE) {
					if(white_piece[index * tot_row + (tot_row - 1) - j] >> i & 0x01) {
						IERG3810_TFTLCD_DrawDot(x + index, y + 8 * j + i, 0xFFFF);
					}
				}
			}
		}
	}
}

void IERG3810_TFTLCD_ShowPieces_RC(u8 row, u8 col, Pieces color) {
	u16 x = 42 + (col * 16) + (col * 4);
	u16 y = 82 + (row * 16) + (row * 4);
	if (color == BLACK) {
		if (gboard[row][col] == WHITE) white_score--;
		black_score++;
	} else if (color == WHITE) {
		if (gboard[row][col] == BLACK) black_score--;
		white_score++;
	}
	
	IERG3810_TFTLCD_ShowPieces(x, y, color);
	IERG3810_TFTLCD_UpdateScore(color);
}

void IERG3810_TFTLCD_ShowLegal_RC(u16 row, u16 col, u16 color) {
	u16 x = 42 + (col * 16) + (col * 4);
	u16 y = 82 + (row * 16) + (row * 4);
	
	IERG3810_TFTLCD_FillRectangle(color, x+6, 4, y+6, 4);
}

void IERG3810_TFTLCD_ClearLegal_RC(void) {
	u8 row, col;
	
	while(!queue_isEmpty(legalQrow)) {
		row = dequeue(legalQrow);
		col = dequeue(legalQcol);
		
		IERG3810_TFTLCD_ShowLegal_RC(row, col, BOARD_COLOR);
	}
}

void IERG3810_TFTLCD_ShowGame(void) {
	u8 i;
	u16 x = 39;
	u16 y = 79;
	char black_score_prompt[] = "BLACK :";
	char white_score_prompt[] = "WHITE :";
	
	IERG3810_TFTLCD_FillRectangle(0x0000, x, 162, y, 162);
	IERG3810_TFTLCD_FillRectangle(BOARD_COLOR, x+2, 158, y+2, 158);
	
	for (i = 1; i < 8; i++) {
		IERG3810_TFTLCD_FillRectangle(0x0000, x+20*i, 2, y+2, 158);
		IERG3810_TFTLCD_FillRectangle(0x0000, x+2, 158, y+20*i, 2);
	}
	
	for (i = 0; i < 8; i++) {
		IERG3810_TFTLCD_ShowChar(x + 7 + 8 * (i) + 12 * (i), y-20, 49+i, 0x0000, BG_COLOR);
		IERG3810_TFTLCD_ShowChar(x - 16, y + 3 + 16 * (i) + 4 * (i), 65+i, 0x0000, BG_COLOR);
	}
	
	x = 80;
	y = 284;
	for(i = 0; i < 7; i++) {
		IERG3810_TFTLCD_OverlayChar(x + 8 * i, y, black_score_prompt[i], 0x0000);
		IERG3810_TFTLCD_OverlayChar(x + 8 * i, y - 264, white_score_prompt[i], 0x0000);
	}
	
	gboard[3][3] = BLACK;
	gboard[4][4] = BLACK;
	gboard[3][4] = WHITE;
	gboard[4][3] = WHITE;
	IERG3810_TFTLCD_ShowPieces_RC(3, 3, BLACK);
	IERG3810_TFTLCD_ShowPieces_RC(4, 4, BLACK);
	IERG3810_TFTLCD_ShowPieces_RC(3, 4, WHITE);
	IERG3810_TFTLCD_ShowPieces_RC(4, 3, WHITE);
}

void IERG3810_TFTLCD_ShowRules(u8 page) {
	u8 i;
	u16 x, y;
	char start_prompt[] = "BACK press 7";
	char rules_prompt[] = "NEXT press 8";
	char *text = (char*)malloc(35);
	
	x = 100;
	y = 288;
	strcpy(text, "RULES");
	for(i = 0; i < 5; i++) {
		IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
	}
	
	if (page == 1) {
		x = 8;
		y -= 32;
		strcpy(text, "START: Black disc plays");
		for(i = 0; i < 23; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		y -= 24;
		strcpy(text, "first. Four discs placed");
		for(i = 0; i < 24; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		y -= 24;
		strcpy(text, "on the board initially.");
		for(i = 0; i < 23; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		y -= 32;
		strcpy(text, "ACTION: Place your disc to");
		for(i = 0; i < 26; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		y -= 24;
		strcpy(text, "surround opponent's disc.");
		for(i = 0; i < 25; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		y -= 24;
		strcpy(text, "Surrounded disc is converted");
		for(i = 0; i < 28; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		y -= 24;
		strcpy(text, "to surrounding disc's color.");
		for(i = 0; i < 27; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		
		x = 68;
		y = 56;
		for(i = 0; i < 11; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8 * i, y, start_prompt[i], 0x0000, BG_COLOR);
			IERG3810_TFTLCD_ShowChar(x + 8 * i, y - 32, rules_prompt[i], 0x0000, BG_COLOR);
		}
		IERG3810_TFTLCD_ShowChar(x + 8 * 11, y, start_prompt[11], 0xF800, BG_COLOR);
		IERG3810_TFTLCD_ShowChar(x + 8 * 11, y - 32, rules_prompt[11], 0x001F, BG_COLOR);
	}
	if (page == 2) {
		IERG3810_TFTLCD_FillRectangle(BG_COLOR, 0, 240, 75, 200);
		
		x = 8;
		y -= 32;
		strcpy(text, "MOVE: Placing a disc must");
		for(i = 0; i < 25; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		y-=24;
		strcpy(text, "surround at least 1");
		for(i = 0; i < 19; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		y-=24;
		strcpy(text, "opponent's disc (a legal");
		for(i = 0; i < 24; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		y-=24;
		strcpy(text, "move). If a player doesn't");
		for(i = 0; i < 26; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		y-=24;
		strcpy(text, "have it, skip their turn.");
		for(i = 0; i < 25; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		y -= 32;
		strcpy(text, "END GAME: if");
		for(i = 0; i < 12; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		y-=24;
		strcpy(text, "(1) Board is full.");
		for(i = 0; i < 18; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
	}
	if (page == 3) {
		IERG3810_TFTLCD_FillRectangle(BG_COLOR, 0, 240, 75, 200);
		
		x = 8;
		y-=32;
		strcpy(text, "(2) One player has no more");
		for(i = 0; i < 26; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		y-=24;
		strcpy(text, "disc on board.");
		for(i = 0; i < 14; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		y-=24;
		strcpy(text, "(3) Both player don't have");
		for(i = 0; i < 26; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		y-=24;
		strcpy(text, "legal move.");
		for(i = 0; i < 11; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		y-=32;
		strcpy(text, "WINNER: player with more");
		for(i = 0; i < 24; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		y-=24;
		strcpy(text, "discs on board wins.");
		for(i = 0; i < 20; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		y-=24;
		strcpy(text, "If equals, then draw.");
		for(i = 0; i < 21; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
	}
	if (page == 4) {
		IERG3810_TFTLCD_FillRectangle(BG_COLOR, 0, 240, 75, 200);
		
		x = 8;
		y-=32;
		strcpy(text, "CONTROLS:");
		for(i = 0; i < 9; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		y-=24;
		strcpy(text, "(1) KP1 = MOVE LEFT");
		for(i = 0; i < 19; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		y-=24;
		strcpy(text, "(2) KP2 = MOVE DOWN");
		for(i = 0; i < 19; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		y-=24;
		strcpy(text, "(3) KP3 = MOVE RIGHT");
		for(i = 0; i < 20; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		y-=24;
		strcpy(text, "(4) KP5 = MOVE UP");
		for(i = 0; i < 17; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		y-=24;
		strcpy(text, "(5) KP4 = PLACE DISC");
		for(i = 0; i < 20; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		y-=24;
		strcpy(text, "(6) KP7 = RESTART");
		for(i = 0; i < 17; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
		y-=24;
		strcpy(text, "(7) KP8 = TO HOME");
		for(i = 0; i < 17; i++) {
			IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
		}
	}
	
	free(text);
}

void IERG3810_TFTLCD_HighlightBorder(u8 row, u8 col, u16 color) {
	u16 x=39+20*col, y=79+20*row;
	
	if(row < GBOARD_SIZE && col < GBOARD_SIZE) {
		IERG3810_TFTLCD_FillRectangle(color, x, 2, y, 22);
		IERG3810_TFTLCD_FillRectangle(color, x+20, 2, y, 22);
		IERG3810_TFTLCD_FillRectangle(color, x, 22, y, 2);
		IERG3810_TFTLCD_FillRectangle(color, x, 22, y+20, 2);
	}
}

void IERG3810_TFTLCD_ShowTurn(Pieces turn) {
	u16 x, y;
	
	if (turn == BLACK) {
		x = 64;
		y = 284;
		//erase dot on white side
		IERG3810_TFTLCD_FillRectangle(BG_COLOR, x, 8, y-264, 16);
		//erase pos. text on black side
		IERG3810_TFTLCD_FillRectangle(BG_COLOR, x-28, 168, y-24, 16);
		//add dot on black side
		IERG3810_TFTLCD_FillRectangle(CURSOR_COLOR, x, 8, y+4, 8);
	} else if (turn == WHITE) {
		x = 64;
		y = 20;
		//erase dot on black side
		IERG3810_TFTLCD_FillRectangle(BG_COLOR, x, 8, y+264, 16);
		//erase pos. text on white side
		IERG3810_TFTLCD_FillRectangle(BG_COLOR, x-28, 168, y+24, 16);
		//add dot on white side
		IERG3810_TFTLCD_FillRectangle(0xF100, x, 8, y+4, 8);
	}
}

void IERG3810_TFTLCD_ShowNoLegalTxt(Pieces color) {
	u8 i;
	u16 x = 36, y;
	char *text = (char*)malloc(35);
	strcpy(text, "NO Legal Move... SKIP");
	if (color == BLACK) {
		y = 260;
	} else if (color == WHITE) {
		y = 44;
	}
	
	for(i = 0; i < 21; i++) {
		IERG3810_TFTLCD_ShowChar(x + 8*i, y, text[i], 0x0000, BG_COLOR);
	}
	free(text);
}

void IERG3810_TFTLCD_ShowGameover(void) {
	u16 x = 0, y = 94;
	u8 i, length = 35;
	char *text = (char*)malloc(35);
	char restart_prompt[] = "RESTART press 7";
	char tohome_prompt[] =  "TO HOME press 8";
	
	if (black_score > white_score) {
		length = 14;
		strcpy(text, "WINNER : BLACK");
		USART_print(2, "WINNER : BLACK");
	} else if (black_score < white_score) {
		length = 14;
		strcpy(text, "WINNER : WHITE");
		USART_print(2, "WINNER : WHITE");
	} else {
		length = 4;
		strcpy(text, "DRAW");
		USART_print(2, "DRAW");
	}
	USART_print(2, "\n");

	IERG3810_TFTLCD_FillRectangle(0x0000, x, 240, y, 132);	
	x = 60;
	y += 30;
	for(i = 0; i < 14; i++) {
		IERG3810_TFTLCD_ShowChar(x + 8 * i, y, tohome_prompt[i], 0xFFFF, 0x0000);
		IERG3810_TFTLCD_ShowChar(x + 8 * i, y + 24, restart_prompt[i], 0xFFFF, 0x0000);
	}
	IERG3810_TFTLCD_ShowChar(x + 8 * 14, y, tohome_prompt[14], 0x001F, 0x0000);
	IERG3810_TFTLCD_ShowChar(x + 8 * 14, y + 24, restart_prompt[14], 0xF800, 0x0000);
	
	x = (240 - 8 * length) / 2;
	y += 56;
	for(i = 0; i < length; i++) {
		IERG3810_TFTLCD_ShowChar(x + 8 * i, y, text[i], 0xFFE0, 0x0000);
	}
	free(text);
}
