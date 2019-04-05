/*!**************************************************************************
*!
*! FILE NAME: OSYS.C
*!
*! DESCRIPTION: This module contains the system interface functions for
*!              the operating system.
*!
*! FUNCTIONS:   os_set_timer   
*! (EXPORTED)   os_remove_timer
*!              os_time_timer  
*!              os_is_started  
*!              os_int_send    
*!              os_send        
*!              os_get         
*!              os_send_sync   
*!              os_get_sync    
*!              os_end_sync    
*!              os_delay       
*!              os_get_time    
*!              os_start       
*!              os_stop        
*!              os_error       
*!              os_stack       
*!              os_get_seconds
*!
*! FUNCTIONS:   mail_debug     
*! (LOCAL)      print_who      
*!
*!-------------------------------------------------------------------------
*!
*! HISTORY
*!                                
*! DATE          NAME              CHANGES
*! ----          ----              -------
*! 920731        Bernt B           Creation
*! 921116        Bernt B           Added os_int_send (support for int mail).
*! 940318        RW                Changed "dos.h" -> <dos.h> so that 
*!                                 mgmake doesn't barf.
*! 940419        IP                Renamed OS function request calls.
*! 940420        IP                Added os_is_started function. Use sprintf
*!                                 instead of itoa.
*! 940422        IP                Include projos.h
*! 940518        BB/IP             os_delay returns immediately when delay is 0
*! 940811        AB                Debug for os_send and os_get
*! 941020        BB                Added by misstake removed debug prints
*! 941031        BB                Added os_xxxxx_timer functions.
*! 941122        IP                Corrected os_get & os_get_sync function
*!                                 headers (big deal)
*! 950116        BB                Changed channel_id to src_id and dst_id,
*!                                 revised timer
*!                                 functions, remove is now automatic
*!                                 when event mails are
*!                                 read by the receiver. Added 
*!                                 MAX_DURATION_CNTR and changed os_stack.
*! 950116        IP                Adm. modification for SCCS.
*!
*! Feb 28 1995   Willy Sagefalk    Changed headers. Use OS_XXX_ID_TYPE.
*! Feb 28 1995   Willy Sagefalk    Changed headers. Use os_xxx_id_type.
*! Mar 29 1995   Willy Sagefalk    Validate event_mail in remove_timer and
*!                                 time_timer
*! Apr 04 1995   Willy Sagefalk    Made a change in os_stop.
*! Apr 21 1995   Bernt B”hmer      Commented os_mail_debug debug calls to
*!                                 improve performace in os_get functions.
*! May 31 1995   Willy Sagefalk    Added count to event mail box
*! Jun  1 1995   Willy Sagefalk    Added debug_remove_timer
*! Jun  6 1995   Willy Sagefalk    Added osys_statistics
*! Jun 19 1995   Willy Sagefalk    Measure used mailboxes
*! Sep 21 1995   Willy Sagefalk    Added init task stacks
*! Oct 12 1995   H-P Nilsson       #ifdef debug
*! May 16 1996   Fredrik Norrman   Fixed compiler warning
*! May 21 1996   H-P Nilsson       Got tired of those "/ SEC_TICS"
*!                                 everywhere, and so added os_get_seconds()
*! Oct 02 1996   H-P Nilsson       Corrected declaration of
*!                                 task_list[].  If someone wants to
*!                                 update a lot of projos.h:s then
*!                                 remove the declaration from here,
*!                                 because it really should be _only_
*!                                 in projos.h.
*! Oct 24 1996   H-P Nilsson       Changed all enable => ENABLE and disable =>
*!                                 DISABLE to ease debugging for Win32 PC-env
*!                                 (__FILE__ and __LINE__ are supplied in
*!                                 ENABLE() and DISABLE() for the PC-env)
*!-------------------------------------------------------------------------
*!
*! (C) Copyright 1992-1996, Axis, LUND, SWEDEN
*!
*!**************************************************************************/

/* @(#) osys.c 1.27 10/24/96 */

/**************************  INCLUDE FILES  ********************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compiler.h"
#include "system.h"
//#include "project.h"
#include "projos.h"
#include "sp_alloc.h"

#ifdef HW_PC
#include <dos.h>
#endif

#include "osys.h"
#include "osys_int.h"

/**************************  CONSTANTS  ************************************/

/**************************  TYPE DEFINITIONS  *****************************/
#ifdef DEBUG
#define OSYSTD(X)  X
#else
#define OSYSTD(X)
#endif
/**************************  EXTERNALS  ************************************/

extern byte           executing_task;       /* Current executing task in OS */
extern os_time_type   duration_cntr;        /* Tick counter inside OS. */
extern os_time_type   uptime_in_seconds;    /* Number of seconds "tick counter". */
extern func_struct    call_info[N_TASKS];   /* Function call info transfer structure. */
extern func_struct    return_info[N_TASKS]; /* Function return info transfer structure. */
extern task_struct    task_list[];          /* Task definition table */
extern event_mail_box_struct HUGE *event_mailbox; /* Pointer to the place to store event mail in. */

/**************************  EXTERNAL FUNCTIONS  ***************************/

