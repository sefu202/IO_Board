/*
 * IO_Board.c
 *
 * Created: 08.02.2024 15:38:28
 * Author : josef
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "RS485.h"
#include "DO.h"
#include "DI.h"
#include "PWM.h"
#include "Step1.h"
#include "SysTick.h"

int main(void)
{	
	// Init
	DDRA = 0;
	DDRB = 0;
	DDRD = 0;
	DDRD = 0;
	
	do_init();
	di_init();
	step1_init();
	PWM_init(PWM_PRESC_64);
	
	rs485_init();
	
	systick_init();
	
	step1_setENA(0);
	
	// Global enable interrupt
	sei();
	
    while (1) 
    {
		rs485_update();
	}
}

