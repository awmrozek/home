#include "compiler.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "iostream.hh"
#include "arp.hh"
#include "ip.hh"

ARPInPacket::ARPInPacket(byte* theData,udword theLength, InPacket*  theFrame): InPacket(theData,theLength,theFrame) {}

void ARPInPacket::decode(){
  const ARPHeader* arpHeader = (const ARPHeader*)myData;

  //sanity check
  if(arpHeader->hardType == 0x0100 && arpHeader->protType == 0x0008 && arpHeader->protSize == 4 && arpHeader->op == 0x0100 ) {
    if(IP::instance().myAddress() == arpHeader->targetIPAddress) {
      byte* aReply = new byte[myLength + myFrame->headerOffset()] + myFrame->headerOffset();

      ARPHeader* replyHeader = (ARPHeader*)aReply;

      replyHeader->hardType = arpHeader->hardType;
      replyHeader->protType = arpHeader->protType;
      replyHeader->hardSize = arpHeader->hardSize;
      replyHeader->protSize = arpHeader->protSize;
      replyHeader->op = 0x0200;
      replyHeader->senderEthAddress = Ethernet::instance().myAddress();
      replyHeader->senderIPAddress = IP::instance().myAddress();
      replyHeader->targetEthAddress = arpHeader->senderEthAddress;
      replyHeader->targetIPAddress = arpHeader->senderIPAddress;


      //28 byte garbage padding

      this->answer(aReply,myLength);
    }
  }

}
void ARPInPacket::answer(byte* theData, udword theLength) {
  myFrame->answer(theData,theLength);
}
uword ARPInPacket::headerOffset() {

  cout << "ARPInPacket::headerOffset used?!" << endl;

  return myFrame->headerOffset() + 0;
}
