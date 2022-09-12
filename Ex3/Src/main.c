//------------------------------------------- main.c CODE STARTS ---------------------------------------------------------------------------
#include <stdio.h>
#include "NUC100Series.h"
#include "sysconf.h"
#include "lcdconf.h"
#include "keypad.h"
#include "variable.h"




volatile char ReceivedByte;   

extern int xy_mani;
extern int current_x;
extern int current_y;


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

enum game_state{WELCOME, LOAD, GAME};
int state = WELCOME;
int btn_pressed = 0;
void display_board_to_lcd();

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
		handleKeymatrix();
		if (xy_mani) {
			PC->DOUT ^= 1 << 15;
		}
		switch(state) {
			case WELCOME:
				
				CLK_SysTickDelay(100000);
				/*
					printS(0,0,"/BATTLESHIP/");
					if (btn_pressed) {
						PC->DOUT ^= 1 << 12;
						btn_pressed = 0;
					}*/
			break;
		}
		
	}
}

void display_board_to_lcd() {
	int tempCol = 0;
	int tempRow = 0;
	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 8; j++) {
				printC_5x7(j + tempCol,i + tempRow, '_');
				tempCol += 10;
		}
		tempCol = 0;
		tempRow += 4;
	}
}


void EINT1_IRQHandler(void){
	btn_pressed = 1;
	PB->ISRC |= (1 << 15);
}

//------------------------------------------- main.c CODE ENDS ---------------------------------------------------------------------------
