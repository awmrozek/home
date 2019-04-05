/*!**************************************************************************
*!
*!  FILE NAME:      system.c
*!
*!  DESCRIPTION:    System (ax_xxxx) functions for NPS boxes
*!
*!
*!  FUNCTIONS:      ax_printf    (Global)
*!                  ax_fprintf   (Global)
*!                  ax_fopen     (Global)
*!                  ax_fclose    (Global)
*!                  ax_fread     (Global)
*!                  ax_fwrite    (Global)
*!                  ax_fseek     (Global)
*!
*!                  ax_malloc    (Global)
*!                  ax_calloc    (Global)
*!                  ax_realloc   (Global)
*!                  ax_free      (Global)
*!
*!---------------------------------------------------------------------------
*!  HISTORY
*!
*!  DATE    NAME            CHANGES
*!  ----    ----            -------
*!  940317  KJ              Initial version
*!  940325  KJ              ax_printf handles os_started.
*!  940328  RW              Added ax_fprintf, ax_sprintf. Linted the code.
*!  940419  RW              Removed ax_sprintf, since sprintf is reentrant.
*!  940420  IP              Removed os_started. Intro os_is_started function.
*!  940422  IP              Include projos.h
*!  940602  AB              Printing of "Prnt_dsk" and "Idle" removed
*!  941007  WS              Added ax_malloc and ax_free.
*!  941213  WS              Added realloc
*!
*! Jan 16 1995   Willy Sagefalk    Changed parameters to os_send_sync
*! Jan 17 1995   Willy Sagefalk    Made ax_printf work with new mail structure
*! Jan 17 1995   Mart Roomus       Added ax_calloc. Modified alloc primitives,
*!                                 now calling amalloc() etc.
*!                                 Malloc test versions of the allocation
*!                                 functions, ax_dmalloc() etc.
*!                                 Includes 'ax_alloc.h'
*!                                 Added ax_coreleft()
*! Jan 20 1995   Mart Roomus       Common alloc_access function
*! Jan 31 1995   Mart Roomus       Added ax_dalloccheck().
*! Mar 21 1995   Jens Johansson    Removed include ether.h.
*! Apr 03 1995   Mart Roomus       Added all those file functions announced
*!                                 in system.h . ax_fopen() etc.
*! Apr 04 1995   Mart Roomus       Corrected bugs in above functions.
*!                                 Removed os_stop() if unknown syncmail.
*! Apr 04 1995   Mart Roomus       Corrected fread/ fwrite return values.
*!                                 Corrected FSEEK action id.
*! Apr 19 1995   Mart Roomus       Renamed RESTORE() to ENABLE().
*! Jul 31 1995   Stefan Jonsson    Added NIC reset in idle_main, when it hangs.
*! Aug 01 1995   Jens Johansson    Added ax_coreleft_total().
*! Aug 10 1995   Stefan Jonsson    Removed NIC reset from idle_main.
*! Oct 12 1995   H-P Nilsson       Pruned code.
*! Oct 12 1995   H-P Nilsson       Goofed -- added os_get_sync() to
*!                                 printf-disk even when not debugging
*!                                 (else will hang lower priorities) 
*! Oct 13 1995   Per Flock         Changed flash load macro names
*! Feb 26 1996   Fredrik Norrman   Added __eprintf for assert() support
*! Feb 28 1996   Fredrik Norrman   Added #ifdef for __eprintf
*! Apr 15 1996   Fredrik Norrman   Added more info in __eprintf
*! Apr 30 1996   Willy Sagefalk    Fixed ifdef for flash
*! Jul  5 1996  Fredrik Norrman    Added time dummy function
*! Aug 27 1996  Fredrik Svensson   Added dump of memory and statistics in
*!                                 __eprintf
*! Oct 29 1996  Fredrik Sjoholm    Telnet debug. (ax_printf changed)
*!                                 please learn about globalStdIO structure
*!                                 (system.c and system.h)
*! Oct 31 1996  Fred Jonsson       Fixed function missmatch between decl. and
*!                                    def. of ax_read and ax_fwrite.
*! Nov 21 1996  Willy Sagefalk     Changed default printf output to serial
*! Nov 25 1996  Fredrik Svensson   Changed DISABLE / ENABLE to
*!                                 DISABLE_SAVE / RESTORE in alloc_access 
*!
*!-------------------------------------------------------------------------
*!
*!        (C) 1994 Axis Communications AB, Lund, Sweden
*!
*!**************************************************************************/
/* @(#) system.c 1.38 12/31/96 */

