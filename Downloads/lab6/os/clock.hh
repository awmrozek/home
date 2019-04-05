/*!**************************************************************************
*!
*! FILE NAME  : Clock.hh
*! DESCRIPTION:
*! The interface to the framework for handling clock related things
*!
*! CLASSES    : <class_list>
*! --------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Jan 27 1996  Fredrik Norrman    Initial version
*! Jan  3 1997  Fredrik Norrman    Added operators for printing time in nice
*!                                 format
*! --------------------------------------------------------------------------
*! (C) Copyright 1996, Axis Communications AB, LUND, SWEDEN
*!**************************************************************************/
// @(#) Clock.hh 1.2 01/03/97

#ifndef Clock_hh
#define Clock_hh

#include "iostream.hh"

extern "C"
{
#include "compiler.h"
#include "system.h"
//#include "project.h"
#include "projos.h"
#include "osys.h"
}

class Duration;

/* ============================================================================
CLASS NAME   : Time
CLASS TYPE   : Concrete type
IDIOMS       :
CONCURRENCY  : Sequential access
INSTANTIATION:

RESPONSIBILITIES:
Representation of absolute time.

SUBCLASSING:
============================================================================ */

class Time
{
 public:
  Time();
  Time(int theTime);
  operator const int() const;

 protected:
  int myTime;
};

ostream&
operator <<(ostream&s, const Time& t);


//Duration operator -(const Time, const Time);

/* ============================================================================
CLASS NAME   : Duration
CLASS TYPE   : Concrete type
IDIOMS       :
CONCURRENCY  : Sequential access
INSTANTIATION:

RESPONSIBILITIES:
Representation of time duration, as opposed to absolute time, this class
represents relative time.

SUBCLASSING:
============================================================================ */

class Duration
{
 public:
  Duration();
  Duration(int theDuration);
  operator const int() const;

 protected:
  int myDuration;
};

ostream&
operator <<(ostream&s, const Duration& t);

/* ============================================================================
CLASS NAME   : Clock
CLASS TYPE   : Concrete type
IDIOMS       :
CONCURRENCY  : Sequential access
INSTANTIATION: Subclassed should be instantiated on the heap

RESPONSIBILITIES:
Namespace for time constants
Singleton provides time functions

SUBCLASSING:
============================================================================ */

class Clock
{
 public:
  static Clock& instance();

  Time currentTime();

  enum
  {
    tics = 1,
    seconds = SEC_TICS,
    minutes = WAIT_ONE_MINUTE,
    hours = minutes*60,
    days  = hours*24
  };

 protected:
  Clock();
};


#endif