/**************************  LOCALS  ***************************************/

bool event_mailbox_ok   = FALSE;     /* TRUE when event box is OK for use by others. */
bool event_mailbox_init = FALSE;     /* TRUE when event box is initialized. */

static void *event_mailbox_id;       /* Object ID for the event mailboxes. */
uword  event_mailbox_size = 0; /* The size of the event mailbox, num of entries. */

static bool  os_started_indicator = FALSE; /* Goes TRUE when os_start is called */

#ifdef DEBUG
void          mail_debug(mail_struct *info,bool return_val,CONST byte *type);
static byte  *print_who(uword src_id,uword dst_id);
#endif

#ifdef OSYS_TIMERS
/*#****************************************************************************
*#
*#                                                                         
*# FUNCTION NAME: uword os_set_timer()                                     
*#                                                                         
*# PARAMETERS:  uword trig_period; Os ticks to wait for mail to be forwardd
*#              uword src_id;      The sender and sub sender for the mail. 
*#              uword dst_id;      The destination and sub adress for mail.
*#              uword action_id;   The required action on this mail.       
*#              byte param_type;   The types of param and param_ptr.       
*#              dword param;       Message to reciver.                     
*#              void   *param_ptr; Message pointer to receiver.            
*#                                                                         
*# RETURNS:     File pointer to event mail or NULL if error or no space.   
*#              EVENT_HAPPEND if the message was forwared immediately i.e. 
*#              the trig period is 0.                                      
*#                                                                         
*# DESCRIPTION: Used to send a mail message to someone after TRIG_PERIOD os
*#              ticks has elapsed. If the timeout has elapsed the message  
*#              is sent to the receiver indicated by the parameters in the 
*#              function call. The function returns a file pointer to the  
*#              event mail if all is well or NULL if something is wrong.   
*#              The file pointer shall be used together with the functions 
*#              os_remove_timer and os_time_timer.                         
*#                                                                         
*#-------------------------------------------------------------------------
*#                                                                         
*# HISTORY                                                                 
*#                                                                         
*# DATE      NAME            CHANGES                                       
*# ----      ----            -------                                       
*# 941031    Bernt B         Creation                                      
*#****************************************************************************/
os_event_index_type os_set_timer(os_time_type      trig_period,
                                 os_src_id_type    src_id,
                                 os_dst_id_type    dst_id,
                                 os_action_id_type action_id,
                                 byte              param_type,
                                 dword             param,
                                 void             *param_ptr)
{
  static volatile bool semafore = FALSE;

  void                 *new_id;
  uword                 n;
  os_event_index_type   return_val;
  uword                 current;
  uword                 empty_space;
  os_time_type          event_time;
  udword                size;

  event_mail_box_struct HUGE *box;
  event_mail_box_struct HUGE *event_box;

  if (trig_period == 0)
  {
    if (os_send(src_id,dst_id,action_id,param_type,param,param_ptr) != FALSE)
    {
      return_val = EVENT_HAPPEND;
    }
    else
    {
      return_val = 0;
    }
  }
  else
  {
    DISABLE();  /* Disable to prevent other HW-ints from creating a mess. */

    while (semafore == TRUE)
    {
      ENABLE();
      os_delay(1);
      DISABLE();
    }
    semafore = TRUE;
    ENABLE();        /* And enable again. */

    if (event_mailbox_size == 0)
    {
      /* Create event mailbox vector */

      size = N_EVENT_MAILBOXES;
      size = size * (udword)(sizeof(event_mail_box_struct));
      event_mailbox_id = obj_alloc(size);

      if (event_mailbox_id == NULL)
      {
        semafore = FALSE;

        return 0;
      }
      else
      {
        /* Initiate slots in event mailbox vector */

        event_mailbox = (event_mail_box_struct HUGE *)obj_lock(event_mailbox_id);

        event_mailbox->used       = BOX_USED;
        event_mailbox->next_entry = 0;
        event_mailbox->prev_entry = 1;
        event_mailbox->count      = 0;

        for (n = 1 ; n < N_EVENT_MAILBOXES ; n++)
        {
          event_mailbox[n].prev_entry = n + 1;
          event_mailbox[n].used       = BOX_FREE;
          event_mailbox[n].count      = 0;
        }
        event_mailbox[n - 1].prev_entry = 0;
        event_mailbox_size = N_EVENT_MAILBOXES;
        event_mailbox_ok   = TRUE;
        event_mailbox_init = TRUE;
      }
    }
    else if (event_mailbox->prev_entry == 0)
    {
      /* Increase size of event mailbox vector */      

      DISABLE();
      event_mailbox_ok = FALSE;
      ENABLE();
      size = INC_EVENT_MAILBOXES;
      size += (udword)event_mailbox_size;
      size *= (udword)(sizeof(event_mail_box_struct));
      new_id = obj_realloc(event_mailbox_id,size);

      if (new_id == NULL)
      {
        event_mailbox = (event_mail_box_struct HUGE *)obj_lock(event_mailbox_id);
        event_mailbox_ok = TRUE;
        semafore = FALSE;

        return 0;
      }
      else
      {
        /* Initiate new slots in event mailbox vector */

        event_mailbox_id = new_id;
        event_mailbox = (event_mail_box_struct HUGE *)obj_lock(event_mailbox_id);
        event_mailbox->prev_entry = event_mailbox_size;
        event_mailbox_size += INC_EVENT_MAILBOXES;

        for (n = event_mailbox->prev_entry ; n < event_mailbox_size ; n++)
        {
          event_mailbox[n].prev_entry = n + 1;
          event_mailbox[n].used       = BOX_FREE;
          event_mailbox[n].count      = 0;
        }
        event_mailbox[n - 1].prev_entry = 0;
        event_mailbox_ok = TRUE;
      }
    }
    DISABLE();  /* Disable to prevent other HW-ints from creating a mess. */
    empty_space = event_mailbox->prev_entry;
    box = &event_mailbox[empty_space];

    if (box->used != BOX_FREE)
    {
      OSYSTD(ax_printf("OSYS: Set timer BOX not FREE\n"));
    }

    if (empty_space == 0)
    {
      ENABLE();         /* And enable again. */
      semafore = FALSE;

      return 0;  /* No empty space is found, return NULL. */
    }
    event_mailbox->prev_entry = box->prev_entry;

    box->send_time  = (os_time_type)(trig_period + duration_cntr);
    box->src_id     = src_id;
    box->dst_id     = dst_id;
    box->action_id  = action_id;
    box->param_type = param_type;
    box->param      = param;
    box->param_ptr  = param_ptr;
    box->used       = BOX_USED;
    box->count      = (box->count+1)%16;

    current = 0;
    event_box = event_mailbox;

#ifdef DEBUG
    osys_statistics.timers_started++;
#endif
    while (event_box->next_entry != 0)
    {
      current = event_box->next_entry;
      event_box = &event_mailbox[event_box->next_entry];
      event_time = event_box->send_time - duration_cntr;

      if (trig_period <= event_time)
      {
        /* Link entry first or middle */

        box->prev_entry = event_box->prev_entry;
        box->next_entry = current;
        event_box->prev_entry = empty_space;
        event_mailbox[box->prev_entry].next_entry = empty_space;
        event_mailbox_ok = TRUE;

        ENABLE();         /* And enable again. */
        semafore = FALSE;
   
         /* Space found and mail copied into mailbox, return index. */

        return_val = (uword)(box->count<<12) ^ empty_space;
        
        /*OSYSTD(ax_printf("OSYS: Return slot %u count %u\n",empty_space,box->count));*/      

        return return_val;  
      }
    }

    /* Link entry last */

    event_box->next_entry = empty_space;
    box->next_entry = 0;
    box->prev_entry = current;

    ENABLE();         /* And enable again. */
    semafore = FALSE;

    /* Space found and mail copied into mailbox, return index. */

    return_val = (uword)(box->count << 12) ^ empty_space;

    /*OSYSTD(ax_printf("OSYS: Return slot %u count %u\n",empty_space,box->count));*/      
  }
  return return_val;  /* Returned from OS. */
}

