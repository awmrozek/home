/*!***************************************************************************
*!
*! FILE NAME  : Ethernet.hh
*!
*! DESCRIPTION: Handles the Ethernet layer
*!
*!***************************************************************************/

#ifndef Ethernet_hh
#define Ethernet_hh

/****************** INCLUDE FILES SECTION ***********************************/

#include "inpacket.hh"
#include "job.hh"

/**************** FUNCTION DEFINITION SECTION ********************************/

extern "C" void
ethernet_interrupt();

/****************** CLASS DEFINITION SECTION ********************************/

/*****************************************************************************
*%
*% CLASS NAME   : EthernetAddress
*%
*% BASE CLASSES :
*%
*% CLASS TYPE   :
*%
*% DESCRIPTION  : Represents an Ethernet address.
*%
*% SUBCLASSING  : None.
*%
*%***************************************************************************/
class EthernetAddress
{
 public:
  EthernetAddress();
  // Constructor: do nothing
  EthernetAddress(byte a0, byte a1, byte a2, byte a3, byte a4, byte a5);
  // Constructor: set myAddress

  void writeTo(byte* theData);
  // Copy myAddress to theData

  enum { length = 6 };
  // Length of an ethernet address

  bool operator == (const EthernetAddress& theAddress) const;
  // Check myAddress against theAddress.myAddress

  friend class ostream&
  operator <<(ostream& theStream, const EthernetAddress& theAddress);
  // print this Address to an ostream

 private:
  byte myAddress[length];
};

/*****************************************************************************
*%
*% CLASS NAME   : Ethernet
*%
*% BASE CLASSES : None
*%
*% CLASS TYPE   : Singleton
*%
*% DESCRIPTION  : Handles the Ethernet Layer.
*%
*% SUBCLASSING  : None.
*%
*%***************************************************************************/
class Ethernet
{
 public:
  static Ethernet& instance();
  // Returns the only instance of Ethernet
  const EthernetAddress& myAddress();
  // Ethernet address of this server

  bool getReceiveBuffer();
  // returns 'true' if a packet was found in the receive buffer.
  // This function is called in interrupt context.

  void decodeReceivedPacket();
  // Handles a packet found in 'getReceiveBuffer' but we have now left
  // interrupt context.
  void returnRXBuffer();
  // Return a portion of the receive buffer that has been handled, called
  // at the end of EthernetInPacket::decode
  void transmittPacket(byte* theData, udword theLength);
  // Transmitt an ethernet packet
  void resetTransmitter();
  // Resets the ethernet transmitter if something is seriously wrong

  enum { endPtrOffset   = 0x40000000 };

  enum { txStartAddress = 0x40000000,
         txBufferSize   = 0x2000,
         txBufferPages  = 32 };

  enum { rxStartAddress = 0x40008000,
         rxBufferOffset = 0x8000,
         rxBufferSize   = 0x8000,
         rxBufferPages  = 128  };

  enum { ethernetHeaderLength = 14,
         // Length of the ethernet packet
         commandLength        = 4,
         // Length of the command portion of the first page in a packet
         crcLength            = 4,
         // Length of the ethernet CRC
         minPacketLength      = 46 };
         // Minimum allowed size for the payload of an ethernet packet

 private:
  Ethernet();
  // Constructor: initiate the ethernet interface

  void initMemory();
  // Initiate the rx and tx buffers
  void initEtrax();
  // Initiate the Etrax chip

  EthernetAddress* myEthernetAddress;
  // The ethernet address of this server

  byte  nextRxPage;
  // Next received buffer to be read. Always runs between 0..rxBufferPages-1
  byte  nextTxPage;
  // Next free position in transmitt buffer. Always runs between
  // 0..txBufferPages-1

  byte*  data1;
  // Pointer to the first byte in a received packet
  udword length1;
  // Length of first part of a packet
  byte*  data2;
  // Pointer to the first byte in the wrapped portion of a received packet
  udword length2;
  // Length of the wrapped portion of a received packet
  // data1, length1, data2 and length2 are set by getReceiveBuffer and used
  // by decodeReceivedPacket

  byte*  wrappedPacket;
  // if data2 != NULL the two portions of the packet are copied here by
  // decodeReceivedPacket.
};

/*****************************************************************************
*%
*% CLASS NAME   : BufferPage
*%
*% BASE CLASSES :
*%
*% CLASS TYPE   :
*%
*% DESCRIPTION  : Description of one page in receive and transmitt buffer.
*%
*% SUBCLASSING  : None
*%
*%***************************************************************************/
class BufferPage
{
 public:
  BufferPage() {}
  // Constructor: not used

  byte  statusCommand;
  // rx: State of this page, tx: transmitt command
  byte  notUsed;
  uword endPointer;
  // points to the last byte of this packet
  byte  data[252];
  // the data portion of the page
};


/*****************************************************************************
*%
*% CLASS NAME   : EthernetJob
*%
*% BASE CLASSES : Job
*%
*% CLASS TYPE   :
*%
*% DESCRIPTION  : Starts the thread that decodes the packet.
*%
*% SUBCLASSING  : Subclass and implement decode.
*%
*%***************************************************************************/
class EthernetInPacket;

class EthernetJob : public Job
{
 public:
   EthernetJob(EthernetInPacket* thePacket);
  // Constructor: Initiate myPacket

 private:
  virtual ~EthernetJob() {}
  void doit();
  // decode myPacket

  EthernetInPacket* myPacket;
};

/*****************************************************************************
*%
*% CLASS NAME   : EthernetInPacket
*%
*% BASE CLASSES : InPacket
*%
*% CLASS TYPE   :
*%
*% DESCRIPTION  : Decode a EthernetPacket.
*%
*% SUBCLASSING  : None.
*%
*%***************************************************************************/
class EthernetInPacket : public InPacket
{
 public:
  // EthernetInPacket(const EthernetInPacket& packet);
   EthernetInPacket(byte* theData,
	   udword theLength,
	   InPacket* theFrame);
  // Constructor: Initiate base class InPacket
  void decode();
  // Decode this ethernet packet.
  // Extract ethernet information and pass it up to LLC. This is done by
  // creating a LLCInPacket and decode it. Call returnRXBuffer when done.
  void answer(byte* theData, udword theLength);
  // Upper layers may choose to send an answer to the sender of this packet
  // prepend the appropriate ethernet information and send the packet
  uword headerOffset();
  // Return the length of the ethernet header,
  // see Ethernet::ethernetHeaderLength
  InPacket* copyAnswerChain();
  
 private:
  EthernetAddress myDestinationAddress;
  EthernetAddress mySourceAddress;
  uword           myTypeLen;
};

/*****************************************************************************
*%
*% CLASS NAME   : EthernetHeader
*%
*% BASE CLASSES :
*%
*% CLASS TYPE   :
*%
*% DESCRIPTION  : Contains the actual fields of the EthernetHeader.
*%                Used to handle the ethernet headers
*%
*% SUBCLASSING  : None.
*%
*%***************************************************************************/
class EthernetHeader
{
 public:
   EthernetHeader();

   EthernetAddress destinationAddress;
   EthernetAddress sourceAddress;
   uword           typeLen;
 private:
};

#endif
/****************** END OF FILE Ethernet.hh *********************************/
