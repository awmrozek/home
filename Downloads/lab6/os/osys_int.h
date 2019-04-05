/*!**************************************************************************
*!                                                                         
*! FILE NAME:   OSYS_INT.H                                                   
*!                                                                         
*! DESCRIPTION: Header file for os internal functions and OS usage.        
*!                                                                         
*!-------------------------------------------------------------------------
*!                                                                         
*! HISTORY                                                                 
*!                                                                         
*! DATE        NAME          CHANGES                                     
*! ----        ----          -------                                     
*! 921009      Bernt B       Creation                                    
*! 921116      Bernt B       Added os_int_send (support for int mail).   
*! 931102      KJ            Moved out all 'not internal definitions'    
*! 941031      BB            Added os_xxxxx_timer functions.              
*! 950116      BB            Changed channel_id to src_id and dst_id, revised timer
*!                           functions, remove is now automatic when event mails are
*!                           read by the receiver. Added MAX_DURATION_CNTR and
*!                           changed os_stack.
*!
*! Feb 27 1995   Willy Sagefalk    Added OS_SRC_TIME_TYPE and os_dst_id_type
*!                                 os_action_id_type
*! Mar 29 1995   Willy Sagefalk    Changed mail_box used type to byte
*! May 17 1995   Bernt B”hmer      Made general performance improvements to
*!                                 most of the os internal and external
*!                                 functions. All wait, execute lists and
*!                                 accesses to them are modified to improve
*!                                 performance. Further more the mail handling
*!                                 functions are improved to handle common
*!                                 situations in a much faster way.
*!                                 The mailbox is changed to a double linked
*!                                 list so that finding empty spots in the box
*!                                 is faster. The accesses and handling of the
*!                                 execute list is done in a different way, the
*!                                 list is now sorted in priority order with
*!                                 the highest priority to execute first in
*!                                 the list.
*! May 26 1995   Willy Sagefalk    Decreased N_EVENT_MAILBOXES to 0x100
*! May 31 1995   Willy Sagefalk    Added counter to event_mail_box_struct
*! Jun  6 1995   Willy Sagefalk    Added osys_statistics
*! Jun 16 1995   Willy Sagefalk    Moved N_MAILBOXES to project.h.
*! Oct 11 1995   H-P Nilsson       Further reduced N_EVENT_MAILBOXES
*!                                 and INC_EVENT_MAILBOXES since it
*!                                 really takes care of reallocation
*!                                 itself, and does not need a lot of
*!                                 initial space (from 256 to 64 both).
*! Oct 02 1996   H-P Nilsson       Added prototype for
*!                                 tim_int_routine() and re-inclusion guards.
*!-------------------------------------------------------------------------
*!                                                                         
*! (C) Copyright 1992, Axis, LUND, SWEDEN                                  
*!                                                                          
*!**************************************************************************/
/* @(#) osys_int.h 1.14 10/07/96 */

#ifndef _osys_int_h
#define _osys_int_h

/****************************************************************************/
/*                                                                          */
/*                     OSYS INTERNAL OPERTAING DEFINES                      */
/*                                                                          */
/****************************************************************************/
#define OSYS_TIMERS         /* Def/undef to get the osys timer functions. */
#define OSYS_UWORD_MAILBOX  /* Def/undef to make max num mailboxes byte/uword. */

#ifdef OSYS_UWORD_MAILBOX
typedef uword os_mailbox_type;
#else
typedef byte  os_mailbox_type;
#endif

typedef uword os_emailbox_type;

/****************************************************************************/
/*                                                                          */
/*                        OSYS INTERNAL STRUCTURES                          */
/*                                                                          */
/****************************************************************************/
typedef struct execute_struct  /* Tasks to execute (internal in OS). */
{
  byte                   priority;
  byte                   task;
  os_src_id_type         src_id;
  os_dst_id_type         dst_id;
  os_action_id_type      action_id;
  byte                   param_type;
  dword                  param;
  void                  *param_ptr;
  byte                   call_func;
  bool                   return_val;
  struct execute_struct *next;

} execute_struct;

typedef struct wait_struct  /* Wait structure. */
{
  bool               used;          /* TRUE if used else FALSE. */
  byte               task;
  os_time_type       delay_time;

} wait_struct;

