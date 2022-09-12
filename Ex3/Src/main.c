//------------------------------------------- main.c CODE STARTS ---------------------------------------------------------------------------
#include <stdio.h>
#include "NUC100Series.h"
#include "sysconf.h"
#include "lcdconf.h"
#include "keypad.h"
#include "variable.h"
#include "7seg.h"



volatile char ReceivedByte;   

extern int xy_mani;
extern int current_x;
extern int current_y;
int idx = 0;
int shot_count = 0;


volatile int game_board[8][8] = {
	{0,0,0,0,0,0,0,0},
	{0,1,1,0,0,0,0,0},
	{0,0,0,0,0,0,1,0},
	{0,0,0,0,0,0,1,0},
	{0,0,1,1,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{1,1,0,0,1,0,0,0},
	{0,0,0,0,1,0,0,0}
};

volatile int player_board[8][8] = {
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0}
};

enum game_state{WELCOME, LOAD, GAME, END};
int state = GAME;
int canResponding = 1;
int btn_pressed = 0;
int alreadyBuzz = 0;
void display_board_to_lcd();
void check_game_condition();
void shoot();
void resultBuzzer();
void reset_player_board();
int message_type = 0;


int main(void)
{ 
	System_Config();
	UART0_Config();
	GPIO_SetMode(PC,BIT12,GPIO_MODE_OUTPUT);
	GPIO_SetMode(PC,BIT13,GPIO_MODE_OUTPUT);
	SPI3_Config();
	LCD_start();
	LCD_clear();

	display_board_to_lcd();
	while(1)
	{
		if (canResponding) {
		handleKeymatrix();		
		}
		if (xy_mani) {
			PC->DOUT ^= 1 << 15;
		}
		switch(state) {
			case WELCOME:
				printS(0,0,"/BATTLESHIP/");
				break;
			case GAME:
				if (btn_pressed) {
					shoot();
					display_board_to_lcd();
					check_game_condition();
					btn_pressed = 0;
				}
			break;
			case END:
				if (message_type) {
					printS(0,0, "You win");
				} else {
					printS(0,0, "You lose");
				}

				if (alreadyBuzz == 0) {
					resultBuzzer();
				}
				
				if (btn_pressed) {
					reset_player_board();
					shot_count = 0;
					alreadyBuzz = 0;
					state = WELCOME;
				}
				break;
		}
		
	}
}

void reset_player_board() {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			player_board[i][j] = 0;
		}
	}
}

void display_board_to_lcd() {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (player_board[i][j] == 0) {
				printC_5x7(j * 8,i * 8, '_');
			} else {
				printC_5x7(j * 8,i * 8, 'X');
			}
		}
	}
}

void check_game_condition() {
	if (shot_count > 16) {
		canResponding = 0;
		LCD_clear();
		message_type = 0;
		state = END;
	} else {
		int cannotWin = 0;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (player_board[i][j] != game_board[i][j]) {
					cannotWin = 1;
					break;
				}
			}
		}
		if (cannotWin == 0) {
			canResponding = 0;
			LCD_clear();
			message_type = 1;
			state = END;
		}
	}
}

void shoot() {
	setShots(++shot_count);
	if (game_board[current_y - 1][current_x - 1] == 1) {
		player_board[current_y - 1][current_x - 1] = 1;
		for (int i = 0; i < 6; i++) {
			PC->DOUT ^= (1 << 12);
			CLK_SysTickDelay(80000);
		}
	}
	
	current_x = 0;
	current_y = 0;
	set7seg(0,0);
	set7seg(1,0);
}


void EINT1_IRQHandler(void){
	btn_pressed = 1;
	PB->ISRC |= (1 << 15);
}

void TMR0_IRQHandler(void) {
	PC->DOUT ^= 1 << 13;
	display7seg(idx++);
	idx %= 4;
	TIMER0->TISR |= (1 << 0);
}

void resultBuzzer() {
	for (int i = 0; i < 10; i++) {
		PB->DOUT ^= (1 << 11);
		CLK_SysTickDelay(100000);
	}
	PB->DOUT |= (1 << 11);
	alreadyBuzz = 1;
}

//------------------------------------------- main.c CODE ENDS ---------------------------------------------------------------------------
