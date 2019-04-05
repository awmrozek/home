/*!**************************************************************************
*!
*! FILE NAME:    GLOBALIO.H
*!
*! DESCRIPTION:  Prototypes for ETRAX global IO resources
*!
*!
*!---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE          NAME              CHANGES
*! ----          ----              -------
*! 940725        MR                Initial version
*! Apr 18 1995   Willy Sagefalk    Added cg16 support
*! May 13 1995   Mart Roomus       Added #ifndef __GLOBALIO_H__
*! Aug 29 1995   Per Karlsson      Added port1_xor().
*!
*!        (C) 1994 Axis Communications AB, Lund, Sweden
*!**************************************************************************/
/* @(#) globalio.h 1.5 08/29/95 */

#ifndef __GLOBALIO_H__
#define __GLOBALIO_H__

/**************************  CONSTANTS  ************************************/

/* Direction specifiers used by port<x>_dir() */
#define PORT_SETIN      0    /* All 1s in mask will be set to inputs */
#define PORT_SETOUT     1    /* All 1s in mask will be set to outputs */
#define PORT_SETDIR     2    /* All 1s in mask will be set to outputs and */
                             /* all 0s will be set to inputs */

/**************************  TYPE DEFINITIONS  *****************************/

/**************************  PUBLIC FUNCTIONS  ***************************/

#ifdef __CRIS__
NO_RET port1_dir(uword, byte);
NO_RET port1_set(byte);
NO_RET port1_and(byte);
NO_RET port1_or(byte);
NO_RET port1_xor(byte mask);
byte   port1_in(NO_PARAMS);
#endif
#ifdef GNX_CG16
void port_out_set(byte data);
void port_out_and(byte mask);
void port_out_or(byte mask);
byte port_in(void);
#endif
/**************************  EXTERNALS  ************************************/

#endif  /*  __GLOBALIO_H__ */
