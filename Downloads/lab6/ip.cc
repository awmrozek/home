
#include "compiler.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
extern "C"
{
  #include "msg.h"
  #include "osys.h"
  #include "system.h"
  #include "etrax.h"
  #include "xprintf.h"
}

#include "iostream.hh"
#include "frontpanel.hh"
#include "ethernet.hh"
#include "icmp.hh"
#include "ip.hh"
#include "ipaddr.hh"
#include "tcp.hh"


IP& IP::instance() {
  static IP ip;
  return ip;
}
const IPAddress& IP::myAddress() {
  return *myIPAddress;
}

IP::IP() {

  // Why the hekk doesn't this work!?
  // static IPAddress addr(130,235,200,112);
  // myIPAddress = &addr;

  myIPAddress = new IPAddress(130,235,200,112);
  cout << "IP: " << myAddress() << endl;
}



IPInPacket::IPInPacket(byte* theData,udword theLength,InPacket* theFrame) : InPacket(theData,theLength,theFrame) {}

void IPInPacket::decode() {


  const IPHeader* ipHeader = (const IPHeader*)myData;

  myProtocol = ipHeader->protocol;
  mySourceIPAddress = ipHeader->sourceIPAddress;
  //Sanity checks,non fragmented
  if(ipHeader->versionNHeaderLength == 0x45 && (ipHeader->fragmentFlagsNOffset & 0xFF3F) == 0) {

    if(myProtocol == 0x06) {
      // TCP
      TCPInPacket tcp(myData + IP::ipHeaderLength,HILO(ipHeader->totalLength) - IP::ipHeaderLength,this,mySourceIPAddress);
      tcp.decode();
    } else if(myProtocol == 0x01) {
      // ICMP
      ICMPInPacket icmp(myData + IP::ipHeaderLength,HILO(ipHeader->totalLength) - IP::ipHeaderLength,this);
      icmp.decode();
    }

  }

}

void IPInPacket::answer(byte* theData, udword theLength) {

  static uword seq = 0; // Used to generate unique IP sequence numbers

  theData -= IP::ipHeaderLength;

  IPHeader* replyHeader = (IPHeader*)theData;

  replyHeader->versionNHeaderLength = 0x45;
  replyHeader->TypeOfService = 0;
  replyHeader->totalLength = HILO(IP::ipHeaderLength + theLength);
  replyHeader->identification = seq++;
  replyHeader->fragmentFlagsNOffset = 0;
  replyHeader->timeToLive = 64;
  replyHeader->protocol = myProtocol;
  replyHeader->headerChecksum = 0;
  replyHeader->sourceIPAddress = IP::instance().myAddress();
  replyHeader->destinationIPAddress = mySourceIPAddress;
  replyHeader->headerChecksum = calculateChecksum(theData,IP::ipHeaderLength);

  myFrame->answer(theData,(theLength > 26) ? (theLength + IP::ipHeaderLength) : 46);
}
uword IPInPacket::headerOffset() {

  return IP::ipHeaderLength + myFrame->headerOffset();
}
InPacket* IPInPacket::copyAnswerChain() {

  IPInPacket* anAnswerPacket = new IPInPacket(*this);
  anAnswerPacket->setNewFrame(myFrame->copyAnswerChain());
  return anAnswerPacket;
}
