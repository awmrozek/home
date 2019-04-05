/*!**********************************************************************
*!
*! FILE NAME:    TIMR.C
*!
*! DESCRIPTION:  
*!
*! FUNCTIONS:    timer_interrupt
*! (EXTERNAL)    get_key_byte   
*!               get_keyin      
*!
*!---------------------------------------------------------------------
*!
*! HISTORY
*!
*! DATE      NAME   CHANGES
*! ------    ----   -------
*! 910515    MG     Initial version
*! 910605    MG     Fixed clock freq. Moved 'fnow()' and 'now()' (to
*!                  common.c (TURBOC) and globs.h (GNX) ). (/EE)
*! 910809    RW     Included system.h
*! 920113    PL     Fixed bug in c_interrupt causing timer to run too
*!                  slow.
*! 920122    EE     Added file and functions headers.
*! 921013    RW     Removed serial and parallel interrupt routines,
*!                  creating TIMER.C.
*! 921015    RW     Removed ax5_time().
*! 921028    RW     Added include of globs.h .
*! 930222    RW     timer_interrupt() now handles LED(s).
*! 930224    RW     New include file for bytpro: mgos.h.
*! 930301    RW     NPS-550 now also uses this module. New constant: KEY_MASK
*!                  for keyboard input.
*! 930310    RW     New function : get_keyin(): returns keys pressed now.
*! 930824    RW     Moved LED control macros to hw.h . This makes this
*!                  file compatible with ETRAX products.
*! 931015    RW     Added 10ms timer tick variable ttime.
*! 931221    PL     Removed front panel control for QMS project.
*! 940308    RW     Removed PROD_xxx.
*! 940421    RW     bytpro() -> os_delay(). mgos.h -> projos.h
*! 940427    RW     Added osys.h .
*! 940928    MR     Modified LED_ON/_OFF() macros.
*! 940930    MR     Included 'globalio.h'
*! 941031    MR     Functions to read time info - tnow(), fnow() & now() 
*!
*! Feb 21 1995   Mart Roomus       No LED fiddling, call LED_control() instead.
*!                                 Removed keyboard stuff.
*! Mar 20 1995   Willy Sagefalk    Removed tnow,fnow & now.Added get time
*! Nov 04 1996   Fred Jonsson      Changed ifdef __Watcom__ to !__GNU_CRIS__
*!---------------------------------------------------------------------
*!
*! (C) Copyright 1994, Axis Communications AB, LUND, SWEDEN
*!
*!**********************************************************************/
/* @(#) timr.c 1.2 11/04/96 */

/*********** INCLUDE FILE SECTION ***********************************/
#include "compiler.h"
#include "system.h"
#include "projos.h"

#ifdef HW_ETRAX
#include "etrax.h"
#endif

#include "osys.h"
#include "timr.h"

/*********** CONSTANT AND MACRO SECTION  *****************************/

#ifndef __GNU_CRIS__
const udword *pc_timer = (udword*)(0x46c); /* PC-haardfakta, p.185 */
#endif

/*********** TYPE DEFINITION SECTION  ********************************/
#if 0
extern void LED_control(void);
#endif
/*********** FUNCTION DECLARATION SECTION ****************************/

/*********** VARIABLE DECLARATION SECTION ***************************/

static udword ttime = 0;           /* Time since power on */

/****************  FUNCTION DEFINITION SECTION  ************************/

/*#****************************************************************************
*#
*# FUNCTION NAME: timer_interrupt
*#
*# PARAMETERS:    None
*#
*# RETURNS:       Nothing
*#
*# SIDE EFFECTS:  none
*#
*# DESCRIPTION:   Increase ttime (Tics since power on)
*#
*#-----------------------------------------------------------------------------
*#
*# HISTORY
*#
*# DATE          NAME              CHANGES
*# ----          ----              ------- 
*# Mar 20 1995   Willy Sagefalk    Initial version
*#**************************************************************************#*/


void timer_interrupt(void)
{
  ttime++; 
#if 0
  LED_control();
#endif
  return;
}

/*#****************************************************************************
*#
*# FUNCTION NAME: get_time
*#
*# PARAMETERS:    None
*#
*# RETURNS:       udword tics since power on
*#
*# SIDE EFFECTS:  none
*#
*# DESCRIPTION:   Reads the local variables ttime.
*#                Since ETRAX uses 32 bit variables we don't need to
*#                disable interrupts while reading...
*#
*#-----------------------------------------------------------------------------
*#
*# HISTORY
*#
*# DATE          NAME              CHANGES
*# ----          ----              ------- 
*# Mar 20 1995   Willy Sagefalk    Initial version
*#**************************************************************************#*/

udword get_time(void)
{
#ifdef HW_PC
  return ((*pc_timer/18)*SEC_TICS);
#else
  return ttime;
#endif
}
/********************** END OF FILE TIMR_AX5.C ****************************/
