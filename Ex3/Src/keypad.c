#include <stdio.h>
#include "NUC100Series.h"
#include "keypad.h"
#include "7seg.h"


#define C3_pressed (!(PA->PIN & (1<<0)))		
#define C2_pressed (!(PA->PIN & (1<<1)))
#define C1_pressed (!(PA->PIN & (1<<2)))
#define SAMPLE_COUNT 100

static int c3_state = 0;
static int c2_state = 0;
static int c1_state = 0;

int tmr0_flag = 0;
int xy_mani = 0;
int current_x = 0;
int current_y = 0;

extern timer[4];
extern pattern;

int search_col1(void)
{
    // Drive ROW1 output pin as LOW. Other ROW pins as HIGH
	PA->DOUT &= ~(1<<3);
	PA->DOUT |= (1<<4);
	PA->DOUT |= (1<<5);
    if (C1_pressed)
    {
				PC->DOUT ^= 1 << 12;
        return 1;	
    }
  else
	{		
    // Drive ROW2 output pin as LOW. Other ROW pins as HIGH
		PA->DOUT |= (1<<3);
		PA->DOUT &= ~(1<<4);
		PA->DOUT |= (1<<5);	
    if (C1_pressed)
    {
        // If column1 is LOW, detect key press as K4 (KEY 4)
        return 4;
    }
     else{
			 
    // Drive ROW3 output pin as LOW. Other ROW pins as HIGH
		PA->DOUT |= (1<<3);
		PA->DOUT |= (1<<4);		
		PA->DOUT &= ~(1<<5);			 
    if (C1_pressed)
    {
        // If column1 is LOW, detect key press as K7 (KEY 7)
        return 7;
    }
	else
		return 10;
	}
		 return 10;
	}
}		

int search_col2(void)
{
    // Drive ROW1 output pin as LOW. Other ROW pins as HIGH
	PA->DOUT &= ~(1<<3);
	PA->DOUT |= (1<<4);
	PA->DOUT |= (1<<5);
    if (C2_pressed)
    {
        // If column2 is LOW, detect key press as K2 (KEY 2)
        return 2;
    }
	else
	{  
    // Drive ROW2 output pin as LOW. Other ROW pins as HIGH
		PA->DOUT |= (1<<3);
		PA->DOUT &= ~(1<<4);
		PA->DOUT |= (1<<5);
    if (C2_pressed)
    {
        // If column2 is LOW, detect key press as K5 (KEY 5)
        return 5;
    }	
		else
		{
    // Drive ROW3 output pin as LOW. Other ROW pins as HIGH
		PA->DOUT |= (1<<3);
		PA->DOUT |= (1<<4);		
		PA->DOUT &= ~(1<<5);
    if (C2_pressed)
    {
        // If column3 is LOW, detect key press as K8 (KEY 8)
        return 8;
    }
		else	
		return 10;
	}
		return 10;
	}
}	

int search_col3(void)
{
    // Drive ROW1 output pin as LOW. Other ROW pins as HIGH

		PA->DOUT &= ~(1<<3);
		PA->DOUT |= (1<<4);
		PA->DOUT |= (1<<5);
    if (C3_pressed)
    {
        // If column3 is LOW, detect key press as K3 (KEY 3)
        return 3;
    }
		else
		{
    // Drive ROW2 output pin as LOW. Other ROW pins as HIGH
		PA->DOUT |= (1<<3);
		PA->DOUT &= ~(1<<4);
		PA->DOUT |= (1<<5);
    if (C3_pressed)
    {
        // If column3 is LOW, detect key press as K6 (KEY 6)
        return 6;
    }
		else
		{
    // Drive ROW3 output pin as LOW. Other ROW pins as HIGH	
		PA->DOUT |= (1<<3);
		PA->DOUT |= (1<<4);		
		PA->DOUT &= ~(1<<5);			
    if (C3_pressed)
    {
        // If column3 is LOW, detect key press as K9 (KEY 9)
        return 9;
    }
		else
		return 10;
	}
		return 10;
	}
}	

void handleKeymatrix() {
		PA->DOUT &= ~(1<<3);
		PA->DOUT &= ~(1<<4);
		PA->DOUT &= ~(1<<5);

		if(debounceC1()) 
			{
				c1_state = 0;
				int temp = search_col1();
				if (temp != 10) {
					if (xy_mani == 0) {
						current_x = temp;
					} else {
						current_y = temp;
					}
				}
			}
		else if(debounceC2()) 
			{
				c2_state = 0;
				int temp = search_col2();
				if (temp != 10) {
					if (xy_mani == 0) {
						current_x = temp;
					} else {
						current_y = temp;
					}				
				}
			}	
		else if(debounceC3()) 
			{
				c3_state = 0;
				int temp = search_col3();
				if (temp != 10) {
					if (temp != 9) {
						if (xy_mani == 0) {
							current_x = temp;
						} else {
							current_y = temp;
						}		
					} else {
						if (xy_mani == 0) {
							xy_mani = 1;
						} else if (xy_mani == 1){
							xy_mani = 0;
						}
						return;
					}
				}
			}					

		if (xy_mani == 0) {
			timer[0] = current_x;
		} else {
			timer[0] = current_y;
		}
}

int debounceC3(void) {
	if (tmr0_flag) {
		c3_state++;
		tmr0_flag = 0;
	}
	if (c3_state == SAMPLE_COUNT) {
		c3_state = 0;
		return 1;
	}
	return 0;
}

int debounceC2(void) {
	if (tmr0_flag) {
		c2_state++;
		tmr0_flag = 0;
	}
	if (c2_state == SAMPLE_COUNT) {
		c2_state = 0;
		return 1;
	}
	return 0;
}

int debounceC1(void) {
	if (tmr0_flag) {
		c1_state++;
		tmr0_flag = 0;
	}
	if (c1_state == SAMPLE_COUNT) {
		c1_state = 0;
		return 1;
	}
	return 0;
}