/*#****************************************************************************
*#                                                                         
*# FUNCTION NAME: byte os_remove_timer()                                   
*#                                                                         
*# PARAMETERS:  uword event_mail;  File pointer to event mail to remove.   
*#                                                                         
*# RETURNS:     0 if Error, 1 if removed and 3 if timed out and removed.   
*#                                                                         
*# DESCRIPTION: Used to remove event mail messages. The event mail message 
*#              file index shall be in event_mail. If the eventmail has    
*#              been forwarded to the receiver or if there is some kind of 
*#              error, 0 is returned. If the eventmail is removed before   
*#              it reached its receicer, 1 is returned. If the eventmail   
*#              has timed out, but is not yet read by the receiver 3 is    
*#              returned and the mail is removed. The return values are bit
*#              coded as follows, 00, not removed or error, 01, removed and
*#              10 timed out but not yet read by the receiver.             
*#                                                                         
*#-------------------------------------------------------------------------
*#                                                                         
*# HISTORY                                                                 
*#                                                                         
*# DATE          NAME              CHANGES                                       
*# ----          ----              -------                                       
*# 941031        Bernt B           Creation                                      
*# May 31 1995   Willy Sagefalk    Added count to event mail box
*#****************************************************************************/
#ifdef DEBUG
byte debug_remove_timer(os_event_index_type event_mail,char *efile,uword eline)
#else
byte os_remove_timer(os_event_index_type event_mail)
#endif
{
  byte                        return_val;
  uword                       event_mail_slot;
  byte                        event_mail_count;
  event_mail_box_struct HUGE *box;

  /* Split event_mail index to slot nbr and count */

  event_mail_slot  = event_mail & 0x0fff;
  event_mail_count = (byte)(event_mail >> 12);

  while (event_mailbox_ok == FALSE)
  {
    os_delay(1);
  }
  DISABLE();  /* Disable to prevent other HW-ints from creating a mess. */

  if ((event_mail_slot == 0) || ( event_mail_slot >= event_mailbox_size))
  {
    ENABLE();
    return 0;
  } /* endif */

  box = &event_mailbox[event_mail_slot];

  if ((box->used  == BOX_FREE)          || 
      (box->used  == EVENT_REMOVED)     ||
      (box->count != event_mail_count))
  {
    /* Trying to remove a timer that is not allocated, already removed or */
    /* or not allocated by you */
#ifdef DEBUG
    ax_printf("OSYS: ILLEGAL OS REMOVE TIMER ID 0x%04X %s LINE %i\n",event_mail,efile,eline);
    osys_statistics.illegal_remove_timer++;
#endif
    return_val = 0;
  }
  else if (box->used == EVENT_TOUT)
  {
    /*OSYSTD(ax_printf("OSYS:    EVENT TOUT \n"));*/
    box->used = EVENT_REMOVED;

#ifdef DEBUG
    osys_statistics.used_mail_slots--;
    osys_statistics.timers_removed++;
#endif

    return_val = 3;
  }
  else if (box->used == BOX_USED)
  {
    event_mailbox[box->prev_entry].next_entry = box->next_entry;

#ifdef DEBUG
    osys_statistics.timers_removed++;
#endif

    if (box->next_entry != 0)
    {
      event_mailbox[box->next_entry].prev_entry = box->prev_entry;
    }
    box->used = BOX_FREE;
    box->prev_entry = event_mailbox->prev_entry;
    event_mailbox->prev_entry = event_mail_slot;

    return_val = 1;
  }
  else
  {
    return_val = 0;
  }
  ENABLE();  /* And enable again. */
/*
  if (return_val == 0)
  {
    ax_printf("###### Task %d from os_remove_timer: no event mail.\n",executing_task);
  }
*/
  return return_val;  /* Return from OS call. */
}

