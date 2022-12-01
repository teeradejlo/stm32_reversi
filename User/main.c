#include "stm32f10x.h"
#include "IERG3810_LED.h"
#include "IERG3810_Buzzer.h"
#include "IERG3810_Clock.h"
#include "IERG3810_Exti.h"
#include "IERG3810_KEY.h"
#include "IERG3810_Timer.h"
#include "IERG3810_TFTLCD.h"
#include "IERG3810_USART.h"
#include "IERG3810_GameLogic.h"
#include "global.H"
#include <string.h>

//global variables initialization
Pieces gboard[GBOARD_SIZE][GBOARD_SIZE] = {
	{NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,},
	{NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,},
	{NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,},
	{NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,},
	{NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,},
	{NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,},
	{NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,},
	{NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,},
};

u8 gboard_legal[GBOARD_SIZE][GBOARD_SIZE] = {
	{0,0,0,0,0,0,0,0,},
	{0,0,0,0,0,0,0,0,},
	{0,0,0,0,0,0,0,0,},
	{0,0,0,0,0,0,0,0,},
	{0,0,0,0,0,0,0,0,},
	{0,0,0,0,0,0,0,0,},
	{0,0,0,0,0,0,0,0,},
	{0,0,0,0,0,0,0,0,},
};

u8 gboard_flipping[GBOARD_SIZE][GBOARD_SIZE] = {
	{0,0,0,0,0,0,0,0,},
	{0,0,0,0,0,0,0,0,},
	{0,0,0,0,0,0,0,0,},
	{0,0,0,0,0,0,0,0,},
	{0,0,0,0,0,0,0,0,},
	{0,0,0,0,0,0,0,0,},
	{0,0,0,0,0,0,0,0,},
	{0,0,0,0,0,0,0,0,},
};

u8 black_score = 0;
u8 white_score = 0;

u8 cursor_pos_rc[2] = {3, 3};
u8 prev_cursor_pos_rc[2] = {8, 8};

Scene prev_scene = GAMEOVER;
Scene curr_scene = START;

u8 keypad_pressed[10] = {0,0,0,0,0,0,0,0,0,0};

u8 is_playing = 0;
u8 is_animating = 0;
u8 flip_delay = 0;
u8 gameplay_display_uptodate = 0;
u8 turn_change_req = 0;
u8 cursor_move_req = 0;
u8 disc_place_req = 0;
u8 disc_placedAtRow = 0;
u8 disc_placedAtCol = 0;
u8 show_legal_req = 0;
Pieces curr_turn = NONE;
Pieces next_turn = NONE;
u8 prev_legal_cnt = 0;
u8 curr_legal_cnt = 0;

u8 ps2DelayTimeout = 0;
u8 keypad1DelayCount = 0;
u8 keypad2DelayCount = 0;
u8 keypad3DelayCount = 0;
u8 keypad5DelayCount = 0;
u8 keypad4FirstPress = 0;

Queue* legalQrow = NULL;
Queue* legalQcol = NULL;
Queue* flippingQrow = NULL;
Queue* flippingQcol = NULL;

Task curr_task = UPDATEDISPLAY;

