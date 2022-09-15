//------------------------------------------- main.c CODE STARTS ---------------------------------------------------------------------------
#include <stdio.h>
#include "NUC100Series.h"
#include "sysconf.h"
#include "lcdconf.h"
#include "keypad.h"
#include "7seg.h"

int main(void)
{
	//////////////////////System configuration///////////////////
	System_Config();
	UART0_Config();
	GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
	SPI3_Config();
	LCD_start();
	LCD_clear();

/////////////////////////////Main program execution////////////
	while (1)
	{
		game_start();
	}
}
