/*!**************************************************************************
*!
*! FILE NAME  : system.h
*!
*! DESCRIPTION: Axis system function definitions.
*!
*! FUNCTIONS  : Many.
*! (EXPORTED)
*!
*!---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME             CHANGES
*! ----         ----             -------
*! Nov 18 1993  Kenneth Jonsson  Added comments to each function prototype.
*!                               The system.doc is not used anymore.
*! Mar 29 1994  Ricard Wolf      Added ax_fprintf, ax_sprintf.
*! Jan 04 1995  MArt Roomus      Renamed 'obj_size' argument name to blk_size
*!                               in fread/fwrite function protos ('obj_size'
*!                               is an object handler function).
*! Jan 5 1995   Willy Sagefalk   Added include of smap.h
*! Jan 5 1995   Willy Sagefalk   Removed prototype for ax_sprintf
*! Jan 17 1995  Mart Roomus      Common header for alloc protos.
*!                               Added ax_calloc.
*!                               Added MALLOC_TEST definition.
*!                               Removed SMAP.H include.
*!                               Alloc prototype redefinition when testing
*!                               allocation functions. (MALLOC_TEST).
*! Jan 30 1995  Mart Roomus      alloctab() definition added.
*! Jan 31 1995  Mart Roomus      Added ax_dalloccheck().
*! Feb 23 1995  Jonas Noren      Defined ax_scanf temporary.
*! Mar 30 1995  Mart Roomus      Added ax_sprintf macro when reentrant.
*!                               Added ax_vsprintf
*! Apr 03 1995  Mart Roomus      Added/modified file function protos.
*!                               Removed 'if PROTOTYPES'.
*!                               Removed ax_sprintf macro definition
*! Apr 20 1995  Mart Roomus      Moved MALLOC_TEST to here
*! May 24 1995  Willy Sagefalk   Removed MALLOC_TEST
*! Jun 01 1995  Jonas Noren      Removed temporary definition of ax_scanf.
*! Aug 01 1995  Jens Johansson   Added ax_coreleft_total(). C-standard. 
*! Jun 14 1996  Fredrik Svensson Prevent mult. include with #ifndef system_h
*! Oct 16 1996  Fredrik Sjoholm  ax_fread arguments extended to udword
*! Oct 29 1996  Fredrik Sjoholm  telnet debug.
*! Oct 30 1996  Stefan Jonsson   nprintf is already declared in xprintf.h
*! Nov 14 1996  Fredrik Svensson Use osys_win32_debug_printf instead of
*!                               ax_printf in PC environment
*!
*!---------------------------------------------------------------------------
*!
*! (C) Copyright 1995, Axis Communications AB, LUND, SWEDEN
*!
*!**************************************************************************/
/* @(#) system.h 1.6 11/14/96 */

#ifndef system_h
#define system_h

/********************** INCLUDE FILES SECTION ******************************/
#include <stdarg.h>                       /* ax_vsprintf() need 'va_list'. */

/********************** CONSTANT AND MACRO SECTION *************************/

#define DEBUG 1

/********************** TYPE DEFINITION SECTION ****************************/

enum DeviceType
{
  STDIO_NULL=0,
  STDIO_SERIAL,
  STDIO_TELNET
};

struct GlobalStdIO 
{
  enum DeviceType inDevice;
  enum DeviceType outDevice;
  void *telnetInSession;
  void *telnetOutSession;
};


/********************** EXPORTED FUNCTION DECLARATION SECTION **************/

extern struct GlobalStdIO globalStdIO;

/*#**************************************************************************
*#
*#  FUNCTION NAME: ax_delay_ms
*#
*#  PARAMETERS:    milliseconds: Number of millisecs to pause process
*#
*#  RETURNS:       Nothing
*#
*#  SIDE EFFECTS:  None
*#
*#  DESCRIPTION:   The function is used to suspend a process for a time.
*#                 If milli_sec == 0 the suspend is until an event.
*#
*#**************************************************************************/
void  ax_delay_ms(uword milliseconds);

/*#**************************************************************************
*#
*#  FUNCTION NAME: ax_malloc, ax_realloc, ax_calloc, ax_free, ax_coreleft
*#
*#  PARAMETERS:    See declaration of standard allocation functions
*#
*#  RETURNS:       -"-
*#
*#  SIDE EFFECTS:  -"-
*#
*#  DESCRIPTION:   Dynamic heap access.
*#
*#**************************************************************************/
/*#define MALLOC_TEST*/
#ifdef MALLOC_TEST

void    *ax_dmalloc(udword amount, char *, unsigned);
void    *ax_drealloc(void *origin, udword amount, char *, unsigned);
void    *ax_dcalloc(udword items, udword size, char *, unsigned);
void     ax_dfree(void *origin, char *, unsigned);
udword   ax_coreleft(void);
bool     ax_dalloccheck(bool listall);

#if 1   /* Set to 1 when using the functions in system.c . */
        /* (normal operation)  */
