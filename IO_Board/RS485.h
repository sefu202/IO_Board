/*
 * RS485.h
 *
 * Created: 31.03.2024 20:19:43
 *  Author: josef
 */ 


#ifndef RS485_H_
#define RS485_H_

#include "usart.h"
#include "ProcessImage.h"

#define RX_ADDRESS		1
#define MAX_FRAME_SIZE	32	// Max frame size in bytes

/**
 * @brief initializes rs485 bus communication
 */
void rs485_init();


void rs485_update();

uint8_t rs485_processFrame(uint8_t *rxFrame, uint8_t rxFrameSize, uint8_t *txFrame);


#endif /* RS485_H_ */