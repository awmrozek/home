/*!**************************************************************************
*!
*! FILE NAME  : Threads.cc
*!
*! DESCRIPTION: The implementation of threads.
*!
*!**************************************************************************/

#include "threads.ihh"
#include "iostream.hh"
#include "strstream.hh"

extern "C"
{
#include "time.h"
#include "system.h"
#include "msg.h"
}

#include <string.h>
#include <assert.h>

//#define DEBUG_THREADS
#ifdef DEBUG_THREADS
#define trace cout
#else
#define trace if (false) cout
#endif

//*#**************************************************************************
//*# 

Thread::Thread(const char* theName, unsigned theStackSize)
    : stackSize(theStackSize),
      stack(new unsigned char[theStackSize]),
      lastActive(os_get_time()),
      waitingFor(0),
      name(theName)
{
}

//*#**************************************************************************
//*# 

Thread::~Thread()
{
  delete stack;
}


//*#**************************************************************************
//*#
// This initializes the thread (i.e. initial context such as stack pointer)
// and then starts running this thread.
//
void
Thread::start(Thread::Interrupt theInterrupt, bool theSchedule)
{
#ifdef __GNU_CRIS__
  trace << "----- start -----\n";

  // Initialize stack so whe later can determine how much has been used.
  for (unsigned i=0; i<stackSize; i++)
    stack[i] = 0xaa;

  if (setjmp(context) == 0)
  {
    context[1] = reinterpret_cast<unsigned int>(stack + stackSize);
    
    if (theInterrupt == interruptOn)
    {
      context[17] |= 0x20;
    }
    else
    {
      context[17] &= 0xDF;
    }
    if (theSchedule)
    {
      Scheduler::instance().enterLast(this);
      Scheduler::instance().rescheduleIsNeeded();
    }
  }
  else
  {
    entryPoint();
  }
#endif
}

//*#**************************************************************************
//*# 

void
Thread::waitFor(const Semaphore* theSemaphore)
{
  waitingFor = theSemaphore;
}

//*#**************************************************************************
//*# 

void
Thread::sleep(Duration theDuration)
{
  ThreadSleep aSleeper(theDuration);
}

//*#**************************************************************************
//*# 

unsigned int
Thread::stackUsage() const
{
  for (unsigned i=0; i<stackSize; i++)
  {
    if (stack[i] != 0xaa)
      return stackSize - i;
  }
  return stackSize;
}

//*#**************************************************************************
//*# 

void
Thread::entryPoint()
{
  trace << "entryP\n";
  this->proc();
  trace << "KILLED\n";
  // return is not allowed, maybe kill thread
  //assert(false); // Requires ___eax_printf
}


//*#**************************************************************************
//*# 

void
Thread::switchTo(Thread* theThread)
{
  if (theThread == this) return;

#ifdef DEBUG
  if (stackSize > sizeof(unsigned) &&
      *((unsigned*) stack) != 0xaaaaaaaa)
  {
    cerr << "Stack overflow in thread " << name << endl;
  }
  
  lastActive = os_get_time();
#endif
  
  trace << "--switchTo (from: " << hex << (int)this;
  trace << " to: " << (int)theThread << ")" << dec << endl;
  
  if (setjmp(context) == 0)
  {
    trace << "setjmp=0\n";
    trace << "  saved sp= " << hex << (int)context[1] << endl;
    trace << "  new sp= " << (int)theThread->context[1] << dec << endl;
    
    longjmp(theThread->context, 1);
  }
  else
  {
    trace << "setjmp!=0\n";
    trace << "-- switched (to: " << hex << (int)this << ")" << endl;
    trace << "  restored sp= " <<  (int)context[1] << dec << endl;
  }
  trace << "--exit switchTo--\n";
}


//*#**************************************************************************
//*# 

void
Thread::writeStatus(ostream& theStream)
{
  theStream << "Thread status:" << endl;
  theStream << endl;
}

//*#**************************************************************************
//*# 

void
Thread::writeTo(ostream& theStream) const
{
  char buffer[79];

  memset(buffer, 0, 78);
  ostrstream bufferStream(buffer, 78);
  
  bufferStream << name << ", inactive for "
            << (os_get_time() - lastActive)  << "s, stack usage "
            << this->stackUsage() << ", ";
  if (waitingFor)
  {
    bufferStream << "waiting for (" << (unsigned) waitingFor << ") ";
    if (((unsigned) waitingFor) > 0x40000000 &&
        ((unsigned) waitingFor) < 0x40040000)
    {
      waitingFor->writeTo(bufferStream);
    }
  }
  else if (Thread::current() == this)
  {
    bufferStream << "running";
  }
  else
  {
    bufferStream << "ready";
  }

  memset(buffer + strlen(buffer), ' ', 78 - strlen(buffer));
  buffer[78] = 0;
  theStream << buffer << endl;
}

