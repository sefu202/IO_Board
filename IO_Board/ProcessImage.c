/*
 * ProcessImage.c
 *
 * Created: 31.03.2024 23:59:20
 *  Author: josef
 */ 

#include "ProcessImage.h"
#include "DI.h"
#include "DO.h"
#include "Step1.h"
#include "PWM.h"
#include <util/delay.h>
#include <AI.h>
#include <assert.h>
#include <EnvSensor/EnvSensor.h>
#include <LightSensor/bh1750.h>
#include <string.h>

static processImage_t ProcessImage;


void processImage_rxFrame(uint8_t *pFrame){
	ProcessImage.rx.DO				= pFrame[0];
	ProcessImage.rx.motor[0].flags	= pFrame[1];
	ProcessImage.rx.motor[0].speed	= pFrame[2];
	ProcessImage.rx.motor[1].flags	= pFrame[3];
	ProcessImage.rx.motor[1].speed	= pFrame[4];
	ProcessImage.rx.motor[2].flags	= pFrame[5];
	ProcessImage.rx.motor[2].speed	= pFrame[6];
	ProcessImage.rx.motor[3].flags	= pFrame[7];
	ProcessImage.rx.motor[3].speed	= pFrame[8];
	
	processImage_writeOutputs();
}

void processImage_writeOutputs(){
	// Write DO
	do_set(ProcessImage.rx.DO);
	
	// Write DC Motors
	PWM_setOutput1(ProcessImage.rx.motor[0].speed);
	PWM_setOutput2(ProcessImage.rx.motor[1].speed);
	
	// Write Stepper Motors
	// Enable, Direction, Speed
	step1(ProcessImage.rx.motor[2].flags & (1 << 0), ProcessImage.rx.motor[2].flags & (1 << 1), ProcessImage.rx.motor[2].speed);
}

uint8_t processImage_txFrame(uint8_t *pFrame, uint8_t maxLen){
	
	processImage_readInputs();
	
	uint8_t i = 0;
	
	// Digital inputs
	pFrame[i++] = ProcessImage.tx.DI;
	
	// Analog inputs
	pFrame[i++] = (uint8_t)(ProcessImage.tx.AI[0] >> 8);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.AI[0] >> 0);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.AI[1] >> 8);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.AI[1] >> 0);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.AI[2] >> 8);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.AI[2] >> 0);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.AI[3] >> 8);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.AI[3] >> 0);
	
	// Environment sensor 1
	pFrame[i++] = (uint8_t)(ProcessImage.tx.env_sensor[0].temp >> 24);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.env_sensor[0].temp >> 16);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.env_sensor[0].temp >> 8);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.env_sensor[0].temp >> 0);	
	pFrame[i++] = (uint8_t)(ProcessImage.tx.env_sensor[0].hum >> 24);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.env_sensor[0].hum >> 16);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.env_sensor[0].hum >> 8);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.env_sensor[0].hum >> 0);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.env_sensor[0].press >> 24);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.env_sensor[0].press >> 16);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.env_sensor[0].press >> 8);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.env_sensor[0].press >> 0);
	
	// Environment sensor 2
	pFrame[i++] = (uint8_t)(ProcessImage.tx.env_sensor[1].temp >> 24);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.env_sensor[1].temp >> 16);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.env_sensor[1].temp >> 8);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.env_sensor[1].temp >> 0);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.env_sensor[1].hum >> 24);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.env_sensor[1].hum >> 16);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.env_sensor[1].hum >> 8);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.env_sensor[1].hum >> 0);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.env_sensor[1].press >> 24);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.env_sensor[1].press >> 16);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.env_sensor[1].press >> 8);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.env_sensor[1].press >> 0);
	
	// Light sensor
	pFrame[i++] = (uint8_t)(ProcessImage.tx.lightSensor >> 8);
	pFrame[i++] = (uint8_t)(ProcessImage.tx.lightSensor >> 0);
	
	// Windsensor
	pFrame[i++] = ProcessImage.tx.maxWindspeed;
	
	// errors
	pFrame[i++] = ProcessImage.tx.errors;
	
	// Check allocated storage while in debug mode
	assert(maxLen >= i);
	assert (i <= maxLen);
	
	return i;	// Size of payload
}

void processImage_readInputs(){
	// Read Digital Inputs
	ProcessImage.tx.DI = di_get();
	_delay_us(100);
	ProcessImage.tx.DI &= di_get();
	
	// Read analog inputs
	ProcessImage.tx.AI[0] = ai_getValue(0);
	ProcessImage.tx.AI[1] = ai_getValue(1);
	ProcessImage.tx.AI[2] = ai_getValue(2);
	ProcessImage.tx.AI[3] = ai_getValue(3);
	
	// Read Environment Sensors
	struct bme280_data envSensor1_data;
	memset(&envSensor1_data, 0, sizeof(envSensor1_data));
	envSensor1_data = envSensor1();
	ProcessImage.tx.env_sensor[0].hum	= envSensor1_data.humidity;
	ProcessImage.tx.env_sensor[0].press = envSensor1_data.pressure;
	ProcessImage.tx.env_sensor[0].temp	= envSensor1_data.temperature;
	
	struct bme280_data envSensor2_data;
	memset(&envSensor2_data, 0, sizeof(envSensor2_data));
	envSensor2_data = envSensor2();
	ProcessImage.tx.env_sensor[1].hum	= envSensor2_data.humidity;
	ProcessImage.tx.env_sensor[1].press = envSensor2_data.pressure;
	ProcessImage.tx.env_sensor[1].temp	= envSensor2_data.temperature;
	
	// Read light sensor
	ProcessImage.tx.lightSensor = bh1750_getLux();
	
	// Read wind sensor
	ProcessImage.tx.maxWindspeed = wind_getMaxSpeed();
	
	// Error flags
	ProcessImage.tx.errors = 0;
	
}