/*
#ifndef DEBOUNCE_S
#define DEBOUNCE_S

typedef struct {
	int pin;
	int state;
	void (*begin)(Button *this, int pin);
	int (*debounce) (Button *this);
} Button;


Button *button_init(void);
void *button_deinit(Button *this);

#endif

*/