/*
 * SysTick.h
 *
 * Created: 19.03.2024 20:55:22
 *  Author: josef
 */ 


#ifndef SYSTICK_H_
#define SYSTICK_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

/**
 * @brief Initializes Timer0 for use as SysTick timer using CTC mode, f = 10kHz
 */
void systick_init();

/**
 * @brief Returns current SysTick status
 */
uint16_t systick_getTick();

#endif /* SYSTICK_H_ */