/**************************  INCLUDE FILES  ********************************/
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#include "compiler.h"                          /* Compiler dependent defs. */
#include "system.h"
#include "projos.h"
#include "osys.h"
#include "sp_alloc.h"                     /* Non-reentrant heap functions. */
#include "xprintf.h"


/**************************  CONSTANTS  ************************************/

enum { OUTPRINT, FILE_OUTPRINT, FILE_FOPEN, FILE_FCLOSE, FILE_FREAD,
       FILE_FWRITE, FILE_FSEEK, FILE_FPUTC};


/**************************  TYPE DEFINITIONS  *****************************/
typedef struct print_message
{
  FILE             *fileptr;      /* For fprintf */
  const byte       *format;
  void             *arg_list;
  word             return_val;
} print_message;

typedef struct                    /* fopen parameters */
{
  FILE          *stream;
  const char    *name;
  const char    *mode;
} fopen_type;

typedef struct                    /* fclose parameters */
{
  FILE          *stream;
  int           ret;
} fclose_type;

typedef struct                    /* fread parameters */
{
  FILE          *stream;
  void          *ptr;
  uword         size;             /* block size */
  uword         blocks;           /* number of blocks */
} fread_type;

typedef struct                    /* fwrite parameters */
{
  FILE          *stream;
  void          *ptr;
  uword         size;             /* block size */
  uword         blocks;           /* number of blocks */
} fwrite_type;

typedef struct                    /* fseek parameters */
{
  FILE          *stream;
  long          offset;
  int           whence;
  int           ret;
} fseek_type;

typedef struct                    /* fseek parameters */
{
  FILE          *stream;
  int           chr;
  int           ret;
} fputc_type;


/**************************  EXTERNALS  ************************************/
extern byte executing_task;              /* Current executing task in OS */
extern FILE *prlogfile;


/* default to serial debug output */
#if defined(DEBUG) || !defined(FLASH_GLOW) /* Only here when debugging */
struct GlobalStdIO globalStdIO = { STDIO_SERIAL,STDIO_SERIAL,NULL,NULL };
#endif

/**************************  EXTERNAL FUNCTIONS  ***************************/



/**************************  LOCALS  ***************************************/
static volatile bool ax_alloc_busy=FALSE;

#if defined(DEBUG) || !defined(FLASH_GLOW) /* Only here when debugging */

/*#***************************************************************************
*# FUNCTION NAME: ax_printf
*#
*# PARAMETERS   : see printf lib function.
*#
*# RETURNS      : see lib function.
*#
*# SIDE EFFECTS : see lib function.
*#
*# DESCRIPTION  : prints messages to default output. telnet, serial, or null
*#
*#----------------------------------------------------------------------------
*# HISTORY
*# 
*# DATE         NAME               CHANGES
*# ----         ----               -------
*# 1996 Oct 29  Fredrik Sjoholm    Added telnet debug support
*#                                 sorry for multiple returns, the print_task
*#                                 does mysterious things to the structure
*#                                 passed down, had better return what's right
*# 
*#***************************************************************************/


int ax_printf(const char *format, ...)
{
  va_list ap;
  print_message print_ptr;

  switch( globalStdIO.outDevice )
  {
   case STDIO_NULL: /* /dev/null */
     return 0;

   case STDIO_SERIAL:
     va_start(ap,format);
     print_ptr.format = format;
     print_ptr.arg_list = ap;
     if ( os_is_started() )
     {
       if (os_send_sync(WAIT_FOREVER, (uword)(executing_task<<8),
                        (uword)(PRINTF_DISK_TASK<<8), OUTPRINT, TYPE_POINTER, 0L,
                        (void*)&print_ptr) == FALSE)
       {
         va_end(ap);
         os_stop();
       }
     }
     else
     {
       print_ptr.return_val = vprintf(print_ptr.format,print_ptr.arg_list);
     }
     va_end(ap);
     return print_ptr.return_val;
   default:
     return 0;
  }
}


