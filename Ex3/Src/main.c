//------------------------------------------- main.c CODE STARTS ---------------------------------------------------------------------------
#include <stdio.h>
#include "NUC100Series.h"
#include "sysconf.h"
#include "lcdconf.h"
#include "keypad.h"
#include "variable.h"
#include "7seg.h"

volatile char ReceivedByte;
volatile int uart_flag = 0;
volatile int ptr1 = 0;
volatile int ptr2 = 0;
volatile int gy = 0;
volatile int gx = 0;
extern int xy_mani;
extern int current_x;
extern int current_y;
extern int tmr0_flag;
int idx = 0;
int shot_count = 0;
int done_uploading = 0;

volatile int game_board[8][8] = {
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 1, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 1, 0},
	{0, 0, 0, 0, 0, 0, 1, 0},
	{0, 0, 1, 1, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{1, 1, 0, 0, 1, 0, 0, 0},
	{0, 0, 0, 0, 1, 0, 0, 0}};

volatile int player_board[8][8] = {
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0}};

enum game_state
{
	WELCOME,
	LOAD,
	GAME,
	END
};
int state = WELCOME;
int canResponding = 1;
int btn_pressed = 0;
int alreadyBuzz = 0;
void display_board_to_lcd();
void check_game_condition();
void shoot();
void resultBuzzer();
void reset_player_board();
void test_display_uart_board();
void handle_welcome();
void handle_load();
void handle_game();
void handle_end();
int message_type = 0;
int game_first_load = 0;

int main(void)
{
	System_Config();
	UART0_Config();
	GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
	SPI3_Config();
	LCD_start();
	LCD_clear();

	//display_board_to_lcd();
	while (1)
	{
		if (canResponding)
		{
			handleKeymatrix();
		}
		if (xy_mani)
		{
			PC->DOUT ^= 1 << 15;
		}
		
		switch (state)
		{
		case WELCOME:
			handle_welcome();
			break;
		case LOAD:
			handle_load();
			break;
		case GAME:
			handle_game();
			break;
		case END:
			handle_end();
			break;
		}
	}
}

void handle_welcome() {
			printS(0, 0, "/BATTLESHIP/");
			set7seg(0,0);
			set7seg(1,10);
			set7seg(2,0);
			set7seg(3,0);
			CLK_SysTickDelay(1000000);
			state = LOAD;
}

void handle_load() {
				if (done_uploading) {
				printS_5x7(0,0, "Map loaded successfully");
				if (btn_pressed) {
					LCD_clear();
					reset_player_board();
					shot_count = 0;
					alreadyBuzz = 0;
					canResponding = 1;
					btn_pressed = 0;
					game_first_load = 0;
					state = GAME;
				}
			} else {
				if (uart_flag) {
					if (ReceivedByte != ' ' && ReceivedByte != '\n' && ReceivedByte != '\r' && ReceivedByte != '\t') {
						if (ReceivedByte == '1') {
								game_board[gy][gx] = 1;
						} else {
								game_board[gy][gx] = 0;
						}
						gx++;
						if (gx == 8) {
							gx = 0;
							gy++;
						}
						if (gy == 8) {
							done_uploading = 1;
							LCD_clear();
						}
					}
					UART0_SendChar(ReceivedByte);
					uart_flag = 0;
				}
				if (btn_pressed) {
					btn_pressed = 0;
				}
			}
}

void handle_game() {
			if (game_first_load == 0) {
				display_board_to_lcd();
				game_first_load = 1;
			}
			if (btn_pressed)
			{
				shoot();
				display_board_to_lcd();
				check_game_condition();
				btn_pressed = 0;
			}
}

void handle_end() {
		if (message_type)
		{
			printS(0, 0, "You win");
		}
		else
		{
			printS(0, 0, "You lose");
		}

		if (alreadyBuzz == 0)
		{
			resultBuzzer();
		}

		if (btn_pressed)
		{
			reset_player_board();
			shot_count = 0;
			alreadyBuzz = 0;
			state = WELCOME;
		}
	
}

void reset_player_board()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			player_board[i][j] = 0;
		}
	}
}

void display_board_to_lcd()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (player_board[i][j] == 0)
			{
				printC_5x7(j * 8, i * 8, '_');
			}
			else
			{
				printC_5x7(j * 8, i * 8, 'X');
			}
		}
	}
}

void test_display_uart_board() {
for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (game_board[i][j] == 0)
			{
				printC_5x7(j * 8, i * 8, '_');
			}
			else
			{
				printC_5x7(j * 8, i * 8, 'X');
			}
		}
	}
}

void check_game_condition()
{
	if (shot_count > 16)
	{
		canResponding = 0;
		LCD_clear();
		message_type = 0;
		state = END;
	}
	else
	{
		int cannotWin = 0;
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				if (player_board[i][j] != game_board[i][j])
				{
					cannotWin = 1;
					break;
				}
			}
		}
		if (cannotWin == 0)
		{
			canResponding = 0;
			LCD_clear();
			message_type = 1;
			state = END;
		}
	}
}

void shoot()
{
	setShots(++shot_count);
	if (game_board[current_y - 1][current_x - 1] == 1)
	{
		player_board[current_y - 1][current_x - 1] = 1;
		for (int i = 0; i < 6; i++)
		{
			PC->DOUT ^= (1 << 12);
			CLK_SysTickDelay(80000);
		}
	}

	current_x = 0;
	current_y = 0;
	if (xy_mani) {
		set7seg(0, current_y);	
		xy_mani = 0;
	} else {
		set7seg(0, current_x);
	}
	


}

void EINT1_IRQHandler(void)
{
	btn_pressed = 1;
	PB->ISRC |= (1 << 15);
}

void TMR0_IRQHandler(void)
{
	display7seg(idx++);
	tmr0_flag = 1;
	idx %= 4;
	TIMER0->TISR |= (1 << 0);
}

void resultBuzzer()
{
	for (int i = 0; i < 10; i++)
	{
		PB->DOUT ^= (1 << 11);
		CLK_SysTickDelay(100000);
	}
	PB->DOUT |= (1 << 11);
	alreadyBuzz = 1;
}

// Load data from UART0
void UART02_IRQHandler(void)
{
	ReceivedByte = UART0->RBR;
	uart_flag = 1;
	UART0->ISR |= (1 << 0);
}


//------------------------------------------- main.c CODE ENDS ---------------------------------------------------------------------------
