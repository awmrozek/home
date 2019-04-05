//*!**************************************************************************
//*! FILE NAME  : strstream.hh
//*!                                                            
//*! DESCRIPTION: The interface to streams stored in character strings
//*!              
//*!---------------------------------------------------------------------------
//*! HISTORY                                                    
//*!                                                            
//*! DATE         NAME            CHANGES                       
//*! ----         ----            -------                       
//*! May 26 1995  Stefan S        Initial version
//*! Nov  8 1996  Christian Matson Included compiler.h  
//*!
//*!---------------------------------------------------------------------------
//*! (C) Copyright 1995, Axis Technologies AB, LUND, SWEDEN
//*!**************************************************************************/
//*! @(#) strstream.hh 1.2 11/08/96

#ifndef strstream_hh
#define strstream_hh

#include "compiler.h"
#include "iostream.hh"


//*#**************************************************************************
//*# CLASS NAME       : istrstream
//*# BASE CLASSES     : istream
//*#                    
//*# DESCRIPTION      : input character stream, source in  memory
//*#                    
//*# RESPONSIBILITIES : 
//*#                    
//*#---------------------------------------------------------------------------
//*# HISTORY                                                    
//*#                                                            
//*# DATE         NAME            CHANGES                       
//*# ----         ----            -------                       
//*# May 26 1995  Stefan S        Initial version
//*#**************************************************************************/

class istrstream : public istream
{
 public:
  istrstream(const char* theData, int theLength);

  bool eof() const;
  streampos tellg() const;

 private:
  char get();

  const char*  buffer;
  int          length;
  int          count;
};


//*#**************************************************************************
//*# CLASS NAME       : ostrstream
//*# BASE CLASSES     : ostream
//*#                    
//*# DESCRIPTION      : output character stream, destination in memory
//*#                    
//*# RESPONSIBILITIES : 
//*#                    
//*#---------------------------------------------------------------------------
//*# HISTORY                                                    
//*#                                                            
//*# DATE         NAME            CHANGES                       
//*# ----         ----            -------                       
//*# May 26 1995  Stefan S        Initial version
//*#**************************************************************************/

class ostrstream : public ostream
{
 public:
  ostrstream(char* theDatam, int theLength);

  streampos tellp() const;
  
 private:
  void put(char);

  char*  buffer;
  int    length;
  int    count;
};








#endif


