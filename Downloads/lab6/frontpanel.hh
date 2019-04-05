/*!***************************************************************************
*!
*! FILE NAME  : frontpanel.hh
*!
*! DESCRIPTION: The frontpanel
*!
*!***************************************************************************/

#ifndef frontpanel_hh
#define frontpanel_hh

/****************** INCLUDE FILES SECTION ***********************************/

#include "job.hh"
#include "threads.hh"
#include "timer.hh"

/****************** CLASS DEFINITION SECTION ********************************/

/*****************************************************************************
*%
*% CLASS NAME   : LED
*%
*% BASE CLASSES : None
*%
*% DESCRIPTION  : Represents one LED
*%
*% SUBCLASSING  : None.
*%
*%***************************************************************************/
class LED
{
 public:
   LED(byte theLedNumber);
   // Constructor: initiate the bitmask 'myLedBit'.

   void on();
   // turn this led on
   void off();
   // turn this led off
   void toggle();
   // toggle this led

 private:
   bool iAmOn;
   byte myLedBit;
   // bitmask containing a '1' in the bit position for this led in the led
   // register.
   static byte writeOutRegisterShadow;
   // Shadow of the content of the led register. Must be used to manipulate
   // one led without reseting the others.
};

/*****************************************************************************
*%
*% CLASS NAME   : NetworkLEDTimer
*%
*% BASE CLASSES : None
*%
*% DESCRIPTION  : Represents one LED
*%
*% SUBCLASSING  : None.
*%
*%***************************************************************************/
class NetworkLEDTimer : public Timed
{
 public:
   NetworkLEDTimer(Duration blinkTime);
   // Constructor: initiate myBlinkTime
   void start();
   // Start timer

 private:
   void timeOut();
   // notify FrontPanel that this timer has expired.
   Duration myBlinkTime;
};

/*****************************************************************************
*%
*% CLASS NAME   : CDLEDTimer
*%
*% BASE CLASSES : None
*%
*% DESCRIPTION  : Represents one LED
*%
*% SUBCLASSING  : None.
*%
*%***************************************************************************/
class CDLEDTimer : public PeriodicTimed
{
 public:
   CDLEDTimer(Duration blinkPeriod);
   // Constructor: initiate and start timer

 private:
  void timerNotify();
   // notify FrontPanel that this timer has expired.
};

/*****************************************************************************
*%
*% CLASS NAME   : StatusLEDTimer
*%
*% BASE CLASSES : None
*%
*% DESCRIPTION  : Represents one LED
*%
*% SUBCLASSING  : None.
*%
*%***************************************************************************/
class StatusLEDTimer : public PeriodicTimed
{
 public:
   StatusLEDTimer(Duration blinkPeriod);
   // Constructor: initiate and start timer

 private:
   void timerNotify();
   // notify FrontPanel that this timer has expired.
};

/*****************************************************************************
*%
*% CLASS NAME   : FrontPanel
*%
*% BASE CLASSES : None
*%
*% CLASS TYPE   : Singleton
*%
*% DESCRIPTION  : Handles the LED:s
*%
*% SUBCLASSING  : None.
*%
*%***************************************************************************/
class FrontPanel : public Job
{
 public:
   static FrontPanel& instance();
   // Returns the instance of FrontPanel, used for accessing the FrontPanel
   void packetReceived();
   // turn Network led on and start network led timer
   void notifyLedEvent(uword theLedId);
   // Called from the timers to notify that a timer has expired.
   // Sets an event flag and signals the semaphore.

   enum { networkLedId = 1,
          cdLedId      = 3,
          statusLedId  = 2 };

 private:
   FrontPanel();
   // Constructor: initializes the semaphore the leds and the event flags.
   void doit();
   // Main thread loop of FrontPanel. Initializes the led timers and goes into
   // a perptual loop where it awaits the semaphore. When it wakes it checks
   // the event flags to see which leds to manipulate and manipulates them.

   Semaphore* mySemaphore;

   NetworkLEDTimer* myNetworkLEDTimer;
   CDLEDTimer*      myCDLEDTimer;
   StatusLEDTimer*  myStatusLEDTimer;

   LED myNetworkLED;
   bool netLedEvent;
   LED myCDLED;
   bool cdLedEvent;
   LED myStatusLED;
   bool statusLedEvent;
};

#endif
/****************** END OF FILE frontpanel.hh ********************************/