/*#************************************************************************
*#
*# FUNCTION NAME: word ax_fprintf()
*#
*# PARAMETERS: Se fprintf lib funktion.
*#
*# RETURNS: Se lib function.
*#
*# SIDE EFFECTS:
*#
*# DESCRIPTION: Used to print messages on any filehandle.
*#
*#**************************************************************************/
word ax_fprintf(VOID *fileptr, const char *format, ...)
{
  va_list ap;

  print_message print_ptr;
 
  print_ptr.fileptr = fileptr;
  print_ptr.format = format;
 
  va_start(ap,format);
  print_ptr.arg_list = ap;

  if ( os_is_started() )
  {
    if (os_send_sync(WAIT_FOREVER, (uword)(executing_task<<8),
                     (PRINTF_DISK_TASK<<8),
                     FILE_OUTPRINT, TYPE_POINTER, 0L,
                     (void*)&print_ptr) == FALSE)
    {
      va_end(ap);
      os_stop();
    }
  }
  else
  {
    print_ptr.return_val = vfprintf(print_ptr.fileptr, print_ptr.format,
                                    print_ptr.arg_list);
  }
  va_end(ap);

  return print_ptr.return_val;
}

#ifdef HW_PC

/*#**************************************************************************
*#
*#  FUNCTION NAME:  ax_fopen(name, mode)
*#
*#  PARAMETERS:     const char *name - File name
*#                  const char *mode - File mode
*#
*#  RETURNS:        FILE *, file handle
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
*#  Apr 03 1995  Mart Roomus      - Initial version
*#
*#        (C) 1995 Axis Communications AB, Lund, Sweden
*#**************************************************************************/

void *ax_fopen(const char *name, const char *mode)
{
fopen_type params;

  if ( os_is_started() )
  {
    params.name = name;
    params.mode = mode;

    while (os_send_sync(WAIT_FOREVER, (uword)(executing_task<<8),
                     (PRINTF_DISK_TASK<<8),
                     FILE_FOPEN, TYPE_POINTER, 0L,
                     (void*)&params) == FALSE)
    {
      os_delay(SEC_TICS);
    }
  }
  else
  {
    params.stream = fopen(name, mode);
  }

  return (void*)params.stream;
}

/*#**************************************************************************
*#
*#  FUNCTION NAME:  ax_fputc(chr, stream)
*#
*#  PARAMETERS:     int     chr
*#                  FILE    *stream;
*#
*#  RETURNS:        int
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
*#  Apr 03 1995  Mart Roomus      - Initial version
*#
*#        (C) 1995 Axis Communications AB, Lund, Sweden
*#**************************************************************************/

int ax_fputc(int chr, void *stream)
{
fputc_type params;

  if ( os_is_started() )
  {
    params.chr = chr;
    params.stream = (FILE*)stream;

    while (os_send_sync(WAIT_FOREVER, (uword)(executing_task<<8),
                     (PRINTF_DISK_TASK<<8),
                     FILE_FPUTC, TYPE_POINTER, 0L,
                     (void*)&params) == FALSE)
    {
      os_delay(SEC_TICS);
    }
  }
  else
  {
    params.ret = fputc(chr, (FILE*)stream);
  }

  return params.ret;
}

/*#**************************************************************************
*#
*#  FUNCTION NAME:  ax_fclose(stream)
*#
*#  PARAMETERS:     FILE *stream - File stream to close
*#
*#  RETURNS:        int, same as 'fclose'
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
*#  Apr 03 1995  Mart Roomus      - Initial version
*#
*#        (C) 1995 Axis Communications AB, Lund, Sweden
*#**************************************************************************/

