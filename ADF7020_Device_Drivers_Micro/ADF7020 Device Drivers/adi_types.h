/****************************************************************************************
*																						*
*   Project:    ADF7020 & ADF70XXMB2 Controller Board - Device Drivers					*
*   File:       adi_types.h																*
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

#ifndef _ADI_TYPES_H_



    #ifndef TRUE
        #define TRUE    -1
        #define FALSE   0
    #endif




    #ifdef __C51__      // Detect KEIL 8051 compiler

        typedef char              CHAR8, * pCHAR8;
        typedef signed char        INT8, * pINT8;
        typedef unsigned char     UINT8, * pUINT8;
        typedef signed int        INT16, * pINT16;
        typedef unsigned int     UINT16, * pUINT16;
        typedef signed long       INT32, * pINT32;
        typedef unsigned long    UINT32, * pUINT32;
        typedef void               VOID, * pVOID;
        typedef unsigned int       BOOL, * pBOOL;

        typedef INT16           dtime_t, * pdtime_t;
        #define MAX_DTIME       32767


        /* Generic prototype for schedule tasks:
           Returns T_NEXT and takes void parameters */
        typedef dtime_t (code *pTASK)(void);

        #define _BIG_ENDIAN_
        #define _MSBYTE_        0
        #define _LSBYTE_        3

    #else

        #ifdef _MSC_VER // Microsoft Visual C++

            typedef signed char       CHAR8, * pCHAR8;
            typedef signed char        INT8, * pINT8;
            typedef unsigned char     UINT8, * pUINT8;
            typedef signed short      INT16, * pINT16;
            typedef unsigned short   UINT16, * pUINT16;
            typedef int               INT32, * pINT32;
            typedef unsigned int     UINT32, * pUINT32;
            typedef void                     * pVOID;
            typedef int                BOOL, * pBOOL;   // INT8 over RPC

            typedef INT16           dtime_t, * pdtime_t;
            #define MAX_DTIME       32767


            /* Generic prototype for schedule tasks:
               Returns T_NEXT and takes void parameters */
            typedef dtime_t ( *pTASK)(void);

            #undef _BIG_ENDIAN_
            #define _MSBYTE_        3
            #define _LSBYTE_        0

        #else               // Assume a WIN32 PC platform
            #error What platform is this?
            typedef signed char       CHAR8, * pCHAR8;
            typedef signed char        INT8, * pINT8;
            typedef unsigned char     UINT8, * pUINT8;
            typedef signed short      INT16, * pINT16;
            typedef unsigned short   UINT16, * pUINT16;
            typedef signed long       INT32, * pINT32;
            typedef unsigned long    UINT32, * pUINT32;
            typedef void               VOID, * pVOID;
            typedef int                BOOL, * pBOOL;

            #undef _BIG_ENDIAN_

        #endif

    #endif



    /* Macro to get around KEIL C51 failure to test correctly for NULL
       pointers when the pointer type is not idata.
       See http://www.keil.com/discuss/docs/thread3443.htm */

    #define ISNULL(x)       ( (UINT16)(x) == 0)



    /* Union to handle ADF7020 register values and other UINT32 values
       that need to be accessed as individual bytes */
    typedef union
    {
        UINT32  whole_reg;
        UINT8   byte[4];    // Warning: Be endian-specific when accessing bytes
    
    } ADF7020_REG_T;



    #define _ADI_TYPES_H_

#endif




/*****************************************************************************
 * $History: adi_types.h $
 * 
 * Date: 4/04/05   
 * Version 1.0 First Release
 ****************************************************************************/


/* EOF */