/*#************************************************************************                                                                         
*# FUNCTION NAME: uword os_time_timer()                                    
*#                                                                         
*# PARAMETERS:  uword event_mail;  File pointer to event mail to time.     
*#                                                                         
*# RETURNS:     The time left until event mail is forwarded.               
*#                                                                         
*# DESCRIPTION: Used to check the time that is left until the event mail   
*#              message in event_mail is forwarded to the receiver. The    
*#              number of timer ticks left is returned.                    
*#                                                                         
*#-------------------------------------------------------------------------
*#                                                                         
*# HISTORY                                                                 
*#                                                                         
*# DATE          NAME              CHANGES                                       
*# ----          ----              -------                                       
*# 941031        Bernt B           Creation                                      
*# Feb 28 1995   Willy Sagefalk    Use MAX_DURATION_CNTR
*# Mar 29 1995   Willy Sagefalk    Changed return_val from uword to udword
*# May 31 1995   Willy Sagefalk    Added count to event mail box
*#************************************************************************#*/
os_time_type os_time_timer(os_event_index_type event_mail)
{
  udword                      return_val;
  uword                       event_mail_slot;
  byte                        event_mail_count;
  event_mail_box_struct HUGE *box;

  /* Split event_mail index to slot nbr and count */

  event_mail_slot  = event_mail & 0x0fff;
  event_mail_count = (byte)(event_mail >> 12);

  while (event_mailbox_ok == FALSE)
  {
    os_delay(1);
  }
  DISABLE();  /* Disable to prevent other HW-ints from creating a mess. */

  if ((event_mail_slot == 0) || ( event_mail_slot >= event_mailbox_size))
  {
    ENABLE();
    return 0;
  } /* endif */

  box = &event_mailbox[event_mail_slot];

  if ((box->used == BOX_FREE)          ||
      (box->used == EVENT_TOUT)        ||
      (box->used == EVENT_REMOVED)     ||
      (box->count != event_mail_count))
  {
    /* Trying to time a timer that is not allocated, already removed or */
    /* or not allocated by you */
    OSYSTD(ax_printf("OSYS: ILLEGAL OS TIME TIMER \n"));
    return_val = 0;
  }
  else if (box->used == BOX_USED)
  {
    return_val = box->send_time - duration_cntr;
  }
  else
  {
    return_val = MAX_DURATION_CNTR;  /* If this happens, something serious is wrong. */
  }
  ENABLE();  /* And enable again. */

  return return_val;  /* Return from OS call. */
}
#endif

/***************************************************************************
*#
*# FUNCTION NAME: bool os_is_started
*#
*# PARAMETERS:    None.
*#
*# RETURNS:       TRUE when OSYS is running, else FALSE.
*#
*# DESCRIPTION:   This function returns TRUE when OSYS is running, else FALSE.
*#
*#**************************************************************************/
bool os_is_started( void )
{
  return os_started_indicator;
}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: bool os_int_send()                                       */
/*#                                                                         */
/*# PARAMETERS:  uword src_id;      The sender and sub sender for the mail. */
/*#              uword dst_id;      The destination and sub adress for mail.*/
/*#              uword action_id;   The required action on this mail.       */
/*#              byte param_type;   The types of param and param_ptr.       */
/*#              dword param;       Message to reciver.                     */
/*#              void   *param_ptr; Message pointer to reciver.             */
/*#                                                                         */
/*# RETURNS:     TRUE if all is ok, FALSE if mailboxes are full.            */
/*#                                                                         */
/*# DESCRIPTION: Used to send a message from an interrupt task to somone    */
/*#              else. The message is in the parameters in the function     */
/*#              call. The function returns TRUE in normal operation. If    */
/*#              FALSE is returned, the mailboxes are full. The function    */
/*#              should always be called at the end of the interrupt routine*/
/*#              since it could take a while before returning. It should be */
/*#              regarded as an interrupt exit routine done last in the     */
/*#              interrupt.                                                 */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 921116    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
bool os_int_send(os_src_id_type    src_id,
                 os_dst_id_type    dst_id,
                 os_action_id_type action_id,
                 byte              param_type,
                 dword             param,
                 void             *param_ptr)

