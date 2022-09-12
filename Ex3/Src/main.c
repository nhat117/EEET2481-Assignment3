//------------------------------------------- main.c CODE STARTS ---------------------------------------------------------------------------
#include <stdio.h>
#include "keymatrix.h"
#include "NUC100Series.h"

#define C3_pressed (!(PA->PIN & (1<<0)))		
#define C2_pressed (!(PA->PIN & (1<<1)))
#define C1_pressed (!(PA->PIN & (1<<2)))

void System_Config(void);
void UART0_Config(void);
void UART0_CLKConfig(void);
void UART0_SendChar(int ch);
char UART0_GetChar();
void SPI3_Config(void);
void LCD_start(void);
void LCD_command(unsigned char temp);
void LCD_data(unsigned char temp);
void LCD_clear(void);
void LCD_SetAddress(uint8_t PageAddr, uint8_t ColumnAddr);
void GPB15_Config(void);

volatile char ReceivedByte;   

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

int main(void)
{ 
	System_Config();
	UART0_Config();
	GPIO_SetMode(PC,BIT12,GPIO_MODE_OUTPUT);
	GPIO_SetMode(PC,BIT13,GPIO_MODE_OUTPUT);
	GPB15_Config();
	SPI3_Config();
	LCD_start();
	LCD_clear();

	
	while(1)
	{
		handleKeymatrix();
	}
}



void System_Config (void){
	
	
	SYS_UnlockReg(); // Unlock protected registers

	// enable clock sources
	CLK->PWRCON |= (1 << 2);
	while(!(CLK->CLKSTATUS & (1 << 4)));
	CLK->PLLCON |= (1<<18); //disable PLLOUT
	// CPU clock source selection
	CLK->CLKSEL0 &= (~(0x07 << 0));
	CLK->CLKSEL0 |= (0b111 << 0);    // chose 22.1184Mhz
	//clock frequency division
	CLK->CLKDIV &= (~0x0F << 0);

	//UART0 Clock selection and configuration
	UART0_CLKConfig();
	CLK->APBCLK |= (1 << 15); // enable spi3

	SYS_LockReg();  // Lock protected registers    
	
	    
	//Configure GPIO for Key Matrix
	//Rows - outputs
	PA->PMD &= (~(0b11<< 6));
  PA->PMD |= (0b01 << 6);    
	PA->PMD &= (~(0b11<< 8));
  PA->PMD |= (0b01 << 8);  		
	PA->PMD &= (~(0b11<< 10));
  PA->PMD |= (0b01 << 10);  
		
		//Updated - We will keep the Column as default(after reset) in quasi-mode instead of inputs
//	//COLUMN - 
//	PA->PMD &= (~(0b11<< 0));		
//	PA->PMD &= (~(0b11<< 2));		
//	PA->PMD &= (~(0b11<< 4));		
		
	//Configure GPIO for 7segment
	//Set mode for PC4 to PC7 
  PC->PMD &= (~(0xFF<< 8));		//clear PMD[15:8] 
  PC->PMD |= (0b01010101 << 8);   //Set output push-pull for PC4 to PC7
	//Set mode for PE0 to PE7
	PE->PMD &= (~(0xFFFF<< 0));		//clear PMD[15:0] 
	PE->PMD |= 0b0101010101010101<<0;   //Set output push-pull for PE0 to PE7
	
	//Select the first digit U11
	PC->DOUT |= (1<<7);     //Logic 1 to turn on the digit
	PC->DOUT &= ~(1<<6);		//SC3
	PC->DOUT &= ~(1<<5);		//SC2
	PC->DOUT &= ~(1<<4);		//SC1		

}

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

	// UART0 operation configuration
	UART0->LCR |= (0b11 << 0); // 8 data bit
	UART0->LCR &= ~(1 << 2); // one stop bit	
	UART0->LCR &= ~(1 << 3); // no parity bit
	UART0->FCR |= (1 << 1); // clear RX FIFO
	UART0->FCR |= (1 << 2); // clear TX FIFO
	
	UART0->FCR &= ~(0xF << 16); // FIFO Trigger Level is 1 byte
	
	//Baud rate config: BRD/A = 1, DIV_X_EN=0
	//--> Mode 0, Baud rate = UART_CLK/[16*(A+2)] = 22.1184 MHz/[16*(1+2)]= 460800 bps
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
///////////////////////////////////LCD Config/////////////////////////////////////
void LCD_start(void)
{
    LCD_command(0xE2); // Set system reset
    LCD_command(0xA1); // Set Frame rate 100 fps  
    LCD_command(0xEB); // Set LCD bias ratio E8~EB for 6~9 (min~max)  
    LCD_command(0x81); // Set V BIAS potentiometer
    LCD_command(0xA0); // Set V BIAS potentiometer: A0 ()           
    LCD_command(0xC0);
    LCD_command(0xAF); // Set Display Enable
}