int ax_fclose(void *stream)
{
fclose_type params;

  if ( os_is_started() )
  {
    params.stream = (FILE*)stream;

    while (os_send_sync(WAIT_FOREVER, (uword)(executing_task<<8),
                     (PRINTF_DISK_TASK<<8),
                     FILE_FCLOSE, TYPE_POINTER, 0L,
                     (void*)&params) == FALSE)
    {
      os_delay(SEC_TICS);
    }
  }
  else
  {
    params.ret = fclose((FILE*)stream);
  }

  return params.ret;
}

/*#**************************************************************************
*#
*#  FUNCTION NAME:  ax_fread(ptr, size, blocks, stream)
*#
*#  PARAMETERS:     void    *ptr    -  Data pointer
*#                  uword   size    -  Block size
*#                  uword   blocks  -  Number of blocks
*#                  void    *stream -  The file
*#
*#  RETURNS:        uword  , number of blocks read
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
*#  Apr 03 1995  Mart Roomus      - Initial version
*#
*#        (C) 1995 Axis Communications AB, Lund, Sweden
*#**************************************************************************/

udword  ax_fread(void *ptr, udword size, udword blocks, void *stream)
{
fread_type  params;

  if ( os_is_started() )
  {
    params.ptr = ptr;
    params.size = size;
    params.blocks = blocks;
    params.stream = (FILE*)stream;
    while (os_send_sync(WAIT_FOREVER, (uword)(executing_task<<8),
                     (PRINTF_DISK_TASK<<8),
                     FILE_FREAD, TYPE_POINTER, 0L,
                     (void*)&params) == FALSE)
    {
      os_delay(SEC_TICS);
    }
  }
  else
  {
    params.blocks = fread(ptr, (size_t)size, (size_t)blocks, (FILE*)stream);
  }
  return params.blocks;
}

/*#**************************************************************************
*#
*#  FUNCTION NAME:  ax_fwrite(ptr, size, blocks, stream)
*#
*#  PARAMETERS:     void    *ptr    -  Data pointer
*#                  uword   size    -  Block size
*#                  uword   blocks  -  Number of blocks
*#                  void    *stream -  The file
*#
*#  RETURNS:        uword  , number of blocks read
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
*#  Apr 03 1995  Mart Roomus      - Initial version
*#
*#        (C) 1995 Axis Communications AB, Lund, Sweden
*#**************************************************************************/

udword  ax_fwrite(void *ptr, udword size, udword blocks, void *stream)
{
fwrite_type  params;

  if ( os_is_started() )
  {
    params.ptr = ptr;
    params.size = size;
    params.blocks = blocks;
    params.stream = (FILE*)stream;
    while (os_send_sync(WAIT_FOREVER, (uword)(executing_task<<8),
                     (PRINTF_DISK_TASK<<8),
                     FILE_FWRITE, TYPE_POINTER, 0L,
                     (void*)&params) == FALSE)
    {
      os_delay(SEC_TICS);
    }
  }
  else
  {
    params.blocks = fwrite(ptr, (size_t)size, (size_t)blocks, (FILE*)stream);
  }
  return params.blocks;
}

/*#**************************************************************************
*#
*#  FUNCTION NAME:  ax_fseek(stream, offset, whence)
*#
*#  PARAMETERS:     void    *stream -  The file
*#                  long    offset  -  diff between whence & new pos
*#                  int     whence  -  Seek position
*#
*#  RETURNS:        int  , number of blocks read
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
*#  Apr 03 1995  Mart Roomus      - Initial version
*#
*#        (C) 1995 Axis Communications AB, Lund, Sweden
*#**************************************************************************/

int  ax_fseek(void *stream, long offset, int whence)
{
fseek_type  params;

  if ( os_is_started() )
  {
    params.stream = (FILE*)stream;
    params.offset = offset;
    params.whence = whence;

    while (os_send_sync(WAIT_FOREVER, (uword)(executing_task<<8),
                     (PRINTF_DISK_TASK<<8),
                     FILE_FSEEK, TYPE_POINTER, 0L,
                     (void*)&params) == FALSE)
    {
      os_delay(SEC_TICS);
    }
  }
  else
  {
    params.ret = fseek((FILE*)stream, offset, whence);
  }
  return params.ret;
}

