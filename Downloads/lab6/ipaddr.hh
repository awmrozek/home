/*!***************************************************************************
*!
*! FILE NAME  : ipaddr.hh
*! 
*! DESCRIPTION: IP Address and checksum
*! 
*!***************************************************************************/

#ifndef ipaddr_hh
#define ipaddr_hh

/****************** INCLUDE FILES SECTION ***********************************/

/****************** FUNCTION DEFINITION SECTION *****************************/

/*****************************************************************************
*%
*% FUNCTION NAME   : calculateChecksum
*%
*% DESCRIPTION     : Calculates the 16 bit one's complement sum used in the 
*%                   TCP/IP protocol suite.
*%
*%***************************************************************************/
uword calculateChecksum(byte* startAddress,
                        udword theLength, 
                        uword initialValue = 0xffff);

/****************** CLASS DEFINITION SECTION ********************************/

/*****************************************************************************
*%
*% CLASS NAME   : IPAddress
*%
*% BASE CLASSES : None
*%
*% CLASS TYPE   : 
*%
*% DESCRIPTION  : Decode a ARP packet.
*%
*% SUBCLASSING  : None.
*%
*%***************************************************************************/
class IPAddress
{
 public:
  IPAddress();
  IPAddress(byte a0, byte a1, byte a2, byte a3);
  
  enum { size = 4 };

  bool operator == (const IPAddress& theAddress) const;

  friend class ostream&
  operator <<(ostream& theStream, const IPAddress& theAddress);

 private:
  byte myAddressArray[size];
};

#endif
/****************** END OF FILE ipaddr.hh ***********************************/

