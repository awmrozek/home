/*!***************************************************************************
*!
*! FILE NAME  : icmp.hh
*! 
*! DESCRIPTION: ICMP, Internet control message protocol
*! 
*!***************************************************************************/

#ifndef icmp_hh
#define icmp_hh

/****************** INCLUDE FILES SECTION ***********************************/

#include "inpacket.hh"
#include "ethernet.hh"
#include "ipaddr.hh"

/****************** CLASS DEFINITION SECTION ********************************/

/*****************************************************************************
*%
*% CLASS NAME   : ICMPInPacket
*%
*% BASE CLASSES : InPacket
*%
*% CLASS TYPE   : 
*%
*% DESCRIPTION  : Decode a ICMP packet. Detects and answers ECHO packets.
*%
*% SUBCLASSING  : None.
*%
*%***************************************************************************/
class ICMPInPacket : public InPacket
{
 public:
  ICMPInPacket(byte*           theData,
               udword          theLength,
               InPacket*       theFrame);
  void decode();
  void answer(byte* theData, udword theLength);
  uword headerOffset();

  enum { icmpHeaderLen = 4,
         icmpEchoHeaderLen = 4 };

 private:
};

/*****************************************************************************
*%
*% CLASS NAME   : ICMPHeader
*%
*% BASE CLASSES : none
*%
*% CLASS TYPE   : 
*%
*% DESCRIPTION  : Describes the fields of an ICMP packet.
*%
*% SUBCLASSING  : None.
*%
*%***************************************************************************/
class ICMPHeader
{
 public:
  ICMPHeader() {}

  byte  type;
  byte  code;
  uword checksum;
};

/*****************************************************************************
*%
*% CLASS NAME   : ICMPECHOHeader
*%
*% BASE CLASSES : none
*%
*% CLASS TYPE   : 
*%
*% DESCRIPTION  : Describes the fields of an ICMP packet.
*%
*% SUBCLASSING  : None.
*%
*%***************************************************************************/
class ICMPECHOHeader
{
 public:
  ICMPECHOHeader() {}
  
  uword identifier;
  uword sequenceNumber;

  // Variable amount of data, at least 1 byte though...
  byte  data[1];
};


#endif
/****************** END OF FILE icmp.hh *************************************/





