/*!***************************************************************************
*!
*! FILE NAME  : InPacket.cc
*!
*! DESCRIPTION: Handles the InPacket layer.
*!
*!***************************************************************************/

/****************** INCLUDE FILES SECTION ***********************************/
extern "C"
{
#include <string.h>
#include "compiler.h"
#include "system.h"
#include "sp_alloc.h"
}

#include "iostream.hh"
#include "inpacket.hh"

/****************** InPacket DEFINITION SECTION *************************/

//----------------------------------------------------------------------------
//
InPacket::InPacket(byte*     theData,
                  udword    theLength,
           		  InPacket* theFrame):
 myData(theData),
 myLength(theLength),
 myFrame(theFrame)
{
}

//----------------------------------------------------------------------------
//
InPacket*
InPacket::copyAnswerChain()
{
  return NULL;
}

//----------------------------------------------------------------------------
//
void
InPacket::setNewFrame(InPacket* theFrame)
{
  myFrame  = theFrame;
  myData   = NULL;
  myLength = 0;
}

//----------------------------------------------------------------------------
//
void
InPacket::deleteAnswerChain()
{
  if (myFrame)
  {
	myFrame->deleteAnswerChain();
  }
  delete this;
}

/****************** END OF FILE InPacket.cc *************************************/
