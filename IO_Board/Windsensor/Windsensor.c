/*
 * Windsensor.c
 *
 * Created: 19.04.2024 13:00:59
 *  Author: josef
 */ 

#include "Windsensor.h"
#include <string.h>

static uint16_t tickStart;	// Start tick of current measurement
static uint8_t count;		// Count of current measurement
static uint8_t idx;			// Index for lastMeasurements Array, latest measurement

static uint8_t lastMeasurements[NUM_MEASUREMENTS_STORED];	// Last NUM_MEASUREMENTS_STORED measuerements

void wind_init(){
	tickStart = systick_getTick();
	idx = 0; count = 0;
	memset(lastMeasurements, 0, sizeof(lastMeasurements));
}

uint8_t wind_getSpeed(){
	return lastMeasurements[NUM_MEASUREMENTS_STORED];
}

uint8_t wind_getMaxSpeed(){
	uint8_t maxSpeed = 0;
	for (uint8_t i = 0; i < NUM_MEASUREMENTS_STORED; i++){
		if (lastMeasurements[i] > maxSpeed){
			maxSpeed = lastMeasurements[i];
		}
	}
	return maxSpeed;
}

void wind_update(){
	static uint8_t lastBitValue = 0;
	uint8_t bitValue = di_get() & (1 << 6) ? 1 : 0;
	
	if (bitValue != lastBitValue){
		count++;
	}
	
	lastBitValue = bitValue;
	
	// Measurement done?
	if (systick_deltat(systick_getTick(), tickStart) > 40000){	// 4s elapsed
		// Advance index
		if (++idx >= NUM_MEASUREMENTS_STORED){
			idx = 0;
		}
		
		// Store measurement
		lastMeasurements[idx] = count;
		count = 0;
		
		// Start next measurement
		tickStart = systick_getTick();
	}
	
}