{                   /* No enable disable needed here, already in int. */
  bool return_val;
  func_struct *call;
  byte saved_executing_task;
/*  byte interrupt_task; */  /* debug */

  saved_executing_task = executing_task;
  executing_task = (byte)((src_id >> 8) & 0xFF);  /* Get the interrupt task number. */
/*  interrupt_task = executing_task; */ /* debug */

  call = &call_info[executing_task];

  call->call_func = OS_INT_SEND;                   /* OS info values. */

  call->src_id = src_id;
  call->dst_id = dst_id;
  call->action_id = action_id;
  call->param_type = param_type;
  call->param = param;
  call->param_ptr = param_ptr;

  OS_FUNCTION_REQUEST();    /* Step into the operating system to save mail. */

  return_val = return_info[executing_task].return_val;
  executing_task = saved_executing_task;  /* Restore to original task. */

/*
  if (return_val == FALSE)
  {
    ax_printf("###### INT %d from os_int_send: mailbox full.\n",interrupt_task);
  }
*/
  return return_val;  /* Return from OS. */
}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: bool os_send()                                           */
/*#                                                                         */
/*# PARAMETERS:  uword src_id;      The sender and sub sender for the mail. */
/*#              uword dst_id;      The destination and sub adress for mail.*/
/*#              uword action_id;   The required action on this mail.       */
/*#              byte param_type;   The types of param and param_ptr.       */
/*#              dword param;       Message to reciver.                     */
/*#              void   *param_ptr; Message pointer to reciver.             */
/*#                                                                         */
/*# RETURNS:     TRUE if all is ok, FALSE if mailboxes are full.            */
/*#                                                                         */
/*# DESCRIPTION: Used to send a message to somone else. The message is in   */
/*#              the parameters in the function call. The function returns  */
/*#              TRUE in normal operation. If FALSE is returned, the mail-  */
/*#              boxes are full. Then, wait for a while and try again.      */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 920731    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
bool os_send(os_src_id_type    src_id,
             os_dst_id_type    dst_id,
             os_action_id_type action_id,
             byte              param_type,
             dword             param,
             void             *param_ptr)
{
  func_struct *call;
  bool return_val;

  DISABLE();  /* Disable to prevent other HW-ints to create a mess. */

  call = &call_info[executing_task];

  call->call_func = OS_SEND;  /* OS info values. */

  call->src_id = src_id;
  call->dst_id = dst_id;
  call->action_id = action_id;
  call->param_type = param_type;
  call->param = param;
  call->param_ptr = param_ptr;

  OS_FUNCTION_REQUEST();      /* Step into the operating system. */

  return_val = return_info[executing_task].return_val;  /* Save return val to stack. */
  ENABLE();  /* And enable again. */

/*
  if (return_val == FALSE)
  {
    ax_printf("###### Task %d from os_send: mailbox full.\n",executing_task);
  }
*/
  return return_val;  /* Returned from OS. */
}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: bool os_get()                                            */
/*#                                                                         */
/*# PARAMETERS:  uword wait_period; Number of os ticks to wait for mail.    */
/*#              mail_struct *msg;  Ptr to msg to fill in mail msg in.      */
/*#                                                                         */
/*# RETURNS:     TRUE if mail arrived, FALSE if timeout occured.            */
/*#                                                                         */
/*# DESCRIPTION: Used to get a message. The message adress is in msg.       */
/*#              If the retuned value is FALSE the timeout time indicated   */
/*#              by wait_period has elapsed without any mail arriving.      */
/*#              If the wait period is 0xFFFF, a timeout wait of infinite   */
/*#              time is assumed. If FALSE is returned (timeout occured)    */
/*#              the contents of the mail msg is undefined.                 */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 920731    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
bool os_get(os_time_type wait_period,mail_struct *msg)
{
  bool return_val;
  func_struct *call;
  func_struct *ret;


  DISABLE();  /* Disable to prevent other HW-ints to create a mess. */

  call = &call_info[executing_task];

  call->call_func = OS_GET;  /* OS info values. */
  call->delay_time = wait_period;

  OS_FUNCTION_REQUEST();

  ret = &return_info[executing_task];

  msg->src_id = ret->src_id;          /* Store the mail message. */
  msg->dst_id = ret->dst_id;
  msg->action_id = ret->action_id;
  msg->param_type = ret->param_type;
  msg->param = ret->param;
  msg->param_ptr = ret->param_ptr;

  return_val = ret->return_val;  /* Store return value for later use. */

  ENABLE();  /* And enable again. */

/* mail_debug(msg,return_val,"MAIL: "); Do after store of data, debug only. */

  return return_val;  /* Return return value. */
}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: bool os_send_sync()                                      */
/*#                                                                         */
/*# PARAMETERS:  uword wait_period; Os ticks, wait for mail to be forwarded */
/*#              uword src_id;      The sender and sub sender for the mail. */
/*#              uword dst_id;      The destination and sub adress for mail.*/
/*#              uword action_id;   The required action on this mail.       */
/*#              byte param_type;   The types of param and param_ptr.       */
/*#              dword param;       Message to reciver.                     */
/*#              void   *param_ptr; Message pointer to reciver.             */
/*#                                                                         */
/*# RETURNS:     TRUE if mail was recived, FALSE if timeout occured.        */
/*#                                                                         */
/*# DESCRIPTION: Used to send a message to someone else and to be sure the  */
/*#              other one got the message. The message is sent in the      */
/*#              parameters in the function call. The function returns TRUE */
/*#              if the receiving task got the message within the time      */
/*#              indicated by wait_period. If the return value is FALSE the */
/*#              timeout time indicated by wait_period has elapsed without  */
/*#              any mail being forwarded. If the wait period is 0xFFFF, a  */
/*#              timeout wait of infinite time is assumed.                  */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 920731    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
bool os_send_sync(os_time_type      wait_period,
                  os_src_id_type    src_id,
                  os_dst_id_type    dst_id,
                  os_action_id_type action_id,
                  byte              param_type,
                  dword             param,
                  void             *param_ptr)
{
  func_struct *call;
  bool return_val;

  DISABLE();  /* Disable to prevent other HW-ints to create a mess. */

  call = &call_info[executing_task];

  call->call_func = OS_SEND_SYNC;  /* OS info values. */
  call->delay_time = wait_period;

  call->src_id = src_id;
  call->dst_id = dst_id;
  call->action_id = action_id;
  call->param_type = param_type;
  call->param = param;
  call->param_ptr = param_ptr;

  OS_FUNCTION_REQUEST();      /* Step into the operating system. */

  return_val = return_info[executing_task].return_val;  /* Save return val to stack. */
  ENABLE();  /* And enable again. */

  return return_val;  /* Returned from OS. */
}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: bool os_get_sync()                                       */
/*#                                                                         */
/*# PARAMETERS:  uword wait_period; Number of os ticks to wait for mail.    */
/*#              mail_struct *msg;  Ptr to adress to fill in mail msg in.   */
/*#                                                                         */
/*# RETURNS:     TRUE if mail arrived, FALSE if timeout occured.            */
/*#                                                                         */
/*# DESCRIPTION: Used to get a message. The mail message is returned in the */
/*#              memory at the location parameter msg is pointing.          */
/*#              When receiving a syncronous mail the other task is waiting */
/*#              for the receiving task to make a call to os_end_sync.      */
/*#              After the os_end_sync call, both tasks operate in parallel */
/*#              again. If the retuned value is FALSE the timeout time      */
/*#              indicated by wait_period has elapsed without any mail      */
/*#              arriving. If the wait period is 255, a timeout wait of     */
/*#              infinite time is assumed. If FALSE is returned (timeout    */
/*#              occured) the contents of the mail msg is undefined.        */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 920731    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
bool os_get_sync(os_time_type wait_period,mail_struct *msg)
{
  byte return_val;
  func_struct *call;
  func_struct *ret;

  DISABLE();  /* Disable to prevent other HW-ints to create a mess. */

  call = &call_info[executing_task];

  call->call_func = OS_GET_SYNC;  /* OS info values. */
  call->delay_time = wait_period;

  OS_FUNCTION_REQUEST();

  ret = &return_info[executing_task];

  msg->src_id = ret->src_id;          /* Store the mail message. */
  msg->dst_id = ret->dst_id;
  msg->action_id = ret->action_id;
  msg->param_type = ret->param_type;
  msg->param = ret->param;
  msg->param_ptr = ret->param_ptr;

  return_val = ret->return_val;  /* Store the return value for later use. */

  ENABLE();  /* And enable again. */

/* mail_debug(msg,return_val,"SYNC: "); Do after store of data, debug only. */

  return return_val;  /* Return return value. */
}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: void   os_end_sync()                                     */
/*#                                                                         */
/*# PARAMETERS:  void                                                       */
/*#                                                                         */
/*# RETURNS:     Nothing.                                                   */
/*#                                                                         */
/*# DESCRIPTION: Used to end a syncronuos mail message forwarding. When     */
/*#              receiving a syncronous mail the sending task is waiting    */
/*#              for the receiving task to make a call to os_end_sync.      */
/*#              After the os_end_sync call, both tasks operate in parallel */
/*#              again.                                                     */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 920731    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
void os_end_sync(void)
{
  DISABLE();  /* Disable to prevent other HW-ints to create a mess. */

  call_info[executing_task].call_func = OS_END_SYNC;  /* OS info values. */

  OS_FUNCTION_REQUEST();  /* Enter operating system. */

  ENABLE();  /* And enable again. */
}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: void   os_delay()                                        */
/*#                                                                         */
/*# PARAMETERS:  uword delay;   The wanted delay in os ticks.               */
/*#                                                                         */
/*# RETURNS:     Nothing.                                                   */
/*#                                                                         */
/*# DESCRIPTION: Used to suspend a task for a time, allowing others to use  */
/*#              the processor. The suspend time is in delay and is in the  */
/*#              form of os ticks.                                          */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 920731    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
void os_delay(os_time_type delay_time)
{
  func_struct *call;

  if (delay_time == 0)
  {
    return;
  }
  DISABLE();  /* Disable to prevent other HW-ints to create a mess. */

  call = &call_info[executing_task];

  call->call_func = OS_DELAY;  /* OS info values. */
  call->delay_time = delay_time;

  OS_FUNCTION_REQUEST();

  ENABLE();  /* And enable again. */
}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: os_time_type os_get_time()                               */
/*#                                                                         */
/*# PARAMETERS:  void                                                       */
/*#                                                                         */
/*# RETURNS:     The current value of the duration counter.                 */
/*#                                                                         */
/*# DESCRIPTION: Used to find out what value the duration counter has. The  */
/*#              duration counter is incremented every time the function    */
/*#              os_tick is called.  It will wrap around after              */
/*#              reaching its maximum value.  For os_time_type ==           */
/*#              uword and SEC_TICS == 100, this will be 10 minutes.  For   */
/*#              os_time_type == udword, this will be 1 year and 11 months  */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 920731    Bernt B         Creation                                      */
/*# 960521    H-P Nilsson     Updated the comment to make it happen         */
/*#-------------------------------------------------------------------------*/
os_time_type os_get_time(void)
{
  return duration_cntr;
}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: os_time_type os_get_seconds()                            */
/*#                                                                         */
/*# PARAMETERS:  void                                                       */
/*#                                                                         */
/*# RETURNS:     The current value of the uptime_in_seconds counter.        */
/*#                                                                         */
/*# DESCRIPTION: Used to find out for how many seconds osys has been        */
/*#              running.  The timer will wraparound after the maximum      */
/*#              value of os_time_type has been reached.  For               */
/*#              os_time_type == uword, this will be 18h.  For              */
/*#              os_time_type === udword, this will be 136 years            */
/*#              (hopefully the silicon will wear out before this happens). */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 960521    H-P Nilsson     Creation.  Note that there is an inline       */
/*#                           definition in osys.h too.                     */
/*#-------------------------------------------------------------------------*/
os_time_type
os_get_seconds(void)
{
  return uptime_in_seconds;
}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: void   os_start()                                        */
/*#                                                                         */
/*# PARAMETERS:  void                                                       */
/*#                                                                         */
/*# RETURNS:     Nothing.                                                   */
/*#                                                                         */
/*# DESCRIPTION: Used to setup the operating system and to start tasks      */
/*#              therein. Should be called from C-code startup routine. If  */
/*#              the operating system is terminated (some task returns from */
/*#              it's main function or calls OS_STOP) the execution will    */
/*#              continue after the os_start call in the startup routine.   */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 920731    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
void os_start(void)
{
#ifdef DEBUG
  osys_statistics.out_of_mailslots     = 0;
  osys_statistics.illegal_remove_timer = 0;
  osys_statistics.timers_started       = 0;
  osys_statistics.timers_removed       = 0;
  osys_statistics.used_mail_slots      = 0;
  osys_statistics.max_used_mail_slots  = 0;
#endif

  STORE_START();  /* Save registers for later return. */
  OS_TICK_OFF();

  executing_task = 0;
  call_info[executing_task].call_func = OS_START;  /* OS info values. */

  init_hw_environment();  /* Setup interrupt vectors etc for OSYS usage. */

  init_task_stacks();

  os_started_indicator = TRUE;          /* OSYS is up and running */

  INITIAL_OS_REQUEST();
  RETRIEVE_START();  /* Retrive registers before returning to OS_START. */
}                    /* This is however not the normal way. (OS_STOP) */

