/*!*************************************************************************
*!    
*! FILE NAME: OSYS_INT.C
*!
*! DESCRIPTION: This module contains the operating system internal funcs.
*!
*! FUNCTIONS:   sw_int_routine      (Operating system internal function)
*!              tim_int_routine     (Operating system internal function)
*!              int_send            (Operating system internal function)
*!              send                (Operating system internal function)
*!              get                 (Operating system internal function)
*!              send_sync           (Operating system internal function)
*!              get_sync            (Operating system internal function)
*!              end_sync            (Operating system internal function)
*!              task_delay          (Operating system internal function)
*!              start               (Operating system internal function)
*!              os_tick             (Operating system internal function)
*!              sync_end_check      (Operating system internal function)
*!              get_next_exec       (Operating system internal function)
*!              put_mail            (Operating system internal function)
*!              event_put_mail      (Operating system internal function)
*!              get_mail            (Operating system internal function)
*!              put_sync_mail       (Operating system internal function)
*!              get_sync_mail       (Operating system internal function)
*!              find_sync_mail      (Operating system internal function)
*!
*!------------------------------------------------------------------------
*!
*! HISTORY
*!
*! DATE    NAME      CHANGES
*! ----    ----      -------
*! 921012  Bernt B   Creation
*! 921116  Bernt B   Added os_int_send (support for int mail).
*! 940422  IP        Include projos.h
*! 940518  BB/IP     get_mail and get_sync_mail now returns immediately
*!                   when no mails available and timeout is 0.
*! 940518  MR        Changed "dos.h" to <dos.h> to make mgmake happy
*! 941031  BB        Added os_xxxxx_timer functions.
*! 950116  BB        Changed channel_id to src_id and dst_id, revised timer
*!                   functions, remove is now automatic when event mails are
*!                   read by the receiver. Added MAX_DURATION_CNTR and changed
*!                   os_stack.
*!
*! Feb 28 1995   Willy Sagefalk    Use os_time_type
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
*! May 29 1995  Hans-Peter Nilsson Change in tim_int_routine().  Does
*!                                 not assume that the current task is
*!                                 the one with the highest priority; now
*!                                 but looks through the execute-list.
*! Jun  6 1995   Willy Sagefalk    Added osys_statistics.
*! Sep  9 1995   Willy Sagefalk    Added processor_usage
*! Sep  9 1995   H-P Nilsson       Added os_save_state() and restore
*!                                 at startup. (But it does not seem
*!                                 to work yet).
*! Sep 14 1995   H-P Nilsson       Now it works as long as there are no
*!                                 timers running out between the jump
*!                                 timer expires (see flashman.c) and
*!                                 the call to os_save_state() is done. 
*! Sep 25 1995   Willy Sagefalk    Added profile info
*! Oct 13 1995   Per Flock         Changed flash load macro names 
*! Oct 17 1995   H-P Nilsson       Changed them a little more
*! May 21 1996   Willy Sagefalk    Added osys_profile_enable ...
*! May 31 1996   H-P Nilsson       Added uptime_in_seconds
*! Sep 18 1996   Willy Sagefalk    Added SERIAL_DEBUG
*!------------------------------------------------------------------------
*!
*! (C) Copyright 1992, Axis, LUND, SWEDEN
*!
*!**************************************************************************/

/* @(#) osys_int.c 1.24 09/25/96 */

/**************************  INCLUDE FILES  ********************************/
#include "compiler.h"
#include "system.h"
//#include "project.h"
#include "projos.h"

#define N_MAILBOXES 0x100

#ifdef HW_CGA3
#include "cga3.h"
#endif

#ifdef HW_PC
#include <dos.h>
#endif

#include "osys.h"
#include "osys_int.h"

#if defined(FLASH_FUNC) || defined(FLASH_GLOW)
#include "flash.h"              /* To get flash_transfer_union_type */
#endif

#if defined(__GNU_CRIS__) && !defined(FLASH_GLOW) 
#define INLINE __inline
#else
#define INLINE
#endif

/**************************  CONSTANTS  ************************************/

/**************************  TYPE DEFINITIONS  *****************************/

/**************************  EXTERNALS  ************************************/
extern task_struct task_list[N_TASKS+1];   /* Task definition table */
#if 0
extern void show_task(void);
#endif
extern bool event_mailbox_ok;   /* TRUE when event box is OK for use by others. */
extern bool event_mailbox_init; /* TRUE when event box is initialized. */
udword      _taskpc;

/**************************  EXTERNAL FUNCTIONS  ***************************/

/**************************  LOCALS  ***************************************/
/*-------------------------------------*/
/* Operating system internal functions */
/*-------------------------------------*/
#ifdef PROTOTYPES
static void send(void);
static void int_send(void);
static INLINE void get(void);
static void send_sync(void);
static void get_sync(void);
static void end_sync(void);
static void task_delay(void);
static void start(void);
static void os_tick(void);
static void sync_end_check(void);
static void get_next_exec(void);
static bool put_mail(void);
static bool event_put_mail(event_mail_box_struct HUGE *event_box);

static INLINE bool get_mail(byte task,os_mailbox_type *mail_entry);

static bool put_sync_mail(void);
static bool get_sync_mail(byte task,byte *mail_entry);
static void find_sync_mail(byte from_task);

#else
static void send();
static void int_send();
static void get();
static void send_sync();
static void get_sync();
static void end_sync();
static void task_delay();
static void start();
static void os_tick();
static void sync_end_check();
static void get_next_exec();
static bool put_mail();
static bool event_put_mail();
static bool get_mail();
static bool put_sync_mail();
static bool get_sync_mail();
static void find_sync_mail();

#endif
#if defined(DEBUG) || defined(SERIAL_DEBUG)
uword processor_usage_count[N_TASKS];
uword processor_usage[N_TASKS];
bool  osys_profile_collect = TRUE;
#endif
/****************************************************************************/
/*                                                                          */
/*                  SPACE ALLOCATION FOR OSYS INTERNAL DATA                 */
/*                                                                          */
/****************************************************************************/

byte executing_task;              /* Current executing task in OS */
func_struct call_info[N_TASKS];       /* Function call info transfer structure. */
func_struct return_info[N_TASKS];     /* Function return info transfer structure. */

static LOCAL execute_struct execute_list[N_TASKS]; /* List of tasks to execute and info. */
static LOCAL execute_struct execute_base;          /* First task, highest priority. */
static LOCAL wait_struct mail_wait[N_TASKS];         /* Wait for mail task list. */
static LOCAL wait_struct sync_mail_wait[N_TASKS];    /* Wait for sync mail task list. */
static LOCAL sync_end_struct sync_end_wait[N_TASKS]; /* Wait for sync end task list. */
static LOCAL wait_struct wait[N_TASKS];              /* Wait for wait task list. */

