/*!**************************************************************************
*!
*! FILE NAME  : Timer.hh
*!
*! DESCRIPTION: The interface to the framework for handling timer objects or 
*!              objects needing a timer.
*!
*!---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Dec 28 1995  Fredrik Norrman    Initial version
*! Jan  3 1996  Fredrik Norrman    Added PeriodicTimed
*! Mar 29 1996  Jens Johansson     Removed path in include. New file header.
*! May  2 1996  Fredrik Norrman    Fixed PeriodicTimed::stop...
*!---------------------------------------------------------------------------
*!
*! (C) Copyright 1995, Axis Communications AB, LUND, SWEDEN
*!
*!**************************************************************************/
/* @(#) Timer.hh 1.6 05/02/96 */

/********************** INCLUDE FILES SECTION ******************************/
#ifndef Timer_hh
#define Timer_hh

extern "C"
{
#include "compiler.h"
}

#include "clock.hh"

/* ============================================================================
CLASS NAME   : Timed
CLASS TYPE   : Abstract type
IDIOMS       : 
CONCURRENCY  : Sequential access
INSTANTIATION: Subclassed should be instantiated on the heap

RESPONSIBILITIES:

SUBCLASSING:
Subclass and implement timeOut
============================================================================ */

class Timed
{
 public:
  Timed();
  virtual ~Timed();
  //void timeOutAt(Time theTimeOutTime);
  void timeOutAfter(Duration theTimeToWait);
  void resetTimeOut();
  void timerExpired();
  bool isWaitingForTimeOut();
  
 protected:
  virtual void timeOut() = 0;
  
  //Time myWakeUpTime;
  uword myTimerID;
 private:
};


/* ============================================================================
CLASS NAME   : PeriodicTimed
CLASS TYPE   : Abstract type
IDIOMS       : 
CONCURRENCY  : Sequential access
INSTANTIATION: Subclassed should be instantiated on the heap

RESPONSIBILITIES:

SUBCLASSING:
Subclass and implement timerNotify
============================================================================ */

class PeriodicTimed : public Timed
{
 public:
  PeriodicTimed();
  void timerInterval(Duration theInterval);
  Duration timerInterval();
  void startPeriodicTimer();
  void stopPeriodicTimer();
  
 protected:
  void timeOut();
  virtual void timerNotify() = 0;

  bool     iAmRunning;
  Duration myTimerInterval;
};

#endif
/********************** END OF FILE Timer.hh *******************************/
