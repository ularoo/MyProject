/****************************************************************************************
*																						*
*   Project:    ADF7020 & ADF70XXMB2 Controller Board - Device Drivers					*
*   File:       device_drivers.h														*
*																						*
*            																			*
*																						*
*	No responsibility is assumed by Analog Devices for use of this code, nor for any	*
*	infringements of patents or other rights of third parties that may result from 		*
*	its use.  No license is granted by implication or otherwise under any patent or		*
*	patent rights of Analog Devices. Trademarks and registered trademarks are the 		*
*	property of their respective owners. 												*
*																						*
*	Analog Devices																		*
*	One Technology Way, P.O. Box 9106, Norwood, MA 02062-9106, U.S.A.					*
*	Tel: 781.329.4700	www.analog.com													*
*	Fax: 781.326.8703	© 2005 Analog Devices, Inc. All rights reserved.				*
****************************************************************************************/



#ifndef _DEVICE_DRIVERS_H_


/* Include dependencies */
#include "ADuC847.h"
#include "adi_types.h"


/****************************************************************************
 *                    Hardware pin mapping                                  *
 ****************************************************************************/


/* ADF7020 register interface */

sbit ADF7020_SCLK               = P2^7;     // Output
sbit ADF7020_SDATA              = P2^6;     // Output
sbit ADF7020_SLE                = P2^5;     // Output
sbit ADF7020_SREAD              = P2^4;     // Input



/* Other ADF7020 connections */

sbit ADF7020_CE                 = P3^7;     // Output
sbit ADF7020_MUXOUT             = P1^5;     // Input


/* GPIO access to SPI lines */

sbit SPI_SCLOCK                 = P2^0;     // Input
sbit SPI_MISO                   = P2^2;     // Output


/* LEDs */

sbit DEBUG_LED                  = P3^4;     // Output

// ADI dev board LED mapping
sbit RED_LED                = P0^1;     // Output
sbit YELLOW_LED             = P0^2;     // Output
sbit GREEN_LED              = P0^0;     // Output
sbit ORANGE_LED             = P0^3;     // Output



/* Default states to set ports to after power up */
/* (These must be set manually based on the mappings above) */

#define P0_DEFAULT              0xFF       // Input (P0 is open drain & has no pullups)
#define P1_DEFAULT              0x5F       // P1.7 & P1.5 digital in, others analogue in
#define P2_DEFAULT              0x5F       // Input, ADF7020_SCLK and ADF7020_SLE low,
#define P3_DEFAULT              0x7F       // Input, ADF7020_CE low,



/* Real time clock constants */
/* On the ADuC845/7 the Timer Interval Counter is used as the master timing engine
   since it can continue to operate from the 32kHz XTAL during sleep mode. */

#define RTC_TICKS_PER_SECOND        128     // 7.8125ms

#define F_CORE                      6291456L    // CPU clock = 192 x 32768

/* Constants for short_delay() and Trt timer */
#define SHORT_DELAY                 1           // 40.69us at Fcore = 6.29MHz
#define DELAY_1mS                   24.576



void dd_write_7020_reg(pUINT8 reg_bytes);
void dd_adf7020_chip_sel(BOOL cs);
void dd_initialise(void);
void dd_short_delay(UINT8 count);


/* Interrupt enable/disable macros */

#ifdef __C51__      // KEIL 8051 compiler
    #define INTERRUPTS_ON           (IE |= 0x80)
    #define INTERRUPTS_OFF          (IE &= ~0x80)

#else /* Macros not defined */
    #define INTERRUPTS_ON
    #define INTERRUPTS_OFF
#endif


#define _DEVICE_DRIVERS_H_




/*****************************************************************************
 * $History: device_drivers.h $
 *
 * Date: 4/04/05   
 * Version 1.0 First Release
 ****************************************************************************/


/* EOF */


