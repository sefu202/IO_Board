/*
 * EnvSensor.c
 *
 * Created: 01.04.2024 18:57:23
 *  Author: josef
 */ 


#include "Fleury_i2cmaster.h"
#include "EnvSensor.h"
#include <util/delay.h>
#include <string.h>


typedef BME280_INTF_RET_TYPE (*bme280_read_fptr_t)(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);

static BME280_INTF_RET_TYPE bme_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr){
	return i2c_readBlock((uint8_t)((uint16_t)intf_ptr), reg_addr, reg_data, len);
}

static BME280_INTF_RET_TYPE bme_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr){
	return i2c_writeBlock((uint8_t)((uint16_t)intf_ptr), reg_addr, (uint8_t *)reg_data, len);
}

static void bme_delay_us(uint32_t period, void *intf_ptr){
	while (period--) _delay_us(1);
}


struct bme280_dev bme280_1, bm280_2;

static uint8_t isWorking;

void envSensor_init(){
	isWorking = 1;
	
	int8_t result;
	
	memset(&bme280_1, 0, sizeof(bme280_1));
	bme280_1.intf_ptr = (void *)((BME280_I2C_ADDR_PRIM) << 1);
	bme280_1.read = bme_i2c_read;
	bme280_1.write = bme_i2c_write;
	bme280_1.delay_us = bme_delay_us;
	bme280_1.intf = BME280_I2C_INTF;
	
	result = bme280_init(&bme280_1);
	
	if (result){
		isWorking = 0;
		return;
	}
	

	bme280_1.settings.osr_h = BME280_OVERSAMPLING_1X;
	bme280_1.settings.osr_p = BME280_OVERSAMPLING_16X;
	bme280_1.settings.osr_t = BME280_OVERSAMPLING_2X;
	bme280_1.settings.filter = BME280_FILTER_COEFF_16;
	uint8_t settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;
	result = bme280_set_sensor_settings(settings_sel, &bme280_1);
	
	if (result){
		isWorking = 0;
		return;
	}
	
	result = bme280_set_sensor_mode(BME280_NORMAL_MODE, &bme280_1);
	
	if (result){
		isWorking = 0;
		return;
	}
}

struct bme280_data envSensor1(void)
{
	struct bme280_data sensorData;
	uint8_t result = bme280_get_sensor_data(BME280_ALL, &sensorData, &bme280_1);
	
	if (result){
		isWorking = 0;
	}
	
	return sensorData;
}
