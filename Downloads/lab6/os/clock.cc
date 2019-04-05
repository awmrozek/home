/*!**************************************************************************
*!
*! FILE NAME  : Clock.cc
*! DESCRIPTION:
*! The implementation of the framework for handling time
*! 
*! CLASSES    : <class_list>
*! --------------------------------------------------------------------------
*! HISTORY
*! 
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Jan 27 1996  Fredrik Norrman    Initial version
*! Jun 27 1996  Fredrik Norrman    Corrected include
*! Jan  3 1997  Fredrik Norrman    Added operators for printing time in nice
*!                                 format
*! --------------------------------------------------------------------------
*! (C) Copyright 1996, Axis Communications AB, LUND, SWEDEN
*!**************************************************************************/
// @(#) Clock.cc 1.4 01/03/97

extern "C"
{
#include "compiler.h"
#include "system.h"
//#include "project.h"
#include "osys.h"
//#include "msg.h"
}

#include "clock.hh"


//-----------------------------------------------------------------------------
// 

Time::Time()
{
  myTime = 0;
}

//-----------------------------------------------------------------------------
// 

Time::Time(int theTime)
{
  myTime = theTime;
}

//-----------------------------------------------------------------------------
// 

Time::operator const int() const
{
  return myTime;
}

//-----------------------------------------------------------------------------
// 

ostream&
operator <<(ostream& s, const Time& theTime)
{
  int t = theTime;
  
  int days = t/Clock::days;
  t -= days*Clock::days;
  int hours = t/Clock::hours;
  t -= hours*Clock::hours;
  int minutes = t/Clock::minutes;
  t -= minutes*Clock::minutes;
  int seconds = t/Clock::seconds;
  
  if (days)
  {
    s << days << "d ";
  }
  
  if (days || hours)
  {
    s << hours << "h ";
  }
  
  if (days || hours || minutes)
  {
    s << minutes << "m ";
  }
  
  s << seconds << "s ";
  
  return s;
}

//-----------------------------------------------------------------------------
// 

Duration::Duration()
{
  myDuration = 0;
}

//-----------------------------------------------------------------------------
// 

Duration::Duration(int theDuration)
{
  myDuration = theDuration;
}

//-----------------------------------------------------------------------------
// 

Duration::operator const int() const
{
  return myDuration;
}

//-----------------------------------------------------------------------------
// 

ostream&
operator <<(ostream& s, const Duration& theDuration)
{
  int t = theDuration;
  
  int days = t/Clock::days;
  t -= days*Clock::days;
  int hours = t/Clock::hours;
  t -= hours*Clock::hours;
  int minutes = t/Clock::minutes;
  t -= minutes*Clock::minutes;
  int seconds = t/Clock::seconds;

  if (days)
  {
    s << days << "d ";
  }
  
  if (days || hours)
  {
    s << hours << "h ";
  }
  
  if (days || hours || minutes)
  {
    s << minutes << "m ";
  }
  
  s << seconds << "s ";
  
  return s;
}

//-----------------------------------------------------------------------------
//

Clock::Clock()
{
}

//-----------------------------------------------------------------------------
//

Clock& Clock::instance()
{
  static Clock myInstance;
  return myInstance;
}

//-----------------------------------------------------------------------------
//

Time Clock::currentTime()
{
  return os_get_time();
}

//-----------------------------------------------------------------------------
//
