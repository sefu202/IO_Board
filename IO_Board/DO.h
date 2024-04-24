/**
 * @file DO.h
 *
 * Created: 22.03.2024 22:58:10
 * @author Josef Aschwanden
 */ 


#ifndef DO_H_
#define DO_H_

#include <stdint.h>

/**
 * @brief Initializes digital outputs
 * 
 */
void do_init();

/**
 * @brief Writes value to digital outputs
 * @note Writes all digital outputs at once 
 */
void do_set(uint8_t);


#endif /* DO_H_ */