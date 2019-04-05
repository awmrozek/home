/*!**************************************************************************
*!                                                              
*! FILE NAME  : projos.c
*!                                                            
*! DESCRIPTION: Contains process declarations.
*!                                                            
*! FUNCTIONS  : get_int_vect
*! (EXPORTED)   init_hw_environment
*!              restore_hw_environment
*!
*!**************************************************************************/

/********************** INCLUDE FILES SECTION ******************************/
#include "compiler.h"
#include "system.h"
#include "projos.h"

#include "osys.h"

/********************** CONSTANT AND MACRO SECTION *************************/
/***************************************************************************/
/*                                                                         */
/*                      TASK STACK SIZE DEFINITION                         */
/*                                                                         */
/***************************************************************************/
#define IDLE_STACK_SIZE        400
#define AX_PRINTF_STACK_SIZE   700 
#define THREAD_MAIN_STACK_SIZE   6000
#define OTHER_INT_STACK_SIZE   700 

/********************** TYPE DEFINITION SECTION ****************************/

/********************** LOCAL FUNCTION DECLARATION SECTION *****************/
extern void idle_main(void);                /* External task entry points. */
extern void ax_printf_disk_main(void);
extern void thread_main(void);


/***************************************************************************/
/*                                                                         */
/*                      EXTERNAL INTERRUPT ROUTINE                         */
/*                                                                         */
/***************************************************************************/

/********************** GLOBAL VARIABLE DECLARATION SECTION ****************/


/***************************************************************************/
/*                                                                         */
/*                      TASK STACK ALLOCATION                              */
/*                                                                         */
/***************************************************************************/
/* Allocation of task stack areas. */
byte idle_task_stack        [IDLE_STACK_SIZE];    
byte ax_printf_task_stack   [AX_PRINTF_STACK_SIZE];
byte thread_main_task_stack [THREAD_MAIN_STACK_SIZE];
byte other_int_task_stack   [OTHER_INT_STACK_SIZE];

/***************************************************************************/
/*                                                                         */
/*                      TASKS DEFINITION                                   */
/*                                                                         */
/***************************************************************************/
task_struct task_list[N_TASKS+1] =           /* The actual task definitions. */
{
/* Name:      Num:              Pri:   Start addr:          
 Stack addr:  Stack size:      Debug */

{"IDLE",      IDLE              ,   0, idle_main,
 NULL, IDLE_STACK_SIZE       , NULL},

{"PRINTF",    PRINTF_DISK_TASK  , 100, ax_printf_disk_main, 
 NULL     , AX_PRINTF_STACK_SIZE  , NULL},

{"PROT",      THREAD_TASK         ,  80, thread_main,           
 NULL     , THREAD_MAIN_STACK_SIZE  , NULL},

{"OTHER_INT", OTHER_INT_TASK    , 255, NULL,                
 NULL     , OTHER_INT_STACK_SIZE  , NULL},

{NULL,        0,    0, NULL,               NULL,0, NULL},
};

byte *os_other_int_stack;

udword stack_list[N_TASKS];

/********************** LOCAL VARIABLE DECLARATION SECTION *****************/
                            /* Place to store the original timer int vect. */
void (interrupt *stored_timer_int_vect) (void);

/********************** FUNCTION DEFINITION SECTION ************************/

/*#**************************************************************************
*#                                                            
*# FUNCTION NAME: get_int_vect
*#                                                            
*# PARAMETERS   : None
*#                                                            
*# RETURNS      : Nothing
*#                                                            
*# SIDE EFFECTS : 
*#                                                            
*# DESCRIPTION  : Used to redirect interrupt vector for clock to interrupt
*#                routine in OSYS, setup for CGA in handler and MIO int.
*#                                                            
*#---------------------------------------------------------------------------
*# HISTORY                                                    
*#                                                            
*# DATE         NAME           CHANGES                       
*# ----         ----           -------                       
*# Jan 19 1992  Bernt Bohmer   Created.
*#
*#**************************************************************************/
void get_int_vect(void)
{
#ifdef HW_PC
#ifndef HW_DOS32

  stored_timer_int_vect = getvect(TIMER_VECT);
  setvect(TIMER_VECT,timer_int);

#endif
#endif
}


