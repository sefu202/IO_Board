/**
 * @file usart.c
 * @author Josef Aschwanden (aschwandenjosef@gmail.com)
 * @brief Implementation of the AVR USART library
 * @version 1.0
 * @date 2022-01-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */
// *** INCLUDES **
#include "usart.h"
#include <assert.h>
#include <string.h>	// memset
#include <avr/io.h>
#include <avr/interrupt.h>

// Callback Function pointers
void (*usart_rxOverflow_callback)(usart_handle_t *pHandle) = NULL;
void (*usart_parityError_callback)(usart_handle_t *pHandle) = NULL;
void (*usart_frameError_callback)(usart_handle_t *phandle) = NULL;
void (*usart_transmit_complete_callback)(usart_handle_t *pHandle) = NULL;

// Static Functions
/**
 * @brief puts a character into the data register
 * @param pHandle uart handle
 * @param c character to transmit
 */
static void _put_into_udr(usart_handle_t *pHandle, usart_basetype c)
{
	#if USART_ALLOW_9BIT
	if (pHandle->character_size == 9)
	{
		if (c & (1 << 8))
		*(pHandle->baseReg + (&UCSR0B-&UDR0)) |= (1 << 0);
		else
		*(pHandle->baseReg + (&UCSR0B-&UDR0)) &= ~(1 << 0);
	}
	#endif
	*(pHandle->baseReg) = c;
}

/** 
 * @brief reads a character from the data register
 * @param pHandle uart handle
 * @return read character
 */
static usart_basetype _read_from_udr(usart_handle_t *pHandle)
{
	usart_basetype ret = 0;
	#if USART_ALLOW_9BIT
	if (pHandle->character_size == 9)
	{
		ret = (*(pHandle->baseReg + (&UCSR0B-&UDR0)) & (1 << 0)) ? (1 << 9) : 0;	// ninth bit set?
	}
	#endif
	ret |= *(pHandle->baseReg);
	return ret;
}

/**
 * @brief Checks usart_init parameters with assert
 * @param usartSettings pointer to usartSettings
 * @param pBuffer pointer to the buffer for the fifos
 * @param size_buffer size of the buffer (pBuffer)
 */
static void usart_init_assert(usart_init_t *usartSettings, void *pBuffer, uint8_t size_buffer){
	// *** Asserts ***
	assert(usartSettings != NULL);
	assert(usartSettings->if_en <= 3);
	assert(usartSettings->baseReg == &UDR0 || usartSettings->baseReg == &UDR1);
	assert(usartSettings->character_size >= 5 && usartSettings->character_size <= 9);
	assert(usartSettings->usart_mode <= 3 && usartSettings->usart_mode != 1);
	assert(usartSettings->stop_bits == 1 || usartSettings->stop_bits == 2);
#if USART_ALLOW_SYNCHRONOUS
	assert(usartSettings->sync_cpol <= 1);
#endif
	assert(size_buffer % 2 == 0);
#if (!USART_ALLOW_9BIT)
	assert(usartSettings->character_size <= 8);
	assert(size_buffer % 4 == 0);
#endif
	assert(pBuffer != NULL && size_buffer != 0);	
}


// Exported Functions
usart_handle_t usart_init(usart_init_t *usartSettings, void *pBuffer, uint8_t size_buffer)
{
	// Check parameters in debug mode
	usart_init_assert(usartSettings, pBuffer, size_buffer);
	
	// Create new Handle
	usart_handle_t newHandle;
	memset(&newHandle, 0, sizeof(newHandle));	// Set everything into a defined state
	newHandle.baseReg = usartSettings->baseReg;
	newHandle.status = 0;
	
	// Only basic checks
	if (usartSettings != NULL && pBuffer != NULL && size_buffer != 0)
	{
		if (fifo_init(&(newHandle.txFifo), pBuffer, size_buffer>>1, sizeof(usart_basetype)) == 0 &&
		fifo_init(&(newHandle.rxFifo), (uint8_t *)pBuffer + (size_buffer >> 1), size_buffer>>1, sizeof(usart_basetype)) == 0)
		{
			*(newHandle.baseReg + (&UCSR0A - &UDR0)) = (usartSettings->baud_reg_flags >> 14) & 0x2; // initialize ucsra with double speed and multiprocessor communication mode flags
			*(newHandle.baseReg + (&UCSR0B - &UDR0)) = ((usartSettings->if_en & 0x3) << 3) | (1 << RXCIE0) |  (1 << TXCIE0) | (usartSettings->character_size >= 9 ? (1 << UCSZ02) : 0); 
			/* initizalize ucsrb containing the interface enable bits, interrupt enable bits and bit 2 of character size */
			*(newHandle.baseReg + (&UCSR0C - &UDR0)) = 
					(usartSettings->usart_mode << 6) | ((usartSettings->parity_mode & 2)<< 4)  | (usartSettings->stop_bits > 1 ? (1 << USBS0) : 0) | ((usartSettings->character_size - 5) & 3) << 1;
#if USART_ALLOW_SYNCHRONOUS				
			*(newHandle.baseReg + (&UCSR0C - &UDR0)) |= usartSettings->sync_cpol & 1;
#endif
#if USART_ALLOW_9BIT
			*(newHandle.baseReg + (&UCSR0C - &UDR0)) |= usartSettings->character_size >= 9 ? 2 << 1 : 0;
#endif
			/* initialize ucsrc containing usart mode, parity mode, stop bits and character size bit 0 - 1 and cpol bit in synchronous mode*/
			*(newHandle.baseReg + (&UBRR0H - &UDR0)) = (usartSettings->baud_reg_flags >> 8) & 0xf;
			*(newHandle.baseReg + (&UBRR0L - &UDR0)) = usartSettings->baud_reg_flags & 0xff;
			// initialize baud rate register
		}
		else
		{
			newHandle.status = USART_ID10T;
		}
	}
	else
	{
		newHandle.status = USART_ID10T;
	}
	
	return newHandle;
}



