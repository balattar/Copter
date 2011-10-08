/*
             LUFA Library
     Copyright (C) Dean Camera, 2010.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2010  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this 
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in 
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting 
  documentation, and that the name of the author not be used in 
  advertising or publicity pertaining to distribution of the 
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *  \brief Master include file for the board temperature sensor driver.
 *
 *  Master include file for the board temperature sensor driver, for the USB boards which contain a temperature sensor.
 */

/** \ingroup Group_BoardDrivers
 *  @defgroup Group_Temperature Temperature Sensor Driver - LUFA/Drivers/Board/Temperature.h
 *
 *  \section Sec_Dependencies Module Source Dependencies
 *  The following files must be built with any user project that uses this module:
 *    - LUFA/Drivers/Board/Temperature.c <i>(Makefile source module name: LUFA_SRC_TEMPERATURE)</i>
 *
 *  \section Module Description
 *  Temperature sensor driver. This provides an easy to use interface for the hardware temperature sensor located
 *  on many boards. It provides an interface to configure the sensor and appropriate ADC channel, plus read out the
 *  current temperature in degrees C. It is designed for and will only work with the temperature sensor located on the
 *  official Atmel USB AVR boards, as each sensor has different characteristics.
 *
 *  @{
 */

#ifndef __TEMPERATURE_H__
#define __TEMPERATURE_H__

	/* Includes: */
		#include <avr/pgmspace.h>

		#include "../../Common/Common.h"
		#include "../Peripheral/ADC.h"
	
		#if (BOARD == BOARD_NONE)
			#error The Board Temperature Sensor driver cannot be used if the makefile BOARD option is not set.
		#elif ((BOARD != BOARD_USBKEY) && (BOARD != BOARD_STK525) && \
		       (BOARD != BOARD_STK526) && (BOARD != BOARD_USER) &&   \
			   (BOARD != BOARD_EVK527))
			#error The selected board does not contain a temperature sensor.
		#endif

	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			/** ADC channel number for the temperature sensor. */
			#define TEMP_ADC_CHANNEL       0
			
			/** ADC channel MUX mask for the temperature sensor. */
			#define TEMP_ADC_CHANNEL_MASK  ADC_CHANNEL0

			/** Minimum returnable temperature from the \ref Temperature_GetTemperature() function. */
			#define TEMP_MIN_TEMP          TEMP_TABLE_OFFSET

			/** Maximum returnable temperature from the \ref Temperature_GetTemperature() function. */
			#define TEMP_MAX_TEMP          ((TEMP_TABLE_SIZE - 1) + TEMP_TABLE_OFFSET)
		
		/* Inline Functions: */
			/** Initialises the temperature sensor driver, including setting up the appropriate ADC channel.
			 *  This must be called before any other temperature sensor routines.
			 *
			 *  \pre The ADC itself (not the ADC channel) must be configured separately before calling the
			 *       temperature sensor functions.
			 */
			static inline void Temperature_Init(void) ATTR_ALWAYS_INLINE;
			static inline void Temperature_Init(void)
			{
				ADC_SetupChannel(TEMP_ADC_CHANNEL);
			}

		/* Function Prototypes: */
			/** Performs a complete ADC on the temperature sensor channel, and converts the result into a
			 *  valid temperature between \ref TEMP_MIN_TEMP and \ref TEMP_MAX_TEMP in degrees Celsius.
			 *
			 *  \return Signed temperature value in degrees Celsius.
			 */
			int8_t Temperature_GetTemperature(void) ATTR_WARN_UNUSED_RESULT;

	/* Private Interface - For use in library only: */
	#if !defined(__DOXYGEN__)
		/* Macros: */
			#define TEMP_TABLE_SIZE   (sizeof(Temperature_Lookup) / sizeof(Temperature_Lookup[0]))
			#define TEMP_TABLE_OFFSET -21
	#endif

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif
		
#endif

/** @} */
