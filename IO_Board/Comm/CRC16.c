/*
 * CRC16.c
 *
 * Created: 01.04.2024 18:42:26
 *  Author: josef
 */ 

#include "CRC16.h"

uint16_t crc16(uint8_t *data, uint16_t length) {
	uint16_t crc = 0xFFFF; // initial value
	uint16_t polynomial = 0xA001; // polynomial used in Modbus

	for (uint16_t i = 0; i < length; ++i) {
		crc ^= data[i]; // XOR next byte into LSB of crc

		for (int j = 0; j < 8; ++j) {
			if (crc & 0x0001) { // LSB is 1
				crc >>= 1;
				crc ^= polynomial;
				} else { // LSB is 0
				crc >>= 1;
			}
		}
	}

	return crc;
}