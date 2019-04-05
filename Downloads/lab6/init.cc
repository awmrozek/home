/*!**************************************************************************
*! FILE NAME  : init.cc
*!
*! DESCRIPTION: The implementation of initialisation for the
*!              CD server
*!
*!**************************************************************************/

#include <stddef.h>

extern "C" {
#include "compiler.h"
#include "system.h"
#include "osys.h"
#include "msg.h"
#include "reschedule.h"
}

#include "iostream.hh"
#include "threads.hh"
#include "job.hh"

#include "timer.hh"

#include "frontpanel.hh"
#include "ethernet.hh"
#include "llc.hh"
#include "ip.hh"
#include "tcp.hh"
#include "fs.hh"

#define D_INIT
#ifdef D_INIT
#define trace cout
#else
#define trace if(false) cout
#endif

//*#**************************************************************************
//*#
void* operator new(size_t mem_size)
{
  return ::ax_malloc(mem_size);
}

//*#**************************************************************************
//*#
void operator delete(void* ptr)
{
  ::ax_free(ptr);
}

//*#**************************************************************************
//*#
enum { numberOfServers   = 20,
       serverStackSize   = 4500,
       jobQueueSoftLimit = 100 };

void initJob() {
  // The enums are casted to ints, 'cos some compilers thinks that's cool.

  Job::createServers(numberOfServers, serverStackSize, jobQueueSoftLimit);
}

//*#**************************************************************************
//*#
extern "C" void
thread_main(void)
{
  initJob();
  Ethernet::instance();
  FrontPanel::instance();
  IP::instance();
  TCP::instance();
  FileSystem::instance();

  Job::schedule(&FrontPanel::instance());

  while (1)
  {
    mail_struct  msg;
    if (os_get(WAIT_FOREVER, &msg))
    {
      switch (msg.action_id)
      {
        case THREAD_TIMER_EXPIRED:
        {
          Timed* timerObj = (Timed*)msg.param_ptr;
          timerObj->timerExpired();
        }
        break;

       case THREAD_RESCHEDULE:
       {
         reschedule();
       }
       break;

       case THREAD_PACKET_RECEIVED:
       {
         Ethernet::instance().decodeReceivedPacket();
       }
       break;

       default:
         break;
      }
    }
  }
}