#ifdef HW_PC
#ifndef HW_DOS32
/*#**************************************************************************
*#                                                            
*# FUNCTION NAME: init_hw_environment
*#                                                            
*# PARAMETERS   : None
*#                                                            
*# RETURNS      : Nothing
*#                                                            
*# SIDE EFFECTS : 
*#                                                            
*# DESCRIPTION  : Used to redirect interrupt vector for sw int to interrupt
*#                routine in OSYS.
*#                                                            
*#---------------------------------------------------------------------------
*# HISTORY                                                    
*#                                                            
*# DATE         NAME           CHANGES                       
*# ----         ----           -------                       
*# Jan 19 1992  Bernt Bohmer   Created.
*#
*#**************************************************************************/
void init_hw_environment(void)
{

  setvect(SOFTWARE_VECT,software_int);
  setvect(INIT_SOFTWARE_VECT,init_software_int);
}
#endif
#endif

/*#****************************************************************************
*#
*# FUNCTION NAME: init_task_stacks
*#
*# PARAMETERS:    None
*#
*# RETURNS:       nothing
*#
*# DESCRIPTION:   Set up osys stacks
*#
*#-----------------------------------------------------------------------------
*#
*# HISTORY
*#
*# DATE          NAME              CHANGES
*# ----          ----              ------- 
*# Sep 21 1995   Willy Sagefalk    Initial version
*#**************************************************************************#*/

void init_task_stacks(void)
{
  task_list[IDLE].stack_start               = &idle_task_stack        [IDLE_STACK_SIZE]; 
  task_list[PRINTF_DISK_TASK].stack_start   = &ax_printf_task_stack   [AX_PRINTF_STACK_SIZE]; 
  task_list[THREAD_TASK].stack_start        = &thread_main_task_stack [THREAD_MAIN_STACK_SIZE]; 
  task_list[OTHER_INT_TASK].stack_start     = &other_int_task_stack   [OTHER_INT_STACK_SIZE]; 

  os_other_int_stack                        = task_list[OTHER_INT_TASK].stack_start;
}


#ifdef HW_PC
#ifndef HW_DOS32
/*#**************************************************************************
*#                                                            
*# FUNCTION NAME: restore_hw_environment
*#                                                            
*# PARAMETERS   : None
*#                                                            
*# RETURNS      : Nothing
*#                                                            
*# SIDE EFFECTS : 
*#                                                            
*# DESCRIPTION  : Used to redirect interrupt vectors for clock, init
*#                mio_int to the DOS original ones.
*#
*#---------------------------------------------------------------------------
*# HISTORY                                                    
*#                                                            
*# DATE         NAME           CHANGES                       
*# ----         ----           -------                       
*# Jan 19 1992  Bernt Bohmer   Created.
*#
*#**************************************************************************/
void restore_hw_environment(void)
{

  setvect(TIMER_VECT,stored_timer_int_vect);

}
#endif
#endif


/*#**************************************************************************
*#                                                            
*# FUNCTION NAME: click
*#                                                            
*# PARAMETERS   : None
*#                                                            
*# RETURNS      : Nothing
*#                                                            
*# SIDE EFFECTS : 
*#                                                            
*# DESCRIPTION  : Clicks.
*#
*#---------------------------------------------------------------------------
*# HISTORY                                                    
*#                                                            
*# DATE         NAME           CHANGES                       
*# ----         ----           -------                       
*# Jan 19 1992  Bernt Bohmer   Created.
*#
*#**************************************************************************/
#ifdef HW_PC
void click(void)
{

  sound(5000);
  nosound();

}
#endif

/********************** END OF FILE projos.c *******************************/
