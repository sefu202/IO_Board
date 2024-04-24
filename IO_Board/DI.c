/**
 * @file DI.c
 *
 * Created: 28.03.2024 18:10:08
 * @author Josef Aschwanden
 */ 

#include "DI.h"
#include <avr/io.h>
#include <util/delay.h>

/**
 * @brief selects multiplexer input
 * @param select select input 0 = low, 1 = high
 */
static void di_setMux(uint8_t select){
	if (select){
		PORTA |= (1 << PA6);
	}
	else {
		PORTA &= ~(1 << PA6);
	}
}

void di_init(){
	DDRA |= (1 << PA6);
}

uint8_t di_get(){
	uint8_t ret = 0xff;
	di_setMux(0);
	_delay_us(10);	// one us delay to allow the multiplexer to switch (propagation delay is max 220ns)
	
	// DI7 Set
	if (PINA & (1 << PA4)){
		ret &= ~(1 << 6);
	}
	// DI5 Set
	if (PINA & (1 << PA5)){
		ret &= ~(1 << 4);
	}
	// DI4 Set
	if (PINA & (1 << PA7)){
		ret &= ~(1 << 3);
	}
	// DI2 Set
	if (PINC & (1 << PC7)){
		ret &= ~(1 << 1);
	}
	
	di_setMux(1);
	_delay_us(10);	// one us delay to allow the multiplexer to switch (propagation delay is max 220ns)
		
	// DI8 Set
	if (PINA & (1 << PA4)){
		ret &= ~(1 << 7);
	}
	// DI6 Set
	if (PINA & (1 << PA5)){
		ret &= ~(1 << 5);
	}
	// DI3 Set
	if (PINA & (1 << PA7)){
		ret &= ~(1 << 2);
	}
	// DI1 Set
	if (PINC & (1 << PC7)){
		ret &= ~(1 << 0);
	}
	
	return ret;
}