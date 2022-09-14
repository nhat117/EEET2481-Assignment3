/*
#include <stdio.h>
#include "NUC100Series.h"
#include "debounce.h"

Button *button_init(void) {
    Button *b = malloc(sizeof(*b));
    if (b != 0)
    {
			b->begin = &begin;
			b->debounce = &debounce;
			b->pin = 0;
			b->state = 0;
    }
    return b;
}

void *button_deinit(Button *this)
{
    if (this != NULL)
    {
        free(this);  
    }  
}

static void begin(Button *this, int pin) {
	this->pin = pin;
	this->state = 0;
}

static int debounce(Button *this) {
	int tmp = 0;
	if (!(PA->PIN & (1 << this->pin))) {
		tmp = 1;
	}
	this->state = (this->state << 1) | tmp | 0xfe00;
	return (this->state == 0xff00); // Okay to receive input
}

*/