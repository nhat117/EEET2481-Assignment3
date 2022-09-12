#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

#define C3_pressed (!(PA->PIN & (1<<0)))		
#define C2_pressed (!(PA->PIN & (1<<1)))
#define C1_pressed (!(PA->PIN & (1<<2)))

void search_col1(void);
void search_col2(void);
void search_col3(void);
#endif