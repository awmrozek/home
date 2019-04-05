#include "compiler.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "iostream.hh"
#include "frontpanel.hh"
#include "icmp.hh"

ICMPInPacket::ICMPInPacket(byte*           theData,
  udword          theLength,
  InPacket*       theFrame) : InPacket(theData,theLength,theFrame) {}
  void ICMPInPacket::decode() {


    ICMPHeader* header = (ICMPHeader*)myData;
    //sanity check
    if(header->type == 8) {

      byte* aReply = new byte[myLength + myFrame->headerOffset()] + myFrame->headerOffset();
      ICMPHeader* replyHeader = (ICMPHeader*)aReply;
      memcpy(aReply, myData, myLength);

      // Change to reply
      replyHeader->type= 0;

      replyHeader->checksum+=0x8;

      FrontPanel::instance().packetReceived();
      this->answer(aReply, myLength);
    }
  }

  void ICMPInPacket::answer(byte* theData, udword theLength) {

    myFrame->answer(theData, theLength);
  }
  uword ICMPInPacket::headerOffset() {

    return 0;
  }
