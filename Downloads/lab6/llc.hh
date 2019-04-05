/*!***************************************************************************
*!
*! FILE NAME  : llc.hh
*!
*! DESCRIPTION: LLC layer
*!
*!***************************************************************************/

#ifndef llc_hh
#define llc_hh

/****************** INCLUDE FILES SECTION ***********************************/

#include "inpacket.hh"
#include "ethernet.hh"

/****************** CLASS DEFINITION SECTION ********************************/

/*****************************************************************************
*%
*% CLASS NAME   : LLCInPacket
*%
*% BASE CLASSES : InPacket
*%
*% DESCRIPTION  : Decode a LLC packet. Only Ethernet encapsulation is used.
*%                Detects IP and ARP packets, creates and decodes IPInPackets
*%                and ARPInPackets respectively.
*%
*% SUBCLASSING  : None.
*%
*%***************************************************************************/
class LLCInPacket : public InPacket
{
 public:
  LLCInPacket(byte*           theData,
              udword          theLength,
              InPacket*       theFrame,
              EthernetAddress theDestinationAddress,
              EthernetAddress theSourceAddress,
              uword           theTypeLen);
  void decode();
  void answer(byte* theData, udword theLength);
  uword headerOffset();
  InPacket* copyAnswerChain();
 private:
  EthernetAddress myDestinationAddress;
  // Destination address extracted from the packet
  EthernetAddress mySourceAddress;
  // Source address extracted from the packet
  uword           myTypeLen;
  // Type/Length extracted from the packet and byte reordered
};

#endif
/****************** END OF FILE llc.hh *************************************/
