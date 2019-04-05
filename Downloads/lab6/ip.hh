/*!***************************************************************************
*!
*! FILE NAME  : ip.hh
*!
*! DESCRIPTION: IP, Internet protocol
*!
*!***************************************************************************/

#ifndef ip_hh
#define ip_hh

/****************** INCLUDE FILES SECTION ***********************************/

#include "inpacket.hh"
#include "ethernet.hh"
#include "ipaddr.hh"

/****************** CLASS DEFINITION SECTION ********************************/

/*****************************************************************************
*%
*% CLASS NAME   : IP
*%
*% BASE CLASSES : None
*%
*% CLASS TYPE   : Singleton
*%
*% DESCRIPTION  : Handles the IP Layer globals.
*%
*% SUBCLASSING  : None.
*%
*%***************************************************************************/
class IP
{
 public:
  static IP& instance();
  const IPAddress& myAddress();
  // return the IP address of this server

  enum { ipHeaderLength = 20 };

 private:
  IP();
  IPAddress* myIPAddress;
  // Holds the IP address of this server
};

/*****************************************************************************
*%
*% CLASS NAME   : IPInPacket
*%
*% BASE CLASSES : InPacket
*%
*% CLASS TYPE   :
*%
*% DESCRIPTION  : Decode an IP packet. Detects ICMP and TCP packets, to this
*%                server. Creates and decodes ICMPInPackets
*%                and TCPInPackets (in lab4) respectively.
*%
*% SUBCLASSING  : None.
*%
*%***************************************************************************/
class IPInPacket : public InPacket
{
 public:
  IPInPacket(byte*           theData,
             udword          theLength,
             InPacket*       theFrame);
  void decode();
  void answer(byte* theData, udword theLength);
  uword headerOffset();
  InPacket* copyAnswerChain();
 private:
  // Things extracted from the packet used in answer:
  byte      myProtocol;
  IPAddress mySourceIPAddress;
};

/*****************************************************************************
*%
*% CLASS NAME   : IPHeader
*%
*% BASE CLASSES : none
*%
*% CLASS TYPE   :
*%
*% DESCRIPTION  : Describes the fields of an IP packet.
*%
*% SUBCLASSING  : None.
*%
*%***************************************************************************/
class IPHeader
{
 public:
  IPHeader() {}

  byte  versionNHeaderLength;
  byte  TypeOfService;
  uword totalLength;
  uword identification;
  uword fragmentFlagsNOffset;
  byte  timeToLive;
  byte  protocol;
  uword headerChecksum;
  IPAddress sourceIPAddress;
  IPAddress destinationIPAddress;
};

#endif
/****************** END OF FILE ip.hh *************************************/