os_time_type duration_cntr;                  /* Tick counter inside OS. */
os_time_type uptime_in_seconds;              /* ... no need to divide by SEC_TICS 
                                                if we have this handy. */
static os_time_type intermediate_ticks;       /* Ticks since last update of SEC_TICS */
event_mail_box_struct HUGE *event_mailbox; /* Pointer to the place to store event mail in. */

static LOCAL mail_box_struct mailbox[N_MAILBOXES]; /* The place to store mail in. */
static LOCAL mail_box_struct sync_mailbox[N_SYNC_MAILBOXES]; /* The place to store sync mail in. */

static LOCAL byte last_sync_entry;       /* Last sync mail entry in use. */

static LOCAL os_mailbox_type last_entry; /* Last entry into mailbox list */

#if defined(DEBUG) || defined(SERIAL_DEBUG)
#define PROFILE_SIZE 2000
#ifdef DEBUG
osys_statistics_type osys_statistics = {0,0,0,0,0,0};
#endif
udword task_pc[PROFILE_SIZE];
#endif

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: void tim_int_routine()                                   */
/*#                                                                         */
/*# PARAMETERS:  void;                                                      */
/*#                                                                         */
/*# RETURNS:     Nothing.                                                   */
/*#                                                                         */
/*# DESCRIPTION: Called from timer interrupt, generated by timer countdown. */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 921012    Bernt B         Creation                                      */
/*# 950530    H-P Nilsson     Now does not assume that the running process  */
/*#                           has the highest priority; now accepts an      */
/*#                           os_int_send() without call to                 */
/*#                           other_int_routine().  See                     */
/*#                           os_other_int_buf_no_reschedule().  Note       */
/*#                           that this does not change the average         */
/*#                           behavior, since there is probably not         */
/*#                           many different os_int_send() mails to         */
/*#                           other tasks with higher priority than         */
/*#                           the current.                                  */
/*#-------------------------------------------------------------------------*/
void   tim_int_routine(void)
{
  execute_struct *execute;
  execute_struct *pre_execute;  /* The task before where this task goes. */
  byte current_pri;
  execute = &execute_list[executing_task];

  execute->priority  = current_pri = task_list[executing_task].priority;
  execute->task      = executing_task;
  execute->call_func = TIMER_TICK;

#if defined(DEBUG) || defined(SERIAL_DEBUG)
  {
    uword  task;
    static uword count = 0;
    static uword  task_pc_count = 0;

    if (osys_profile_collect == TRUE)
    {
      task_pc_count = (task_pc_count + 1) % PROFILE_SIZE;
      task_pc[task_pc_count] = _taskpc;
    }
    processor_usage_count[executing_task]++;
    if (count++ > 2000)
    {
      count = 0;
      for (task = 0; task < N_TASKS ;task++ )
      {
        processor_usage[task]       = processor_usage_count[task]; 
        processor_usage_count[task] = 0;
      } /* endfor */
    } /* endif */
  }
#endif

  /* Remember that "base" is only a list head; its contents is not
     used and the "priority"-entry is invalid. */
  pre_execute = &execute_base;
  
  while (pre_execute->next != NULL &&
         current_pri <= pre_execute->next->priority)
  {
    pre_execute = pre_execute->next;
  }
  execute->next = pre_execute->next;
  pre_execute->next = execute;

  os_tick();
  get_next_exec();

#ifdef TASK_PROBE
  outportb(OS_DEBUG_PORT,executing_task << 4);
#endif

}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: void other_int_routine()                                 */
/*#                                                                         */
/*# PARAMETERS:  void;                                                      */
/*#                                                                         */
/*# RETURNS:     Nothing.                                                   */
/*#                                                                         */
/*# DESCRIPTION: Called from interrupt, generated by hardware.              */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 921012    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
void   other_int_routine(void)
{
  execute_struct *execute;
  execute_struct *last_execute;
  execute_struct *next_execute;

  execute = &execute_list[executing_task];

  execute->priority  = task_list[executing_task].priority;
  execute->task      = executing_task;
  execute->call_func = SCHEDULE;

  last_execute = &execute_base;
  next_execute = execute_base.next;

  while (next_execute != NULL && execute->priority <= next_execute->priority)
  {
    last_execute = next_execute;
    next_execute = last_execute->next;
  }
  execute->next = last_execute->next;
  last_execute->next = execute;

  get_next_exec();

#ifdef TASK_PROBE
  outportb(OS_DEBUG_PORT,executing_task << 4);
#endif

}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: void   int_send()                                        */
/*#                                                                         */
/*# PARAMETERS:  void;                                                      */
/*#                                                                         */
/*# RETURNS:     Nothing.                                                   */
/*#                                                                         */
/*# DESCRIPTION: Called to handle the sending of an interrupt mail message. */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 921116    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
static void   int_send(void)
{
  func_struct *call;
  wait_struct *waiting;
  execute_struct *execute;
  execute_struct *last_execute;
  execute_struct *next_execute;

  call = &call_info[executing_task];
  waiting = &mail_wait[((call->dst_id >> 8) & 0xFF)];

  if (waiting->used == TRUE)
  {
    return_info[executing_task].return_val = TRUE;  /* Mail sent return TRUE. */

    execute = &execute_list[waiting->task];

    execute->priority   = task_list[waiting->task].priority;
    execute->task       = waiting->task;
    execute->call_func  = OS_GET;
    execute->return_val = TRUE;

    execute->src_id     = call->src_id;
    execute->dst_id     = call->dst_id;
    execute->action_id  = call->action_id;
    execute->param_type = call->param_type;
    execute->param      = call->param;
    execute->param_ptr  = call->param_ptr;

    waiting->used = FALSE;

    last_execute = &execute_base;
    next_execute = execute_base.next;

    while (next_execute != NULL && execute->priority <= next_execute->priority)
    {
      last_execute = next_execute;
      next_execute = last_execute->next;
    }
    execute->next = last_execute->next;
    last_execute->next = execute;
  }
  else if (put_mail() == TRUE)
  {
    return_info[executing_task].return_val = TRUE;  /* All clear return TRUE. */
  }
  else  /* If empty mailboxes was out. */
  {
    return_info[executing_task].return_val = FALSE;  /* FALSE return. */
  }
}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: void   send()                                            */
/*#                                                                         */
/*# PARAMETERS:  void;                                                      */
/*#                                                                         */
/*# RETURNS:     Nothing.                                                   */
/*#                                                                         */
/*# DESCRIPTION: Called to handle the sending of a mail message.            */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 921012    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
static void send(void)
{
  func_struct *ret;
  func_struct *call;
  wait_struct *waiting;
  execute_struct *execute;
  execute_struct *last_execute;
  execute_struct *next_execute;

  call = &call_info[executing_task];
  waiting = &mail_wait[((call->dst_id >> 8) & 0xFF)];

  if (waiting->used == TRUE)
  {
    execute = &execute_list[executing_task];

    execute->priority   = task_list[executing_task].priority;
    execute->task       = executing_task;
    execute->call_func  = OS_SEND;
    execute->return_val = TRUE;        /* Mail is forwarded immediately. */

    execute->next = execute_base.next; /* Have to have highest priority if executing */
    execute_base.next = execute;       /* Put first in list */

    execute = &execute_list[waiting->task];

    execute->priority   = task_list[waiting->task].priority;
    execute->task       = waiting->task;
    execute->call_func  = OS_GET;
    execute->return_val = TRUE;

    execute->src_id     = call->src_id;
    execute->dst_id     = call->dst_id;
    execute->action_id  = call->action_id;
    execute->param_type = call->param_type;
    execute->param      = call->param;
    execute->param_ptr  = call->param_ptr;

    waiting->used = FALSE;

    last_execute = &execute_base;
    next_execute = execute_base.next;

    while (next_execute != NULL && execute->priority <= next_execute->priority)
    {
      last_execute = next_execute;
      next_execute = last_execute->next;
    }
    execute->next = last_execute->next;
    last_execute->next = execute;

    get_next_exec();
  }
  else if (put_mail() == TRUE)
  {
    ret = &return_info[executing_task];

    ret->call_func  = OS_SEND;
    ret->return_val = TRUE;       /* There was space for mail. */
  }
  else  /* If empty mailboxes was out. */
  {
    ret = &return_info[executing_task];

    ret->call_func  = OS_SEND;
    ret->return_val = FALSE;      /* There was no space. */
  }
}
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: bool get_mail()                                          */
/*#                                                                         */
/*# PARAMETERS:  byte       task;                                           */
/*#              byte/uword *mail_entry;                                    */
/*#                                                                         */
/*# RETURNS:     TRUE if there was a mail for task task else FALSE.         */
/*#                                                                         */
/*# DESCRIPTION: Called to search for a mail in mailbox. If a mail is found */
/*#              for task task, TRUE is returned and the entry in mailbox   */
/*#              is put in mail_entry. The mail is also assumed read and    */
/*#              is consequently taken from the mail queue.                 */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 921012    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
static INLINE bool get_mail(byte task,os_mailbox_type *mail_entry)
{
  mail_box_struct *box;
#ifdef OSYS_TIMERS
  event_mail_box_struct HUGE *event_box;
#endif

  box = mailbox;

  while (box->next_entry != 0)
  {
    box = &mailbox[box->next_entry];

    if ((byte)((box->dst_id >> 8) & 0xFF) == task)
    {
#ifdef OSYS_TIMERS
      if (box->event_ind != 0)
      {
        event_box = &event_mailbox[box->event_ind];

        event_box->prev_entry = event_mailbox->prev_entry;
        event_mailbox->prev_entry = box->event_ind;

        *mail_entry = mailbox[box->prev_entry].next_entry; /* Get current mail entry. */

        mailbox[box->prev_entry].next_entry = box->next_entry;

        if (box->next_entry != 0)
        {
          mailbox[box->next_entry].prev_entry = box->prev_entry;
        }
        else  /* Next entry is 0, adjust last_entry to previous (now last). */
        {
          last_entry = box->prev_entry;
        }
        box->prev_entry = mailbox->prev_entry;
        mailbox->prev_entry = *mail_entry;
        box->event_ind = 0;

        if (event_box->used == EVENT_TOUT)
        {
          event_box->used = BOX_FREE;
#ifdef DEBUG
          osys_statistics.used_mail_slots--;
#endif
          return TRUE;  /* Mail found, return TRUE. */
        }
        event_box->used = BOX_FREE;
      }
      else
      {
#endif
        *mail_entry = mailbox[box->prev_entry].next_entry; /* Get current mail entry. */

        mailbox[box->prev_entry].next_entry = box->next_entry;

        if (box->next_entry != 0)
        {
          mailbox[box->next_entry].prev_entry = box->prev_entry;
        }
        else  /* Next entry is 0, adjust last_entry to previous (now last). */
        {
          last_entry = box->prev_entry;
        }
        box->prev_entry = mailbox->prev_entry;
        mailbox->prev_entry = *mail_entry;

#ifdef DEBUG
        osys_statistics.used_mail_slots--;
#endif
        return TRUE;  /* Mail found, return TRUE. */
#ifdef OSYS_TIMERS
      }
#endif
    }
  }
  return FALSE;  /* No mail for task task is found, return FALSE. */
}


