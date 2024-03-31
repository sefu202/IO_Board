/*
 * RS485.c
 *
 * Created: 31.03.2024 20:20:41
 *  Author: josef
 */ 

#include "RS485.h"
#include "SysTick.h"
#include <util/delay.h>
#include <avr/interrupt.h>

static usart_handle_t usart0;
static uint8_t usart0Buffer[MAX_FRAME_SIZE*4];

static uint16_t lastRxTick;
static uint16_t rxDisableTick;

static void rs485_enableRx(){
	UCSR0B |= (1 << RXEN0);
}

static void rs485_disableRx(){
	rxDisableTick = systick_getTick();
	UCSR0B &= ~(1 << RXEN0);
}

static void rs485_setTx(){
	rs485_disableRx();
	PORTD |= (1<<PD2)|(1<<PD3);
	_delay_us(5);
}

static void rs485_setRx(){
	rs485_enableRx();
	PORTD &= ~((1<<PD2)|(1<<PD3));
}



void rs485_init(){
	// Initialize Pins
	DDRD |= (1<<PD2)|(1<<PD3);
	
	// initialize usart
	usart_init_t usart0Init;
	usart0Init.baseReg = USART_BASE_USART0;
	usart0Init.baud_reg_flags = 25;	// 19200 Baud
	usart0Init.character_size = 8;
	usart0Init.if_en = USART_IF_EN_RX | USART_IF_EN_TX;
	usart0Init.parity_mode = USART_PARITY_DISABLE;
	usart0Init.stop_bits = 1;
	usart0Init.usart_mode = USART_MODE_ASYNC;
	
	usart0 = usart_init(&usart0Init, usart0Buffer, sizeof(usart0Buffer));
	
	// Put in rx mode first
	rs485_setRx();
}

void rs485_update(){
	uint8_t rxFifoLevel = usart_getRxFifoLevel(&usart0);
	
	// Reenable Rx if it was disabled for more than 20ms in any case
	if ((systick_getTick() - rxDisableTick) > 200){
		rs485_setRx();
	}
	
	if ((systick_getTick() - lastRxTick) > 30 && rxFifoLevel) {	// 3ms since last symbol received
		
		if (rxFifoLevel > MAX_FRAME_SIZE){	// clear bogus content
			fifo_flush(&(usart0.rxFifo));
		}
		else{
			
			rs485_setTx();
			uint8_t rxFrame[MAX_FRAME_SIZE];
			uint8_t txFrame[MAX_FRAME_SIZE];
			
			for (uint8_t i = 0; i < rxFifoLevel; i++){
				rxFrame[i] = usart_getc(&usart0);
			}
			
			uint8_t txFrameSize = rs485_processFrame(rxFrame, rxFifoLevel, txFrame);
			
			for (uint8_t j = 0; j < txFrameSize; j++){
				usart_putc(&usart0, txFrame[j]);
			}
		}
		
	}
}

uint8_t rs485_processFrame(uint8_t *rxFrame, uint8_t rxFrameSize, uint8_t *txFrame){
	// Check correct address
	if (rxFrame[0] != RX_ADDRESS){
		return 0;
	}
	
	// Check correct frame size
	if (rxFrameSize != rxFrame[1]){
		return 0;
	}
	
	// Check crc TODO
	
	
	// Process frame
	if (rxFrameSize == 13){
		
		processImage_rxFrame(rxFrame+2);	// Process Payload data of rx frame
		
		memcpy(txFrame, rxFrame, rxFrameSize);
	}
	
	return rxFrameSize;
	
}


ISR (USART0_UDRE_vect){
	usart_udre_irq(&usart0);
}

ISR (USART0_RX_vect){
	usart_rx_irq(&usart0);
	lastRxTick = systick_getTick();
}

ISR (USART0_TX_vect){
	//usart_tx_irq(&usart0);
	rs485_setRx();
}