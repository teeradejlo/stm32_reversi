#ifndef __GLOBAL_H
#define __GLOBAL_H
#include "stm32f10x.h"
#include <stdlib.h>

#define GBOARD_SIZE 8

#define BG_COLOR 0xFFFF
#define BOARD_COLOR 0x07EF
#define CURSOR_COLOR 0x001F

typedef enum {
	UPDATEDISPLAY = 0,
	READINPUT,
	GAMELOGIC,
} Task;

typedef enum {
	START = 0,
	INSTRUCTION1,
	INSTRUCTION2,
	INSTRUCTION3,
	INSTRUCTION4,
	GAMEPLAY,
	RESTART,
	GAMEOVER,
} Scene;

typedef enum {
	NONE = 0,
	BLACK,
	WHITE,
} Pieces;

typedef struct {
	u8 front, rear, size;
	u8 capacity;
	u8* array;
} Queue;

Queue* createQueue(u8 cap);

u8 queue_isFull (Queue* q);

u8 queue_isEmpty (Queue* q);

void enqueue(Queue* q, u8 item);

u8 dequeue(Queue* q);

u8 queue_front(Queue* q);

u8 queue_rear(Queue* q);

u8 queue_getItem(Queue* q, u8 item_no);

void queue_free(Queue** q);

extern u8 black_score;
extern u8 white_score;

extern Scene prev_scene;
extern Scene curr_scene;

extern Pieces gboard[GBOARD_SIZE][GBOARD_SIZE];
extern u8 gboard_legal[GBOARD_SIZE][GBOARD_SIZE];
extern u8 gboard_flipping[GBOARD_SIZE][GBOARD_SIZE];
//[0] = row, [1] = col
extern u8 cursor_pos_rc[2];
extern u8 prev_cursor_pos_rc[2];

extern u8 keypad_pressed[10];

extern u8 is_playing;
extern u8 is_animating;
extern u8 flip_delay;
extern u8 gameplay_display_uptodate;
extern u8 turn_change_req;
extern u8 cursor_move_req;
extern u8 disc_place_req;
extern u8 disc_placedAtRow;
extern u8 disc_placedAtCol;
extern u8 show_legal_req;
extern Pieces curr_turn;
extern Pieces next_turn;

extern u8 ps2DelayTimeout;
extern u8 keypad4FirstPress;
extern u8 keypad1DelayCount;
extern u8 keypad2DelayCount;
extern u8 keypad3DelayCount;
extern u8 keypad5DelayCount;

extern u8 prev_legal_cnt;
extern u8 curr_legal_cnt;

extern Queue* legalQrow;
extern Queue* legalQcol;
extern Queue* flippingQrow;
extern Queue* flippingQcol;

#endif
