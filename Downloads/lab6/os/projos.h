/*!**************************************************************************
*!
*!  FILE NAME:    PROJOS.H
*!
*!  DESCRIPTION:  Definitions for OSYS
*!
*!**************************************************************************/

#ifndef projos_h
#define projos_h

#include "osys.h"               /* to get task_struct */

/**************************  CONSTANTS  ************************************/

/* Macro for building Channel-ID */
#define CHN(origin,offset,dest) ((origin) * 0x1000 + (offset) * 0x100 + (dest))
#ifdef __GNU_CRIS__
#define HW_ETRAX
#endif


/****************************************************************************/
/*                                                                          */
/*                            TASKS DEFINITION                              */
/*                                                                          */
/****************************************************************************/

#define N_TASKS  4  /* Number of tasks in system (task 0 always idle task). */

/****************************************************************************/
/*                                                                          */
/*                       SOFTWARE CALL/INTERRUPT HANDLING                   */
/*                                                                          */
/****************************************************************************/

#ifdef HW_PC

void enable(void);
void disable(void);

#define SEC_TICS      100                /* Number if OS ticks per second */

#ifdef HW_DOS32
#define TIMER_VECT           0x08

void os_entry(void);
void init_software_int(void);
void enable_hw_int(void);
void disable_hw_int(void);

#define OS_FUNCTION_REQUEST() os_entry()
#define INITIAL_OS_REQUEST()  init_software_int()
#define OS_TICK_ON()          enable_hw_int()
#define OS_TICK_OFF()         disable_hw_int()

#else
#define TIMER_VECT           0x08
#define SOFTWARE_VECT        0x72
#define INIT_SOFTWARE_VECT   0x73

#define OS_FUNCTION_REQUEST() geninterrupt(SOFTWARE_VECT);
#define INITIAL_OS_REQUEST()  geninterrupt(INIT_SOFTWARE_VECT);
#define OS_TICK_ON()
#define OS_TICK_OFF()

#endif
#endif

#ifdef HW_ETRAX
#define SEC_TICS      100               /* Number if OS ticks per second */

void enable(void);
void disable(void);
void os_entry(void);
void init_software_int(void);
void enable_hw_int(void);
void disable_hw_int(void);

#define OS_FUNCTION_REQUEST() os_entry()
#define INITIAL_OS_REQUEST()  init_software_int()
#define OS_TICK_ON()          enable_hw_int()
#define OS_TICK_OFF()         disable_hw_int()

#define interrupt
#endif

#ifdef HW_NPS550
#define SEC_TICS      100               /* Number if OS ticks per second */

void os_entry(void);
void init_software_int(void);
void enable_hw_int(void);
void disable_hw_int(void);

#define OS_FUNCTION_REQUEST() os_entry()
#define INITIAL_OS_REQUEST()  init_software_int()
#define OS_TICK_ON()          enable_hw_int()
#define OS_TICK_OFF()         disable_hw_int()

#define interrupt
#endif

#if defined(_WIN32) && defined(_CONSOLE) && defined(_M_IX86)

#define SEC_TICS      100               /* Number if OS ticks per second */

void enable(const char *, unsigned long);
void disable(const char *, unsigned long);
void os_entry(void);
void init_software_int(void);
void enable_hw_int(const char *, unsigned long);
void disable_hw_int(const char *, unsigned long);

#define OS_FUNCTION_REQUEST() os_entry()
#define INITIAL_OS_REQUEST()  init_software_int()
#define OS_TICK_ON()          enable_hw_int(__FILE__, __LINE__)
#define OS_TICK_OFF()         disable_hw_int(__FILE__, __LINE__)

#define interrupt
#endif

/************************************************************************
*
*                   TIMING CONSTANTS FOR VARIOUS PROCESSES
*
*************************************************************************/

#define FP_DELAY_TIME (50 * SEC_TICS / 1000)

/****************************************************************************/
/*                                                                          */
/*                             TASK IDENTITIES                              */
/*                                                                          */
/****************************************************************************/
#define IDLE                0   /* Idle task id.               */
#define PRINTF_DISK_TASK    1   /* Printf and disk task id.    */
#define THREAD_TASK         2   /* Protocol stack task id.     */
#define OTHER_INT_TASK      3   /* Interrupt task              */ 

/**************************  TYPE DEFINITIONS  *****************************/
/* Forward-declare struct task_struct, and leave task_list as an
   incomplete declaration. */
struct task_struct;
extern struct task_struct    task_list[];   /* Task definition table */

/**************************  EXTERNALS  ************************************/

/**************************  EXTERNAL FUNCTIONS  ***************************/

#endif /* projos_h */