int main(void){
	IERG3810_clock_tree_init();
	IERG3810_USART2_init(36, 9600);
	IERG3810_NVIC_SetPriorityGroup(5);
	IERG3810_PS2key_ExtiInit();

	IERG3810_Buzzer_Init();
	IERG3810_LED_Init();

	IERG3810_SYSTICK_Init10ms();

	prev_scene = GAMEOVER;
	curr_scene = START;
	curr_task = UPDATEDISPLAY;

	//Init TFTLCD and wait for screen to be ready
	IERG3810_TFTLCD_Init();
	while(flip_delay <= 10) {
		//DELAY for screen
		//Use RANDOM dummy counter
	}
	flip_delay = 0;

	while(1) {
		if (curr_task == UPDATEDISPLAY) {
			if (curr_scene != prev_scene) {
				//need to change scene, each scene has different initialization
				switch (curr_scene) {
					case START:
						IERG3810_TFTLCD_FillRectangle(BG_COLOR, 0, 240, 0, 320);
						IERG3810_TFTLCD_ShowStart();
						is_playing = 0;
						break;
					case INSTRUCTION1:
						//first page of rules need to erase whole screen
						IERG3810_TFTLCD_FillRectangle(BG_COLOR, 0, 240, 0, 320);
						IERG3810_TFTLCD_ShowRules(1);
						is_playing = 0;
						break;
					case INSTRUCTION2:
						//when change rules page, erase only the content part
						//done in the function
						IERG3810_TFTLCD_ShowRules(2);
						is_playing = 0;
						break;
					case INSTRUCTION3:
						IERG3810_TFTLCD_ShowRules(3);
						is_playing = 0;
						break;
					case INSTRUCTION4:
						IERG3810_TFTLCD_ShowRules(4);
						is_playing = 0;
						break;
					case GAMEPLAY:
						//Init game related variables
						IERG3810_TFTLCD_FillRectangle(BG_COLOR, 0, 240, 0, 320);
						USART_print(2, "START GAME!!!");
						USART_print(2, "\n");
						black_score = 0;
						white_score = 0;
						is_playing = 1;
						is_animating = 0;
						show_legal_req = 1;
						turn_change_req = 0;
						cursor_move_req = 0;
						disc_place_req = 0;
						curr_turn = BLACK;
						next_turn = WHITE;
						prev_cursor_pos_rc[0] = 8;
						prev_cursor_pos_rc[1] = 8;
						cursor_pos_rc[0] = 3;
						cursor_pos_rc[1] = 3;
						queue_free(&legalQrow);
						queue_free(&legalQcol);
						queue_free(&flippingQrow);
						queue_free(&flippingQcol);
						legalQrow = createQueue(64);
						legalQcol = createQueue(64);
						flippingQrow = createQueue(64);
						flippingQcol = createQueue(64);
						IEGR3810_GameLogic_ClearLegal();
						IEGR3810_GameLogic_ClearFlipping();
						IEGR3810_GameLogic_ClearGBoard();
						IERG3810_TFTLCD_ShowGame();
						IERG3810_TFTLCD_ShowTurn(curr_turn);
						gameplay_display_uptodate = 1;
						break;
					case RESTART:
						//DUMMY scene for restarting game
						is_playing = 0;
						break;
					case GAMEOVER:
						IERG3810_TFTLCD_ShowGameover();
						is_playing = 0;
						break;
					default:
						break;
				}

				prev_scene = curr_scene;
				if (curr_scene == RESTART) {
					curr_scene = GAMEPLAY;
				}
			}
			if (curr_scene == GAMEPLAY && !gameplay_display_uptodate) {
				//if playing game and some actions that require change of display are made
				//show legal move
				if (show_legal_req) {
					u8 i;
					u16 temp_color = CURSOR_COLOR;
					
					if (curr_turn == WHITE) temp_color = 0xF100;
					
					for(i = 0; i < legalQrow->size; i++) {
						IERG3810_TFTLCD_ShowLegal_RC(queue_getItem(legalQrow, i), queue_getItem(legalQcol, i), temp_color);
					}

					show_legal_req = 0;
				}
				//move cursor
				if (cursor_move_req) {
					IERG3810_TFTLCD_HighlightBorder(prev_cursor_pos_rc[0], prev_cursor_pos_rc[1], 0x0000);
					if (curr_turn == BLACK) {
						IERG3810_TFTLCD_HighlightBorder(cursor_pos_rc[0], cursor_pos_rc[1], CURSOR_COLOR);
					} else if (curr_turn == WHITE) {
						IERG3810_TFTLCD_HighlightBorder(cursor_pos_rc[0], cursor_pos_rc[1], 0xF100);
					}
					prev_cursor_pos_rc[0] = cursor_pos_rc[0];
					prev_cursor_pos_rc[1] = cursor_pos_rc[1];
					cursor_move_req = 0;
				}
				//place disc
				if (disc_place_req) {
					IERG3810_TFTLCD_ShowPieces_RC(disc_placedAtRow, disc_placedAtCol, curr_turn);
					IERG3810_TFTLCD_UpdateScore(next_turn);
					gboard[disc_placedAtRow][disc_placedAtCol] = curr_turn;
					disc_place_req = 0;
				}
				//change turn
				if (turn_change_req) {
					Pieces dummy = curr_turn;
					IERG3810_TFTLCD_ShowTurn(next_turn);
					curr_turn = next_turn;
					next_turn = dummy;
					prev_legal_cnt = curr_legal_cnt;
					turn_change_req = 0;
					show_legal_req = 1;
					cursor_move_req = 1;
					//initial cursor position of each turn = rowD, col4
					cursor_pos_rc[0] = 3;
					cursor_pos_rc[1] = 3;
				}
				//indicate => has made all required change to screen
				gameplay_display_uptodate = 1;
			}

			curr_task = READINPUT;
		}
		else if (curr_task == READINPUT) {
			if (ps2count >= 11) {
				//Mask interrupt, prevent key reading when processing
				EXTI->IMR &= ~(1<<11);

				ps2DelayTimeout = 0;
				ps2dataReady = 0;
				ps2data = (ps2key & 0x000001FE) >> 1;

				//if correct start/stop bit, data is ready
				if (((ps2key & 1) == 0) && (((ps2key >> 10) & 1) == 1)) {
					ps2dataReady = 1;
				}

				if (ps2dataReady) {
					//USART_print(2, (char*)(&ps2data));
					//check parity by comparing with lookup_table (XOR method)
					expected_parity = parity_lookup[(ps2data >> 4) ^ (ps2data & 0x0F)];
					if (expected_parity == ((ps2key >> 9) & 1)) {
						//process data only if parity is also correct
						if (ps2data == 0xF0) {
							prevF0 = 1;
						} else {
							//Used key = KP1,2,3,4,5,7,8
							//if released
							if (prevF0) {
								switch (ps2data) {
									case 0x69:
										keypad_pressed[1] = 0;
										break;
									case 0x72:
										keypad_pressed[2] = 0;
										break;
									case 0x7A:
										keypad_pressed[3] = 0;
										break;
									case 0x6B:
										keypad_pressed[4] = 0;
										break;
									case 0x73:
										keypad_pressed[5] = 0;
										break;
									case 0x6C:
										keypad_pressed[7] = 0;
										break;
									case 0x75:
										keypad_pressed[8] = 0;
										break;
								}
							} else {
								//if pressed
								switch (ps2data) {
									case 0x69:
										if (!keypad_pressed[1] && is_playing) {
											/*
												when key is first pressed, set delay count to higher than threshold immediately
												so that action related to the key will be taken right away in next cycle
											*/
											keypad1DelayCount = 40;
										}
										keypad_pressed[1] = 1;
										break;
									case 0x72:
										if (!keypad_pressed[2] && is_playing) {
											keypad2DelayCount = 40;
										}
										keypad_pressed[2] = 1;
										break;
									case 0x7A:
										if (!keypad_pressed[3] && is_playing) {
											keypad3DelayCount = 40;
										}
										keypad_pressed[3] = 1;
										break;
									case 0x6B:
										if (!keypad_pressed[4] && is_playing) {
											keypad4FirstPress = 1;
										}
										keypad_pressed[4] = 1;
										break;
									case 0x73:
										if (!keypad_pressed[5] && is_playing) {
											keypad5DelayCount = 40;
										}
										keypad_pressed[5] = 1;
										break;
									case 0x6C:
										//if it is pressed for the first time, ie. not holding
										if (!keypad_pressed[7]) {
											//change scene according to current scene
											switch (curr_scene) {
												case START:
												{
													curr_scene = GAMEPLAY;
													break;
												}
												case INSTRUCTION1:
												{
													curr_scene = START;
													break;
												}
												case INSTRUCTION2:
												{
													curr_scene = INSTRUCTION1;
													break;
												}
												case INSTRUCTION3:
												{
													curr_scene = INSTRUCTION2;
													break;
												}
												case INSTRUCTION4:
												{
													curr_scene = INSTRUCTION3;
													break;
												}
												case GAMEPLAY:
												{
													curr_scene = RESTART;
													break;
												}
												case GAMEOVER:
												{
													curr_scene = GAMEPLAY;
													break;
												}
												default:
													break;
											}
										}
										keypad_pressed[7] = 1;
										break;
									case 0x75:
										//if it is pressed for the first time, ie. not holding
										if (!keypad_pressed[8]) {
											switch (curr_scene) {
												//change scene according to current scene
												case START:
												{
													curr_scene = INSTRUCTION1;
													break;
												}
												case INSTRUCTION1:
												{
													curr_scene = INSTRUCTION2;
													break;
												}
												case INSTRUCTION2:
												{
													curr_scene = INSTRUCTION3;
													break;
												}
												case INSTRUCTION3:
												{
													curr_scene = INSTRUCTION4;
													break;
												}
												case INSTRUCTION4:
												{
													curr_scene = START;
													break;
												}
												case GAMEPLAY:
												{
													curr_scene = START;
													break;
												}
												case GAMEOVER:
												{
													curr_scene = START;
													break;
												}
												default:
													break;
											}
										}
										keypad_pressed[8] = 1;
										break;
								}
							}
							prevF0 = 0;
						}
					}
					//clear out processed data
					ps2key >>= 11;
					ps2count -= 11;
				} else {
					//data buffer corrupted, clear all input and input history
					ps2count = 0;
					ps2key = 0;
					prevF0 = 0;
				}

				EXTI->PR = 1 << 11;
				//Unmask Interrupt
				EXTI->IMR |= (1<<11);
			}

			/*
				Do this only if ps2count >= 11 hasn't been satisfied for too long
				use systick for delay counting, timeout every 600ms
				when holding key, first time diff. of incoming data is around 500ms
			*/
			if (ps2DelayTimeout >= 60) {
				u8 i;
				//reset data
				ps2DelayTimeout = 0;
				ps2key = 0;
				ps2count = 0;
				prevF0 = 0;

				for (i = 0; i < 10;i++) {
					keypad_pressed[i] = 0;
				}
			}
			curr_task = GAMELOGIC;
		}
		else if (curr_task == GAMELOGIC) {
			//Give Request to GAMEDISPLAY
			if (is_playing) {
				if (!is_animating) {
					if (show_legal_req) {
						//enqueue the row/col of legal moves for curr. turn's color
						curr_legal_cnt = IEGR3810_GameLogic_UpdateLegal(curr_turn);
					}

					/*
						when press KP4 (place disc) for the first time, cursor move won't be check
						prevent cursor not being at the disc placing spot when playing animation
					*/
					if (keypad4FirstPress) {
						//place disc
						if (gboard_legal[cursor_pos_rc[0]][cursor_pos_rc[1]]) {
							//if at legal pos
							disc_place_req = 1;
							disc_placedAtRow = cursor_pos_rc[0];
							disc_placedAtCol = cursor_pos_rc[1];
							IERG3810_TFTLCD_ClearLegal_RC();
							IEGR3810_GameLogic_ClearLegal();
							IEGR3810_GameLogic_EnqueueFlippingNeeded(disc_placedAtRow, disc_placedAtCol, curr_turn);
							is_animating = 1;
							flip_delay = 0;
							//Mask PS2
							EXTI->IMR &= ~(1<<11);
							GPIOB->BSRR = 1 << 8;
							buzzer_on = 1;
							GPIOB->BRR = 1 << 5;
							led0_on = 1;
							//Log to USART2
							if (curr_turn == WHITE) {
								char *text = (char*)malloc(6);
								USART_print(2, "WHITE: ");
								strcpy(text, "ROW=  ");
								text[4] = disc_placedAtRow+65;
								USART_print(2, text);
								strcpy(text, "COL=  ");
								text[4] = disc_placedAtCol+49;
								USART_print(2, text);
								free(text);
							} else if (curr_turn == BLACK) {
								char *text = (char*)malloc(6);
								USART_print(2, "BLACK: ");
								strcpy(text, "ROW=  ");
								text[4] = disc_placedAtRow+65;
								USART_print(2, text);
								strcpy(text, "COL=  ");
								text[4] = disc_placedAtCol+49;
								USART_print(2, text);
								free(text);
							}
							USART_print(2, "\n");
						}
						/*
							if position is not legal, then will continue to move cursor in next cycle
							Also, need to release and press the key again to invoke disc placing
						*/
						keypad4FirstPress = 0;
					} else {
						//try to move cursor only if hold press for certain time
						if (keypad1DelayCount >= 20 && keypad_pressed[1]) {
							IEGR3810_GameLogic_UpdateCursorPos(-1, 0);
							keypad1DelayCount = 0;
						}
						if (keypad2DelayCount >= 20 && keypad_pressed[2]) {
							IEGR3810_GameLogic_UpdateCursorPos(0, -1);
							keypad2DelayCount = 0;
						}
						if (keypad3DelayCount >= 20 && keypad_pressed[3]) {
							IEGR3810_GameLogic_UpdateCursorPos(1, 0);
							keypad3DelayCount = 0;
						}
						if (keypad5DelayCount >= 20 && keypad_pressed[5]) {
							IEGR3810_GameLogic_UpdateCursorPos(0, 1);
							keypad5DelayCount = 0;
						}
					}

					if (prev_cursor_pos_rc[0] != cursor_pos_rc[0] || prev_cursor_pos_rc[1] != cursor_pos_rc[1]) {
						cursor_move_req = 1;
					}

					if (curr_legal_cnt == 0 && prev_legal_cnt != 0) {
						IERG3810_TFTLCD_ShowNoLegalTxt(curr_turn);
						turn_change_req = 1;
					}

					if ((curr_legal_cnt == 0 && prev_legal_cnt == 0) || (black_score+white_score >= GBOARD_SIZE * GBOARD_SIZE)) {
						//Board full or both dont have legal move...
						is_playing = 0;
						curr_scene = GAMEOVER;
					}

					//if have any req, then display is not up to date
					if (cursor_move_req || show_legal_req || disc_place_req) {
						gameplay_display_uptodate = 0;
					}
				} else {
					//flipping disc...
					if (buzzer_on && flip_delay >= 2) {
						GPIOB->BRR = 1 << 8;
						buzzer_on = 0;
					}
					if (led0_on && flip_delay >= 2) {
						GPIOB->BSRR = 1 << 5;
						led0_on = 0;
					}
					if (flip_delay >= 20) {
						if (queue_isEmpty(flippingQrow)) {
							//NO more disc left to flip
							IEGR3810_GameLogic_ClearFlipping();
							is_animating = 0;
							EXTI->IMR |= (1<<11);
							turn_change_req = 1;
						} else {
							disc_place_req = 1;
							disc_placedAtRow = dequeue(flippingQrow);
							disc_placedAtCol = dequeue(flippingQcol);

							flip_delay = 0;
							GPIOB->BSRR = 1 << 8;
							buzzer_on = 1;
							
							GPIOB->BRR = 1 << 5;
							led0_on = 1;
						}
						gameplay_display_uptodate = 0;
					}
				}
			}
			curr_task = UPDATEDISPLAY;
		}
	}
}
