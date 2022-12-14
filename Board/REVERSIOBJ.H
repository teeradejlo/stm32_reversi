#ifndef __REVERSIOBJ_H
#define __REVERSIOBJ_H

const unsigned char title[696] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF,
0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xFE,
0x1F, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x3F, 0xFF, 0xFF, 0xFF, 0xFE, 0x00,
0x3F, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x07, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
0x07, 0xC0, 0x01, 0xFF, 0xC0, 0x00, 0x0F, 0x80, 0x01, 0xFF, 0xF8, 0x00, 0x1F, 0x00, 0x03, 0xFF, 0xFE, 0x00,
0x3F, 0x00, 0x0F, 0xFF, 0xFF, 0xC0, 0x7E, 0x00, 0x1F, 0xF7, 0xFF, 0xE0, 0x7C, 0x00, 0x3F, 0xE1, 0xFF, 0xF8,
0x7C, 0x00, 0xFF, 0xC0, 0x3F, 0xFC, 0x7E, 0x03, 0xFF, 0x00, 0x07, 0xFC, 0x7F, 0x8F, 0xFE, 0x00, 0x01, 0xFC,
0x7F, 0xFF, 0xFC, 0x00, 0x00, 0x7C, 0x3F, 0xFF, 0xF8, 0x00, 0x00, 0x38, 0x1F, 0xFF, 0xE0, 0x00, 0x7F, 0x00,
0x0F, 0xFF, 0x80, 0x01, 0xFF, 0xC0, 0x01, 0xFC, 0x00, 0x07, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x1F, 0xFF, 0xF0,
0x00, 0x00, 0x00, 0x3F, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x7F, 0x9D, 0xFC,
0x00, 0x00, 0x00, 0x7F, 0x3C, 0xFC, 0x00, 0x00, 0x00, 0x7C, 0xFC, 0x7C, 0x00, 0x00, 0x00, 0x7F, 0xFC, 0x7C,
0x00, 0x00, 0x00, 0x7F, 0xF8, 0xFC, 0x00, 0x00, 0x00, 0x7F, 0xF0, 0xFC, 0x00, 0x00, 0x00, 0x7F, 0xE1, 0xFC,
0x00, 0x00, 0x00, 0x7F, 0xC1, 0xF8, 0x00, 0x00, 0x00, 0x3F, 0x83, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x01, 0xF0,
0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00,
0x00, 0x00, 0x00, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x80,
0x00, 0x00, 0x00, 0x1F, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x07, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFE,
0x00, 0x00, 0x00, 0x00, 0x3F, 0xFE, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFC,
0x00, 0x00, 0x00, 0x3F, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xF0, 0x00,
0x00, 0x00, 0x00, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x07, 0xFF, 0xE0,
0x00, 0x00, 0x00, 0x1F, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xFC,
0x00, 0x00, 0x00, 0x7F, 0x9D, 0xFC, 0x00, 0x00, 0x00, 0x7F, 0x3C, 0xFC, 0x00, 0x00, 0x00, 0x7C, 0xFC, 0x7C,
0x00, 0x00, 0x00, 0x7F, 0xFC, 0x7C, 0x00, 0x00, 0x00, 0x7F, 0xF8, 0xFC, 0x00, 0x00, 0x00, 0x7F, 0xF0, 0xFC,
0x00, 0x00, 0x00, 0x7F, 0xE1, 0xFC, 0x00, 0x00, 0x00, 0x7F, 0xC1, 0xF8, 0x00, 0x00, 0x00, 0x3F, 0x83, 0xF8,
0x00, 0x00, 0x00, 0x00, 0x01, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x78, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x7F, 0xE3, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFC,
0x00, 0x00, 0x00, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x01, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFC, 0x00, 0x00,
0x00, 0x00, 0x03, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xF0, 0x00, 0x00,
0x00, 0x00, 0x03, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x01, 0xFF, 0x80, 0x00,
0x00, 0x00, 0x00, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x1F, 0x00, 0x10, 0x00, 0x00, 0x00, 0x3F, 0x80, 0x3C, 0x00, 0x00, 0x00, 0x7F, 0xC0, 0x7E,
0x00, 0x00, 0x00, 0xFF, 0xE0, 0x7F, 0x00, 0x00, 0x01, 0xFF, 0xE0, 0x7F, 0x00, 0x00, 0x03, 0xFF, 0xF0, 0x3F,
0x00, 0x00, 0x03, 0xFB, 0xF8, 0x7F, 0x00, 0x00, 0x03, 0xF3, 0xFF, 0xFE, 0x00, 0x00, 0x03, 0xE1, 0xFF, 0xFE,
0x00, 0x00, 0x03, 0xE0, 0xFF, 0xFC, 0x00, 0x00, 0x01, 0xC0, 0x7F, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xF0,
0x00, 0x00, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x80, 0x00, 0x00, 0x00,
0x00, 0x1F, 0xC0, 0xFF, 0xFF, 0xC0, 0x00, 0x1F, 0xC3, 0xFF, 0xFF, 0xF8, 0x00, 0x1F, 0xC3, 0xFF, 0xFF, 0xFC,
0x00, 0x1F, 0xC3, 0xFF, 0xFF, 0xFC, 0x00, 0x0F, 0x80, 0xFF, 0xFF, 0xFE, 0x00, 0x07, 0x00, 0x00, 0x00, 0xFE,
0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C };

const unsigned char black_piece[32] = {0x03, 0xC0, 0x0F, 0xF0,
0x3F, 0xFC, 0x3F, 0xFC, 0x7F, 0xFE, 0x7F, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFE, 0x7F, 0xFE, 0x3F, 0xFC, 0x3F, 0xFC,
0x0F, 0xF0, 0x03, 0xC0, };

const unsigned char white_piece[32] = {0x00, 0x00, 0x01, 0x80,
0x07, 0xE0, 0x1F, 0xF8, 0x1F, 0xF8, 0x3F, 0xFC, 0x3F, 0xFC, 0x7F, 0xFE,
0x7F, 0xFE, 0x3F, 0xFC, 0x3F, 0xFC, 0x1F, 0xF8, 0x1F, 0xF8, 0x07, 0xE0,
0x01, 0x80, 0x00, 0x00, };

#endif
