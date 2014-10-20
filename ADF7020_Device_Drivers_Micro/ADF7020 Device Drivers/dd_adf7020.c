/****************************************************************************************
*																						*
*   Project:    ADF7020 & ADF70XXMB2 Controller Board - Device Drivers					*
*   File:       dd_adf7020.c															*
*																						*
*               device driver routines for ADF7020 specific functions					*
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


#include "device_drivers.h"



/****************************************************************************
 *                    ADF7020 Control Functions                             *
 ****************************************************************************/



/*****************************************************************************
 * Function:    dd_read_7020_reg
 * Parameters:  4 bit readback register address
 * Returns:     ADF7020_REG_T readback value
 * ===========================================================================
 * Description:
 *      Bit-bashed read from ADF7020 register file
 *      Register address is 4 bits: RB2 RB1 AD2 AD1
 ****************************************************************************/

ADF7020_REG_T dd_read_7020_reg(UINT8 readback_config)
{
    ADF7020_REG_T register_value;
    INT8 i, j;
    UINT8 byte;



    /* Write readback and ADC control value */
    register_value.whole_reg = (readback_config & 0x1F) << 4;
    register_value.whole_reg |= 7; // Address the readback setup register

  

    register_value.whole_reg = 0;


    /* Read back value */

    ADF7020_SDATA = 0;
    ADF7020_SCLK = 0;
    ADF7020_SLE = 1;

    /* Clock in first bit and discard */
    ADF7020_SCLK = 1;
    byte = 0; // Slight pulse extend
    ADF7020_SCLK = 0;


    /* Clock in data MSbit first */

#ifdef _BIG_ENDIAN_
    for (i=2; i<=3; i++)
#else
    for (i=1; i>=0; i--)
#endif
    {
        for (j=8; j>0; j--)
        {
            ADF7020_SCLK = 1;
            byte += byte; // left shift 1
            ADF7020_SCLK = 0;

            if (ADF7020_SREAD) byte |= 1;
        }

        register_value.byte[i] = byte;
    }

    ADF7020_SLE = 0;

    /* All port lines left low */

    return register_value;
}





/*****************************************************************************
 * Function:    dd_write_7020_reg
 * Parameters:  pUINT8 -> ADF7020_REG_T register value (inc register address)
 * Returns:     Nothing
 * ===========================================================================
 * Description:
 *      Bit-bashed write to ADF7020 register file
 *      Input ptr -> a 4 byte array representing a UINT32
 *          E.g. dd_write_7020_reg( & register_value.byte[0] )
 *
 *      Register address is contained in the 4 LSbits of the register value
 *      (see 7020 datasheet)
 ****************************************************************************/

void dd_write_7020_reg(pUINT8 reg_bytes)
{
    INT8 i, j;
    UINT8 byte;

    ADF7020_SLE = 0;
    ADF7020_SCLK = 0;


    /* Clock data out MSbit first */
#ifdef _BIG_ENDIAN_
    for (i=0; i<=3; i++)
#else
    for (i=3; i>=0; i--)
#endif
    {
        byte = reg_bytes[i];

        for (j=8; j>0; j--)
        {
            ADF7020_SCLK = 0;
            if (byte & 0x80) ADF7020_SDATA = 1;
            else ADF7020_SDATA = 0;
            ADF7020_SCLK = 1;
            byte += byte; // left shift 1
        }
        ADF7020_SCLK = 0;
    }


    /* Strobe the latch */

    ADF7020_SLE = 1;
    ADF7020_SLE = 1; // Slight pulse extend
    ADF7020_SDATA = 0;
    ADF7020_SLE = 0;

    /* All port lines left low */
}




/*****************************************************************************
 * Function:    dd_adf7020_chip_sel
 * Parameters:  TRUE to select, FALSE to de-select
 * Returns:     Nothing
 * ===========================================================================
 * Description:
 *      Controls ADF7020_CE I/O line
 ****************************************************************************/

void dd_adf7020_chip_sel(BOOL cs)
{
    if (cs) ADF7020_CE = 1;
    else ADF7020_CE = 0;
}





/*****************************************************************************
 * $History: dd_adf7020.c $
 *
 * Date: 4/04/05   
 * Version 1.0 First Release
 ****************************************************************************/



/* EOF */