/*#****************************************************************************
*#
*# FUNCTION NAME: void   os_stop()
*#
*# PARAMETERS:    void
*#
*# RETURNS:       Nothing.
*#
*# SIDE EFFECTS:  None
*#
*# DESCRIPTION:   Used to terminate all tasks and stop the operating system.
*#                When called the function will return the processing to the
*#                code after the function call os_start().
*#
*#-------------------------------------------------------------------------
*#
*# HISTORY
*#
*# DATE         NAME              CHANGES
*# ----         ----              -------
*# 920731       Bernt B           Creation
*# Apr 4 1995   Willy Sagefalk    Added disable
*#****************************************************************************/
void os_stop(void)
{
  DISABLE();  /* Disable to prevent other HW-ints to create a mess. */
  os_started_indicator = FALSE;          /* OSYS is shut down */
  restore_hw_environment();  /* Restore interrupt vectors etc. */
  RETRIEVE_START();  /* Retrive registers before returning to OS_START. */
}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: void   os_error()                                        */
/*#                                                                         */
/*# PARAMETERS:  uword src_id;      The sender and sub sender for the mail. */
/*#              uword dst_id;      The destination and sub adress for mail.*/
/*#              uword action_id;     The required action on this mail.     */
/*#              byte param_type;     The types of param and param_ptr.     */
/*#              dword param;         Message to reciver.                   */
/*#              void   *param_ptr;   Message pointer to reciver.           */
/*#                                                                         */
/*# RETURNS:     Nothing.                                                   */
/*#                                                                         */
/*# DESCRIPTION: Should be called if a mailmessage has invalid contents or  */
/*#              could not be understood. The function prints the message   */
/*#              contents.                                                  */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 921019    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
void os_error(os_src_id_type     src_id,
              os_dst_id_type     dst_id,
              os_action_id_type  action_id,
              byte               param_type,
              dword              param,
              void              *param_ptr)
{
#ifdef DEBUG
  ax_printf("ERROR: Task %u: %s Act: %u Pt: %u Pr: %lX Pr_ptr: %p\n",
             executing_task,
             print_who(src_id,dst_id),
             action_id,
             param_type,
             param,
             param_ptr);
#endif
}


