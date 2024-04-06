/*
 * CRC16.h
 *
 * Created: 01.04.2024 18:40:16
 *  Author: josef
 */ 


#ifndef CRC16_H_
#define CRC16_H_

#include <stdint.h>

/**
 * @brief calculates CRC-16
 * @param data pointer to data
 * @param length length of data
 */
uint16_t crc16(uint8_t *data, uint16_t length);



#endif /* CRC16_H_ */