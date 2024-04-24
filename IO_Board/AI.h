/**
 * @file AI.h
 *
 * Created: 02.04.2024 16:18:45
 * @author Josef Aschwanden
 */ 


#ifndef AI_H_
#define AI_H_

#include <stdint.h>

/**
 * @brief Initializes analog inputs
 */
void ai_init();

/**
 * @brief Reads analog input
 * 
 * @param aiNumber number of analog input to read
 * @return uint16_t read value of the analog input
 */
uint16_t ai_getValue(uint8_t aiNumber);



#endif /* AI_H_ */