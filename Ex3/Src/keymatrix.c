#include "keymatrix.h"
#include <stdio.h>
#include "NUC100Series.h"
#include "7seg.h"
			
void search_col1(void)
{
    // Drive ROW1 output pin as LOW. Other ROW pins as HIGH
	PA->DOUT &= ~(1<<3);
	PA->DOUT |= (1<<4);
	PA->DOUT |= (1<<5);
    if (C1_pressed)
    {
		PE->DOUT = pattern[1];
        return;	
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
		PE->DOUT = pattern[4];
        return;
    }
     else{
			 
    // Drive ROW3 output pin as LOW. Other ROW pins as HIGH
		PA->DOUT |= (1<<3);
		PA->DOUT |= (1<<4);		
		PA->DOUT &= ~(1<<5);	
    if (C1_pressed)
    {
        // If column1 is LOW, detect key press as K7 (KEY 7)
				PE->DOUT = pattern[7];
        return;
    }
	else
		return;
	}
	}
}		

void search_col2(void)
{
    // Drive ROW1 output pin as LOW. Other ROW pins as HIGH
	PA->DOUT &= ~(1<<3);
	PA->DOUT |= (1<<4);
	PA->DOUT |= (1<<5);
    if (C2_pressed)
    {
        // If column2 is LOW, detect key press as K2 (KEY 2)
		PE->DOUT = pattern[2];
        return;
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
		PE->DOUT = pattern[5];
        return;
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
		PE->DOUT = pattern[8];
        return;
    }
		else	
		return;
	}
	}
}	

void search_col3(void)
{
    // Drive ROW1 output pin as LOW. Other ROW pins as HIGH
		PA->DOUT &= ~(1<<3);
		PA->DOUT |= (1<<4);
		PA->DOUT |= (1<<5);
     
    if (C3_pressed)
    {
        // If column3 is LOW, detect key press as K3 (KEY 3)
		PE->DOUT = pattern[3];
        return;
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
		PE->DOUT = pattern[6];
        return;
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
		PE->DOUT = pattern[9];
        return;
    }
		else
		return;
	}
	}
}	