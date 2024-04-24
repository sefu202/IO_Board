/**
 * @file bh1750.c
 *
 * Created: 06.04.2024 19:20:22
 * @author Josef Aschwanden
 */ 

#include "bh1750.h"

#define BH1750_ADDR 0x23  /// I2C address of BH1750 sensor

static uint8_t isConnected;	/// Connection status of light sensor

void bh1750_init(){
	isConnected = !i2c_start(BH1750_ADDR<<1);
	if (isConnected){
		isConnected = !i2c_write(0x10); // Power on, continuous high resolution mode
	}
	i2c_stop();
}

uint8_t bh1750_isConnected(){
	return isConnected;
}

uint16_t bh1750_getLux(){
	uint16_t lux = 0;
	if (isConnected){
		isConnected = !i2c_start((BH1750_ADDR << 1) | 1);	// Start in read mode
		if (isConnected){
			lux = ((uint16_t)i2c_readAck() << 8); // Read MSB
			lux |= i2c_readNak(); // Read LSB
		}
		i2c_stop();
	}
	

	return lux;
}