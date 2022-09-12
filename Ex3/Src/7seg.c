#include <stdio.h>
#include "NUC100Series.h"
#include "7seg.h"

int pattern[] = {
               //   gedbaf_dot_c
                  0b10000010,  //Number 0          // ---a----
                  0b11101110,  //Number 1          // |      |
                  0b00000111,  //Number 2          // f      b
                  0b01000110,  //Number 3          // |      |
                  0b01101010,  //Number 4          // ---g----
                  0b01010010,  //Number 5          // |      |
                  0b00010010,  //Number 6          // e      c
                  0b11100110,  //Number 7          // |      |
                  0b00000010,  //Number 8          // ---d----
                  0b01000010,   //Number 9
                  0b11111111   //Blank LED 
                }; 

int timer[4] = {10,10,10,10};
int cur = 0;

void set7seg(int index, int value) {
		timer[index] = value;
}

void display7seg(int cur) {
	switch (cur) {
		case 0:
			PC->DOUT |= (1<<7);     //Logic 1 to turn on the digit
			PC->DOUT &= ~(1<<6);		//SC3
			PC->DOUT &= ~(1<<5);		//SC2
			PC->DOUT &= ~(1<<4);		//SC1	
			break;
		case 1:
			PC->DOUT &= ~(1<<7);     //Logic 1 to turn on the digit
			PC->DOUT |= (1<<6);		//SC3
			PC->DOUT &= ~(1<<5);		//SC2
			PC->DOUT &= ~(1<<4);		//SC1	
			break;
		case 2:
			PC->DOUT &= ~(1<<7);     //Logic 1 to turn on the digit
			PC->DOUT &= ~(1<<6);		//SC3
			PC->DOUT |= (1<<5);		//SC2
			PC->DOUT &= ~(1<<4);		//SC1	
			break;
		case 3:
			PC->DOUT &= ~(1<<7);     //Logic 1 to turn on the digit
			PC->DOUT &= ~(1<<6);		//SC3
			PC->DOUT &= ~(1<<5);		//SC2
			PC->DOUT |= (1<<4);		//SC1	
			break;
	}
	PE->DOUT = pattern[timer[cur]];
}

void setShots(int shot) {
	int first_part = shot / 10;
	int second_part = shot % 10;
	timer[2] = first_part;
	timer[3] = second_part;
}