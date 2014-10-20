/****************************************************************************************
*																						*
*   Project:    ADF7020 & ADF70XXMB2 Controller Board - Device Drivers					*
*   File:       config.h																*
*																						*
*               																		*
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



#ifndef _CONFIG_H_

#include "adi_types.h"


/* Structure to hold static configuration parameters in RAM
   for use at run time */

typedef struct
{
    /* ADF7020 register values - Rx */
    ADF7020_REG_T       r0_rx;                  // N register
    ADF7020_REG_T       r1_rx;                  // Oscillator / filter
    ADF7020_REG_T       r2_rx;                  // Transmit modulation
    ADF7020_REG_T       r3_rx;                  // Receiver clock
    ADF7020_REG_T       r4_rx;                  // Demodulator setup
    ADF7020_REG_T       r5_rx;                  // Sync byte
    ADF7020_REG_T       r6_rx;                  // Correlator / Demodulator
    ADF7020_REG_T       r9_rx;                  // AGC
    ADF7020_REG_T       r11_rx;                 // AFC


    /* ADF7020 register values - Tx */
    ADF7020_REG_T       r0_tx;                  // N register
	ADF7020_REG_T       r1_tx;                  // Oscillator / filter
    ADF7020_REG_T       r2_tx;                  // Transmit modulation


} CONFIG_STATIC_T;


void config_use_defaults(void);


/* Externs provided by config.c */

extern CONFIG_STATIC_T config;


#define _CONFIG_H_


#endif


/*****************************************************************************
 * $History: config.h $
 *
 * Date: 4/04/05   
 * Version 1.0 First Release
 ****************************************************************************/



/* EOF */

