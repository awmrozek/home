/*!***************************************************************************
*!
*! FILE NAME  : FrontPanel.cc
*!
*! DESCRIPTION: Handles the LED:s
*!
*!***************************************************************************/

/****************** INCLUDE FILES SECTION ***********************************/

#include "compiler.h"

#include "iostream.hh"
#include "frontpanel.hh"
#include "memory/sp_alloc.h"

extern "C" {
#include "system.h"
}

//#define D_FP
#ifdef D_FP
#define trace cout
#else
#define trace if(false) cout
#endif

/****************** FrontPanel DEFINITION SECTION ***************************/

//----------------------------------------------------------------------------
//
byte LED::writeOutRegisterShadow = 0x78;

LED::LED(byte theLedNumber) {

  iAmOn = false;
  myLedBit = 4 << theLedNumber;
}

void LED::on() {
  if(iAmOn) return;
  iAmOn = true;
  *(VOLATILE byte*)0x80000000 = writeOutRegisterShadow &= ~myLedBit;
}
void LED::off() {
  if(!iAmOn) return;
  iAmOn = false;
  *(VOLATILE byte*)0x80000000 = writeOutRegisterShadow |= myLedBit;
}
void LED::toggle() {

  if(iAmOn) {
    off();
  } else {
    on();
  }
}


//----------------------------------------------------------------------------
//
NetworkLEDTimer::NetworkLEDTimer(Duration blinkTime):myBlinkTime(blinkTime){}

void NetworkLEDTimer::start() {

  timeOutAfter(myBlinkTime);
}

void NetworkLEDTimer::timeOut() {

  FrontPanel::instance().notifyLedEvent(FrontPanel::networkLedId);
}

//----------------------------------------------------------------------------
//

CDLEDTimer::CDLEDTimer(Duration blinkPeriod) {

  timerInterval(blinkPeriod);
}
void CDLEDTimer::timerNotify() {

  FrontPanel::instance().notifyLedEvent(FrontPanel::cdLedId);
}

//----------------------------------------------------------------------------
//

StatusLEDTimer::StatusLEDTimer(Duration blinkPeriod) {

  timerInterval(blinkPeriod);
}
void StatusLEDTimer::timerNotify() {

  FrontPanel::instance().notifyLedEvent(FrontPanel::statusLedId);
}

//----------------------------------------------------------------------------
//

FrontPanel& FrontPanel::instance() {
  static FrontPanel myInstance;
  return myInstance;
}
void FrontPanel::packetReceived() {


  myNetworkLED.on();
  myNetworkLEDTimer->start();
}
void FrontPanel::notifyLedEvent(uword theLedId) {

  switch(theLedId) {

    case networkLedId:
      netLedEvent = true;
      break;

    case cdLedId:
      cdLedEvent = true;
      break;

    case statusLedId:
      statusLedEvent = true;
      break;
  }

  mySemaphore->signal();
}

FrontPanel::FrontPanel() : myNetworkLED(networkLedId),myCDLED(cdLedId),myStatusLED(statusLedId) {

  netLedEvent = false;
  cdLedEvent = false;
  statusLedEvent = false;

  mySemaphore = Semaphore::createQueueSemaphore("semi",0);
  myNetworkLEDTimer = new NetworkLEDTimer(SEC_TICS / 4);
  myCDLEDTimer = new CDLEDTimer(SEC_TICS / 4);
  myStatusLEDTimer = new StatusLEDTimer(10 * SEC_TICS);

}

void FrontPanel::doit() {

  myCDLEDTimer->startPeriodicTimer();
  myStatusLEDTimer->startPeriodicTimer();

  while(1) {

    mySemaphore->wait();

    if(netLedEvent) {
      myNetworkLED.toggle();
      netLedEvent = false;
      ax_printf("Memory left: %u\r\n",ax_coreleft_total());
    }
    if(cdLedEvent) {
      myCDLED.toggle();
      cdLedEvent = false;
    }
    if(statusLedEvent) {
      myStatusLED.toggle();
      statusLedEvent = false;
    }

  }

}
// Main thread loop of FrontPanel. Initializes the led timers and goes into
// a perptual loop where it awaits the semaphore. When it wakes it checks
// the event flags to see which leds to manipulate and manipulates them.


/****************** END OF FILE FrontPanel.cc ********************************/