/**
 * @brief transmits a single char 
 * @note this function is only allowed to be used once at a time with the same handle(protect with cli() if necessairy)
 */
void usart_putc(usart_handle_t *pHandle, usart_basetype c)
{
	uint8_t ctx = SREG;
	cli();
	assert(pHandle != NULL);
	
	if (pHandle != NULL){
		pHandle->status = USART_NO_ERROR;
		/*if (! (*(pHandle->baseReg + (&UCSR0A-&UDR0)) & (1 << UDRE0))) // data register is not empty -> put data into the fifo
		{*/
			if (fifo_put(&(pHandle->txFifo), &c) != FIFO_NO_ERROR){
				pHandle->status = USART_TX_FULL;
			}
			
			*(pHandle->baseReg + (&UCSR0B-&UDR0)) |= (1 << UDRIE0);	// enable the udr empty interrupt
		/*}
		else // otherwise directly write to the udr // well this shit is incorrect
		{
			_put_into_udr(pHandle, c);
		}*/
	}
	else{
		pHandle->status = USART_ID10T;
	}
	SREG = ctx;
}



usart_basetype usart_getc(usart_handle_t *pHandle)
{
	usart_basetype c = 0;
	assert(pHandle != NULL);
	pHandle->status = USART_NO_ERROR;
	
	if (pHandle != NULL){
		if (fifo_get(&(pHandle->rxFifo), &c) != FIFO_NO_ERROR){
			c = 0;
			pHandle->status = USART_RX_EMPTY;	// assume the fifo is empty on any error in the fifo for simpler code, the other cases (NULL Pointer parameter) should be impossible
		}
	}
	else{
		pHandle->status = USART_ID10T;
	}
	return c;
}

// IRQ handlers

void usart_udre_irq(usart_handle_t *pHandle)
{
	assert(pHandle != NULL);
	uint8_t fifolevel = fifo_getLevel(&(pHandle->txFifo));
	usart_basetype c;

	if (fifolevel)
	{
		fifo_get(&(pHandle->txFifo), &c);	// read character from fifo
		_put_into_udr(pHandle, c);			// put character into udr
	}
	else
	{
		*(pHandle->baseReg + (&UCSR0B-&UDR0)) &= ~(1 << UDRIE0);	// disable udr empty interrupt
	}
}

void usart_tx_irq(usart_handle_t *pHandle)
{
	assert(pHandle != NULL);
	if (usart_transmit_complete_callback != NULL){
		usart_transmit_complete_callback(pHandle);
	}
}

void usart_rx_irq(usart_handle_t *pHandle)
{
	assert(pHandle!=NULL);
	uint8_t ucsra = *(pHandle->baseReg + (&UCSR0A-&UDR0));
	usart_basetype c = _read_from_udr(pHandle);
	if (!c){
		c = c++;
	}

	/*if (ucsra & (1 << FE0))
	{
		if (usart_frameError_callback != NULL){
			usart_frameError_callback(pHandle);
		}
		return;
	}

	if (ucsra & (1 << UPE0))
	{
		if (usart_parityError_callback != NULL){
			usart_parityError_callback(pHandle);
		}
		return;
	}*/

	if (fifo_put(&(pHandle->rxFifo), &c) == FIFO_FULL || ucsra & (1 << DOR0))	// software fifo overflow or uart (hardware fifo) overflow
	{
		if (usart_rxOverflow_callback != NULL){
			usart_rxOverflow_callback(pHandle);
		}
	}
}

uint8_t usart_getRxFifoLevel(usart_handle_t *pHandle)
{
	assert(pHandle != NULL);
	return fifo_getLevel(&(pHandle->rxFifo));
}

uint8_t usart_getTxFifoEmptySpace(usart_handle_t *pHandle){
	assert(pHandle != NULL);
	return fifo_getEmptySpace(&(pHandle->txFifo));
}