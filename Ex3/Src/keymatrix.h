#ifndef KEYMATRIX_INCLUDED
#define KEYMATRIX_INCLUDED

#define C3_pressed (!(PA->PIN & (1<<0)))		
#define C2_pressed (!(PA->PIN & (1<<1)))
#define C1_pressed (!(PA->PIN & (1<<2)))

int search_col1(void);
int search_col2(void);
int search_col3(void);
void handleKeymatrix(void);
#endif