//*#**************************************************************************
//*# 

void
Thread::reschedule()
{
  Scheduler::instance().reschedule();
}

//*#**************************************************************************
//*# 

Thread*
Thread::current()
{
  return Scheduler::instance().current();
}


//*#**************************************************************************
//*#
// main thread (doesn't use stack space)

MainThread::MainThread()
    : Thread("Main", 0)
{}

//*#**************************************************************************
//*# 

void
MainThread::proc()
{}

//*#**************************************************************************
//*# 

Thread*
IdleThread::instance()
{
  static IdleThread* aThread = 0;
  trace << "--- idle thread instance ---\n";
  if (!aThread)
  {
    aThread = new IdleThread;
    aThread->start(Thread::interruptOn, false);
  }
  return aThread;
}

//*#**************************************************************************
//*# 

IdleThread::IdleThread()
    : Thread("Idle", 1024)
{}

//*#**************************************************************************
//*# 

void
IdleThread::proc()
{
  while(true)
  {
    trace << "--- idle thread ---\n";
    for (volatile int i = 0; i < 100; i++);
    DISABLE_SAVE();
    Scheduler::instance().runFirstReady();
    RESTORE();
  }
}


//----------------------------------------------------------------------
//
// Scheduler class methods
//

//*#**************************************************************************
//*# 

Scheduler&
Scheduler::instance()
{
  static Scheduler scheduler;
  return scheduler;
}

//*#**************************************************************************
//*#
// Put current thread last in ready queue and make first thread
// in ready queue the new current thread and switch to that.
// 
void
Scheduler::reschedule()
{
  static time_t aLastReschedule = 0;

  DISABLE_SAVE();

  trace << "--- reschedule ----\n";
  if (!readyQueue.IsEmpty())
  {
    pendingReschedule = true;
    do 
    {
      aLastReschedule = os_get_time();
      Thread* aThread = currentThread;
      readyQueue.Append(currentThread);
      currentThread = readyQueue.Front();
      readyQueue.Pop();
      aThread->switchTo(currentThread);
    }
    while (!readyQueue.IsEmpty());
    pendingReschedule = false;
  }
  else
  {
    trace << " no reschedule done (empty readyQ)\n";
#ifdef DEBUG    
    if ((os_get_time() - aLastReschedule) > 10)
    {
      cerr << "No reschedule in 10s" << endl;
      Thread::writeStatus(cerr);
      aLastReschedule = os_get_time();
    }
#endif    
  }
  
  RESTORE();
}

//*#**************************************************************************
//*#
// enter_last() will put theThread last in the readyQueue but wont
// switch thread.
//
void
Scheduler::enterLast(Thread* theThread)
{
  trace << "--- enter_last ----\n";
  readyQueue.Append(theThread);
  trace << "-----------------\n";
}

//*#**************************************************************************
//*# 

void
Scheduler::enterFirst(Thread* theThread)
{
  trace << "--- enter_first ----\n";
  readyQueue.Prepend(theThread);
  this->intRescheduleIsNeeded();
  trace << "-----------------\n";
}

//*#**************************************************************************
//*#
// Put current thread last in ready queue and enter new thread as
// current. Then switch to the new thread.
//
void
Scheduler::runNew(Thread* theThread)
{
  Thread* aThread = currentThread;

  trace << "--- run_new ----\n";
  readyQueue.Append(currentThread);
  currentThread = theThread;
  aThread->switchTo(currentThread);
}

//*#**************************************************************************
//*#
// Takes first thread in ready queue and makes it current,
// then switches to that thread.
//
void
Scheduler::runFirstReady()
{
  Thread* aThread = currentThread;

  trace << "--- run_first_ready ----\n";
  // hack
  if (readyQueue.IsEmpty())
  {
    trace << "--- ready queue empty --- \n";
    readyQueue.Append(IdleThread::instance());
  }
  // end hack
  currentThread = readyQueue.Front();
  readyQueue.Pop();
  aThread->switchTo(currentThread);
}

//*#**************************************************************************
//*# 

Thread*
Scheduler::current()
{
  return currentThread;
}

//-----------------------------------------------------------------------------
// 

