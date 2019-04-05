/*!**************************************************************************
*!
*! FILE NAME: OSYS.H
*!
*! DESCRIPTION: Header file for os functions and OS usage.
*!
*!------------------------------------------------------------------------
*!
*! HISTORY
*!
*! DATE    NAME      CHANGES
*! ----    ----      -------
*! 921009  Bernt B   Creation
*! 921116  Bernt B   Added os_int_send (support for int mail).
*! 931102  KJ        Moved out all OSYS internal definitions
*! 940215  IP        Renamed hw-init function get_sw_int_vect
*!                   to init_hw_environment
*! 940420  IP        Added proto for os_is_started
*! 940422  IP        Removed inclusion of projos.h
*! 941031  BB        Added os_xxxxx_timer functions.
*! 950116  BB        Changed channel_id to src_id and dst_id, revised timer
*!                   functions, remove is now automatic when event mails are
*!                   read by the receiver. Added MAX_DURATION_CNTR and changed
*!                   os_stack.
*!
*! Jan 16 1995   Willy Sagefalk    Changed returnvalue on os_remove_timer
*!                                 to byte
*! Jan 30 1995   Inge Persson      Added WAIT_TEN_MSECONDS and WAIT_HUNDRED_MSECONDS. 
*! Feb 27 1995   Willy Sagefalk    Added OS_ACTION_ID_TYPE,OS_SRC_ID_TYPE
*!                                 and os_dst_id_type
*! Feb 27 1995   Willy Sagefalk    Changed NO_RET and NO_PARAMS to void
*! Feb 28 1995   Willy Sagefalk    Removed ifdef PROTOTYPES
*! May 31 1995   Willy Sagefalk    Added os_event_index_type
*! Jun  1 1995   Willy Sagefalk    Added debug_remove_timer
*! Sep 08 1995   H-P Nilsson       Added os_save_state()
*! Sep 21 1995   Willy Sagefalk    Added init task stacks
*! Oct 06 1995   H-P Nilsson       Somebody changed WAIT_XXX to using
*!                                 SEC_TICS without asserting that
*!                                 SEC_TICS was defined.  Done by
*!                                 including projos.h (why not? It's
*!                                 logical to include projos in osys)  
*! May 21 1996   H-P Nilsson       Added function os_get_seconds(), so
*!                                 the caller won't need to divide
*!                                 [the value returned by
*!                                  os_get_time()] by SEC_TICS as most
*!                                 often is the case in use.
*!------------------------------------------------------------------------
*!
*! (c) Copyright 1992-1996, Axis, LUND, SWEDEN
*!
*!**************************************************************************/

/* @(#) osys.h 1.16 05/31/96 */

#ifndef _OSYS_H
#define _OSYS_H

/********************** INCLUDE FILES SECTION ******************************/
#include "projos.h"
#include "compiler.h"

#define OSYS_UDWORD_TIME

/****************************************************************************/
/*                                                                          */
/*                      TIMER INTERFACE DEFINITIONS                         */
/*                                                                          */
/****************************************************************************/
#define TIMER_NOT_REMOVED  0x00  /* No timer mail available or error. */
#define TIMER_REMOVED      0x01  /* Timer mail successfully removed. */
#define TIMER_TIMEOUT      0x03  /* Removed timer mail has timed out. */

#define EVENT_HAPPEND 0xFFFF /* File pointer for imediately forwarded event mails. */


/****************************************************************************/
/*                                                                          */
/*                           DURATION COUNTER                               */
/*                                                                          */
/****************************************************************************/
#ifdef OSYS_UDWORD_TIME
#define MAX_DURATION_CNTR  0xFFFFFFFF  /* The max value in the duration counter. */
#else
#define MAX_DURATION_CNTR  0xFFFF      /* The max value in the duration counter. */
#endif

/****************************************************************************/
/*                                                                          */
/*                           WAIT DEFINITION                                */
/*                                                                          */
/****************************************************************************/
#ifdef OSYS_UDWORD_TIME
#define WAIT_FOREVER          0xFFFFFFFF      /* Wait infinite time in OSYS. */
#else
#define WAIT_FOREVER          0xFFFF          /* Wait infinite time in OSYS. */
#endif
#define WAIT_NO_TIME          0               /* Wait no os ticks. */
#define WAIT_TEN_MSECONDS     (SEC_TICS/100)  /* Wait 10 milliseconds. */
#define WAIT_HUNDRED_MSECONDS (SEC_TICS/10)   /* Wait 100 milliseconds. */
#define WAIT_ONE_SECOND       SEC_TICS        /* Wait one second. */
                                              /* 1000 * WAIT_ONE_SECOND must fit in */
                                              /* a uword to handle parameters for timeouts */
#define WAIT_ONE_MINUTE       (SEC_TICS*60)   /* Wait one minute. */


/****************************************************************************/
/*                                                                          */
/*     DATA TYPES IN MAIL STRUCTURE MEMBER PARAM BY VALUES IN PARAM_TYPE    */
/*                                                                          */
/****************************************************************************/

/* No data in param or param_ptr. */
#define NO_DATA        0x00

/* One signed byte in param. Param_ptr not used. */
#define TYPE_SBYTE     0x01

/* One unsigned byte in param. Param_ptr not used. */
#define TYPE_BYTE      0x02