#define ax_malloc(n)       ax_dmalloc(n, __FILE__, __LINE__)
#define ax_realloc(p, n)   ax_drealloc(p, n, __FILE__, __LINE__)
#define ax_calloc(i,s)     ax_dcalloc(i, s, __FILE__, __LINE__)
#define ax_free(p)         ax_dfree(p, __FILE__, __LINE__)

#else

#define ax_malloc(n)       admalloc(n, __FILE__, __LINE__)
#define ax_realloc(p, n)   adrealloc(p, n, __FILE__, __LINE__)
#define ax_calloc(i,s)     adcalloc(i, s, __FILE__, __LINE__)
#define ax_free(p)         adfree(p, __FILE__, __LINE__)
#define ax_coreleft()      acoreleft()

#endif

#define allocstart()     dallocstart()
#define alloccheck()     ax_dalloccheck(FALSE)   /* Test errors only */
#define allocend()       dallocend()
#define alloctab()       ax_dalloccheck(TRUE)

#else

#if 1   /* Set to 1 when using the functions in SYSTEM.C . */
        /* (normal operation)  */

void    *ax_malloc(udword amount);
void    *ax_realloc(void *origin, udword amount);
void    *ax_calloc(udword items, udword size);
void    ax_free(void *origin);
udword  ax_coreleft(void);

#else

#define ax_malloc(n)       amalloc(n)
#define ax_realloc(p, n)   arealloc(p, n)
#define ax_calloc(i,s)     acalloc(i, s)
#define ax_free(p)         afree(p)
#define ax_coreleft()      acoreleft()

#endif

#define allocstart()
#define alloccheck()
#define allocend()
#define alloctab()

#endif

udword  ax_coreleft(void);
udword  ax_coreleft_total(void);

/*#**************************************************************************
*#
*#  FUNCTION NAME:   File function protos
*#
*#  PARAMETERS:
*#
*#  RETURNS:
*#
*#  SIDE EFFECTS:
*#
*#  DESCRIPTION:
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE         NAME             CHANGES
*#  ----         ----             -------
*#  Apr 04 1995  Mart Roomus      - One common header
*#
*#        (C) 1995 Axis Communications AB, Lund, Sweden
*#**************************************************************************/
void   *ax_fopen(const char *name, const char *mode);
int     ax_fclose(void *file);
int     ax_fgetc(void *file);
int     ax_fseek(void* stream, long offset, int whence);
int     ax_fputc(int data, void *file);
udword  ax_fread(void *ptr, udword n_bytes, udword blk_size, void *file);
udword  ax_fwrite(void *ptr, udword n_bytes, udword blk_size, void *file);

word    ax_fprintf(void *fileptr, const char *format, ...);

/*#**************************************************************************
*#
*#  FUNCTION NAME: ax_printf / ax_sprintf / ax_vsprintf
*#
*#  PARAMETERS:    See declaration of standard printf
*#
*#  RETURNS:       -"-
*#
*#  SIDE EFFECTS:  -"-
*#
*#  DESCRIPTION:   Formatted printing
*#
*#**************************************************************************/
#ifdef PC_DEBUG
void osys_win32_debug_printf(const char*, ...);
#define ax_printf osys_win32_debug_printf
#else
int     ax_printf(const char *format, ...);
#endif
int     ax_sprintf(char *buf, const char *format, ...);
int     ax_vsprintf(char *buf, const char *format, va_list arglist);

/*#**************************************************************************
*#
*#  FUNCTION NAME: ax_getchar
*#
*#  PARAMETERS:    See declaration of standard getchar
*#
*#  RETURNS:       -"-
*#
*#  SIDE EFFECTS:  -"-
*#
*#  DESCRIPTION:   Used to read one character from standard input.
*#
*#**************************************************************************/
word    ax_getchar(void);

/*#**************************************************************************
*#
*#  FUNCTION NAME: ax_exact_delay
*#
*#  PARAMETERS:    None
*#
*#  RETURNS:       Delay time
*#
*#  SIDE EFFECTS:  None
*#
*#  DESCRIPTION:   Waits a project specific time that must lie between 1000
*#                 and 10000 micro seconds. Used to time check_interface_type.
*#
*#**************************************************************************/
udword  ax_exact_delay(void);

/*#**************************************************************************
*#
*#  FUNCTION NAME: ax_error
*#
*#  PARAMETERS:    task:     Reporting task
*#                 errclass: Error class (AX_WARNING, AX_ERROR, AX_FATAL, 
*#                           AX_UNKNOWN)
*#                 errcode:  Error code
*#                 text:     Extra text (NULL allowed)
*#
*#  RETURNS:       Delay time
*#
*#  SIDE EFFECTS:  None
*#
*#  DESCRIPTION:   Used to report software errors / warnings
*#
*#**************************************************************************/
/* Error codes must be defined sequentially (ref error_class text list     */
/* in SYSTEM.C)                                                            */
/* AX_FATAL & AX_UNKNOWN must always have the highest codes                */

#define AX_WARNING   0
#define AX_ERROR     1
#define AX_FATAL     2
#define AX_UNKNOWN   3

void    ax_error(uword task, uword errclass, uword errcode, char *text);

#endif /* system_h */

/********************** END OF FILE system.h *******************************/
