#include "stm32f10x.h"
#include "IERG3810_USART.h"
#include "IERG3810_GameLogic.h"
#include <string.h>

void IEGR3810_GameLogic_ClearGBoard(void) {
	u8 i, j;
	
	for (i = 0; i < GBOARD_SIZE; i++) {
		for (j = 0; j < GBOARD_SIZE; j++) {
			gboard[i][j] = NONE;
		}
	}
}

void IEGR3810_GameLogic_ClearLegal(void) {
	memset(&gboard_legal, 0, GBOARD_SIZE * GBOARD_SIZE * sizeof(u8));
}

void IEGR3810_GameLogic_ClearFlipping(void) {
	memset(&gboard_flipping, 0, GBOARD_SIZE * GBOARD_SIZE * sizeof(u8));
}

u8 IEGR3810_GameLogic_UpdateLegal(Pieces turn) {
	u8 legal_count = 0;
	u8 i, j, k;
	//[0] = hor, [1] = vert
	signed char dir_list[8][2] = {
		{0, 1}, {0, -1},
		{1, 0}, {1, 1}, {1, -1},
		{-1, 0}, {-1, 1}, {-1, -1},
	};
	signed char dir[2];
	Pieces opponent = next_turn;
	
	//for each piece on board
	for (i = 0; i < GBOARD_SIZE; i++) {
		for (j = 0; j < GBOARD_SIZE; j++) {
			if (gboard[i][j] == turn) {
				//loop through each direction
				for (k = 0; k < 8; k++) {
					u8 at_least_one = 0;
					signed char row = i, col = j;
					dir[0]=dir_list[k][0];
					dir[1]=dir_list[k][1];
					row += dir[1];
					col += dir[0];
					//if row/col not out of board...
					while (row >= 0 && row < GBOARD_SIZE && col >= 0 && col < GBOARD_SIZE) {
						//if found your own color, exit immediately
						if (gboard[row][col] == turn) {
							break;
						}
						//if haven't found any opponent but found empty spot, exit
						if (!at_least_one && gboard[row][col] == NONE) {
							break;
						}
						//if found opponent, then set at least one to true
						if (gboard[row][col] == opponent) {
							at_least_one = 1;
						}
						//if found at least one opponent and current spot is empty, this is legal spot
						if (at_least_one && gboard[row][col] == NONE) {
							legal_count++;
							gboard_legal[row][col] = 1;
							enqueue(legalQrow, row);
							enqueue(legalQcol, col);
							break;
						} 
						row += dir[1];
						col += dir[0];
					}
				}
			}
		}
	}
	return legal_count;
}

void IEGR3810_GameLogic_UpdateCursorPos(signed char dirX, signed char dirY) {
	signed char row = cursor_pos_rc[0], col = cursor_pos_rc[1];
	row += dirY;
	col += dirX;
	
	if (row >= 0 && row < GBOARD_SIZE) cursor_pos_rc[0] = row;
	if (col >= 0 && col < GBOARD_SIZE) cursor_pos_rc[1] = col;
}

void IEGR3810_GameLogic_EnqueueFlippingNeeded(u8 src_row, u8 src_col, Pieces turn) {
	u8 k;
	//[0] = hor, [1] = vert
	signed char dir_list[8][2] = {
		{0, 1}, {0, -1},
		{1, 0}, {1, 1}, {1, -1},
		{-1, 0}, {-1, 1}, {-1, -1},
	};
	signed char dir[2];
	Pieces opponent = next_turn;
	
	//for each dir from curr spot
	for (k = 0; k < 8; k++) {
		u8 at_least_one = 0;
		signed char row = src_row, col = src_col;
		dir[0]=dir_list[k][0];
		dir[1]=dir_list[k][1];
		row += dir[1];
		col += dir[0];
		while (row >= 0 && row < GBOARD_SIZE && col >= 0 && col < GBOARD_SIZE) {
			//no opponent and found your own color, exit
			if (!at_least_one && gboard[row][col] == turn) {
				break;
			}
			//if found empty spot, exit immediately
			if (gboard[row][col] == NONE) {
				break;
			}
			//if found opponent, then set at least one to true
			if (gboard[row][col] == opponent) {
				at_least_one = 1;
			}
			//if can surround
			if (at_least_one && gboard[row][col] == turn) {
				//reverse dir
				dir[0] *= -1;
				dir[1] *= -1;
				row += dir[1];
				col += dir[0];
				//go back until original pos
				while(row != src_row || col != src_col) {
					//if havent added to queue yet, then add
					if (!gboard_flipping[row][col]) {
						enqueue(flippingQrow, row);
						enqueue(flippingQcol, col);
						gboard_flipping[row][col] = 1;
					}
					row += dir[1];
					col += dir[0];
				}
				break;
			} 
			row += dir[1];
			col += dir[0];
		}
	}
}
