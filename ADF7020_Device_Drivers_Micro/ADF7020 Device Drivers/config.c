/****************************************************************************************
*																						*
*   Project:    ADF7020 & ADF70XXMB2 Controller Board - Device Drivers					*
*   File:       config.c																*
*																						*
*               ADF7020 Register Configuration											*
*																						*
*	No responsibility is assumed by Analog Devices for use of this code, nor for any	*
*	infringements of patents or other rights of third parties that may result from 		*
*	its use.  No license is granted by implication or otherwise under any patent or		*
*	patent rights of Analog Devices. Trademarks and registered trademarks are the 		*
*	property of their respective owners. 												*
*																						*
*	Analog Devices																		*
*	One Technology Way, P.O. Box 9106, Norwood, MA 02062-9106, U.S.A.					*
*	Tel: 781.329.4700	2005 Analog Devices, Inc. All rights reserved.www.analog.com													*
*	Fax: 781.326.8703																	*
****************************************************************************************/


#include <string.h>

#include "config.h"
#include "device_drivers.h"




/****************************************************************************
 *          Set key parameters here for config_use_defaults()               *
 ****************************************************************************/

#define BAUD    10800


#define RF_FREQ     (9050000UL)         // Hz/100, may need to be adjusted to give exact frequency due to crystal error.

#define PA_LEVEL    5UL                 // ~ -7dBm

#define XTAL_FREQ   110592UL            // Hz/100. Do not change without reviewing
                                        // rest of register config below
#define R           1UL



/* Derived values - do not edit */
#define RX_LO       (RF_FREQ-2000UL)

#define INT_N_RX    (RX_LO*R/XTAL_FREQ)
#define FRAC_N_RX   (((RX_LO-((RX_LO*R/XTAL_FREQ)*XTAL_FREQ/R))*32768UL)/XTAL_FREQ)
#define INT_N_TX    (RF_FREQ*R/XTAL_FREQ)
#define FRAC_N_TX   (((RF_FREQ-((RF_FREQ*R/XTAL_FREQ)*XTAL_FREQ/R))*32768UL)/XTAL_FREQ)


/* Static global configuration structure */

CONFIG_STATIC_T config;



/*****************************************************************************
 * Function:    config_use_defaults
 * Parameters:  None
 * Returns:     Nothing
 * ===========================================================================
 * Description:
 *      Runtime setup of config structure using default values that operate
 *      the 7020 at 10800baud GFSK.
 ****************************************************************************/

void config_use_defaults(void)
{


    /* Rx mode configuration */

    config.r0_rx.whole_reg =   // N Register
    (3UL<<29)|(1UL<<28)|(1UL<<27)|      // MUX=DigLockDet, PLL=On, Rx mode
    (INT_N_RX<<19)|                     // Integer N
    (FRAC_N_RX<<4);                     // Frac N

    config.r1_rx.whole_reg =   // Oscillator/Filter register
    (0UL<<22)|(0UL<<20)|(6UL<<16)|      // IFBW=100, VCOad=0, VCObias=3mA
    (2UL<<14)|(0UL<<13)|(1UL<<12)|      // CP=1.5mA, VCOband=868, XTAL=on
    (0UL<<8)|(0UL<<7)|(R<<4)|1UL;       // CLKout=off, XTALdoub=off, R=1

    config.r2_rx.whole_reg =   // Transmit modulation register (21600Hz 10800baud)
    (2UL<<30)|(3UL<<27)|(3UL<<24)|      // PAbias=9uA, TxInvert=off, IndexC=128, GFSKmodctrl=3
    (8UL<<15)|(1UL<<6)|2UL;             // DIVfact=8, Mod=GFSK, PA=off

    config.r3_rx.whole_reg =   // Receiver clock register (10800baud)
    (111UL<<16)|(32UL<<8)|              // SeqClkDiv=111, CdrClkDiv=32
    (1UL<<6)|(1UL<<4)|3UL;              // DemodClkDiv=1, BBosClkDiv=8

    config.r4_rx.whole_reg =   // Demodulator setup register
    (2UL<<23)|(0UL<<16)|                // DemodLock=mode2, LockTime=N/A
    (5UL<<6)|(0UL<<4)|4UL;              // PostDemodBW=5, Demod=linear
    
    config.r5_rx.whole_reg =   // Sync byte register
    (1UL<<31)|                          // Logic 1 preamble bit shifted in
    ((0x123456UL & 0xFFFFFE)<<7)|       // Load syncword bytes >> 1
    (0UL<<6)|(3UL<<4)|5UL;              // Errors=0, Length=24
    
    config.r6_rx.whole_reg =   // Correlator / demodulator register
    (0UL<<29)|(221UL<<20)|(0UL<<19)|    // RxInvert=off, FilterDiv=221, FilterCal=off
    (0UL<<18)|(0UL<<16)|(0UL<<15)|      // MixerLin=def, LNAbias=def, LNAgain=def
    (0UL<<14)|(124UL<<4)|6UL;           // CrossProduct, DiscrimBW=124

    config.r9_rx.whole_reg =   // AGC register
    (0UL<<20)|(0UL<<19)|(0UL<<18)|      // Gain=auto, AGC=auto
    (70UL<<11)|(30UL<<4)|9UL;           // HiThresh=70, LowThresh=30

    config.r11_rx.whole_reg =  // AFC register
    (1UL<<20)|(759UL<<4)|11UL;          // AFC=on, Scaling=759


    /* Tx mode configuration */

    config.r0_tx.whole_reg =   // N Register
    (3UL<<29)|(1UL<<28)|(0UL<<27)|      // MUX=DigLockDet, PLL=On, Tx mode
    (INT_N_TX<<19)|                     // Integer N
    (FRAC_N_TX<<4);                     // Frac N

	config.r1_tx.whole_reg = config.r1_rx.whole_reg ;  // Oscillator/Filter register
		

    config.r2_tx.whole_reg = config.r2_rx.whole_reg |
    (PA_LEVEL<<9)|(1UL<<5)|(1UL<<4);    // Set PA level, MuteTillLock=on, PA=on

}




/*****************************************************************************
 * $History: config.c $
 * 
 * Date: 4/04/05   
 * Version 1.0 First Release
 ****************************************************************************/


/* EOF */
