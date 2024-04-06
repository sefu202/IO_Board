/**
 * adc.h
 *
 * Created: 27.08.2021 19:35:15
 *  Author: asj
 * @file
 */


#ifndef ADC_H_
#define ADC_H_

// Includes
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

// Defines
#define ADC_CHANNEL_NUM  8						///< Number of adc Channels
#define ADC_CHANNEL_LAST (ADC_CHANNEL_NUM -1)	///< last ADC channel

/**
 * @brief ADC Register Map
 */
typedef struct{
	volatile uint8_t adcl;		/**< The ADC Data Register Low Byte  */
	volatile uint8_t adch;		/**< The ADC Data Register High Byte  */
	volatile uint8_t adcsra;	/**< ADC Control and Status Register A  */
	volatile uint8_t adcsrb;	/**< ADC Control and Status Register B  */
	volatile uint8_t admux;		/**< Multiplexer Selection Register  */
	volatile uint8_t didr2;		/**< Digital Input Disable Register 2  */
	volatile uint8_t didr0;		/**< Digital Input Disable Register 0  */
}adc_register_t;

/**
 * @brief ADC Voltage Reference Selection Enum
 */
typedef enum {
	ADC_VREF_AREF = 0,	/**< AREF as voltage reference */
	ADC_VREF_AVCC,	    /**< Avcc as voltage reference */
	ADC_VREF_11,	    /**< Internal 1.1 Voltage Reference */
	ADC_VREF_256	    /**< Internal 2.56 Voltage Reference */
}adc_vref_t;

/**
 * @brief ADC Prescaler Selection Enum
 */
typedef enum {
    ADC_DIV_2 = 0,  /**< Divide Clock by 2 */
    ADC_DIV_2_,     /**< Divide Clock by 2 */
    ADC_DIV_4,      /**< Divide Clock by 4 */
    ADC_DIV_8,      /**< Divide Clock by 8 */
    ADC_DIV_16,     /**< Divide Clock by 16 */
    ADC_DIV_32,     /**< Divide Clock by 32 */
    ADC_DIV_64,     /**< Divide Clock by 64 */
    ADC_DIV_128     /**< Divide Clock by 128 */
}adc_prescaler_t;

/**
 * @brief ADC Handle
 */
typedef struct{
	volatile uint8_t currentChannel;					/**< Channel of the current conversion */
	volatile uint8_t status;							/**< Status: 0 = No Error, gets set by calling adc functions */
    adc_register_t *regs;								/**< Pointer to the ADC Base Address (ADCL Register) */
    uint16_t enabledChannels;							/**< Enabled channels flags */
    volatile uint16_t resultArray[ADC_CHANNEL_NUM];		/**< Pointer to the array where to store the result data */
}adc_t;

/**
 * @brief ADC init Struct
 */
typedef struct{
    volatile uint8_t 	*baseAddress;  	/**< Adc Peripherial base address */
    adc_vref_t			vref;			/**< voltage reference */
	uint16_t			channelEnFlags;	/**< Binary flags to enable ADC channels bit 0..15 = channels 0..15 */
    adc_prescaler_t 	prescaler;      /**< Prescaler  */
}adc_init_t;

/**
 * @brief Initializes an ADC according to a adc_init_t struct, but does not start the conversion
 * @param[in] pInit	Init settings struct
 * @note pAdc->status gets set to a non zero value on error
 * @return adc handle
 */
adc_t adc_init(adc_init_t *pInit);

/**
 * @brief Starts the conversion
 * @param[in] pAdc	ADC Handle
 */
void adc_startConversion(adc_t *pAdc);

/**
 * @brief stops the ADC conversion by disabling the ADC
 * @param[in] pAdc 	ADC Handle
 */
void adc_stop(adc_t *pAdc);

/**
 * @brief gets result of adc conversion of channel
 * @param[in] pAdc ADC Handle
 * @param[in] channel
 * @return conversion result of specified channel
 */
uint16_t adc_getResult(adc_t *pAdc, uint8_t channel);

/**
 * @brief Adc Interrupt Routine
 * @param[inout] pAdc	ADC handle
 * @note disables ADC and sets pAdc->status to a non zero value on error
 */
void adc_isr(adc_t *pAdc);

#endif /* ADC_H_ */