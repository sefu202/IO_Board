/*
 * EnvSensor.h
 *
 * Created: 01.04.2024 18:57:15
 *  Author: josef
 */ 


#ifndef ENVSENSOR_H_
#define ENVSENSOR_H_

#include "bme280.h"


void envSensor_init();

struct bme280_data envSensor1(void);
struct bme280_data envSensor2(void);



#endif /* ENVSENSOR_H_ */