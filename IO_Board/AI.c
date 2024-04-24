/**
 * @file AI.c
 *
 * Created: 02.04.2024 16:19:33
 * @author Josef Aschwanden
 */ 

#include "AI.h"
#include "adc.h"
#include <avr/interrupt.h>

static adc_t adc;

void ai_init(){
	adc_init_t adcSettings;
	adcSettings.baseAddress = &ADCL;
	adcSettings.channelEnFlags = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3);
	adcSettings.prescaler = ADC_DIV_128;
	adcSettings.vref = ADC_VREF_256;
	adc = adc_init(&adcSettings);
	adc_startConversion(&adc);
}

uint16_t ai_getValue(uint8_t aiNumber){
	return adc_getResult(&adc, aiNumber);
}


ISR(ADC_vect){
	adc_isr(&adc);
}