#endif

#endif /* debug or not pruned */


/*#*************************************************************************
*#
*# FUNCTION NAME: void printf_disk_main()
*#
*# PARAMETERS: void
*#
*# RETURNS: Nothing.
*#
*# DESCRIPTION: Main function for printf and disk handling task.
*#
*#************************************************************************/
void ax_printf_disk_main(void)
{
  mail_struct mail_message;

#if defined(DEBUG) || !defined(FLASH_GLOW)
  union
  {
    print_message *print;
    fwrite_type   *fwr;
    fread_type    *frd;
    fopen_type    *fop;
    fclose_type   *fcl;
    fseek_type    *fsk;
    fputc_type    *fpc;
    void          *p;
  } ptr;

  /*lint -e716 */
  while(TRUE)
  /*lint +e716 */
  {
    if (os_get_sync(WAIT_FOREVER,&mail_message) == TRUE)
    {
      ptr.p = mail_message.param_ptr;  /* Assign all pointers in the union */

      switch (mail_message.action_id)
      {
        case OUTPRINT:
          ptr.print->return_val = vprintf(ptr.print->format,
                                          ptr.print->arg_list);
          break;

        case FILE_OUTPRINT:
          ptr.print->return_val = vfprintf(ptr.print->fileptr,
                                           ptr.print->format,
                                           ptr.print->arg_list);
          break;
#ifdef HW_PC
        case FILE_FOPEN:
          ptr.fop->stream = fopen(ptr.fop->name, ptr.fop->mode);
          break;

        case FILE_FCLOSE:
          ptr.fcl->ret = fclose(ptr.fcl->stream);
          break;

        case FILE_FREAD:
          ptr.frd->blocks = fread(ptr.frd->ptr, ptr.frd->size,
                                ptr.frd->blocks, ptr.frd->stream);
          break;

        case FILE_FWRITE:
          ptr.fwr->blocks = fwrite(ptr.fwr->ptr, ptr.fwr->size,
                                 ptr.fwr->blocks, ptr.fwr->stream);
          break;

        case FILE_FSEEK:
          ptr.fsk->ret = fseek(ptr.fsk->stream, ptr.fsk->offset,
                               ptr.fsk->whence);
          break;

        case FILE_FPUTC:
          ptr.fpc->ret = fputc(ptr.fpc->chr, ptr.fpc->stream);
          break;
#endif

        default:
          printf("\aWarning: Unknown action in 'ax_printf_disk_main'\n");
          break;

      }
      os_end_sync();
    }
  }

#else
  /* No debugging, just discard sync mails */
  while (TRUE)
  {
    if (os_get_sync(WAIT_FOREVER,&mail_message) == TRUE)
    {
      os_end_sync();
    }
  }

#endif /* debug or not pruned */
}


/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: void idle_main()                                         */
/*#                                                                         */
/*# PARAMETERS: void                                                        */
/*#                                                                         */
/*# RETURNS: Nothing.                                                       */
/*#                                                                         */
/*# DESCRIPTION: Main function for idle task.                               */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE          NAME             CHANGES                                  */
/*# ----          ----             -------                                  */
/*# 921019        Bernt B          Creation.                                */
/*#-------------------------------------------------------------------------*/
extern void enable_irqx(void);

void idle_main(void)
{
  byte n;
#ifdef HW_PC
  uword os_counter;
  uword current_timer;
  uword err_count = 0;
#endif

  for (n = 0; n < N_TASKS; n++)
    os_stack(n);

  while (TRUE)
  {
#ifdef hw_PC
    if (get_NIC_status() == 0)
    {
      err_count = 0;
    }

    current_timer = os_get_time();
    if (os_counter != current_timer)
    {
      os_counter = current_timer;
      if (get_NIC_status() != 0)
      {
        err_count++;
        if (err_count > 4)
        {
          reset_NIC();
        }
      }
    }
#endif
  }
}

