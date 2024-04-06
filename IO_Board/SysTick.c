/*
 * SysTick.c
 *
 * Created: 19.03.2024 20:55:31
 *  Author: josef
 */ 

#include "SysTick.h"
#include "Step1.h"

static uint16_t tick;

void systick_init(){
	// Set mode to CTC
	TCCR0A = (1 << WGM01);
	
	// Set prescaler to 8
	TCCR0B = (1 << CS01);
	
	// Set max count of timer to 100
	OCR0A = 100;
	
	// Enable Timer Compare Match interrupt
	TIMSK0 = (1 << OCIE0A);
	
	// Total division = prescaler * Max Count = 8 * 100 = 800
	// f_systick = F_CPU / Total div = 8000000 / 800 = 10000
	
	tick = 0;
	
}

uint16_t systick_getTick(){
	return tick;
}

uint16_t systick_deltat(uint16_t later, uint16_t earlier){
	if (later > earlier){
		return later - earlier;
	}
	else{
		return (UINT16_MAX - later) + earlier + 1;
	}
}


// SysTick Interrupt service routine
ISR (TIMER0_COMPA_vect) {
	tick++;
	
	// Stepper interface status update
	static uint8_t count = 0;
	count++;
	
	if (count >= 10){
		count = 0;
		step1_update();
		//step2_update();
	}
}



