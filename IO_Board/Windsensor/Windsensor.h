/**
 * @file Windsensor.h
 *
 * Created: 19.04.2024 13:00:50
 * @author Josef Aschwanden
 */ 


#ifndef WINDSENSOR_H_
#define WINDSENSOR_H_

#define NUM_MEASUREMENTS_STORED	128 /// Number of measurements stored in the ring buffer

#include <DI.h>
#include <SysTick.h>

/**
 * @brief Initializes Wind measurement
 */
void wind_init();

/**
 * @brief returns last speed (frequency) measurement
 * @note to convert to windspeed, use the following formula: v_wind = f_wind * 10/3 where f_wind is 1/4 * count => v_wind = count * 5/6
 * @return last measurement count (in 4*f [Hz])
 */
uint8_t wind_getSpeed();

/**
 * @brief returns max stored speed measurement
 * @return max measurement (in 4*f [Hz])
 */
uint8_t wind_getMaxSpeed();

/**
 * @brief Periodic update of wind speed measurement, call at a rate of 1kHz
 */
void wind_update();


#endif /* WINDSENSOR_H_ */