/**
 * @file usart.h
 *
 * Created: 01.01.2021 00:41:48
 * @author josef
 */ 


#ifndef USART_H_
#define USART_H_
// *** INCLUDES ***
#include "fifo.h"
#include <stdint.h>
#include <avr/io.h>
#include <assert.h>
#include <stdlib.h>

// *** DEFINES ***
#define USART_ALLOW_SYNCHRONOUS 0	// enables feature for synchronous mode
#define USART_ALLOW_9BIT 0			// sets fifo to sizeof int instead of sizeof char


#ifdef __cplusplus
extern "C" {
#endif


#if USART_ALLOW_9BIT
typedef int usart_basetype;
#else
typedef char usart_basetype;
#endif

typedef enum{
	USART_NO_ERROR = 0,	///< No error
	USART_RX_OVERFLOW,	///< Rx Buffer Overflow
	USART_TX_FULL,		///< Transmit Buffer full
	USART_RX_EMPTY,		///< Read attempt on empty rx buffer
	USART_ID10T,		///< Usage error
}usart_error_t;


#define USART_BASE_USART0 &UDR0
#define USART_BASE_USART1 &UDR1

/**
 * @brief flags to enable the tx and rx interfaces
 */
enum USART_IF_EN{
	USART_IF_EN_TX = 1, /*< enable tx interface */
	USART_IF_EN_RX = 2	/*< enable rx interface */
};

/**
 * @brief usart mode of operation
 */
enum USART_MODE{
	USART_MODE_ASYNC = 0,	/*< asynchronous (uart) mode */
	USART_MODE_SYNC = 1,	/*< synchronous (usrt) mode */
	USART_MODE_SPI = 3		/*< spi mode */
};

/**
 * @brief usart parity bit
 */
enum USART_PARITY{
	USART_PARITY_DISABLE = 0,	/*< Disable parity bit */
	USART_PARITY_EVEN = 2,		/*< enable even parity bit */
	USART_PARITY_ODD = 3		/*< enable odd parity bit */
};

#if USART_ALLOW_SYNCHRONOUS
/**
 * @brief usart clock polarity
 * @note only used in synchronous mode
 */
enum USART_SYNC_CPOL{
	USART_SYNC_CPOL_RISING = 0,	/*< Sample on rising clock */
	USART_SYNC_CPOL_FALLING = 1	/*< Sample on falling clock */
};
#endif

/**
 * @brief various usart flags
 */
enum USART_FLAGS{
	USART_FLAGS_MULTIPROCESSOR = (1 << 14), /*< Multiprocessor communication enable */
	USART_FLAGS_2XSPEED = (1 << 15)			/*< Double speed */
};

/**
 * @brief Init setting for uart
 */
typedef struct{
	volatile uint8_t *baseReg;	/** udrn register address */
	uint8_t if_en;				/** enable interfaces: 
									bit 0: enable transmitter
									bit 1: enable receiver */
	uint8_t character_size;		/** size of the transmitted character: 5 - 9 (bit) */
	uint8_t usart_mode;			/** mode of the usart: 0 = async, 1 = sync, 3 = spi */
	uint8_t parity_mode;		/** parity bit: 0 = disabled, 2 = even, 3 = odd */
	uint8_t stop_bits;			/** 1 - 2 stopbits */
#if USART_ALLOW_SYNCHRONOUS
	uint8_t sync_cpol;			/** clock polarity(only used in synchronous mode) 0 = rising edge, 1 = falling edge */
#endif
	uint16_t baud_reg_flags;	/** baud rate register, configures the baudrate generator, bit 0 - 11, 
									also contains uart double speed on the msb: 1 = double speed, 0 = normal operation
									and multiprocessor communication enable bit on bit 14: 1 = enable multiprocessor
									communication (only affects the receiver) see datasheet:
									https://ww1.microchip.com/downloads/en/devicedoc/atmel-2549-8-bit-avr-microcontroller-atmega640-1280-1281-2560-2561_datasheet.pdf
									page 216 for more info, I dont recommend using mpcm because it seems weird to me */	
}usart_init_t;

/**
 * @brief Uart handle
 * @note the Handle doesn't have to be declared volatile, because it's members are already
 */
typedef struct{
	fifo_handle_t txFifo;		/** tx fifo handle */
	fifo_handle_t rxFifo;		/** rx fifo handle */
	volatile uint8_t *baseReg;	/** base register */
	usart_error_t status;		/** status variable */
}usart_handle_t;

/**
 * @brief initializes the usart according to the handle
 * @param usartSettings pointer to usartSettings
 * @param pBuffer pointer to the buffer for the fifos
 * @param size_buffer size of the buffer (pBuffer)
 * @return initialized usart_handle_t
 * @note make sure that size_buffer is a multiple of 2 and if USART_ALLOW_9BIT is enabled a multiple of 4 because the fifo is split into a rx and a tx fifo
 */
usart_handle_t usart_init(usart_init_t *usartSettings, void *pBuffer, uint8_t size_buffer);

/**
 * @brief Checks the level of the rx fifo
 * @param pHandle usart handle
 * @return ammount of characters in the rx fifo
 */
uint8_t usart_getRxFifoLevel(usart_handle_t *pHandle);

/**
 * @brief Checks empty space in the Tx Fifo
 * @param pHandle usart handle
 * @return ammount of empty space in the tx fifo
 */
uint8_t usart_getTxFifoEmptySpace(usart_handle_t *pHandle);

/**
 * @brief writes a single character to the usart
 * @param pHandle usart handle
 * @param c character to write
 * @note sets pHandle->status on usage error or tx buffer full
 */
void usart_putc(usart_handle_t *pHandle, usart_basetype c);

/**
 * @brief reads a single character from the usart
 * @param pHandle usart handle
 * @return read character
 * @note sets pHandle->status and returns 0 on Empty rx buffer or usage error
 */
usart_basetype usart_getc(usart_handle_t *pHandle);



/**
 * @defgroup usart_callbacks Event callbacks for USART
 * @note Callbacks get initialized to NULL. The callback caller checks the function pointers to NULL and executes them if they aint NULL
 * @{
 */
/**
 * @brief rx overflow callback
 * @param pHandle, uart with overflow condition
 */
extern void (*usart_rxOverflow_callback)(usart_handle_t *pHandle);

/**
 * @brief parity error callback
 * @param pHandle, uart with parity error
 */
extern void (*usart_parityError_callback)(usart_handle_t *pHandle);

/**
 * @brief frame error callback
 * @param pHandle, uart with frame error condition
 */
extern void (*usart_frameError_callback)(usart_handle_t *phandle);

/**
 * @brief transmission complete callback
 * @param pHandle, uart with a complete transmission
 */
extern void (*usart_transmit_complete_callback)(usart_handle_t *pHandle);
/**
 * @}
 */



/**
 * @defgroup usart_irqs IRQs of the USART 
 * @{
 */

/**
 * @brief usart data register empty interrupt
 * @param pHandle pointer to the interrupting usart handle
 */
void usart_udre_irq(usart_handle_t *pHandle);

/**
 * @brief transmit complete interrupt
 * @param pHandle handle of the usart interface with the completed transmission
 */
void usart_tx_irq(usart_handle_t *pHandle);

/**
 * @brief receved on uart interrupt
 * @param pHandle of the receiving usart interface
 */
void usart_rx_irq(usart_handle_t *pHandle);

/**
 * @}
 */






#ifdef __cplusplus
}
#endif


#endif /* USART_H_ */