/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: void   get()                                             */
/*#                                                                         */
/*# PARAMETERS:  void;                                                      */
/*#                                                                         */
/*# RETURNS:     Nothing.                                                   */
/*#                                                                         */
/*# DESCRIPTION: Called to handle retrieve of sent mail message.            */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 921012    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
static INLINE void get(void)
{
  os_mailbox_type  mail;
  func_struct     *ret;
  wait_struct     *waiting;
  mail_box_struct *mailb;

  if (get_mail(executing_task,&mail) == TRUE)
  {
    ret = &return_info[executing_task];
    mailb = &mailbox[mail];

    ret->src_id     = mailb->src_id;
    ret->dst_id     = mailb->dst_id;
    ret->action_id  = mailb->action_id;
    ret->param_type = mailb->param_type;
    ret->param      = mailb->param;
    ret->param_ptr  = mailb->param_ptr;
    ret->call_func  = OS_GET;
    ret->return_val = TRUE;
  }
  else if (call_info[executing_task].delay_time != 0)
  {
    waiting = &mail_wait[executing_task];

    waiting->task       = executing_task;
    waiting->delay_time = call_info[executing_task].delay_time;
    waiting->used       = TRUE;

    get_next_exec();
  }
  else
  {
    ret = &return_info[executing_task];

    ret->call_func  = OS_GET;
    ret->return_val = FALSE;
  }
}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: void   send_sync()                                       */
/*#                                                                         */
/*# PARAMETERS:  void;                                                      */
/*#                                                                         */
/*# RETURNS:     Nothing.                                                   */
/*#                                                                         */
/*# DESCRIPTION: Called to handle the sending of a syncronous mail message. */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 921012    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
static void   send_sync(void)
{
  func_struct *ret;
  func_struct *call;
  wait_struct *waiting;
  execute_struct *execute;
  execute_struct *last_execute;
  execute_struct *next_execute;
  sync_end_struct *end_wait;

  call = &call_info[executing_task];
  waiting = &sync_mail_wait[((call->dst_id >> 8) & 0xFF)];

  if (waiting->used == TRUE)
  {
    end_wait = &sync_end_wait[executing_task];

    end_wait->task           = executing_task;
    end_wait->delay_time     = call->delay_time;
    end_wait->waiting_for    = (byte)((call->dst_id >> 8) & 0xFF);
    end_wait->collected_mail = TRUE;
    end_wait->used           = TRUE;

    execute = &execute_list[waiting->task];

    execute->priority   = task_list[waiting->task].priority;
    execute->task       = waiting->task;
    execute->call_func  = OS_GET_SYNC;
    execute->return_val = TRUE;

    execute->src_id     = call->src_id;
    execute->dst_id     = call->dst_id;
    execute->action_id  = call->action_id;
    execute->param_type = call->param_type;
    execute->param      = call->param;
    execute->param_ptr  = call->param_ptr;

    waiting->used = FALSE;

    last_execute = &execute_base;
    next_execute = execute_base.next;

    while (next_execute != NULL && execute->priority <= next_execute->priority)
    {
      last_execute = next_execute;
      next_execute = last_execute->next;
    }
    execute->next = last_execute->next;
    last_execute->next = execute;

    get_next_exec();
  }
  else if (call->delay_time == 0)
  {
    ret = &return_info[executing_task];

    ret->call_func  = OS_SEND_SYNC;
    ret->return_val = FALSE;         /* No one was waiting for sync mail. */
  }
  else if (put_sync_mail() == TRUE)
  {
    end_wait = &sync_end_wait[executing_task];

    end_wait->task           = executing_task;
    end_wait->delay_time     = call->delay_time;
    end_wait->waiting_for    = (byte)((call->dst_id >> 8) & 0xFF);
    end_wait->collected_mail = FALSE;
    end_wait->used           = TRUE;

    get_next_exec();
  }
  else  /* If empty mailboxes was out. */
  {
    os_stop();  /* If this happens, something serious is wrong. */
  }
}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: void   get_sync()                                        */
/*#                                                                         */
/*# PARAMETERS:  void;                                                      */
/*#                                                                         */
/*# RETURNS:     Nothing.                                                   */
/*#                                                                         */
/*# DESCRIPTION: Called to handle retrive of sent syncronous mail message.  */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 921012    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
static void   get_sync(void)
{
  byte  mail;
  func_struct     *ret;
  wait_struct     *waiting;
  mail_box_struct *mailb;

  if (get_sync_mail(executing_task,&mail) == TRUE)
  {
    ret = &return_info[executing_task];
    mailb = &sync_mailbox[mail];

    ret->src_id     = mailb->src_id;
    ret->dst_id     = mailb->dst_id;
    ret->action_id  = mailb->action_id;
    ret->param_type = mailb->param_type;
    ret->param      = mailb->param;
    ret->param_ptr  = mailb->param_ptr;
    ret->call_func  = OS_GET;
    ret->return_val = TRUE;

    sync_end_wait[(ret->src_id >> 8) & 0xFF].collected_mail = TRUE;
  }
  else if (call_info[executing_task].delay_time != 0)
  {
    waiting = &sync_mail_wait[executing_task];

    waiting->task       = executing_task;
    waiting->delay_time = call_info[executing_task].delay_time;
    waiting->used       = TRUE;

    get_next_exec();
  }
  else
  {
    ret = &return_info[executing_task];

    ret->call_func  = OS_GET_SYNC;
    ret->return_val = FALSE;
  }
}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: void   end_sync()                                        */
/*#                                                                         */
/*# PARAMETERS:  void;                                                      */
/*#                                                                         */
/*# RETURNS:     Nothing.                                                   */
/*#                                                                         */
/*# DESCRIPTION: Called to handle the ending sequence of a syncronous mail. */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 921012    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
static void   end_sync(void)
{
  execute_struct *execute;

  execute = &execute_list[executing_task];

  execute->priority  = task_list[executing_task].priority;
  execute->task      = executing_task;
  execute->call_func = OS_END_SYNC;

  execute->next = execute_base.next; /* Have to have highest priority if executing */
  execute_base.next = execute;       /* Put first in list */

  sync_end_check();
  get_next_exec();
}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: void   task_delay()                                      */
/*#                                                                         */
/*# PARAMETERS:  void;                                                      */
/*#                                                                         */
/*# RETURNS:     Nothing.                                                   */
/*#                                                                         */
/*# DESCRIPTION: Called to handle suspension of a task for a while.         */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 921012    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
static void   task_delay(void)
{
  wait_struct *waiting;

  waiting = &wait[executing_task];

  waiting->task = executing_task;
  waiting->delay_time = call_info[executing_task].delay_time;
  waiting->used = TRUE;

  get_next_exec();
}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: void   start()                                           */
/*#                                                                         */
/*# PARAMETERS:  void;                                                      */
/*#                                                                         */
/*# RETURNS:     Nothing.                                                   */
/*#                                                                         */
/*# DESCRIPTION: Called to startup the operating system from scratch.       */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 921012    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
static void   start(void)
{
  execute_struct *execute;
  execute_struct *last_execute;
  execute_struct *next_execute;
  uword    n;
  uword    stack_pos;
  byte    *stack_ptr;
  byte     pattern;  /* Pattern to put onto stack. */
  
  pattern = STACK_PATTERN;

#if defined(DEBUG) || defined(SERIAL_DEBUG)
  {
    uword task_num;
    for (task_num = 0; task_num < N_TASKS; task_num++)
    {
      processor_usage[task_num] = 0;
      processor_usage_count[task_num] = 0;
    }
  }
#endif

  for (n = 0 ; n < N_TASKS ; n++)
  {
/*    
    printf( "task=%d  task_list[task].name=%s   &task_list[task].stack_start=%08X   \n",  n, task_list[n].name, &task_list[n].stack_start );*/
    stack_ptr = task_list[n].stack_start;
    /*
    printf( "task_list[task].stack_start=%08X  \n\n",  task_list[n].stack_start );*/
    if (stack_ptr != NULL)
    {
      for (stack_pos = task_list[n].stack_size ; stack_pos != 0 ; stack_pos--)
      {
        stack_ptr--;
        *stack_ptr = pattern;
      }
    }
  }

  for (n = 0 ; n < N_MAILBOXES ; n++)
  {
    mailbox[n].prev_entry = n + 1;
#ifdef OSYS_TIMERS
    mailbox[n].event_ind = 0;
#endif
  }
  mailbox[n - 1].prev_entry = 0;
  mailbox->next_entry = 0;
  last_entry = 0;

  sync_mailbox[N_TASKS].next_entry = N_TASKS;
  last_sync_entry = N_TASKS;

  for (n = 0 ; n < N_TASKS ; n++)
  {
    wait[n].used = FALSE;
  }
  for (n = 0 ; n < N_TASKS ; n++)
  {
    mail_wait[n].used = FALSE;
  }
  for (n = 0 ; n < N_TASKS ; n++)
  {
    sync_mail_wait[n].used = FALSE;
  }
  for (n = 0 ; n < N_TASKS ; n++)
  {
    sync_end_wait[n].used = FALSE;
  }

#ifdef FLASH_GLOW              /* Running in RAM, so we startup with
                                   "time" != 0 */
  duration_cntr = saved_state_p->generic_base.osys.current_time;

  /* These fields were not in saved_state_p at the time of definition
     of the osys-field in flash.h, and they are NOT important enough
     to include at the price of adding a new loader version.  */
  uptime_in_seconds = duration_cntr / SEC_TICS;
  intermediate_ticks = duration_cntr % SEC_TICS;
#else
  duration_cntr = 0;
  uptime_in_seconds = 0;
  intermediate_ticks = 0;
#endif

  execute_base.next = NULL;

  for (n = 0 ; n < N_TASKS ; n++)
  {
    if (task_list[n].priority != 255)
    {
      execute = &execute_list[task_list[n].task_num];

      execute->priority  = task_list[n].priority;
      execute->task      = task_list[n].task_num;

      last_execute = &execute_base;
      next_execute = execute_base.next;

      while (next_execute != NULL && execute->priority <= next_execute->priority)
      {
        last_execute = next_execute;
        next_execute = last_execute->next;
      }
      execute->next = last_execute->next;
      last_execute->next = execute;
    }
  }
  INIT_SEGS_INFO();  /* Init segments, stacks and starting points for all */
                     /* tasks.                                            */
  get_next_exec();

  OS_TICK_ON();    /* Turn on timer interrupt */
}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: void   os_tick()                                         */
/*#                                                                         */
/*# PARAMETERS:  void;                                                      */
/*#                                                                         */
/*# RETURNS:     Nothing.                                                   */
/*#                                                                         */
/*# DESCRIPTION: Called to handle tick tock actions.                        */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 921012    Bernt B         Creation                                      */
/*# 960521    H-P Nilsson     Added update of uptime_in_seconds             */
/*#-------------------------------------------------------------------------*/
static void   os_tick(void)
{
  os_emailbox_type current;
  execute_struct  *execute;
  execute_struct  *last_execute;
  execute_struct  *next_execute;
  wait_struct     *waiting;
  wait_struct     *mail_waiting;
  wait_struct     *sync_mail_waiting;
  sync_end_struct *end;

  event_mail_box_struct HUGE *box;

  byte n;

  if (event_mailbox_init == FALSE || event_mailbox_ok == TRUE)
  {
    duration_cntr++;

    if (++intermediate_ticks == SEC_TICS)
    {
      uptime_in_seconds++;
      intermediate_ticks = 0;
    }
  }

#ifdef OSYS_TIMERS
  if (event_mailbox_ok == TRUE && event_mailbox[0].next_entry != 0)
  {
    if (event_mailbox[event_mailbox[0].next_entry].send_time == duration_cntr)
    {
      box = &event_mailbox[event_mailbox[0].next_entry];

      while (box->send_time == duration_cntr)
      {
        waiting = &mail_wait[((box->dst_id >> 8) & 0xFF)];

        if (waiting->used == TRUE)
        {
          execute = &execute_list[waiting->task];

          execute->priority   = task_list[waiting->task].priority;
          execute->task       = waiting->task;
          execute->call_func  = OS_GET;
          execute->return_val = TRUE;

          execute->src_id     = box->src_id;
          execute->dst_id     = box->dst_id;
          execute->action_id  = box->action_id;
          execute->param_type = box->param_type;
          execute->param      = box->param;
          execute->param_ptr  = box->param_ptr;

          waiting->used = FALSE;

          last_execute = &execute_base;
          next_execute = execute_base.next;

          while (next_execute != NULL && execute->priority <= next_execute->priority)
          {
            last_execute = next_execute;
            next_execute = last_execute->next;
          }
          execute->next = last_execute->next;
          last_execute->next = execute;

          current = event_mailbox[box->prev_entry].next_entry;
          event_mailbox[box->prev_entry].next_entry = box->next_entry;

          if (box->next_entry != 0)
          {
            event_mailbox[box->next_entry].prev_entry = box->prev_entry;
          }
          box->used = BOX_FREE;

          box->prev_entry = event_mailbox->prev_entry;
          event_mailbox->prev_entry = current;
        }
        else if (event_put_mail(box) == TRUE)
        {
          event_mailbox[box->prev_entry].next_entry = box->next_entry;

          if (box->next_entry != 0)
          {
            event_mailbox[box->next_entry].prev_entry = box->prev_entry;
          }
          box->used = EVENT_TOUT;
        }
        else
        {
          while (box->send_time == duration_cntr)
          {
            box->send_time++;

            if (box->next_entry != 0)
            {
              box = &event_mailbox[box->next_entry];
            }
            else
            {
              break;
            }
          }
          break;
        }
        if (box->next_entry != 0)
        {
          box = &event_mailbox[box->next_entry];
        }
        else
        {
          break;
        }
      }
    }
  }
#endif

  n = 0;
  waiting = wait;
  mail_waiting = mail_wait;
  sync_mail_waiting = sync_mail_wait;
  end = sync_end_wait;

  while (n < N_TASKS)
  {
    if (waiting->used == TRUE)
    {
      if (waiting->delay_time <= 1)
      {
        execute = &execute_list[waiting->task];

        execute->priority  = task_list[waiting->task].priority;
        execute->task      = waiting->task;
        execute->call_func = OS_DELAY;

        waiting->used = FALSE;

        last_execute = &execute_base;
        next_execute = execute_base.next;

        while (next_execute != NULL && execute->priority <= next_execute->priority)
        {
          last_execute = next_execute;
          next_execute = last_execute->next;
        }
        execute->next = last_execute->next;
        last_execute->next = execute;
      }
      else if (waiting->delay_time != WAIT_FOREVER)
      {
        waiting->delay_time--;
      }
    }
    waiting++;

    if (mail_waiting->used == TRUE)
    {
      if (mail_waiting->delay_time <= 1)
      {
        execute = &execute_list[mail_waiting->task];

        execute->priority  = task_list[mail_waiting->task].priority;
        execute->task      = mail_waiting->task;
        execute->call_func = OS_GET;
        execute->return_val = FALSE;

        mail_waiting->used = FALSE;

        last_execute = &execute_base;
        next_execute = execute_base.next;

        while (next_execute != NULL && execute->priority <= next_execute->priority)
        {
          last_execute = next_execute;
          next_execute = last_execute->next;
        }
        execute->next = last_execute->next;
        last_execute->next = execute;
      }
      else if (mail_waiting->delay_time != WAIT_FOREVER)
      {
        mail_waiting->delay_time--;
      }
    }
    mail_waiting++;

    if (sync_mail_waiting->used == TRUE)
    {
      if (sync_mail_waiting->delay_time <= 1)
      {
        execute = &execute_list[sync_mail_waiting->task];

        execute->priority  = task_list[sync_mail_waiting->task].priority;
        execute->task      = sync_mail_waiting->task;
        execute->call_func = OS_GET_SYNC;
        execute->return_val = FALSE;

        sync_mail_waiting->used = FALSE;

        last_execute = &execute_base;
        next_execute = execute_base.next;

        while (next_execute != NULL && execute->priority <= next_execute->priority)
        {
          last_execute = next_execute;
          next_execute = last_execute->next;
        }
        execute->next = last_execute->next;
        last_execute->next = execute;
      }
      else if (sync_mail_waiting->delay_time != WAIT_FOREVER)
      {
        sync_mail_waiting->delay_time--;
      }
    }
    sync_mail_waiting++;

    if (end->used == TRUE)
    {
      if (end->delay_time <= 1 && end->collected_mail != TRUE)
      {
        execute = &execute_list[end->task];

        execute->priority   = task_list[end->task].priority;
        execute->task       = end->task;
        execute->call_func  = OS_SEND_SYNC;
        execute->return_val = FALSE;

        last_execute = &execute_base;
        next_execute = execute_base.next;

        while (next_execute != NULL && execute->priority <= next_execute->priority)
        {
          last_execute = next_execute;
          next_execute = last_execute->next;
        }
        execute->next = last_execute->next;
        last_execute->next = execute;

        find_sync_mail(end->task);

        end->used = FALSE;
      }
      if (end->delay_time != WAIT_FOREVER)
      {
        if (end->collected_mail != TRUE)
        {
          end->delay_time--;
        }
      }
    }
    end++;

    n++;
  }
}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: void   sync_end_check()                                  */
/*#                                                                         */
/*# PARAMETERS:  void;                                                      */
/*#                                                                         */
/*# RETURNS:     Nothing.                                                   */
/*#                                                                         */
/*# DESCRIPTION: Called to end a syncronous mail message transfer.          */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 921012    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
static void sync_end_check(void)
{
  byte n;
  execute_struct *execute;
  execute_struct *last_execute;
  execute_struct *next_execute;
  sync_end_struct *end;

  n = 0;
  end = sync_end_wait;

  while (n < N_TASKS)
  {
    if (end->used == TRUE)
    {
      if (end->waiting_for == executing_task && end->collected_mail == TRUE)
      {
        execute = &execute_list[end->task];

        execute->priority   = task_list[end->task].priority;
        execute->task       = end->task;
        execute->call_func  = OS_SEND_SYNC;
        execute->return_val = TRUE;

        end->used = FALSE;

        last_execute = &execute_base;
        next_execute = execute_base.next;

        while (next_execute != NULL && execute->priority <= next_execute->priority)
        {
          last_execute = next_execute;
          next_execute = last_execute->next;
        }
        execute->next = last_execute->next;
        last_execute->next = execute;

        break;  /* Found what we're looking for. */
      }
    }
    n++;
    end++;
  }
}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: void   get_next_exec()                                   */
/*#                                                                         */
/*# PARAMETERS:  void;                                                      */
/*#                                                                         */
/*# RETURNS:     Nothing.                                                   */
/*#                                                                         */
/*# DESCRIPTION: Called to search for next task to execute in execute list. */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE          NAME            CHANGES                                   */
/*# ----          ----            -------                                   */
/*# 921012        Bernt B         Creation                                  */
/*# Mar 29 1995   Willy Sagefalk  Removed lint warning (entry=execute)      */
/*#-------------------------------------------------------------------------*/
static void   get_next_exec(void)
{
  func_struct *ret;
  execute_struct *execute;

  executing_task = execute_base.next->task;
  execute_base.next = execute_base.next->next;

  execute = &execute_list[executing_task];
  ret = &return_info[executing_task];

  ret->src_id     = execute->src_id;
  ret->dst_id     = execute->dst_id;
  ret->action_id  = execute->action_id;
  ret->param_type = execute->param_type;
  ret->param      = execute->param;
  ret->param_ptr  = execute->param_ptr;
  ret->call_func  = execute->call_func;
  ret->return_val = execute->return_val;
}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: bool put_mail()                                          */
/*#                                                                         */
/*# PARAMETERS:  void;                                                      */
/*#                                                                         */
/*# RETURNS:     TRUE if there was space for mail, else FALSE.              */
/*#                                                                         */
/*# DESCRIPTION: Called to search for space in mailbox and to copy mail info*/
/*#              into the mailbox. If no space is available FALSE is        */
/*#              returned.                                                  */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 921012    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
static bool put_mail(void)
{
  os_mailbox_type current;
  mail_box_struct *box;
  func_struct *call;

  if (mailbox->prev_entry == 0)
  {
    return FALSE;  /* No empty space is found, return FALSE. */
  }
#ifdef DEBUG
  osys_statistics.used_mail_slots++;
  if (osys_statistics.used_mail_slots > osys_statistics.max_used_mail_slots)
  {
    osys_statistics.max_used_mail_slots = osys_statistics.used_mail_slots;
  }
#endif

  box = &mailbox[mailbox->prev_entry];

  mailbox[last_entry].next_entry = current = mailbox->prev_entry;
  mailbox->prev_entry = box->prev_entry;
  box->prev_entry = last_entry;
  last_entry = current;
  box->next_entry = 0;

  call = &call_info[executing_task];

  box->src_id     = call->src_id;
  box->dst_id     = call->dst_id;
  box->action_id  = call->action_id;
  box->param_type = call->param_type;
  box->param      = call->param;
  box->param_ptr  = call->param_ptr;

  return TRUE;  /* Space found and mail copied into mailbox, return TRUE. */
}

