#include <stdio.h>
#include "NUC100Series.h"
#include "uartconf.h"

void System_Config(void)
{
	SYS_UnlockReg(); // Unlock protected registers
	// enable clock sources
	CLK->PWRCON |= (1 << 2);
	while (!(CLK->CLKSTATUS & (1 << 4)));
	CLK->PLLCON |= (1 << 18); // disable PLLOUT
	// CPU clock source selection
	CLK->CLKSEL0 &= (~(0x07 << 0));
	CLK->CLKSEL0 |= (0b111 << 0); // chose 22.1184Mhz
	// clock frequency division
	CLK->CLKDIV &= (~0x0F << 0);

	// TIMER0 Conf
	CLK->CLKSEL1 &= ~(0b111 << 8);
	CLK->CLKSEL1 |= (0b111 << 8); // 22.1184Mhz
	CLK->APBCLK |= (1 << 2);	  // enable timer 0
	TIMER0->TCSR &= ~(0xFF << 0);
	// reset   Ti mer   0
	TIMER0->TCSR |= (1 << 26);
	// define  Timer   0  operation  mode
	TIMER0->TCSR &= ~(0b11 << 27);
	TIMER0->TCSR |= (0b01 << 27); // periodic  mode
	TIMER0->TCSR &= ~(1 << 24);
	// TDR to  be  updated  continuously  while  t i mer   counter   is  counting
	TIMER0->TCSR |= (1 << 16);
	// Enable  TE  bit   ( bi t   29)   of   TCSR
	// The bi t   will   enabl e  t he  t i mer   i nt er r upt f l ag  TIF
	TIMER0->TCSR |= (1 << 29);
	// TimeOut   =  0. 5s  - - >  Counter ' s  TCMPR  =  0. 5s  /   ( 1/ ( 32768  Hz)   =  16384
	TIMER0->TCMPR = 110592 - 1; // T = 0.005s
	// start counting
	TIMER0->TCSR |= (1 << 30);
	NVIC->ISER[0] |= (1 << 8);
	NVIC->IP[2] &= ~(0b11 << 6);

	// UART0 Clock selection and configuration
	UART0_CLKConfig();
	CLK->APBCLK |= (1 << 16); // enable uart
	CLK->APBCLK |= (1 << 15); // enable spi3

	SYS_LockReg(); // Lock protected registers
	// Configure GPIO for Key Matrix
	// Rows - outputs
	PA->PMD &= (~(0b11 << 6));
	PA->PMD |= (0b01 << 6);
	PA->PMD &= (~(0b11 << 8));
	PA->PMD |= (0b01 << 8);
	PA->PMD &= (~(0b11 << 10));
	PA->PMD |= (0b01 << 10);

	// Configure GPIO for 7segment
	// Set mode for PC4 to PC7
	PC->PMD &= (~(0xFF << 8));	  // clear PMD[15:8]
	PC->PMD |= (0b01010101 << 8); // Set output push-pull for PC4 to PC7
	// Set mode for PE0 to PE7
	PE->PMD &= (~(0xFFFF << 0));		// clear PMD[15:0]
	PE->PMD |= 0b0101010101010101 << 0; // Set output push-pull for PE0 to PE7

	// Select the first digit U11
	PC->DOUT |= (1 << 7);  // Logic 1 to turn on the digit
	PC->DOUT &= ~(1 << 6); // SC3
	PC->DOUT &= ~(1 << 5); // SC2
	PC->DOUT &= ~(1 << 4); // SC1

	// GPB15 config
	PB->PMD &= (~(0b11 << 30));
	PB->IMD &= (~(1 << 15));
	PB->IEN |= (1 << 15); // Enable interrupt
	NVIC->ISER[0] |= 1 << 3;
	NVIC->IP[0] &= (~(0b11 << 30));

	// debounce
	PB->DBEN |= (1 << 15);
	PA->DBEN |= (0b1111111 << 0);
	GPIO->DBNCECON &= ~(1 << 4);
	GPIO->DBNCECON &= ~(0xF << 0);
	GPIO->DBNCECON |= 0xF << 0;

	// Buzzer
	PB->PMD &= (~(0b11) << 22);
	PB->PMD |= (0b01 << 22);
}