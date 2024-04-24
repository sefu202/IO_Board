/**
 * @file EnvSensor.h
 *
 * Created: 01.04.2024 18:57:15
 *  @author Josef Aschwanden
 */ 


#ifndef ENVSENSOR_H_
#define ENVSENSOR_H_

#include "bme280.h"

/**
 * @brief Initializes environment sensor 1 and 2
 */
void envSensor_init();

/**
 * @brief Reads data from environment sensor 1
 * @return struct bme280_data environment sensor data
 */
struct bme280_data envSensor1(void);

/**
 * @brief Reads data from environment sensor 2
 * @return struct bme280_data environment sensor data
 */
struct bme280_data envSensor2(void);



#endif /* ENVSENSOR_H_ */