/*!**************************************************************************
*! FILE NAME  : Threads.hh
*!                                                            
*! DESCRIPTION: The interface to threads
*!              
*!              
*!---------------------------------------------------------------------------
*! HISTORY                                                    
*!                                                            
*! DATE         NAME            CHANGES                       
*! ----         ----            -------                       
*! Apr 28 1995  Kenny R         Initial version               
*! Nov 23 1995  Stefan S        Removed TimeOutSemaphore
*! Feb 08 1996  Stefan S        Added ExclusiveEvent
*! Feb 08 1996  Sven Ekstrom    Moved #endif after ExclusiveEvent.
*! Mar 27 1996  Fredrik Norrman Added Thread::sleep.
*! Apr 01 1996  Jens Johansson  Added destruction of semaphore.
*! May 17 1996  Stefan S        Changed stackSize to unsigned int.
*!---------------------------------------------------------------------------
*! (C) Copyright 1995, Axis Technologies AB, LUND, SWEDEN
*!**************************************************************************/
/* @(#) threads.hh 1.7 05/17/96 */

#ifndef Threads_hh
#define Threads_hh

#include "setjmp.hh"
#include "time.h"
#include "timer.hh"

//*#**************************************************************************
//*# CLASS NAME       : Thread
//*# BASE CLASSES     : none
//*#                    
//*# DESCRIPTION      : Thread of execution
//*#                    
//*# RESPONSIBILITIES : Know the state of one thread of execution,
//*#                    switch to other threads of execution
//*#---------------------------------------------------------------------------
//*# HISTORY                                                    
//*#                                                            
//*# DATE         NAME            CHANGES                       
//*# ----         ----            -------                       
//*# 12 Aug 1995  Stefan S        Initial version               
//*#**************************************************************************/



class Thread
{
  friend class Scheduler;
  friend class ThreadCollection;
  
 public:
  Thread(const char* theName, unsigned theStackSize);
  virtual ~Thread();

  enum Interrupt { interruptOn, interruptOff };
  void start(Interrupt = interruptOn, bool theSchedule = true);
//  void kill();
  void waitFor(const class Semaphore*);
  
  void sleep(Duration theDuration);
  
  static void reschedule();
  
  static Thread* current();
  
  static void writeStatus(class ostream&);
  
  unsigned int stackUsage() const;

 private:
  virtual void proc() = 0;

  void entryPoint();
  void switchTo(Thread*);

  void writeTo(class ostream&) const;
  
  unsigned               stackSize;
  unsigned char*         stack;
  jmp_buf                context;
  time_t                 lastActive;
  const class Semaphore* waitingFor;
  const char*            name;
};





//*#**************************************************************************
//*# CLASS NAME       : Semaphore
//*# BASE CLASSES     : none
//*#                    
//*# DESCRIPTION      : Syncronisation between threads
//*#                    
//*# RESPONSIBILITIES : Stop / start threads of execution 
//*#                    
//*#---------------------------------------------------------------------------
//*# HISTORY                                                    
//*#                                                            
//*# DATE         NAME            CHANGES                       
//*# ----         ----            -------                       
//*# 12 Aug 1995  Stefan S        Initial version
//*#**************************************************************************/

class Semaphore
{
 public:
  Semaphore(const char* theName, unsigned int);
  virtual ~Semaphore();
  
  virtual void signal() = 0;
  virtual void signalAndSwitch() = 0;
  virtual void signalUrgent() = 0;
  virtual void wait() = 0;
  virtual void writeTo(class ostream&) const;


  static Semaphore* createQueueSemaphore(const char* theName, unsigned int);
  static void destroyQueueSemaphore(Semaphore* theSemaphore);
  
 protected:
  volatile unsigned int count;
  const char*           name;
};


/* ============================================================================
CLASS NAME   : GateSemaphore
CLASS TYPE   : Abstract type
IDIOMS       : 
CONCURRENCY  : automatic
INSTANTIATION: by GateSemaphore::create()

RESPONSIBILITIES:
open/close a Thread gate. if gate is closed all threads waiting for it will be
stopped. When the gate opens all threads on hold will be released.

SUBCLASSING:
For concrete implementation.
============================================================================ */

class GateSemaphore
{
 public:
  // public interface:
  virtual void open() = 0;
  virtual void close() = 0;
  bool         isClosed() const;
  virtual void wait() = 0;

  static GateSemaphore* create(bool itIsClosed);
  
 protected:
  GateSemaphore(bool itIsClosed);
  virtual ~GateSemaphore();

  // data:
  bool iAmClosed;
};
  


/* ============================================================================
CLASS NAME   : ExclusiveEvent
CLASS TYPE   : Concrete implementation.
CONCURRENCY  : Automatic.
INSTANTIATION: Not specified.

RESPONSIBILITIES:
Make current thread await some event caused by another thread.
Only one thread at a time may wait for the event.

SUBCLASSING:
Not intended to be subclassed.
============================================================================ */

class ExclusiveEvent
{
 public:
  ExclusiveEvent();

  void await();
  void cause();

 private:
  Thread* myWaitingThread;
};

#endif