#ifdef OSYS_TIMERS
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: bool event_put_mail()                                    */
/*#                                                                         */
/*# PARAMETERS:  event_mail_box_struct HUGE *event_box;                     */
/*#                                                                         */
/*# RETURNS:     TRUE if there was space for mail, else FALSE.              */
/*#                                                                         */
/*# DESCRIPTION: Called to search for space in mailbox and to copy mail info*/
/*#              into the mailbox. If no space is available FALSE is        */
/*#              returned.                                                  */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 941103    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
static bool event_put_mail(event_mail_box_struct HUGE *event_box)
{
  os_mailbox_type current;
  mail_box_struct *box;

  if (mailbox->prev_entry == 0)
  {
    return FALSE;  /* No empty space is found, return FALSE. */
  }

#ifdef DEBUG
  osys_statistics.used_mail_slots++;
  if (osys_statistics.used_mail_slots > osys_statistics.max_used_mail_slots)
  {
    osys_statistics.max_used_mail_slots = osys_statistics.used_mail_slots;
  }
#endif

  box = &mailbox[mailbox->prev_entry];

  mailbox[last_entry].next_entry = current = mailbox->prev_entry;
  mailbox->prev_entry = box->prev_entry;
  box->prev_entry = last_entry;
  last_entry = current;
  box->next_entry = 0;

  box->src_id     = event_box->src_id;
  box->dst_id     = event_box->dst_id;
  box->action_id  = event_box->action_id;
  box->param_type = event_box->param_type;
  box->param      = event_box->param;
  box->param_ptr  = event_box->param_ptr;
  box->event_ind  = event_mailbox[event_box->prev_entry].next_entry;

  return TRUE;  /* Space found and mail copied into mailbox, return TRUE. */
}
#endif

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: bool put_sync_mail()                                     */
/*#                                                                         */
/*# PARAMETERS:  void;                                                      */
/*#                                                                         */
/*# RETURNS:     TRUE if there was space for sync_mail, else FALSE.         */
/*#                                                                         */
/*# DESCRIPTION: Called to search for space in sync_mailbox and to copy mail*/
/*#              info into the mailbox. If no space is available FALSE is   */
/*#              returned. Note that this is a serious error condition.     */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 921012    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
static bool put_sync_mail(void)
{
  mail_box_struct *box;
  func_struct *call;

  box = &sync_mailbox[executing_task];

  sync_mailbox[last_sync_entry].next_entry = (os_mailbox_type)executing_task;
  box->prev_entry = (os_mailbox_type)last_sync_entry;
  last_sync_entry = executing_task;
  box->next_entry = N_TASKS;

  call = &call_info[executing_task];

  box->src_id     = call->src_id;
  box->dst_id     = call->dst_id;
  box->action_id  = call->action_id;
  box->param_type = call->param_type;
  box->param      = call->param;
  box->param_ptr  = call->param_ptr;

  return TRUE;  /* Space found and mail copied into sync_mailbox, return TRUE. */
}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: bool get_sync_mail()                                     */
/*#                                                                         */
/*# PARAMETERS:  byte task;                                                 */
/*#              byte *mail_entry;                                          */
/*#                                                                         */
/*# RETURNS:     TRUE if there was a sync_mail for task task else FALSE.    */
/*#                                                                         */
/*# DESCRIPTION: Called to search for a sync_mail in sync_mailbox. If a mail*/
/*#              is found for task task, TRUE is returned and the entry in  */
/*#              sync_mailbox is put in mail_entry. The mail is also assumed*/
/*#              read and is consequently taken from the mail queue.        */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 921012    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
static bool get_sync_mail(byte task,byte *mail_entry)
{
  mail_box_struct *box;

  box = &sync_mailbox[N_TASKS];

  while (box->next_entry != N_TASKS)
  {
    box = &sync_mailbox[box->next_entry];

    if ((byte)((box->dst_id >> 8) & 0xFF) == task)
    {
      *mail_entry = (byte)sync_mailbox[box->prev_entry].next_entry; /* Get current mail entry. */

      sync_mailbox[box->prev_entry].next_entry = box->next_entry;

      if (box->next_entry != N_TASKS)
      {
        sync_mailbox[box->next_entry].prev_entry = box->prev_entry;
      }
      else  /* Next entry is N_TASKS, adjust last_sync_entry to previous (now last). */
      {
        last_sync_entry = (byte)box->prev_entry;
      }

      return TRUE;  /* Mail found, return TRUE. */
    }
  }
  return FALSE;  /* No sync mail for task task is found, return FALSE. */
}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: void find_sync_mail()                                    */
/*#                                                                         */
/*# PARAMETERS:  byte from_task;                                            */
/*#                                                                         */
/*# RETURNS:     Nothing.                                                   */
/*#                                                                         */
/*# DESCRIPTION: Called to search for a sync_mail in sync_mailbox. If a mail*/
/*#              from task from_task is found, the entry in sync_mailbox is */
/*#              asssumed read and is consequently marked as obsolete.      */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 921012    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
static void find_sync_mail(byte from_task)
{
  mail_box_struct *box;

  box = &sync_mailbox[from_task];

  sync_mailbox[box->prev_entry].next_entry = box->next_entry;

  if (box->next_entry != N_TASKS)
  {
    sync_mailbox[box->next_entry].prev_entry = box->prev_entry;
  }
  else  /* Next entry is N_TASKS, adjust last_sync_entry to previous (now last). */
  {
    last_sync_entry = (byte)box->prev_entry;
  }
}

