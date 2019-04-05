/*!**************************************************************************
*! FILE NAME  : Time.h
*!                                                            
*! DESCRIPTION: The interface to time conversion routines
*!              Modified versions of the GNU library implementations.
*!              
*!---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME            CHANGES
*! ----         ----            -------
*! May 26 1995  Stefan S        Initial version
*! Sep 20 1996  Fredrik Sjoholm Added ax_time & ax_settime <- USE THESE !!
*! Sep 27 1996  Fredrik Sjoholm Added ax_flushtime
*! Oct 10 1996  Mart Roomus     Renamed to time.h
*! Oct 18 1996  Mart Roomus     Added ax_compiletime()
*!                              Added gmTime_r()
*! Oct 21 1996  Mart Roomus     Modified C++ '//'-comments to C comments
*! Oct 27 1996  Fredrik Sjoholm added pretty smart ax_parsetime() function
*! Oct 27 1996  Fredrik Sjoholm month abbriv tables referenced from here
*! Oct 28 1996  Mart Roomus     ax_softtime().
*!---------------------------------------------------------------------------
*! (C) Copyright 1995, Axis Technologies AB, LUND, SWEDEN
*!**************************************************************************/
/*! @(#) time.h 1.9 10/28/96 */

#ifndef Time_h
#define Time_h

/********************** INCLUDE FILES SECTION ******************************/

#include "compiler.h"
#include <ctype.h>

/********************** CONSTANT AND MACRO SECTION *************************/

/********************** TYPE DEFINITION SECTION ****************************/

typedef long time_t;
struct tm
{
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
};


/********************** EXPORTED FUNCTION DECLARATION SECTION **************/


extern const char* month_letters[];
extern const char* month_letters_mixed[];

/*
** Time format conversion functions.
** For info on gmtime etc., try:
** > man gmtime
*/
struct tm* gmTime(const time_t *);
struct tm* gmTime_r(const time_t*, struct tm*);
time_t     mkTime(struct tm *);
/*
** Some macros to be compatible with code using the standard
** names gmtime, mktime etc.
*/
#define gmtime_r gmTime_r
#define gmtime   gmTime
#define mktime   mkTime

time_t     systemTime(void);      /* gives system uptime in seconds */
time_t     actualTime(void);      /* gives actual time in seconds since 1970 */
void       setActualTime(time_t); /* sets actual time in seconds since 1970 */

/*
** THE FOLLOWING ROUTINES ARE CACHED, AND ARE THUS NOT ACCESSING THE
** TIMEKEEPER MORE THAN REQUIRED. DON NOT USE actualTime(), use ax_time() !!
*/

time_t    ax_time();         /* gives actual time in seconds since 1970 */
time_t    ax_softtime();     /* as ax_time() but never uses hardware */
void      ax_settime(time_t);/* sets actual time in seconds since 1970 */
void      ax_flushtime();    /* force timer reload */
time_t    ax_compiletime();  /* returns compile time in secs since 1970 */
time_t    ax_parsetime(const char *theDateString);
                             /* parses a string for time components */


#endif
/********************** END OF FILE time.h *****************************/



