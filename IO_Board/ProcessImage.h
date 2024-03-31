/*
 * ProcessImage.h
 *
 * Created: 31.03.2024 23:50:41
 *  Author: josef
 */ 


#ifndef PROCESSIMAGE_H_
#define PROCESSIMAGE_H_

#include <stdint.h>

typedef struct {
	struct rx_t{
		uint8_t DO;
		struct motor_t{
			uint8_t flags;	// Bit 0: Enable, Bit 1: Direction
			uint8_t speed;
		}motor[4];
	}rx;
	
	struct tx_t{
		uint8_t DI;
		uint16_t AI[4];
		struct env_sensor_t{
			uint16_t temp;
			uint16_t hum;
			uint16_t press;
		}env_sensor[2]; 
		uint16_t lightSensor;
	}tx;
	
}processImage_t;

void processImage_rxFrame(uint8_t *pFrame);

void processImage_writeOutputs();

void processImage_readInputs();

#endif /* PROCESSIMAGE_H_ */