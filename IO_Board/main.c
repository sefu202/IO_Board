/**
 * @file IO_Board.c
 *
 * Created: 08.02.2024 15:38:28
 * @author Josef Aschwanden
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <Fleury_i2cmaster.h>
#include <EnvSensor/EnvSensor.h>
#include <LightSensor/bh1750.h>
#include <AI.h>
#include "Comm/RS485.h"
#include "DO.h"
#include "DI.h"
#include "PWM.h"
#include "Step1.h"
#include "SysTick.h"
#include "Windsensor/Windsensor.h"

/**
 * @brief Main Routine for IO-Print
 * 
 * @return int never
 */
int main(void)
{	
	// Oscillator Calibration
	OSCCAL = 0x93;
	
	_delay_ms(1000);
	
	// Init
	DDRA = 0;
	DDRB = 0;
	DDRD = 0;
	DDRD = 0;
	
	systick_init();
	
	do_init();
	di_init();
	ai_init();
	
	step1_init();
	PWM_init(PWM_PRESC_64);
	
	wind_init();
	
	rs485_init();
	
	// I2C init
	i2c_init();
	envSensor_init();
	bh1750_init();
	
	
	step1_setENA(0);
	
	// Global enable interrupt
	sei();
	
    while (1) 
    {
		rs485_update();
	}
}

