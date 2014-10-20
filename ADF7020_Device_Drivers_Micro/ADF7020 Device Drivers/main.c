/****************************************************************************************
*																						*
*   Project:    ADF7020 & ADF70XXMB2 Controller Board - Device Drivers					*
*   File:       main.c																	*
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


#include <stdlib.h> // Needed for init_mempool on Keil compiler

#include "config.h"
#include "device_drivers.h"


int main(void)
{



/***********************************************
 *         initialisation             		   *
 ***********************************************/

    /* Initialise memory heap */

	#ifdef __C51__      // Detect KEIL 8051 compiler
    	init_mempool((void xdata *) 0x0400, 0x03FF);
	#endif

    dd_initialise();	// see dd_uc845 
    
	INTERRUPTS_ON;	


/************************************************
 *         Write to ADF7020 Registers    		*
 *												*
 * 	The following code is example code that 	*
 *	sets the ADF7020 into transmit mode using 	*
 *	the default settings of config.c			*
 ***********************************************/

	config_use_defaults();	//Use standard register configuration as defined in config.c

	dd_adf7020_chip_sel(TRUE); 	//ADF7020 CE high 

	dd_short_delay(2*DELAY_1mS);	//2ms delay, not strictly required

	dd_write_7020_reg(&config.r0_tx.byte[0]);	//Write to register 0 using value defined in config.c

	dd_write_7020_reg(&config.r1_rx.byte[0]);	//Write to register 1 using value defined in config.c

	dd_write_7020_reg(&config.r2_tx.byte[0]);	//Write to register 2 using value defined in config.c

	DEBUG_LED				= 0;	// see device_drivers.h
	RED_LED                	= 0;    // see device_drivers.h

	while(1)	//Never ending while loop
	{
	}	
	
    return 0;	//Should never get here
}





/*****************************************************************************
 * $History: main.c $
 *
 * Date: 4/04/05   
 * Version 1.0 First Release
 ****************************************************************************/


/* EOF */
