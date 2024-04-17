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


/**
 * @brief calculates diffrence in time between 2 timestamps
 * @param later the later timestamp
 * @param earlier the earlier timestamp
 * @return diffrence in 100us steps
 */
uint16_t systick_deltat(uint16_t later, uint16_t earlier);

#endif /* SYSTICK_H_ */