/**
 * @file DO.c
 *
 * Created: 22.03.2024 22:58:44
 * @author Josef Aschwanden
 */ 

#include <avr/io.h>
#include "DO.h"

void do_init(){
	// Set DO pins as output:
	DDRD |= (1 << PORTD7);	// DO4
	DDRB |= (1 << PORTB6);	// DO3
	DDRB |= (1 << PORTB7);	// DO2	
	DDRD |= (1 << PORTD6);	// DO1
}

void do_set(uint8_t value){
	// Set or clear DOs
	
	// DO4
	if (value & (1<<3)){
		PORTD |= (1 << PORTD7);
	}
	else{
		PORTD &= ~(1 << PORTD7);
	}
	
	// DO3
	if (value & (1<<2)){
		PORTB |= (1 << PORTB6);
	}
	else{
		PORTB &= ~(1 << PORTB6);
	}
	
	// DO2
	if (value & (1<<1)){
		PORTB |= (1 << PORTB7);
	}
	else{
		PORTB &= ~(1 << PORTB7);
	}
	
	// DO1
	if (value & (1<<0)){
		PORTD |= (1 << PORTD6);
	}
	else{
		PORTD &= ~(1 << PORTD6);
	}
}