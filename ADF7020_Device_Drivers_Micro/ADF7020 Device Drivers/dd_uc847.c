/****************************************************************************************
*																						*
*   Project:    ADF7020 & ADF70XXMB2 Controller Board - Device Drivers					*
*   File:       dd_uc847.c																*
*																						*
*               device driver routines for ADuC845/847 specific functions				*
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



#include "config.h"
#include "device_drivers.h"



/****************************************************************************
 *                        Device Driver Variables                           *
 ****************************************************************************/
//Currently None



/****************************************************************************
 *                   CPU Specific Control Functions                         *
 ****************************************************************************/


/*****************************************************************************
 * Function:    dd_initialise
 * Parameters:  None
 * Returns:     Nothing
 * ===========================================================================
 * Description:
 *      Function to set default IO state and pin direction after power on reset
 *      Registers are set to absolute values so this function should only be
 *      called during initial startup
 ****************************************************************************/

void dd_initialise(void)
{
    /* Enable 2k on-chip XRAM */
    //CFG845 = 0x01;

    /* On chip XRAM is now enabled and cleared within start_ad.a51
       It seems necessary to make this file the first entry in the
       uVision project source group in order for start_ad.obj to get
       linked in place of the default startup located in the C51 library */


    /* Force ports to known state/direction */
    P0 = P0_DEFAULT;
    P1 = P1_DEFAULT;
    P2 = P2_DEFAULT;
    P3 = P3_DEFAULT;


    /* Set PLL and power down mode for clocks */
    PLLCON = 0x01; // Fcore=6.29MHz and 32kHz runs during power down
    DEBUG_LED = 0; // Show the CPU is awake


    /* Start the uC845 TIC */
    HOUR = 0;
    MIN = 0;
    SEC = 0;
    HTHSEC = 0;
    TIMECON = 0x39;         // Interval=hours, Single time evt, Start running


    /* Set up timers */
    TMOD = 0x11;            // Both 16bit cascaded
    TCON = 0x55;            // Timers running, /INT0 and /INT1 trigger type = edge


    /* Set up interrupts */

    TCON |= 0x05;           // External interrupts /INT0 and /INT1 edge sensitive
    P3 |= 0x0C;             // Ensure /INT0 and /INT1 are inputs

    IEIP2 |= 0x01;          // Enable I2C/SPI int
    IEIP2 |= 0x04;          // Enable TIC int
    IE |= 0x01;             // Enable INT0 interrupt
    IE |= 0x04;             // Enable INT1 interrupt

}







/****************************************************************************
 *                      Real time clock functions                           *
 ****************************************************************************/




/*****************************************************************************
 * Function:    dd_short_delay
 * Parameters:  Number of units to wait (1 unit = 256/Fcore = 1/24576)
 * Returns:     Nothing
 * ===========================================================================
 * Description:
 *      Short delay function based on high byte of timer 0
 *      (Timer 0 must be configured as a 16 bit timer clocked at CPU core rate)
 *      Max delay = 10.4ms
 ****************************************************************************/

void dd_short_delay(UINT8 count)
{
    TL0 = 0;
    TH0 = 0;

    while(TH0 != count);
}



/*****************************************************************************
 * $History: dd_uc845.c $
 *
 * Date: 4/04/05   
 * Version 1.0 First Release
 ****************************************************************************/


/* EOF */


