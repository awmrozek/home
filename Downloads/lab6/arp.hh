/*!***************************************************************************
*!
*! FILE NAME  : arp.hh
*! 
*! DESCRIPTION: ARP, Address resolution protocol
*! 
*!***************************************************************************/

#ifndef arp_hh
#define arp_hh

/****************** INCLUDE FILES SECTION ***********************************/

#include "inpacket.hh"
#include "ethernet.hh"
#include "ipaddr.hh"

/****************** CLASS DEFINITION SECTION ********************************/

/*****************************************************************************
*%
*% CLASS NAME   : ARPInPacket
*%
*% BASE CLASSES : InPacket
*%
*% DESCRIPTION  : Decode an ARP packet. If it is an ARP request for my IP an
*%                answer is created and sent back.
*%
*% SUBCLASSING  : None.
*%
*%***************************************************************************/
class ARPInPacket : public InPacket
{
 public:
  ARPInPacket(byte*      theData,
              udword     theLength,
              InPacket*  theFrame);
  void decode();
  void answer(byte* theData, udword theLength);
  uword headerOffset();  
 private:
};

/*****************************************************************************
*%
*% CLASS NAME   : ARPHeader
*%
*% BASE CLASSES : none
*%
*% DESCRIPTION  : Describes the fields of an ARP packet.
*%
*% SUBCLASSING  : None.
*%
*%***************************************************************************/
class ARPHeader
{
 public:
  ARPHeader() {}

  uword hardType;
  uword protType;
  byte  hardSize;
  byte  protSize;
  uword op;
  EthernetAddress senderEthAddress;
  IPAddress       senderIPAddress;
  EthernetAddress targetEthAddress;
  IPAddress       targetIPAddress;
};

#endif
/****************** END OF FILE llc.hh *************************************/

