/*!**************************************************************************
*! FILE NAME  : Timer.cc
*! DESCRIPTION:
*! The implementation of the framework for handling timer objects or objects
*! needing a timer.
*! 
*! CLASSES    : <class_list>
*! --------------------------------------------------------------------------
*! HISTORY
*! 
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Dec 28 1995  Fredrik Norrman    Initial version
*! Jan  3 1996  Fredrik Norrman    Added PeriodicTimed
*! Mar 08 1996  Fredrik Norrman    Changed timer message, now directed to
*!                                 cdserv_prog
*! May  2 1996  Fredrik Norrman    Fixed PeriodicTimed::stop...
*! May 15 1996  Fredrik Norrman    Removed compiler warnings
*! Jun 27 1996  Fredrik Norrman    Corrected include
*! Aug 15 1996  Mart Roomus        Using THREAD_MAIN instead of CDSERV_PROG
*! --------------------------------------------------------------------------
*! (C) Copyright 1995, Axis Communications AB, LUND, SWEDEN
*!**************************************************************************/
/* @(#) Timer.cc 1.10 08/16/96 */

extern "C"
{
#include "compiler.h"
#include "system.h"
//#include "project.h"
#include "osys.h"
#include "msg.h"
}

#include "timer.hh"


//-----------------------------------------------------------------------------
// 

Timed::Timed()
{
  myTimerID = 0;
}

//-----------------------------------------------------------------------------
// 

Timed::~Timed()
{
  this->resetTimeOut();
}

//-----------------------------------------------------------------------------
//

void
Timed::timeOutAfter(Duration theTimeToWait)
{
  if (myTimerID)
  {
    os_remove_timer((os_event_index_type)myTimerID);
  }
  myTimerID = os_set_timer((int)theTimeToWait,
                           THREAD_MAIN, THREAD_MAIN, THREAD_TIMER_EXPIRED,
                           TYPE_POINTER, 0, this);
}

//-----------------------------------------------------------------------------
// 

void
Timed::resetTimeOut()
{
  if (myTimerID)
  {
    os_remove_timer((os_event_index_type)this->myTimerID);
    myTimerID = 0;
  } 
}

//-----------------------------------------------------------------------------
// 

void
Timed::timerExpired()
{
  myTimerID = 0;
  this->timeOut();
}

//-----------------------------------------------------------------------------
// 

bool
Timed::isWaitingForTimeOut()
{
  return (myTimerID != 0);
}

//-----------------------------------------------------------------------------
// 

PeriodicTimed::PeriodicTimed()
        : iAmRunning(false),
          myTimerInterval(0)
{
}

//-----------------------------------------------------------------------------
// 

void
PeriodicTimed::timerInterval(Duration theInterval)
{
  myTimerInterval = theInterval;
}

//-----------------------------------------------------------------------------
// 

Duration
PeriodicTimed::timerInterval()
{
  return myTimerInterval;
}

//-----------------------------------------------------------------------------
// 

void
PeriodicTimed::startPeriodicTimer()
{
  if (myTimerInterval)
  {
    this->timeOutAfter(myTimerInterval);
    iAmRunning = true;
  }
}

//-----------------------------------------------------------------------------
// 

void
PeriodicTimed::stopPeriodicTimer()
{
  this->resetTimeOut();
  iAmRunning = false;
}

//-----------------------------------------------------------------------------
// 

void
PeriodicTimed::timeOut()
{
  this->timerNotify();
  if (iAmRunning)
  {
    this->startPeriodicTimer();
  }
}

//-----------------------------------------------------------------------------
// 

