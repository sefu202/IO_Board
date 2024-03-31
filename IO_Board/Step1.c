/**
 * @file Step1.c
 *
 * Created: 24.03.2024 18:32:02
 *  Author: josef
 */ 

#include "Step1.h"

static uint8_t m_initialized;
static uint8_t m_pulseState;
static uint8_t m_speed;

void step1_init(){
	DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2);	// Set Step 1 pins as output
	step1_setENA(0);								// Disable Motor at init
	m_pulseState = 0;
	m_speed = 0;
	
	// initialization done
	m_initialized = 1;
}


void step1(uint8_t enable, uint8_t dir, uint8_t speed){
	m_speed = speed;
	
	step1_setENA(enable);
	step1_setDIR(dir);
}

void step1_setENA(uint8_t value){
	if (!value){
		PORTB &= ~(1 << PB0);
	}
	else {
		PORTB |= (1 << PB0);
	}
}


void step1_setDIR(uint8_t value){
	if (value){
		PORTB &= ~(1 << PB1);
	}
	else {
		PORTB |= (1 << PB1);
	}
}


void step1_setPUL(uint8_t value){
	if (value){
		PORTB &= ~(1 << PB2);
	}
	else {
		PORTB |= (1 << PB2);
	}	
}


void step1_update(){
	if (m_initialized && m_speed) {
		m_pulseState = !m_pulseState;	// Toggle pulse signal
	}
	else{
		m_pulseState = 0;
	}
	
	step1_setPUL(m_pulseState);
}