/*#****************************************************************************
*#
*# FUNCTION NAME: osys_print_statistics
*#
*# PARAMETERS:    None
*#
*# RETURNS:       Nothing
*#
*# DESCRIPTION:   Print out internal osys statistics
*#
*#-----------------------------------------------------------------------------
*#
*# HISTORY
*#
*# DATE          NAME              CHANGES
*# ----          ----              ------- 
*# Jun  6 1995   Willy Sagefalk    Initial version
*# Sep  9 1995   Willy Sagefalk    Added processor_usage
*#**************************************************************************#*/
#ifdef DEBUG
void osys_print_statistics(void)
{
  uword task_num;
  uword used_stack;
  uword pdiv = 0;

  ax_printf("OSYS Statistics \n");
  ax_printf("*************************************\n");
  ax_printf("Out of mailslots     : %u\n",osys_statistics.out_of_mailslots);
  ax_printf("Illegal remove timer : %u\n",osys_statistics.illegal_remove_timer);
  ax_printf("Timers started       : %u\n",osys_statistics.timers_started);
  ax_printf("Timers removed       : %u\n",osys_statistics.timers_removed);
  ax_printf("Used mailboxes       : %u\n",osys_statistics.used_mail_slots);
  ax_printf("Max used mailboxes   : %u\n",osys_statistics.max_used_mail_slots);  
  ax_printf("OSYS up time         : %u\n",duration_cntr);
  ax_printf("OSYS up time, seconds: %u\n",uptime_in_seconds);
  ax_printf("Stack Size Left Used Perc Processor Pri Task\n");
  ax_printf("-------------------------------------\n");

  for (task_num = 0; task_num < N_TASKS; task_num++)
  {
    pdiv += processor_usage[task_num];
  }

  for (task_num = 0; task_num < N_TASKS; task_num++)
  {
    used_stack = task_list[task_num].stack_size - os_stack(task_num);

    ax_printf("      %04u ",task_list[task_num].stack_size);
    ax_printf("%04u ",os_stack(task_num));
    ax_printf("%04u ",used_stack);
    ax_printf("%02u  ",(used_stack*100)/task_list[task_num].stack_size);
    ax_printf("%02u        ",(processor_usage[task_num]*100)/pdiv);
    ax_printf("%03u ",task_list[task_num].priority);
    ax_printf("%s\n"  ,task_list[task_num].name);                                       
  }
  ax_printf("%08X \n",task_pc[0]);
  ax_printf("*************************************\n");
}
#endif
/*#****************************************************************************
*#
*# FUNCTION NAME: osys_profile_enable
*#                osys_profile_disable
*#                osys_print_profile_info
*#
*# PARAMETERS:    None
*#
*# RETURNS:       Nothing
*#
*# DESCRIPTION:   Print out internal osys statistics
*#
*#-----------------------------------------------------------------------------
*#
*# HISTORY
*#
*# DATE          NAME              CHANGES
*# ----          ----              ------- 
*# Jun 06 1995   Willy Sagefalk    Initial version
*#**************************************************************************#*/
#if defined(DEBUG) || defined(SERIAL_DEBUG)
void osys_profile_enable(void)
{
  osys_profile_collect = TRUE;
}