void LCD_command(unsigned char temp)
{
    SPI3->SSR |= 1 << 0;
    SPI3->TX[0] = temp;
    SPI3->CNTRL |= 1 << 0;
    while (SPI3->CNTRL & (1 << 0));
    SPI3->SSR &= ~(1 << 0);
}

void LCD_data(unsigned char temp)
{
    SPI3->SSR |= 1 << 0;
    SPI3->TX[0] = 0x0100 + temp;
    SPI3->CNTRL |= 1 << 0;
    while (SPI3->CNTRL & (1 << 0));
    SPI3->SSR &= ~(1 << 0);
}

void LCD_clear(void)
{
    int16_t i;
    LCD_SetAddress(0x0, 0x0);
    for (i = 0; i < 132 * 8; i++)
    {
        LCD_data(0x00);
    }
}

void LCD_SetAddress(uint8_t PageAddr, uint8_t ColumnAddr)
{
    LCD_command(0xB0 | PageAddr);
    LCD_command(0x10 | (ColumnAddr >> 4) & 0xF);
    LCD_command(0x00 | (ColumnAddr & 0xF));
}

void SPI3_Config(void) {
	GPIO_SetMode(PD,BIT8,GPIO_MODE_OUTPUT);
	GPIO_SetMode(PD,BIT9,GPIO_MODE_OUTPUT);
	GPIO_SetMode(PD,BIT11,GPIO_MODE_OUTPUT);
	SYS->GPD_MFP |= 1 << 11; //1: PD11 is configured for alternative function
	SYS->GPD_MFP |= 1 << 9; //1: PD9 is configured for alternative function
	SYS->GPD_MFP |= 1 << 8; //1: PD8 is configured for alternative function
	SPI3->CNTRL &= ~(1 << 23); //0: disable variable clock feature
	SPI3->CNTRL &= ~(1 << 22); //0: disable two bits transfer mode
	SPI3->CNTRL &= ~(1 << 18); //0: select Master mode
	SPI3->CNTRL &= ~(1 << 17); //0: disable SPI interrupt
	SPI3->CNTRL |= 1 << 11; //1: SPI clock idle high
	SPI3->CNTRL &= ~(1 << 10); //0: MSB is sent first
	SPI3->CNTRL &= ~(0b11 << 8); //00: one transmit/receive word will be executed in one data transfer
	SPI3->CNTRL &= ~(0b11111 << 3);
	SPI3->CNTRL |= 9 << 3; //9 bits/word
	SPI3->CNTRL |= (1 << 2);  //1: Transmit at negative edge of SPI CLK
	SPI3->DIVIDER = 24;
}
//SW_INT 1 Config

void GPB15_Config(void) {
 PB->PMD &= (~(0b11 << 30));
 PB->IMD &= (~(1 << 15));
 PB->IEN |= (1 << 15); //Enable interrupt	
	NVIC->ISER[0] |= 1<<3;
	NVIC->IP[0] &= (~(0b11<<30));
	
}

void EINT1_IRQHandler(void){
//Do something
	PC->DOUT ^= 1<<12;
	PB->ISRC |= (1 << 15);
}

//------------------------------------------- main.c CODE ENDS ---------------------------------------------------------------------------
