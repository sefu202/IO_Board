/**
 * @file DI.h
 *
 * Created: 28.03.2024 18:05:29
 * @author Josef Aschwanden
 */ 


#ifndef DI_H_
#define DI_H_

#include <stdint.h>

/**
 * @brief initializes digital inputs
 */
void di_init();

/**
 * @brief reads digital input
 * Switches multiplexer to read all inputs at once
 */
uint8_t di_get();


#endif /* DI_H_ */