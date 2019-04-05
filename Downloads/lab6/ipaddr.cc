/*!***************************************************************************
*!
*! FILE NAME  : ipaddr.cc
*!
*! DESCRIPTION: Handles IP addresses and calculates the checksum
*!
*!***************************************************************************/

/****************** INCLUDE FILES SECTION ***********************************/

#include "compiler.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
extern "C"
{
#include "system.h"
}

#include "iostream.hh"
#include "ipaddr.hh"

//#define D_IPADDR
#ifdef D_IPADDR
#define trace cout
#else
#define trace if(false) cout
#endif

/****************** FUNCTION DEFINITION SECTION *************************/

//----------------------------------------------------------------------------
//
uword calculateChecksum(byte* startAddress,
                        udword theLength, 
                        uword initialValue)
{
  udword  csum = ~initialValue & 0x0000ffff;
  const byte  *aBytePointer;

  udword aWordCount = theLength / 2;
  aBytePointer = startAddress;
  uword *aUWPointer = (uword *)aBytePointer;

  while (aWordCount-- > 0)
  {
    csum += *aUWPointer++;
  }

  if (theLength % 2)
  { // The segment ends with odd byte, add it.
    aBytePointer = (byte *)aUWPointer;
    csum += *aBytePointer++;
  }
  while (csum >> 16)
  {
    csum = (csum & 0xffff) + (csum >> 16); 
    /* add in end-around carry */
  }
  return ~(uword) csum;
}

/****************** IPAddress DEFINITION SECTION *************************/

//----------------------------------------------------------------------------
//
IPAddress::IPAddress()
{
}

ostream& operator <<(ostream& theStream, const IPAddress& theAddress)
{
  static char aString[4*2+1];
  aString[0] = '\0';
  int i;
  for (i = 0; i < 3; i++)
  {
    sprintf(aString, "%s%d.", aString, theAddress.myAddressArray[i]);
  }
  sprintf(aString, "%s%d", aString, theAddress.myAddressArray[i]);
  theStream << aString;
  return theStream;
}

IPAddress::IPAddress(byte a0, byte a1, byte a2, byte a3)
{
  myAddressArray[0] = a0;
  myAddressArray[1] = a1;
  myAddressArray[2] = a2;
  myAddressArray[3] = a3;
}

bool
IPAddress::operator == (const IPAddress& theAddress) const
{
  for (int i=0; i < size; i++)
  {
	if (theAddress.myAddressArray[i] != myAddressArray[i])
	{
	  return false;
	}
  }
  return true;
}

/****************** END OF FILE ipaddr.cc ***********************************/

