/*!***************************************************************************
*!
*! FILE NAME  : InPacket.hh
*! 
*! DESCRIPTION: The InPacket
*! 
*!***************************************************************************/

#ifndef InPacket_hh
#define InPacket_hh

/****************** INCLUDE FILES SECTION ***********************************/

/****************** CLASS DEFINITION SECTION ********************************/

/*****************************************************************************
*%
*% CLASS NAME   : InPacket
*%
*% BASE CLASSES : None
*%
*% CLASS TYPE   : 
*%
*% DESCRIPTION  : Handles the InPacket Layer.
*%
*% SUBCLASSING  : Subclass and implement decode.
*%
*%***************************************************************************/
class InPacket
{
 public:
   InPacket(byte*     theData,
            udword    theLength,
            InPacket* theFrame);

   virtual void decode() = 0;
   virtual void answer(byte* theData, udword theLength) = 0;
   virtual uword headerOffset() = 0;

  /* The following three methods are not used until lab4 */
   virtual InPacket* copyAnswerChain();
   void    setNewFrame(InPacket* theFrame);
   void    deleteAnswerChain();
 
 protected:
   byte *myData;
   udword myLength;
   InPacket* myFrame;
};

#endif
/****************** END OF FILE InPacket.hh *************************************/

