// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -main.c CODE STARTS-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
#include <stdio.h>
#include "NUC100Series.h"
#include "LCD.h"

#define HXT_STATUS 1 << 0
#define PLL_STATUS 1 << 2
#define VOLTAGE_THRESHOLD 2000
#define DELAY_TIME 80
//TODO: Configuring the delay time
//TODO: Verify the clck
void System_Config(void);
void UART0_Config(void);
void SPI3_Config(void);
void LCD_start(void);
void LCD_command(unsigned char temp);
void LCD_data(unsigned char temp);
void LCD_clear(void);
void LCD_SetAddress(uint8_t PageAddr, uint8_t ColumnAddr);

int main(void)
{

	System_Config();
	GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
	UART0_Config();
	SPI3_Config();
	LCD_start();
	LCD_clear();

	//--------------------------------
	// LCD static content
	//--------------------------------

	/*
		printS_5x7(2, 0, "EEET2481 Ex1");
		printS_5x7(2, 8, "ADC7 conversion test");
		printS_5x7(2, 16, "Reference voltage: 5 V");
		printS_5x7(2, 24, "A/D resolution: 1.221 mV");
		printS_5x7(2, 40, "A/D value:");
	*/

	char ReceivedByte;
	char s[] = "xxxxx";
	while (1)
	{
			while((UART0->FSR & (0x01 << 14)))
				{
						
			
				}
				ReceivedByte = UART0->DATA;

							//s[0] = ReceivedByte;
					PC->DOUT ^= (1 << 12);
				printC(2,0,ReceivedByte);
					CLK_SysTickDelay(2000000);				
				

				
				
		
	}
}

//------------------------------------------------------------------------------------------------------------------------------------
// Functions definition
//------------------------------------------------------------------------------------------------------------------------------------

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
	UART0->FCR &= ~(0xF << 16); // FIFO Trigger Level is 1 byte]
	
	//Baud rate config: BRD/A = 1, DIV_X_EN=0
	//--> Mode 0, Baud rate = UART_CLK/[16*(A+2)] = 22.1184 MHz/[16*(1+2)]= 460800 bps
	UART0->BAUD &= ~(0b11 << 28); // mode 0	
	UART0->BAUD &= ~(0xFFFF << 0);
	UART0->BAUD |= 70;
}

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
	while (SPI3->CNTRL & (1 << 0))
		;
	SPI3->SSR &= ~(1 << 0);
}

void LCD_data(unsigned char temp)
{
	SPI3->SSR |= 1 << 0;
	SPI3->TX[0] = 0x0100 + temp;
	SPI3->CNTRL |= 1 << 0;
	while (SPI3->CNTRL & (1 << 0))
		;
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

void System_Config(void)
{
	SYS_UnlockReg(); // Unlock protected registers

	CLK->PWRCON |= (1 << 0);
	while (!(CLK->CLKSTATUS & HXT_STATUS))
		;

	// PLL configuration starts
	CLK->PLLCON &= ~(1 << 19); // 0: PLL input is HXT
	CLK->PLLCON &= ~(1 << 16); // PLL in normal mode
	CLK->PLLCON &= (~(0x01FF << 0));
	CLK->PLLCON |= 48;
	CLK->PLLCON &= ~(1 << 18); // 0: enable PLLOUT
	while (!(CLK->CLKSTATUS & PLL_STATUS))
		;
	// PLL configuration ends

	// clock source selection
	CLK->CLKSEL0 &= (~(0x07 << 0));
	CLK->CLKSEL0 |= (0x02 << 0);
	// clock frequency division
	CLK->CLKDIV &= (~0x0F << 0);

	// SPI3 clock enable
	CLK->APBCLK |= 1 << 15;
	// SPI2 clock enable
	CLK->APBCLK |= 1 << 14;

	// ADC Clock selection and configuration
	CLK->CLKSEL1 &= ~(0x03 << 2); // ADC clock source is 12 MHz
	CLK->CLKDIV &= ~(0x0FF << 16);
	CLK->CLKDIV |= (0x0B << 16); // ADC clock divider is (11+1) --> ADC clock is 12/12 = 1 MHz
	CLK->APBCLK |= (0x01 << 28); // enable ADC clock

	SYS_LockReg(); // Lock protected registers
}

void SPI3_Config(void)
{

	GPIO_SetMode(PD, BIT11, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PD, BIT8, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PD, BIT9, GPIO_MODE_OUTPUT);
	SYS->GPD_MFP |= 1 << 11; // 1: PD11 is configured for SPI3
	SYS->GPD_MFP |= 1 << 9;	 // 1: PD9 is configured for SPI3
	SYS->GPD_MFP |= 1 << 8;	 // 1: PD8 is configured for SPI3

	SPI3->CNTRL &= ~(1 << 23); // 0: disable variable clock feature
	SPI3->CNTRL &= ~(1 << 22); // 0: disable two bits transfer mode
	SPI3->CNTRL &= ~(1 << 18); // 0: select Master mode
	SPI3->CNTRL &= ~(1 << 17); // 0: disable SPI interrupt
	SPI3->CNTRL |= 1 << 11;	   // 1: SPI clock idle high
	SPI3->CNTRL &= ~(1 << 10); // 0: MSB is sent first
	SPI3->CNTRL &= ~(3 << 8);  // 00: one transmit/receive word will be executed in one data transfer

	SPI3->CNTRL &= ~(31 << 3); // Transmit/Receive bit length
	SPI3->CNTRL |= 9 << 3;	   // 9: 9 bits transmitted/received per data transfer

	SPI3->CNTRL |= (1 << 2); // 1: Transmit at negative edge of SPI CLK
	SPI3->DIVIDER = 0;		 // SPI clock divider. SPI clock = HCLK / ((DIVIDER+1)*2). HCLK = 50 MHz
}

//------------------------------------------- main.c CODE ENDS ---------------------------------------------------------------------------
