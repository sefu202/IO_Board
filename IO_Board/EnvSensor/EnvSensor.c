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

// Static function declaration
static uint8_t envSensor_initSensor(struct bme280_dev* handle, uint8_t addr);


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


struct bme280_dev bme280_1, bme280_2;

static uint8_t isWorking1, isWorking2;

static uint8_t envSensor_initSensor(struct bme280_dev* handle, uint8_t addr){
	uint8_t isWorking = 1;
		
	int8_t result;
		
	memset(handle, 0, sizeof(*handle));
	handle->intf_ptr = (void *)((addr) << 1);
	handle->read = bme_i2c_read;
	handle->write = bme_i2c_write;
	handle->delay_us = bme_delay_us;
	handle->intf = BME280_I2C_INTF;
		
	result = bme280_init(handle);
		
	if (result){
		isWorking = 0;
		return isWorking;
	}
		

	handle->settings.osr_h = BME280_OVERSAMPLING_1X;
	handle->settings.osr_p = BME280_OVERSAMPLING_16X;
	handle->settings.osr_t = BME280_OVERSAMPLING_2X;
	handle->settings.filter = BME280_FILTER_COEFF_16;
	uint8_t settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;
	result = bme280_set_sensor_settings(settings_sel, handle);
		
	if (result){
		isWorking = 0;
		return isWorking;
	}
		
	result = bme280_set_sensor_mode(BME280_NORMAL_MODE, handle);
		
	if (result){
		isWorking = 0;
		return isWorking;
	}
	return isWorking;
}

void envSensor_init(){
	isWorking1 = envSensor_initSensor(&bme280_1, BME280_I2C_ADDR_PRIM);
	isWorking2 = envSensor_initSensor(&bme280_2, BME280_I2C_ADDR_SEC);
}

struct bme280_data envSensor1(void)
{
	struct bme280_data sensorData;
	uint8_t result = bme280_get_sensor_data(BME280_ALL, &sensorData, &bme280_1);
	
	if (result){
		memset(&sensorData, 0, sizeof(sensorData));
		isWorking1 = 0;
	}
	else{
		isWorking1 = 1;
	}
	
	return sensorData;
}

struct bme280_data envSensor2(void)
{
	struct bme280_data sensorData;
	uint8_t result = bme280_get_sensor_data(BME280_ALL, &sensorData, &bme280_2);
	
	if (result){
		memset(&sensorData, 0, sizeof(sensorData));
		isWorking2 = 0;
	}
	else{
		isWorking2 = 1;
	}
	
	return sensorData;
}
