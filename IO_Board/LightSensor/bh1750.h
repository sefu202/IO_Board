/*
 * bh1750.h
 *
 * Created: 06.04.2024 19:20:10
 *  Author: josef
 */ 


#ifndef BH1750_H_
#define BH1750_H_

#include <Fleury_i2cmaster.h>

/**
 * @brief Initialises BH1750 sensor with high resolution contiuous mode
 */
void bh1750_init();

/** 
 * @brief Returns connection status of sensor
 * @retval 1 sensor is connected
 * @retval 0 sensor is not connected
 */
uint8_t bh1750_isConnected();

/**
 * @brief reads measurement values over i2c
 * @return light intensity in 1.2 * lux
 */
uint16_t bh1750_getLux();




#endif /* BH1750_H_ */