void
Scheduler::rescheduleIsNeeded()
{
  if (!pendingReschedule)
  {
    trace << "Scheduler::rescheduleIsNeeded()\n";
    pendingReschedule = true;
    os_send(THREAD_MAIN, THREAD_MAIN, THREAD_RESCHEDULE, NO_DATA, 0, 0);
  }
}

//****************************************************************************
void
Scheduler::intRescheduleIsNeeded()
{
  if (!pendingReschedule)
  {
    pendingReschedule = true;
    trace << "Scheduler::intRescheduleIsNeeded --- Sending OSYS interrupt mail.\n";
    os_int_send(OTHER_INT_PROG, THREAD_MAIN, THREAD_RESCHEDULE,
                NO_DATA, 0, 0);
    trace << "intRescheduleIsNeeded --- OSYS interrupt mail sent.\n";
  }
}

//*#**************************************************************************
//*# 

Scheduler::Scheduler()
    : currentThread(new MainThread),
      pendingReschedule(false)
{
  trace << "----- init scheduler ------\n";
}


//----------------------------------------------------------------------
//   Semaphore class methods
//

//*#**************************************************************************
//*# 

Semaphore::Semaphore(const char* theName, unsigned int theCount)
    : count(theCount),
      name(theName)
{}

Semaphore::~Semaphore()
{}

//*#**************************************************************************
//*# 

void
Semaphore::writeTo(ostream& theStream) const
{
  theStream << name;
}

//*#**************************************************************************
//*# 

Semaphore*
Semaphore::createQueueSemaphore(const char* theName, unsigned int theCount)
{
  return new QueueSemaphore(theName, theCount);
}

//*#**************************************************************************
//*# 

void
Semaphore::destroyQueueSemaphore(Semaphore* theSemaphore)
{
  delete theSemaphore;
}

//*#**************************************************************************
//*# 

QueueSemaphore::QueueSemaphore(const char* theName, unsigned int theCount)
    : Semaphore(theName, theCount)
{}


//*#**************************************************************************
//*#
// signal() places the first thread from the wait queue last
// in the ready queue. No context switch will be performed.
void
QueueSemaphore::signal()
{
  DISABLE_SAVE();

  trace << "----- signal -----\n";

  // Indicate that we've generated one signal.
  bool aThreadsInQueue = false;
  if (!waitQueue.IsEmpty())
  {
    // If any process in wait queue awaken the first one.
    Thread* aThread = waitQueue.Front();
    aThread->waitFor(0);
    waitQueue.Pop();
    Scheduler::instance().enterLast(aThread);
    aThreadsInQueue = true;
  }
  else
  {
    count++;
  }

  trace << "----- end signal ------\n";

  RESTORE();

  /* 
  ** Must do this outside the DISABLE_SAVE / RESTORE context, because
  ** rescheduleIsNeeded sends an OSYS mail, and OSYS always enables
  ** interrupts.
  */
  if (aThreadsInQueue)
  {
    // Only do this if the waitQueue was not empty above
    Scheduler::instance().rescheduleIsNeeded();
  }  
}


//*#**************************************************************************
//*#
// signal_and_switch() places the first thread from the wait queue
// first in the readQueue and then switches to that, i.e. an immediate thread
// switch to the waiting thread is performed. The current thread
// is placed last in the readyQueue.
//
void
QueueSemaphore::signalAndSwitch()
{
  DISABLE_SAVE();

  trace << "----- signal_and_switch -----\n";

  // Indicate that we've generated one signal.
  if (!waitQueue.IsEmpty())
  {
    // If any process in wait queue we move that from
    // wait queue to first in ready queue. Then we start
    // running that.
    Thread* aThread = waitQueue.Front();
    aThread->waitFor(0);
    waitQueue.Pop();
    Scheduler::instance().runNew(aThread);
  }
  else
  {
    count++;
  }

  trace << "----- end signal_and_switch ------\n";

  RESTORE();
}

//*#**************************************************************************
//*# 
// signalUrgent() places the first thread from the wait queue first
// in the ready queue, if wait queue is empty count is increased.
// No context switch will be performed.
void
QueueSemaphore::signalUrgent()
{
  DISABLE_SAVE();

  trace << "----- signalUrgent -----\n";

  // Indicate that we've generated one signal.
  if (!waitQueue.IsEmpty())
  {
    // If any process in wait queue awaken the first one.
    Thread* aThread = waitQueue.Front();
    aThread->waitFor(0);
    waitQueue.Pop();
    Scheduler::instance().enterFirst(aThread);
  }
  else
  {
    count++;
  }
  
  trace << "----- end signalUrgent ------\n";

  RESTORE();
}

