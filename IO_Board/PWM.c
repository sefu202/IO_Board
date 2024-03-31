/*
 * PWM.c
 *
 * Created: 19.03.2024 20:54:56
 *  Author: josef
 */ 

#include "PWM.h"

void PWM_init(uint8_t prescaler){
	
	// Set PWM1 and PWM2 as output
	DDRD |= (1 << PD4) | (1 << PD5);
	
	// Set non inverting PWM mode for PWM output A and B
	TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM10);
	
	// Set Mode to 8-Bit fast PWM (WGM10)
	TCCR1A |= (1 << WGM10);
	TCCR1B = (1 << WGM12);
	
	// Set prescaler for the timer
	TCCR1B |= prescaler & ((1 << CS12) | (1 << CS11) | (1 << CS10));
}

void PWM_setOutput1(uint8_t value){
	OCR1A = value;
}

void PWM_setOutput2(uint8_t value){
	OCR1B = value;
}