/* One signed word in param. Param_ptr not used. */
#define TYPE_WORD      0x03

/* One unsigned word in param. Param_ptr not used. */
#define TYPE_UWORD     0x04

/* One signed double word in param. Param_ptr not used. */
#define TYPE_DWORD     0x05

/* One unsigned double word in param. Param_ptr not used. */
#define TYPE_UDWORD    0x06

/* Param_ptr pointer to user defined block. Param not in use. */
#define TYPE_POINTER   0x07

/* One signed byte in param. Param_ptr pointer to user block. */
#define TYPE_P_SBYTE   0x08

/* One unsigned byte in param. Param_ptr pointer to user block. */
#define TYPE_P_BYTE    0x09

/* One signed word in param. Param_ptr pointer to user block. */
#define TYPE_P_WORD    0x0A

/* One unsigned word in param. Param_ptr pointer to user block. */
#define TYPE_P_UWORD   0x0B

/* One signed double word in param. Param_ptr pointer to user block. */
#define TYPE_P_DWORD   0x0C

/* One unsigned double word in param. Param_ptr pointer to user block. */
#define TYPE_P_UDWORD  0x0D


/****************************************************************************/
/*                                                                          */
/*                             MAIL STRUCTURE                               */
/*                                                                          */
/****************************************************************************/

typedef udword os_time_type;       

typedef uword  os_src_id_type; 
typedef uword  os_dst_id_type;
typedef uword  os_action_id_type;

typedef uword  os_event_index_type;

typedef struct mail_struct
{
  os_src_id_type      src_id;
  os_dst_id_type      dst_id;
  os_action_id_type   action_id;
  byte                param_type;
  dword               param;
  void               *param_ptr;
} mail_struct;

#ifndef HW_CGA3
/* Task definition structure */
typedef struct task_struct  /* Tasks definition structure. */
{
  char      *name;
  byte       task_num;
  byte       priority;      /* 255 indicates Interrupt task */
  void      *start_adress;
  byte      *stack_start;
  uword      stack_size;
  bool      *debug;
} task_struct;
#else
typedef struct task_struct  /* Tasks definition structure. */
{
  char      *name;
  byte       task_num;
  byte       priority;      /* 255 indicates Interrupt task */
  void      *start_adress;
  uword      segment0;      /* Data segment 8k Block */
  uword      segment1;      /* Data segment 56k Block */
  uword      segment3;      /* Code segment 64k Block */
  byte      *stack_start;
  uword      stack_size;
  bool      *debug;
} task_struct;

#endif

void   STORE_START(void);
void   RETRIEVE_START(void);
void   INIT_SEGS_INFO(void);

void   init_hw_environment(void);
void   restore_hw_environment(void);
void   get_int_vect(void);
void   put_int_vect(void);
void   init_task_stacks(void);

/****************************************************************************/
/*                                                                          */
/*                    PROTOTYPES FOR OSYS FUNCTIONS                         */
/*                                                                          */
/****************************************************************************/

/*--------------------------------------*/
/* Operating system interface functions */
/*--------------------------------------*/

os_event_index_type  os_set_timer(os_time_type      trig_period,
                                  os_src_id_type    src_id,
                                  os_dst_id_type    dst_id,
                                  os_action_id_type action_id,
                                  byte              param_type,
                                  dword             param,
                                  void             *param_ptr);
#ifdef DEBUG
#define       os_remove_timer(event_mail) debug_remove_timer(event_mail,__FILE__, __LINE__)
byte          debug_remove_timer(os_event_index_type event_mail,char *efile,uword eline);
#else
byte          os_remove_timer(os_event_index_type event_mail);
#endif

os_time_type  os_time_timer(os_event_index_type event_mail);

bool          os_int_send(os_src_id_type    src_id,
                          os_dst_id_type    dst_id,
                          os_action_id_type action_id,
                          byte              param_type,
                          dword             param,
                          void             *param_ptr);

bool          os_send(os_src_id_type    src_id,
                      os_dst_id_type    dst_id,
                      os_action_id_type action_id,
                      byte              param_type,
                      dword             param,
                      void             *param_ptr);

bool          os_get(os_time_type wait_period,mail_struct *msg);

bool          os_send_sync(os_time_type      wait_period,
                           os_src_id_type    src_id,
                           os_dst_id_type    dst_id,
                           os_action_id_type action_id,
                           byte              param_type,
                           dword             param,
                           void             *param_ptr);

bool          os_get_sync(os_time_type wait_period,mail_struct *msg);
void          os_end_sync(void);

void          os_delay(os_time_type delay_time);
os_time_type  os_get_time(void);

#ifdef __GNU_CRIS__

extern os_time_type uptime_in_seconds;

extern __inline__ os_time_type
os_get_seconds(void) { return uptime_in_seconds; }

#else  /* Not gcc and cris */

extern os_time_type
os_get_seconds(void);

#endif /* gcc and cris test */

void          os_start(void);
void          os_stop(void);
bool          os_is_started(void);

void          os_error(os_src_id_type    src_id,
                       os_dst_id_type    dst_id,
                       os_action_id_type action_id,
                       byte              param_type,
                       dword             param,
                       void             *param_ptr);

uword         os_stack(byte task_num);

os_time_type  os_save_state(void *where);
#endif
