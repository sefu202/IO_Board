/*
 * PWM.h
 *
 * Created: 19.03.2024 20:54:37
 *  Author: josef
 */ 


#ifndef PWM_H_
#define PWM_H_

#include <avr/io.h>
#include <stdint.h>

// Macros for Prescaler
#define PWM_PRESC_OFF	0	// Disable PWM
#define PWM_PRESC_1		1	// Divide Clock by 1
#define PWM_PRESC_8		2	// Divide Clock by 8
#define PWM_PRESC_64	3	// Divide Clock by 64
#define PWM_PRESC_256	4	// Divide Clock by 256
#define PWM_PRESC_1024	5	// Divide Clock by 1024

/**
 * @brief initializes Timer1 for PWM operation
 * @param prescaler	prescaler setting, use PWM_PRESC_xxx macros
 */
void PWM_init(uint8_t prescaler);

void PWM_setOutput1(uint8_t value);
void PWM_setOutput2(uint8_t value);


#endif /* PWM_H_ */