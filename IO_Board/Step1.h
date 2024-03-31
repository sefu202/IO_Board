/**
 * @file Step1.h
 *
 * Created: 24.03.2024 18:27:33
 *  Author: josef
 */ 


#ifndef STEP1_H_
#define STEP1_H_

#include <stdint.h>
#include <avr/io.h>

/**
 * @brief initializes stepper interface
 */
void step1_init();


/**
 * @brief Control of step1 interface
 * @param enable enable signal: 1 = Enable Motor (Turn on)
 * @param dir direction of motor
 * @param speed 0 = no turning, else = turning
 */
void step1(uint8_t enable, uint8_t dir, uint8_t speed);

/**
 * @brief Sets enable of the motor
 * @param value 0 = disable, 1 = enable
 */
void step1_setENA(uint8_t value);


/**
 * @brief sets direction of the motor
 * @param value 0 = fwd, 1 = rwd
 */
void step1_setDIR(uint8_t value);

/**
 * @brief set value of pulse output
 * @note Steps are made on positive edge of Pulse
 * @param value value to set for pulse
 */
void step1_setPUL(uint8_t value);

/**
 * @brief Periodic status update, call at a rate of 1kHz
 */
void step1_update();


#endif /* STEP1_H_ */