/**
 * @file adc.c
 *
 * @date 27.08.2021
 * @author asj
 */

// Includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include "adc.h"

// Non export Function Prototypes
/**
 * @brief gets the next enabled channel
 * @param[inout] pAdc	ADC handle
 * @note pAdc->status gets set to a non zero value on error
 * @return returns next channel as an index 0..15
 */
static uint8_t getNextChannel(adc_t *pAdc);


/**
 * @brief sets the adc mux channel
 * @brief[in] pAdc		Adc handle
 * @param[in] channel 	channel index
 */
static void setChannel(adc_t *pAdc, uint8_t channel);



// Exported Functions

adc_t adc_init(adc_init_t *pInit){
    adc_t adc;		// create a new adc handle
    adc.status = 1;	// Init to fail
	adc.currentChannel = ADC_CHANNEL_LAST;	// Start with the lowest Channel (getNextChannel will overflow)

	// Assertions
	assert(pInit != NULL);
	//assert(pInit->pResultArray != NULL);
	assert(pInit->vref <= ADC_VREF_256);
	assert(pInit->prescaler <= ADC_DIV_128);

	// Parameter Checking
    if (pInit->baseAddress != NULL){
		// Initialize ADC handle
		adc.status = 0;	// Success
		adc.enabledChannels = pInit->channelEnFlags;
        adc.regs = (adc_register_t *)pInit->baseAddress;

		// Set result array to 0 for defined state at init
		for (uint8_t i = 0; i < ADC_CHANNEL_NUM; i++){
			adc.resultArray[i] = 0;
		}

		// vref bits | enable left adjusted result
		adc.regs->admux = ((pInit->vref & 0x3)  << 6);

		// Disable digital inputs that are used by the adc
		adc.regs->didr0 = pInit->channelEnFlags & 0x8;
		adc.regs->didr2 = pInit->channelEnFlags >> 8;

		// Set prescaler
		adc.regs->adcsra = pInit->prescaler & 0x7;

		// Try setting a channel
		uint8_t channel = getNextChannel(&adc);
		if (adc.status == 0){
			setChannel(&adc, channel);
			adc.regs->adcsra |= (1 << ADEN) | (1 << ADIE);	// Enable ADC | interrupt enable
		}
    }
    return adc;
}

void adc_startConversion(adc_t *pAdc){
	pAdc->regs->adcsra |= (1 << ADSC) | (1 << ADEN) | (1 << ADIE);
}

void adc_stop(adc_t *pAdc){
	pAdc->regs->adcsra &= ~(1 << ADEN);
}

void adc_isr(adc_t *pAdc){
	pAdc->resultArray[pAdc->currentChannel] = ADC;	// Read ADC Resul register

	// Switch to the next channel
	// Because the adc is running in single conversion mode, the conversion starts after calling startConversion,
	// and it is save to switch mux channels
	uint8_t nextChannel = getNextChannel(pAdc);
	if (pAdc->status == 0){	// success
		setChannel(pAdc, nextChannel);
		adc_startConversion(pAdc);
	}
	else{
		// Disable adc on error
		adc_stop(pAdc);
	}
}

uint16_t adc_getResult(adc_t *pAdc, uint8_t channel){
	uint16_t ret = 0;
	if (channel < ADC_CHANNEL_NUM){
		uint8_t sreg = SREG;	// store SREG content
		cli();					// Disable interrupts
		ret = pAdc->resultArray[channel];
		SREG = sreg;			// restore SREG content
	}
	return ret;
}


// *** Non export functions ***

static uint8_t getNextChannel(adc_t *pAdc){
	uint8_t bit = pAdc->currentChannel;
	pAdc->status = 2;

	// scan all bits for a set one, starting with the next possible one
	for (uint8_t i = 0; i < (ADC_CHANNEL_LAST + 1); i++) {
		bit = ((bit + 1) & ADC_CHANNEL_LAST);
		if ((1 << bit) & (pAdc->enabledChannels)){
			pAdc->status = 0;
			break;
		}
	}

	return bit;
}


static void setChannel(adc_t *pAdc, uint8_t channel){
	pAdc->regs->admux &= ~(0x1f);	// clear all mux flags
	pAdc->regs->admux |= channel & 0x7;	// set lowest 3 bits
	pAdc->currentChannel = channel;	// update current channel
}