/*#**************************************************************************
*#
*#  FUNCTION NAME:  alloc_access()
*#
*#  PARAMETERS:     None
*#
*#  RETURNS:        Nothing
*#
*#  SIDE EFFECTS:   Sets ax_alloc_busy = TRUE
*#
*#  DESCRIPTION:    Resolve heap access collisions
*#
*#
*#**************************************************************************/
static void alloc_access(void)
{
  bool busy;

  do
  {
    DISABLE_SAVE();
    busy = ax_alloc_busy;
    if (!busy)
    {
      ax_alloc_busy = TRUE;
    }
    RESTORE();
    if (busy)
    {
      os_delay(1);
    }
  } while (busy);
}


#ifndef MALLOC_TEST   /* STD functions */
/*#************************************************************************
*#
*# FUNCTION NAME:  ax_malloc
*#
*# PARAMETERS:
*#
*# RETURNS:
*#
*# DESCRIPTION:    
*#
*#-------------------------------------------------------------------------
*#
*# HISTORY
*#
*# DATE    NAME  CHANGES
*# ----    ----  -------
*# 941007  WS    Created
*# 941213  WS    Added reentrance protection
*#**********************************************************************#*/
void  *ax_malloc(udword amount)
{
  void       *retval;

  alloc_access();
  retval = amalloc(amount);
  ax_alloc_busy = FALSE;

  return retval;
}


/*#************************************************************************
*#
*# FUNCTION NAME:  ax_calloc
*#
*# PARAMETERS:
*#
*# RETURNS:
*#
*# DESCRIPTION:
*#
*#-------------------------------------------------------------------------
*#
*# HISTORY
*#
*# DATE         NAME            CHANGES
*# ----         ----            -------
*# Jan 17 1995  Mart Roomus     Created
*#
*#**********************************************************************#*/
void  *ax_calloc(udword items, udword size)
{
  void       *retval;

  alloc_access();
  retval = acalloc(items, size);
  ax_alloc_busy = FALSE;
  return retval;
}


/*#************************************************************************
*#
*# FUNCTION NAME:  ax_realloc
*#
*# PARAMETERS:
*#
*# RETURNS:
*#
*# DESCRIPTION:
*#
*#-------------------------------------------------------------------------
*#
*# HISTORY
*#
*# DATE    NAME  CHANGES
*# ----    ----  -------
*# 941007  WS    Created
*# 941213  WS    Added reentrance protection
*#**********************************************************************#*/
void  *ax_realloc(VOID *origin, udword amount)
{
  void       *retval;

  alloc_access();
  retval = arealloc(origin,amount);
  ax_alloc_busy = FALSE;

  return retval;
}


/*#************************************************************************
*#
*# FUNCTION NAME:  ax_free
*#
*# PARAMETERS:
*#
*# RETURNS:
*#
*# DESCRIPTION:
*#
*#-------------------------------------------------------------------------
*#
*# HISTORY
*#
*# DATE    NAME  CHANGES
*# ----    ----  -------
*# 941007  WS    Created
*# 941213  WS    Added reentrance protection
*#**********************************************************************#*/
void    ax_free(void *origin)
{
  alloc_access();
  (void)afree(origin);
  ax_alloc_busy = FALSE;
}

#else  /* MALLOC_TEST */

/*#**************************************************************************
*#
*#  FUNCTION NAME:   Debug versions of alloc functions
*#
*#  PARAMETERS:      default + char *file & unsigned line
*#
*#  RETURNS:
*#
*#  SIDE EFFECTS:
*#
*#  DESCRIPTION:
*#
*#
*#**************************************************************************/
void  *ax_dmalloc(udword amount, char *file, unsigned line)
{
  void       *retval;

  alloc_access();
  retval = admalloc(amount, file, line);
  ax_alloc_busy = FALSE;

  return retval;
}


void  *ax_dcalloc(udword items, udword size, char *file, unsigned line)
{
  void       *retval;

  alloc_access();
  retval = adcalloc(items, size, file, line);
  ax_alloc_busy = FALSE;
  return retval;
}


void  *ax_drealloc(VOID *origin, udword amount, char *file, unsigned line)
{
  void       *retval;

  alloc_access();
  retval = adrealloc(origin,amount, file, line);
  ax_alloc_busy = FALSE;

  return retval;
}