void osys_profile_disable(void)
{
  osys_profile_collect = FALSE;
}

void osys_profile_reset(void)
{
  uword i;
  
  for (i = 0; i < PROFILE_SIZE; i++)
  {
    task_pc[i] = 0;
  }
}

void osys_print_profile_info(void)
{
  uword i;
  osys_profile_collect = FALSE;
  ax_printf("\n");
  ax_printf("OSYS profile info start\n");
  for (i = 0; i < PROFILE_SIZE; i++)
  {
    ax_printf("%X*",task_pc[i]);
  }
  ax_printf("Q\n");
  ax_printf("OSYS profile info stop\n");
  osys_profile_collect = TRUE;
}
#endif

#ifdef FLASH_FUNC
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: void   os_save_state()                                   */
/*#                                                                         */
/*# PARAMETERS:  void * whereto_p -- really a "flash_transfer_union_type *" */
/*#                                  to where to save any state needed      */
/*#                                  to start up osys in another            */
/*#                                  context without loss of essential      */
/*#                                  information                            */ 
/*#                                                                         */
/*#                                                                         */
/*#                                                                         */
/*#                                                                         */
/*# RETURNS:     0 if osys was in a state where it looks ok to startup      */
/*#              somewhere else.  Non-zero, indicating a time interval      */
/*#              until a retry may be succesful.                            */
/*#                                                                         */
/*# DESCRIPTION: Called to save the essential state of osys needed to       */
/*#              startup somewhere else in a controlled manner.             */
/*#               Any state that needs to be saved will be saved at         */
/*#              whereto_p.                                                 */
/*#               Timers (delayed mail) are not considered essential.       */
/*#              Mails, on the other hand, are considered to be             */
/*#              non-saveable, so there must be no mails pending to be      */
/*#              sent in the system (the call is considered                 */
/*#              unsuccessful if there are).                                */
/*#               Thus, the only data that will be saved is the             */
/*#              current-time info.                                         */
/*#              NOTE: THIS FUNCTION IS CALLED WITH INTERRUPTS OFF,         */
/*#              AND THEY MUST REMAIN OFF so no osys calls, please!         */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE         NAME              CHANGES                                  */
/*# ----         ----              -------                                  */
/*# Sep 08 1995  H-P Nilsson       Creation                                 */
/*# May 21 1996  H-P Nilsson       Updated comment (it obviously works now) */
/*#-------------------------------------------------------------------------*/
os_time_type
os_save_state(void *parp)
{
  flash_transfer_union_type *where = parp;
  os_time_type delay = 0;

  /* Check if there are mails that are not received, i.e. if mailbox
     or sync_mailbox are non-empty.
      Note that timers that have just ran out are immediately moved to
     a "normal" mailbox and thus are included in this test.
      If any link in the "save_state-chain" has a timer that runs out
     each 10:th millisecond, then that timer should be removed when
     the save-state call arrives. */
  if (mailbox[0].next_entry != 0 || sync_mailbox[0].next_entry != 0)
  {
    delay = WAIT_TEN_MSECONDS;
  }
  else
  {
    where->generic_base.osys.current_time = duration_cntr;
  }

  return delay;
}

#endif /* FLASH_FUNC */

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: void sw_int_routine()                                    */
/*#                                                                         */
/*# PARAMETERS:  void;                                                      */
/*#                                                                         */
/*# RETURNS:     Nothing.                                                   */
/*#                                                                         */
/*# DESCRIPTION: Called from SW interrupt, generated by task code.          */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 921012    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
void   sw_int_routine(void)
{
  switch (call_info[executing_task].call_func)
  {
    case OS_INT_SEND:
      int_send();
      break;

    case OS_SEND:
      send();
      break;

    case OS_GET:
      get();
      break;

    case OS_SEND_SYNC:
      send_sync();
      break;

    case OS_GET_SYNC:
      get_sync();
      break;

    case OS_END_SYNC:
      end_sync();
      break;

    case OS_DELAY:
      task_delay();
      break;

    case OS_START:
      start();
      break;

    default:
      os_stop();  /* Someone sent a message that couldn't be understood. */
      break;
  }

#ifdef TASK_PROBE
  outportb(OS_DEBUG_PORT,executing_task << 4);
#endif

}


/********************** END OF FILE osys_int.c *****************************/
