/*!*************************************************************************
*!
*! FILE NAME   : msg.h
*!
*! DESCRIPTION : Message handler
*!
*!**************************************************************************/

#ifndef _MSG_H_
#define _MSG_H_

#include "projos.h"
#include "osys.h"

/**************** CONSTANTS ************************************************/

/*****  MESSAGE ORIGINATOR BUILDING MACRO  *****************/

#define ORIG(task,originator)  (((task) << 8) + (originator))

/*****  MESSAGE RECIPIENTS AND ORIGINATORS  ****************/

#define OTHER_INT_PROG ORIG(OTHER_INT_TASK, 2)   /* Interrupt task          */
#define THREAD_MAIN    ORIG(THREAD_TASK,    3)

/*****  MESSAGE RECIPIENTS AND ORIGINATORS  ****************/

/*****  MESSAGE BUILDING MACROS  ****************/

#define MSG(task,msg)  ((task & 0x00ff) * 100 + (msg))
#define MSGP(task,msg) ((PROJ_PROG + (task & 0x00ff)) * 100 + (msg))

/******************* Messages ***********************/

#define THREAD_RESCHEDULE              MSG(THREAD_MAIN, 0)
#define THREAD_TIMER_EXPIRED           MSG(THREAD_MAIN, 1)
#define THREAD_PACKET_RECEIVED         MSG(THREAD_MAIN, 2)

/*************************** END OF FILE msg.h ***************************/
#endif /* _MSG_H_ */
