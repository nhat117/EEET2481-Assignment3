#include <stdio.h>
#include "NUC100Series.h"
#include "uartconf.h"


////////////////////UART Config/////////////////////////////////////
void UART0_CLKConfig(void) {
	CLK->CLKSEL1 |= (0b11 << 24); // UART0 clock source is 22.1184 MHz
	CLK->CLKDIV &= ~(0xF << 8); // clock divider is 1
	CLK->APBCLK |= (1 << 16); // enable UART0 clock
}

void UART0_Config(void) {
	// UART0 pin configuration. PB.1 pin is for UART0 TX
	PB->PMD &= ~(0b11 << 2);
	PB->PMD |= (0b01 << 2); // PB.1 is output pin
	SYS->GPB_MFP |= (1 << 1); // GPB_MFP[1] = 1 -> PB.1 is UART0 TX pin
	
	SYS->GPB_MFP |= (1 << 0); // GPB_MFP[0] = 1 -> PB.0 is UART0 RX pin	
	PB->PMD &= ~(0b11 << 0);	// Set Pin Mode for GPB.0(RX - Input)

	//Enable UART Interrupt
	UART0->IER |= (1 << 0); // Enable RDA interrupt
	NVIC->ISER[0] |= (1 << 12); // Enable UART0 interrupt
	NVIC->IP[3] &= ~(0b11 << 6); // Set priority to 0


	// UART0 operation configuration
	UART0->LCR |= (0b11 << 0); // 8 data bit
	UART0->LCR &= ~(1 << 2); // one stop bit	
	UART0->LCR &= ~(1 << 3); // no parity bit
	UART0->FCR |= (1 << 1); // clear RX FIFO
	UART0->FCR |= (1 << 2); // clear TX FIFO
	
	UART0->FCR &= ~(0xF << 16); // FIFO Trigger Level is 1 byte
	
	//Baud rate config: BRD/A = 1, DIV_X_EN=0
	//--> Mode 0, baud rate 19200a
	UART0->BAUD &= ~(0b11 << 28); // mode 0	
	UART0->BAUD &= ~(0xFFFF << 0);
	UART0->BAUD |= 70;
}

void UART0_SendChar(int ch){
	while(UART0->FSR & (0x01 << 23)); //wait until TX FIFO is not full
	UART0->DATA = ch;
	if (ch == '\n') { // \n is new line
		while(UART0->FSR & (0x01 << 23));
		UART0->DATA = '\r'; // '\r' - Carriage
		return;
	}
}