/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: uword os_stack()                                         */
/*#                                                                         */
/*# PARAMETERS:  byte task_num;       Task number to probe stack on.        */
/*#                                                                         */
/*# RETURNS:     The number of bytes left on the task stack.                */
/*#                                                                         */
/*# DESCRIPTION: A stack probe function. Finds out how many bytes that are  */
/*#              left unused on the task stack indicated by parameter       */
/*#              task_num. The result, number of bytes left is returned.    */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 921019    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
uword os_stack(byte task_num)
{
  uword n;
  uword stack_size;
  byte *task_stack;

  if (task_num < N_TASKS)
  {
    stack_size = task_list[task_num].stack_size;
    task_stack = task_list[task_num].stack_start - stack_size;

    for (n = 0 ; n < stack_size ; n++)
    {
      if (task_stack[n] != STACK_PATTERN)
      {
        break;
      }
    }
    return n;
  }
  else
  {
    return 0;  /* No bytes left on nonexisting task stack. */
  }
}

/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: void   mail_debug()                                      */
/*#                                                                         */
/*# PARAMETERS:  func_struct *info;   The info to print if debug is on.     */
/*#              bool return_val;     The return code from osys function.   */
/*#              CONST byte *type;    String telling what type of mail.     */
/*#                                                                         */
/*# RETURNS:     Nothing.                                                   */
/*#                                                                         */
/*# DESCRIPTION: Used to print debug message when returning from mail wait  */
/*#              call to OSYS.                                              */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 921105    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
#ifdef DEBUG
void mail_debug(mail_struct *info,bool return_val,CONST byte *type)
{
  byte receiver;

  receiver = (byte)((info->dst_id >> 8) & 0xFF); /* Reciver */

  if ((return_val == TRUE) && (task_list[receiver].debug != NULL) &&
                              (*task_list[receiver].debug == TRUE))
  {
    ax_printf("%s%s Act: %u Pt: %u Pr: %lX Pr_ptr: %p\n",type,print_who(info->src_id,info->dst_id),info->action_id,info->param_type,info->param,info->param_ptr);
  }
}
#endif
#ifdef DEBUG
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# FUNCTION NAME: byte print_who()                                         */
/*#                                                                         */
/*# PARAMETERS:  uword src_id;      The sender and sub sender for the mail. */
/*#              uword dst_id;      The destination and sub adress for mail.*/
/*#                                                                         */
/*# RETURNS:     A string containing the message to print.                  */
/*#                                                                         */
/*# DESCRIPTION: Called to get the sender and reciver of a mail in ascii    */
/*#              text. Returns a pointer to the text string containg the    */
/*#              print information.                                         */
/*#                                                                         */
/*#-------------------------------------------------------------------------*/
/*#                                                                         */
/*# HISTORY                                                                 */
/*#                                                                         */
/*# DATE      NAME            CHANGES                                       */
/*# ----      ----            -------                                       */
/*# 921106    Bernt B         Creation                                      */
/*#-------------------------------------------------------------------------*/
static byte *print_who(os_src_id_type src_id,os_dst_id_type dst_id)
{
  static byte string[25];  /* Cat string. HAS TO BE LONGER IF MORE TEXT IS ADDED. */
  static byte value_str[5];  /* If unknown, id put in this string. */
  byte n;
  byte who;

  string[0] = 0;  /* Init output string. */

  for (n = 0 ; n < 2 ; n++)  /* Do this twice. */
  {
    if (n == 0)
    {
      who = (byte)((src_id >> 8) & 0xFF);  /* Sender */
    }
    else
    {
      strcat(&string[0],"->");

      who = (byte)((dst_id >> 8) & 0xFF);  /* Reciever */
    }

    if (who < N_TASKS)
    {
      strcat(&string[0], task_list[who].name);
    }
    else
    {
      strcat( &string[0], "Unknown " );
      sprintf( value_str, "%d", who );
      strcat( &string[0], value_str );
    }
  }
  return &string[0];
}
#endif /* DEBUG */

/* end of osys.c */