//*#**************************************************************************
//*# 

void
QueueSemaphore::wait()
{
  DISABLE_SAVE();

  trace << "----- wait -----\n";
  if (count == 0) // If there isn't any signal, then wait in queue.
  {               // else return immediately.
    Thread::current()->waitFor(this);
    waitQueue.Append(Thread::current());
    Scheduler::instance().runFirstReady();
  }
  else
  {
    count--;  // Indicate that we've consumed one signal.
  }
  
  trace << "----- end wait ---\n";

  RESTORE();
}

//-----------------------------------------------------------------------------
// 

GateSemaphore::GateSemaphore(bool itIsClosed)
    : iAmClosed(itIsClosed)
{}

GateSemaphore::~GateSemaphore()
{}

//-----------------------------------------------------------------------------
// 

bool
GateSemaphore::isClosed() const
{
  return iAmClosed;
}

//-----------------------------------------------------------------------------
// 

GateSemaphore*
GateSemaphore::create(bool itIsClosed)
{
  return new QueueGateSemaphore(itIsClosed);
}

//-----------------------------------------------------------------------------
// 

QueueGateSemaphore::QueueGateSemaphore(bool itIsClosed)
    : GateSemaphore(itIsClosed)
{}

//-----------------------------------------------------------------------------
// 

void
QueueGateSemaphore::open()
{
  DISABLE_SAVE();

  bool aThreadsInQueue = false;
  while (!waitQueue.IsEmpty())
  {
    // If any thread in wait queue awaken all
    Thread* aThread = waitQueue.Front();
    aThread->waitFor(0);
    waitQueue.Pop();
    Scheduler::instance().enterLast(aThread);
    aThreadsInQueue = true;
  }
  iAmClosed = false;

  RESTORE();

  /* 
  ** Must do this outside the DISABLE_SAVE / RESTORE context, because
  ** rescheduleIsNeeded sends an OSYS mail, and OSYS always enables
  ** interrupts.
  */
  if (aThreadsInQueue)
  {
    // Only do this if there were threads in the queue
    Scheduler::instance().rescheduleIsNeeded();
  }  
}

//-----------------------------------------------------------------------------
// 

void
QueueGateSemaphore::close()
{
  iAmClosed = false;
}

//-----------------------------------------------------------------------------
// 

void
QueueGateSemaphore::wait()
{
  if (iAmClosed)
  {
    DISABLE_SAVE();
    waitQueue.Append(Thread::current());
    Scheduler::instance().runFirstReady();
    RESTORE();
  }
}


//-----------------------------------------------------------------------------
// 

ExclusiveEvent::ExclusiveEvent()
    : myWaitingThread(0)
{}

//-----------------------------------------------------------------------------
// 

void
ExclusiveEvent::await()
{
  DISABLE_SAVE();

  trace << "----- await -----\n";
  assert(!myWaitingThread);  // only one thread may wait at a time!
  Thread::current()->waitFor(0);
  myWaitingThread = Thread::current();
  Scheduler::instance().runFirstReady();
  
  trace << "----- end await ---\n";

  RESTORE();
}


//-----------------------------------------------------------------------------
// 

void
ExclusiveEvent::cause()
{
  DISABLE_SAVE();

  trace << "----- cause -----\n";

  bool aThreadWasWaiting = false;
  if (myWaitingThread)
  {
    Scheduler::instance().enterLast(myWaitingThread);
    myWaitingThread = 0;
    aThreadWasWaiting = true;
  }
  
  trace << "----- cause ------\n";

  RESTORE();

  /* 
  ** Must do this outside the DISABLE_SAVE / RESTORE context, because
  ** rescheduleIsNeeded sends an OSYS mail, and OSYS always enables
  ** interrupts.
  */
  if (aThreadWasWaiting)
  {
    // Only do this if there really was a thread waiting for the event
    Scheduler::instance().rescheduleIsNeeded();
  }  
}

//-----------------------------------------------------------------------------
// 

ThreadSleep::ThreadSleep(Duration theDuration)
{
  this->timeOutAfter(theDuration);
  if (this->isWaitingForTimeOut())
  {
    iHaveTimedOut.await();
  }
}

//-----------------------------------------------------------------------------
// 

void
ThreadSleep::timeOut()
{
  iHaveTimedOut.cause();  
}



