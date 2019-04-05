;TITLE    Osys assembler routines for ETRAX

/* We only want valid the macros. A typedef or extern declaration
   would not make us happy  */
#define ASSEMBLER_MACROS_ONLY        
#if 0
#include "project.h"
#endif
;!*************************************************************************
;!
;! FILE NAME: OSYS_ETR.S
;!
;! DESCRIPTION: Contains assembler functions used by OSYS in the
;! GNU/ETRAX environment.
;!
;!
;!
;! FUNCTIONS:
;!            osys_timer_int            (Interrupt function called from int.)
;!            enable_hw_int             (Normal function called from C.)
;!            disable_hw_int            (Normal function called from C.)
;!            enable                    (Normal function called from C.)
;!            disable                   (Normal function called from C.)
;!            init_hw_environment       (Normal function called from C.)
;!            restore_hw_environment    (Normal function called from C.)
;!            os_entry                  (Normal function called from C.)
;!            init_software_int         (Normal function called from C.)
;!            STORE_START               (Normal function called from C.)
;!            RETRIEVE_START            (Normal function called from C.)
;!            INIT_SEGS_INFO            (Normal function called from C.)
;!
;!------------------------------------------------------------------------
;!
;! HISTORY
;!
;! DATE     NAME     CHANGES
;! ----     ----     -------
;! 940414   Inge P   Initial version
;! 940418   IP       New reg-saving strategy.
;! 940418   IP       Changed the reg-saving descriptions as well
;! 940418   IP       Include proj_etr.h instead of projos.asi
;! 940427   IP       Corrected timescaling bug in osys_timer_int
;! 941214   Bernt B  Added os_other_int function.
;! 950125   IP       OS_TIMER_FREQ 5->1. enable_timer_int -> _hw_int
;! 950125   IP       Removed illegal characters in os_other_int comments.
;! Mar 10 1995  Stefan Jonsson     Added os_other_int_io and os_other_int_buf.
;! Apr 20 1995  Bernt B”hmer       Made task_list external and renamed it to
;!                                 _task_list so that it can be defined in C-
;!                                 code with only one define of N_TASKS.
;! Apr 26 1995  Hans-Peter Nilsson Added os_other_int_buf_no_reschedule
;! Jul  5 1995  Per Flock          Made sys_stack global for the flashload
;!                                 warp jump.
;! Dec 08 1995  Patrik Bannura     Removed inclusion of proj_etr.h
;!------------------------------------------------------------------------
;!
;! (c) Copyright 1994, Axis Communications AB, LUND, SWEDEN
;!
;!*************************************************************************/
; @(#) osys_etr.s 1.12 12/08/95

;*****   CONSTANTS AND MACROS   ********************************************

.set    OS_DEBUG,       0               ; 1 = ON, 0 = OFF
.set    OS_TIMER_FREQ,  1               ; HW/OS timer int frequency relation

; gcc-cris allocates 20 bytes for the task_list structure in OSYS.H
; NOTE: This structure must be identical !

.set    ti.name_ptr_off,      0         ; dword = 4
.set    ti.task_num_off,      4         ; byte  = 1
.set    ti.priority_off,      5         ; byte  = 1
.set    ti.start_addr_off,    6         ; dword = 4     Task entry address
.set    ti.stack_off,        10         ; dword = 4     stack init address
.set    ti.stack_size_off,   14         ; word  = 2
.set    ti.debug_off,        16         ; dword = 4
                                        ; ---------
.set    TASK_LIST_SIZE,                          20  ; Size of task_list struct

;*****   LOCAL DATA   ******************************************************

.data
.align 1

;*****   GLOBAL DATA   ******************************************************

.comm    sys_stack,  4                 ; Holds system stack pointer

;*****   LOCAL DATA   ******************************************************

hw_int_counter:
.word     0                             ; Counts hw timerints to scale OS calls

hw_int_allowed:
.byte     0                             ; Enables/Disables OS timer and other ints

;*****   CODE SEGMENT   *****************************************************

.text
.align 1

;/*#*************************************************************************
; *#
; *#                       LOCAL PUBLIC ROUTINES
; *#
; *#*************************************************************************/

.global _disable
.global _disable_hw_int
.global _enable
.global _enable_hw_int
.global _init_software_int
.global _init_hw_environment
.global _os_entry
.global osys_timer_int
.global os_other_int_buf
.global os_other_int_io
.global _restore_hw_environment

.global _STORE_START
.global _RETRIEVE_START
.global _INIT_SEGS_INFO
.global os_other_int_buf_no_reschedule

;***************************************************************************


;/*#************************************************************************
; *#
; *# FUNCTION NAME: enable_hw_int
; *#
; *# PARAMETERS:    None.
; *#
; *# RETURNS:       Nothing.
; *#
; *# DESCRIPTION:   Enables OS timer/other interrupts.
; *#
; *#************************************************************************/

_enable_hw_int:

  push    r0
  moveq   1,  r0
  move.b  r0, [hw_int_allowed]
  pop     r0
  ret
  nop                                   ; Delay slot
          

;/*#************************************************************************
; *#
; *# FUNCTION NAME: disable_hw_int
; *#
; *# PARAMETERS:    None.
; *#
; *# RETURNS:       Nothing.
; *#
; *# DESCRIPTION:   Disables OS timer/other interrupts.
; *#
; *#************************************************************************/

_disable_hw_int:

  push    r0
  clear.b r0
  move.b  r0, [hw_int_allowed]
  pop     r0
  ret
  nop                                   ; Delay slot


;/*#************************************************************************
; *#
; *# FUNCTION NAME: enable
; *#
; *# PARAMETERS:    None.
; *#
; *# RETURNS:       Nothing.
; *#
; *# DESCRIPTION:   Enables interrupts.
; *#
; *#*************************************************************************/

_enable:

  ret
  ei                                    ; Delay slot


;/*#************************************************************************
; *#
; *# FUNCTION NAME: disable
; *#
; *# PARAMETERS:    None.
; *#
; *# RETURNS:       Nothing.
; *#
; *# DESCRIPTION:   disables interrupts.
; *#
; *#*************************************************************************/

_disable:

  ret
  di                                    ; Delay slot


;/*#************************************************************************
; *#
; *# FUNCTION NAME: osys_timer_int
; *#
; *# PARAMETERS:    None.
; *#
; *# RETURNS:       Nothing.
; *#
; *# DESCRIPTION:   The timer int used to tick the duration counter.
; *# The CPU-status must have been saved in the following order before
; *# this function is entered: irp, srp, ccr, r0 - r13.
; *#*************************************************************************/

osys_timer_int:

;*****   CHECK IF ENOUGH TIME HAS ELAPSED TO NOTIFY OSYS   *****************

  movu.w  [hw_int_counter], r0
  addq    1, r0
  move.w  r0, [hw_int_counter]
  cmp.w   OS_TIMER_FREQ, r0
  blt     exit_osys_timer_int
  nop                                   ; Delay slot

;*****   CHECK THAT MAC ALLOWS STACK SWITCH   ******************************

  test.b  [_enable_process_swapping]    ; Sets Zero-flag when zero
  beq     exit_osys_timer_int
  nop                                   ; Delay slot

;*****   CHECK THAT THE KERNEL ALLOWS TIMER INTERRUPTS   *******************

  test.b  [hw_int_allowed]              ; Sets Zero-flag when zero
  beq     exit_osys_timer_int
  nop                                   ; Delay slot
  clear.w [hw_int_counter]              ; Actually entering OS, clear int cnt

;*****   SAVE STACK POINTER FOR CURRENT TASK   *****************************
                      
  movu.b  [_executing_task], r0         ; Current task number
  move.d  _stack_list, r1
  move.d  sp, [r1 + r0.d]

;*****   RUN OS TIMER TICK ROUTINE WITH OS STACK   ************************

  move.d  [sys_stack], sp               ; Fetch OS stack pointer
  jsr     _tim_int_routine              ; The OS timer tick function

;*****   RESTORE STACK POINTER FOR CURRENT TASK   *************************

  movu.b  [_executing_task], r0         ; Current task number
  move.d  _stack_list, r1
  move.d  [r1 + r0.d], sp

;*****   RESTORE CPU STATE AND EXIT   ***************************************

exit_osys_timer_int:
  movem   [sp+], r13                    ; Restore regs saved by boot_etr.s
  pop     ccr                           ; Restore int flag to original status
  pop     srp
  jump    [sp+]                         ; Ints will be enabled after this instr


;/*#************************************************************************
; *#
; *# FUNCTION NAME: os_other_int_buf
; *#
; *# PARAMETERS:    None.
; *#
; *# RETURNS:       Nothing.
; *#
; *# DESCRIPTION:   Called to execute interrupt routines that are supposed to
; *#                send interrupt mails with os_int_send.
; *#                The CPU-status must have been saved in the following order
; *#                before this function is entered: irp, srp, ccr, r0 - r13.
; *#
; *#*************************************************************************/

os_other_int_buf:

;*****   CHECK THAT THE KERNEL ALLOWS INTERRUPTS   *******************

  test.b  [hw_int_allowed]              ; Sets Zero-flag when zero
  beq     exit_osys_other_int_buf
  nop                                   ; Delay slot

;*****   SAVE STACK POINTER FOR CURRENT TASK   *****************************

  movu.b  [_executing_task], r0         ; Current task number
  move.d  _stack_list, r1
  move.d  sp, [r1 + r0.d]

;*****   RUN INTERRUPT ROUTINE WITH OTHER INTS STACK   *******************

  move.d  [_os_other_int_stack], sp     ; Fetch other ints stack pointer
  jsr     _ethernet_interrupt                ; The interrupt function

;*****   RUN OS OTHER INT ROUTINE WITH OS STACK   ************************

  move.d  [sys_stack], sp               ; Fetch OS stack pointer
  jsr     _other_int_routine            ; The OS other int function

;*****   RESTORE STACK POINTER FOR CURRENT TASK   *************************

  movu.b  [_executing_task], r0         ; Current task number
  move.d  _stack_list, r1
  move.d  [r1 + r0.d], sp

;*****   RESTORE CPU STATE AND EXIT   ***************************************

exit_osys_other_int_buf:
  movem   [sp+], r13                    ; Restore regs saved by boot_etr.s
  pop     ccr                           ; Restore int flag to original status
  pop     srp
  jump    [sp+]                         ; Ints will be enabled after this instr

;/*#************************************************************************
; *#
; *# FUNCTION NAME: os_other_int_buf_no_reschedule
; *#
; *# PARAMETERS:    None.
; *#
; *# RETURNS:       Nothing.
; *#
; *# DESCRIPTION:   Called to execute interrupt routines that are supposed to
; *#                send interrupt mails with os_int_send, but at
; *#                the timer-interrupt (tr_timer_interrupt() and timer_interrupt() )
; *#                THIS IS A NORMAL FUNCTION. NO SPECIAL CPU STATUS
; *#                OR REGISTERS NEED BE SAVED BEFORE.
; *#                 The purpose is just to set variables checked by
; *#                os_int_send(), with no rescheduling after nic_interrupt().
; *#                 It is assumed that os_timer_int() (called after
; *#                this) will reschedule (call get_next_exec() in osysese).
; *#
; *#*************************************************************************/

os_other_int_buf_no_reschedule:
  push srp
;*****   CHECK THAT THE KERNEL ALLOWS INTERRUPTS   *******************

  test.b  [hw_int_allowed]              ; Sets Zero-flag when zero
  beq     exit_osys_other_int_buf_no_reschedule
  nop                                   ; Delay slot

;*****   SAVE STACK POINTER FOR CURRENT TASK   *****************************

; Since no rescheduling will be done, we need not save the current
; task, only shift the stack to "other_int"s stack.
  move.d  sp,r10

;*****   RUN INTERRUPT ROUTINE WITH OTHER INTS STACK   *******************

  move.d  [_os_other_int_stack], sp     ; Fetch other ints stack pointer
  push    r10                           ; Save previous stack pointer
  jsr     _ethernet_interrupt                ; The interrupt function

;*****   RUN OS OTHER INT ROUTINE WITH OS STACK   ************************

; Since no rescheduling is done, the task is the same.
; We need just unlink the saved sp.
  move.d  [sp],sp

exit_osys_other_int_buf_no_reschedule:
  jump    [sp+]                         ; Ints will be enabled after this instr

;/*#************************************************************************
; *#
; *# FUNCTION NAME: os_other_int_io
; *#
; *# PARAMETERS:    None.
; *#
; *# RETURNS:       Nothing.
; *#
; *# DESCRIPTION:   Called to execute interrupt routines that are supposed to
; *#                send interrupt mails with os_int_send.
; *#                The CPU-status must have been saved in the following order
; *#                before this function is entered: irp, srp, ccr, r0 - r13.
; *#
; *#*************************************************************************/

os_other_int_io:

;*****   CHECK THAT THE KERNEL ALLOWS INTERRUPTS   *******************

  test.b  [hw_int_allowed]              ; Sets Zero-flag when zero
  beq     exit_osys_other_int_io
  nop                                   ; Delay slot

;*****   SAVE STACK POINTER FOR CURRENT TASK   *****************************

  movu.b  [_executing_task], r0         ; Current task number
  move.d  _stack_list, r1
  move.d  sp, [r1 + r0.d]

;*****   RUN INTERRUPT ROUTINE WITH OTHER INTS STACK   *******************

  move.d  [_os_other_int_stack], sp     ; Fetch other ints stack pointer
  jsr     io_interrupt                  ; The interrupt function

;*****   RUN OS OTHER INT ROUTINE WITH OS STACK   ************************

  move.d  [sys_stack], sp               ; Fetch OS stack pointer
  jsr     _other_int_routine            ; The OS other int function

;*****   RESTORE STACK POINTER FOR CURRENT TASK   *************************

  movu.b  [_executing_task], r0         ; Current task number
  move.d  _stack_list, r1
  move.d  [r1 + r0.d], sp

;*****   RESTORE CPU STATE AND EXIT   ***************************************

exit_osys_other_int_io:
  movem   [sp+], r13                    ; Restore regs saved by boot_etr.s
  pop     ccr                           ; Restore int flag to original status
  pop     srp
  jump    [sp+]                         ; Ints will be enabled after this instr


;/*#************************************************************************
; *#
; *# FUNCTION NAME: init_hw_environment
; *#
; *# PARAMETERS:    None.
; *#
; *# RETURNS:       Nothing.
; *#
; *# DESCRIPTION:   This funtion initializes the hardware for OS operation.
; *#                Not much to do in the NPS XXX-environment.
; *#*************************************************************************/

_init_hw_environment:

  ret
  nop                                   ; Delay slot


;/*#************************************************************************
; *#
; *# FUNCTION NAME: restore_hw_environment
; *#
; *# PARAMETERS:    None.
; *#
; *# RETURNS:       Nothing.
; *#
; *# DESCRIPTION:   This function restores the hardware after OS operation.
; *#                Not much to do in the NPS XXX-environment.
; *#*************************************************************************/

_restore_hw_environment:
  
  ret
  nop                                   ; Delay slot


;/*#************************************************************************
; *#
; *# FUNCTION NAME: os_entry
; *#
; *# PARAMETERS:    None.
; *#
; *# RETURNS:       Nothing.
; *#
; *# DESCRIPTION:   The interface used to enter the operating system.
; *#
; *#*************************************************************************/

_os_entry:

;*****   ADJUST STACK CONTENTS TO MATCH A HW INTERRUPT   *******************

  push    srp                           ; Note: srp
  push    srp
  push    ccr
  subq    56, sp
  movem   r13, [sp]  
  
;*****   SAVE STACK POINTER FOR CURRENT TASK   *****************************
                      
  movu.b  [_executing_task], r0         ; Current task number
  move.d  _stack_list, r1
  move.d  sp, [r1 + r0.d]

;*****   EXECUTE OS FUNCTION WITH OS STACK   *****************************

  move.d  [sys_stack], sp               ; Fetch OS stack pointer
  jsr     _sw_int_routine               ; The OS main function
  move.d  sp, [sys_stack]               ; Save OS stack pointer

;*****   RESTORE STACK POINTER FOR CURRENT TASK   *************************

  movu.b  [_executing_task], r0         ; Current task number
  move.d  _stack_list, r1
  move.d  [r1 + r0.d], sp

;*****   RESTORE CPU STATE AND JUMP TO SELECTED TASK   **********************

  movem   [sp+], r13
  pop     ccr                           ; Restore int flag to original status
  pop     srp
  jump    [sp+]                         ; Ints will be enabled after this instr


;/*#************************************************************************
; *#
; *# FUNCTION NAME: init_software_int
; *#
; *# PARAMETERS:    NO_PARAMS
; *#
; *# RETURNS:       Nothing.
; *#
; *# DESCRIPTION:   This function starts the OS.
; *#
; *#*************************************************************************/

_init_software_int:

  jsr     _sw_int_routine               ; The OS main function

;*****   SAVE OSYS sp AND RESTORE sp FOR CURRENT TASK   *******************

  move.d  sp, [sys_stack]               ; Save OS stack pointer
  movu.b  [_executing_task], r0         ; Current task number
  move.d  _stack_list, r1
  move.d  [r1 + r0.d], sp

;*****   TIDY UP AND EXIT TO INIT POINT OF FIRST TASK  *********************

  movem   [sp+], r13
  pop     ccr                           ; Restore int flag to original status
  pop     srp
  jump    [sp+]                         ; Ints will be enabled after this instr


;/*#************************************************************************
; *#
; *# FUNCTION NAME: STORE_START
; *#
; *# PARAMETERS:    None. 
; *#
; *# RETURNS:       Nothing.
; *#
; *# DESCRIPTION:   This function stores the CPU state for os_start.
; *#                This state is used when the OS is terminated through os_stop.
; *#                Not much to do in the NPS XXX-environment.
; *#*************************************************************************/

_STORE_START:

  ret
  nop                                   ; Delay slot


;/*#************************************************************************
; *#
; *# FUNCTION NAME: RETRIEVE_START
; *#
; *# PARAMETERS:    None.
; *#
; *# RETURNS:       Nothing.
; *#
; *# DESCRIPTION:   Used to retrieve the startup routine status and regs
; *#                before returning to startup routine.
; *#                Not much to do in the NPS XXX-environment.
; *#*************************************************************************/

_RETRIEVE_START:

  ret
  nop                                   ; Delay slot


;/*#************************************************************************
; *#
; *# FUNCTION NAME: INIT_SEGS_INFO
; *#
; *# PARAMETERS: NO_PARAMS
; *#
; *# RETURNS: Nothing.
; *#
; *# DESCRIPTION: Used to setup initial task executing environment. 
; *# The initial stack frame built for all tasks looks like this:
; *# 
; *#  r0-r13                       56
; *#  ccr                           2
; *#  srp                           4
; *#  irp                           4
; *#  master_reset return address   4
; *#
; *#*************************************************************************/

_INIT_SEGS_INFO:

  subq    56, sp                        ; Reserve space for regs,
  movem   r13, [sp]                     ; and save them.

;*****   BUILD STACK FOR ALL TASKS   *************************************

  clear.d r1                            ; Current processed task #
  move.d  _task_list, r2                ; r2 holds current task_list entry

init_loop:
  move.d  [r2 + ti.name_ptr_off], r3    ; Points at task name
  cmpq    0, r3                         ; At end if name ptr null
  beq     exit_init_segs_info           ; All task stacks processed?
  nop

;*****   NULL STACKS ARE IGNORED   **************************************

  move.d  [r2 + ti.stack_off], r3       ; Points at task stack
  cmpq    0, r3                         ; Skip null stacks 
  beq     skip_task
  nop                                   ; Delay slot    
  
;*****   STORE RESET AS LAST RETURN ADDRESS OF TASK (SHOULD NEVER OCCUR)  ***

  subq    4, r3
  move.d  _master_reset, r4
  move.d  r4, [r3]  

;*****   STORE TASK INIT ADDRESS AT irp SLOT   ******************************

  subq    4, r3
  move.d  [r2 + ti.start_addr_off], r4       ; Current tasks init address
  move.d  r4, [r3]  

;*****   STORE RESET AT srp SLOT (SHOULD NEVER OCCUR)   *********************

  subq    4, r3
  move.d  _master_reset, r4                  ; Ultimate reset
  move.d  r4, [r3]  

;*****   PUT A "NORMAL" COPY OF ccr ON THE STACK   ************************

  subq    2,    r3
  push    ccr
  ei                                    ; Make sure ints are enabled.
  move    ccr, [r3]                     ; Note: 2 bytes only
  pop     ccr
  
;*****   ADJUST FOR r0-r13 AND SAVE INITIAL STACKPOINTER   *****************

  subq    56, r3
  move.d  _stack_list, r4
  move.d  r3, [r4 + r1.d]

skip_task:

  add.d   TASK_LIST_SIZE, r2            ; next entry in task_list
  addq    1, r1                         ; next task #
  ba      init_loop 
  nop                                   ; Delay slot

exit_init_segs_info:
  movem   [sp+], r13                    ; Restore saved regs
  ret
  nop                                   ; Delay slot
  

;*****   End of OSYS_ETR.S   ***********************************************
