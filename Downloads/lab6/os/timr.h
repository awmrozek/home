/*!**********************************************************************
*!
*! FILE NAME: TIMR.H
*!
*! DESCRIPTION:  Timer interface.
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
*! 921028    RW     Replaced compiler-specific #ifdefs where possible.
*! 921209    RW     fnow, now access BIOS timer variable on PC.
*! 930308    RW     Type cast to make now/fnow return udwords on PC.
*! 930310    RW     New function: get_keyin(). Returns keys pressed.
*! 930803    RW     Added HW_ETRAX case.
*! 930913    RW     Added volatile to ftime and time_sec variables.
*! 931015    RW     Added 10ms timer tick macro tnow(), and corresponding
*!                  variable ttime.
*! 941031    MR     Changed tnow(), fnow() & now() to functions instead
*!                  of macros.
*! 950112    SJ     Changed HW_PC TICKS from 18.2 to 18. (To make LINT happy).
*!
*! Mar 20 1995   Willy Sagefalk    Removed tnow,fnow & now.Added get time
*!---------------------------------------------------------------------
*!
*! (C) Copyright 1992, Axis Communications AB, LUND, SWEDEN
*!
*!**********************************************************************/
/* @(#) timr.h 1.5 03/20/95 */

/*********** CONSTANT AND MACRO SECTION  *****************************/

/*********** TYPE DEFINITION SECTION  ********************************/

/*********** FUNCTION DECLARATION SECTION ****************************/

/*********** VARIABLE DECLARATION SECTION ***************************/

/****************  FUNCTION DEFINITION SECTION  ************************/

udword get_time(void);
void   timer_interrupt(void);

/********************** END OF FILE TIMR.H ****************************/
