//*!**************************************************************************
//*! FILE NAME  : strstream.cc
//*!                                                            
//*! DESCRIPTION: The implementation of 
//*!              
//*!              
//*!---------------------------------------------------------------------------
//*! HISTORY                                                    
//*!                                                            
//*! DATE         NAME            CHANGES                       
//*! ----         ----            -------                       
//*! May 26 1995  Stefan S        Initial version
//*!
//*!---------------------------------------------------------------------------
//*! (C) Copyright 1995, Axis Technologies AB, LUND, SWEDEN
//*!**************************************************************************/
//*! @(#) strstream.cc 1.1 10/18/95

#include "strstream.hh"

//*#**************************************************************************
//*# 

istrstream::istrstream(const char* theData, int theLength)
    : buffer(theData),
      length(theLength),
      count(0)
{}

//*#**************************************************************************
//*# 

bool
istrstream::eof() const
{
  return (count >= length);
}

//*#**************************************************************************
//*# 

streampos
istrstream::tellg() const
{
  return count;
}

//*#**************************************************************************
//*# 

char
istrstream::get()
{
  if (count < length)
  {
    return buffer[count++];
  }
  return 0;
}

//*#**************************************************************************
//*# 

ostrstream::ostrstream(char *theData, int theLength)
    : buffer(theData),
      length(theLength),
      count(0)
{}

//*#**************************************************************************
//*# 

streampos
ostrstream::tellp() const
{
  return count;
}

//*#**************************************************************************
//*# 

void
ostrstream::put(char theChar)
{
  if (count < length)
  {
    buffer[count++] = theChar;
  }
}