void    ax_dfree(void *origin, char *file, unsigned line)
{
  alloc_access();
  adfree(origin, file, line);
  ax_alloc_busy = FALSE;
}


bool ax_dalloccheck(bool x)        /* Debug info function */
{

  alloc_access();
  x = dalloccheck(x);
  ax_alloc_busy = FALSE;

  return x;
}

#endif   /* MALLOC_TEST */

/*#************************************************************************
*#
*# FUNCTION NAME:  ax_coreleft
*#
*# PARAMETERS:     None
*#
*# RETURNS:        udword
*#
*# DESCRIPTION:    Return number of free RAM bytes
*#
*#-------------------------------------------------------------------------
*#
*# HISTORY
*#
*# DATE         NAME            CHANGES
*# ----         ----            -------
*# Jan 17 1995  Mart Roomus     Created
*#
*#**********************************************************************#*/
udword  ax_coreleft(void)
{
  udword n;

  alloc_access();
  n = acoreleft();
  ax_alloc_busy = FALSE;
  return n;
}


/*#**************************************************************************
*#
*# FUNCTION NAME: ax_coreleft_total
*#
*# PARAMETERS   : None
*#
*# RETURNS      : udword
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  : Return size of memory left.
*#
*#---------------------------------------------------------------------------
*# HISTORY                                                    
*#
*# DATE          NAME             CHANGES                       
*# ----          ----             -------                       
*# Aug 01 1995   Jens Johansson   Initial version.
*#
*#**************************************************************************/
udword ax_coreleft_total(void)
{
  udword n;

  alloc_access();
  n = acoreleft_total();
  ax_alloc_busy = FALSE;
  return n;
}

/*#**************************************************************************
*#
*# FUNCTION NAME: __eprintf
*#
*# PARAMETERS   : None
*#
*# RETURNS      : 
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  : Help function for assert(). Prints out the error message
*#                and halts program execution.
*#
*#---------------------------------------------------------------------------
*# HISTORY                                                    
*#
*# DATE          NAME             CHANGES                       
*# ----          ----             -------                       
*# Feb 26 1996   Fredrik Norrman  Initial version.
*# Apr 15 1996   Fredrik Norrman  Added more info in output
*#
*#**************************************************************************/
#if defined(DEBUG) || !defined(FLASH_GLOW) /* Only here when debugging */

#if CD_SERVER && !defined(FLASH_GLOW)
extern void writeMemoryFileToOutput();
extern void writeStatFileToOutput();
#endif

void __eprintf (const char *fmt, const char *file, int line, const char *expr)
{
  (void)ax_printf(fmt, file, line, expr);
  
  /* Print memory statistics */
  ax_printf("\nMemory statistics:\n");
  ax_printf("Memory left (unfragmented): %d\n", ax_coreleft());
  ax_printf("Memory left (total)       : %d\n", ax_coreleft_total());

#if CD_SERVER && !defined(FLASH_GLOW)
  /* For CD-server, dump memory pool statistics */
  ax_printf("\nmemory.txt:\n");
  writeMemoryFileToOutput();
  ax_printf("\nstat.txt:\n");
  writeStatFileToOutput();
#endif
  
  while (1)
  {
    /* This is a never ending loop */
  }
}
#endif

/* Newer toolchains use this function instead. */
void __assert (const char *file, int line, const char *expr)
{
  __eprintf ("assertion failed: file \"%s\", line %d, \"%s\"\n",
	     file, line, expr);
}

/*#**************************************************************************
*#
*# FUNCTION NAME: time
*#
*# PARAMETERS   : None
*#
*# RETURNS      : 
*#
*# SIDE EFFECTS : 
*#
*# DESCRIPTION  : Dummy implementation of standard <time.h> function
*#
*#---------------------------------------------------------------------------
*# HISTORY                                                    
*#
*# DATE          NAME             CHANGES                       
*# ----          ----             -------                       
*# Jul 05 1996   Fredrik Norrman  Initial version.
*#
*#**************************************************************************/

time_t time(time_t *t)
{
  return 0;
}

/********************** END OF FILE system.c ***************************/