typedef struct sync_end_struct  /* Wait for sync end structure. */
{
  bool               used;          /* TRUE if used else FALSE. */
  byte               task;
  byte               waiting_for;
  bool               collected_mail;
  os_time_type       delay_time;

} sync_end_struct;

typedef struct mail_box_struct  /* Internal structure in mailboxes (uword size). */
{
  os_mailbox_type    prev_entry;
  os_mailbox_type    next_entry;
#ifdef OSYS_TIMERS
  uword              event_ind;
#endif
  os_src_id_type     src_id;
  os_dst_id_type     dst_id;
  os_action_id_type  action_id;
  byte               param_type;
  dword              param;
  void              *param_ptr;

} mail_box_struct;

typedef struct event_mail_box_struct  /* Internal structure in event mailboxes. */
{
  byte               used;
  byte               count;           /* Increased when this box is allocated    */
                                      /* Used to get a 'unique' event_mail_index */
  os_emailbox_type   prev_entry;
  os_emailbox_type   next_entry;
  os_time_type       send_time;
  os_src_id_type     src_id;
  os_dst_id_type     dst_id;
  os_action_id_type  action_id;
  byte               param_type;
  dword              param;
  void              *param_ptr;

} event_mail_box_struct;

typedef struct func_struct    /* OS calls and returns, info transfer structure. */
{
  os_time_type       delay_time;
  os_src_id_type     src_id;
  os_dst_id_type     dst_id;
  os_action_id_type  action_id;
  byte               param_type;
  dword              param;
  void              *param_ptr;
  byte               call_func;
  bool               return_val;

} func_struct;

#ifdef DEBUG
typedef struct osys_statistics_type
{
  uword   out_of_mailslots;
  uword   illegal_remove_timer;
  udword  timers_started;
  udword  timers_removed;
  uword   used_mail_slots;
  uword   max_used_mail_slots;
} osys_statistics_type;
extern osys_statistics_type osys_statistics;
#endif

/****************************************************************************/
/*                                                                          */
/*                         OSYS INTERNAL DEFINES                            */
/*                                                                          */
/****************************************************************************/

#define OS_INT_SEND   1       /* Send mail from interrupt function call. */
#define OS_SEND       2       /* Send mail function call. */
#define OS_GET        3       /* Receive mail function call. */
#define OS_SEND_SYNC  4       /* Send syncronous mail funtion call. */
#define OS_GET_SYNC   5       /* Receive syncronous mail function call. */
#define OS_END_SYNC   6       /* End syncronous mail sequence funtion call. */
#define OS_DELAY      7       /* Suspend task function call. */
#define OS_START      8       /* Startup call. */
#define TIMER_TICK    9       /* Task interrupted by timer tick. */
#define OTHER_INT    10       /* Task interrupted by other interrupt. */
#define SCHEDULE     11       /* Tell OSYS to make a schedulation. */

#define STACK_PATTERN 0x88    /* Pattern stacks are filled with, used by stack probe. */
#define EVENT_HAPPEND 0xFFFF  /* File pointer for imediately forwarded event mails. */

/****************************************************************************/
/*                                                                          */
/*                            MAILBOX DEFINITION                            */
/*                                                                          */
/****************************************************************************/

#define N_EVENT_MAILBOXES       64  /* Number of mailboxes to store event mail in (max value is one uword). */
#define INC_EVENT_MAILBOXES     64  /* Increment when first boxes are out. */

#define BOX_FREE                 0x00
#define BOX_USED                 0x01
#define EVENT_TOUT               0x55  /* Put in event_box->used when event timed out. */
#define EVENT_REMOVED            0xAA  /* Put in event_box->used when timed out but not read and removed. */

#if 0
#ifdef OSYS_UWORD_MAILBOX 

#define N_MAILBOXES             0x200  /* Number of mailboxes to store mail in (max value is one uword). */

#else

#define N_MAILBOXES              0xFF  /* Number of mailboxes to store mail in (max is 255 (max value in one byte)). */

#endif
#endif
#define N_SYNC_MAILBOXES (N_TASKS + 1) /* Number of sync mailboxes to store sync mail in (Never any need for more than this). */


/********************** EXPORTED VARIABLE DECLARATION SECTION **************/
extern byte executing_task;

/********************** EXPORTED FUNCTION DECLARATION SECTION **************/
extern void tim_int_routine(void);
extern void other_int_routine(void);
extern void sw_int_routine(void);

#endif /* _osys_int_h */

/* END OF osys_int.h */
