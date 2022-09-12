#ifndef UART_INCLUDED
#define UART_INCLUDED


void UART0_Config(void);
void UART0_CLKConfig(void);
void UART0_SendChar(int ch);
char UART0_GetChar();


#endif