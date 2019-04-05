/*!**************************************************************************
*! FILE NAME  : iostream.cc
*!                                                            
*! DESCRIPTION: The implementation of iostream
*!              
*!---------------------------------------------------------------------------
*! HISTORY                                                    
*!                                                            
*! DATE         NAME            CHANGES                       
*! ----         ----            -------                       
*! May 26 1995  Stefan S        Initial version
*! May 11 1996  Patrik Bannura  Removed warning about new ANSI `for' scoping.
*! June 5 1996  Stefan S        Made cin, cout and cerr work independent of
*!                              link order.
*! Nov 25 1996  Fredrik Svensson   Included compiler.h
*!---------------------------------------------------------------------------
*! (C) Copyright 1995, 1996 Axis Technologies AB, LUND, SWEDEN
*!**************************************************************************/
// @(#) iostream.cc 1.4 11/25/96

#include "compiler.h"
#include "iostream.ihh"

#include <ctype.h>
#include <string.h>

#include "ciostream.icc"

//*#**************************************************************************
//*#

ios::ios()
    : currentWidth(0),
      currentBase(10),
      fillCharacter(' '),
      currentFlush(ios::right)
{}

//*#**************************************************************************
//*# 

int
ios::width(int theWidth)
{
  currentWidth = theWidth;
  return currentWidth;
}

//*#**************************************************************************
//*# 

int
ios::width() const
{
  return currentWidth;
}

//*#**************************************************************************
//*# 

void
ios::base(ios::Base theBase)
{
  if (theBase == dec)
  {
    currentBase = 10;
  }
  else
  {
    currentBase = 16;
  }
}

//*#**************************************************************************
//*# 

char
ios::fill(char theFillChar)
{
  fillCharacter = theFillChar;
  return fillCharacter;
}

//*#**************************************************************************
//*#

char
ios::fill() const
{
  return fillCharacter;
}

//*#**************************************************************************
//*# 

void
ios::setFlush(ios::Flush theFlush)
{
  currentFlush = theFlush;
}

//*#**************************************************************************
//*# 

istream::istream() 
    : nextChar(-1),
      putBackChar(-1)
{}

//*#**************************************************************************
//*# 

istream&
istream::operator >> (char& theData)
{
  this->skipWhiteSpaces();
  theData = (char) this->lastChar();
  this->next();
  return *this;
}

//*#**************************************************************************
//*# 

istream&
istream::operator >> (unsigned char& theData)
{
  this->skipWhiteSpaces();
  theData = (unsigned char) this->lastChar();
  this->next();
  return *this;
}

//*#**************************************************************************
//*# 

istream&
istream::operator >> (short& theData)
{
  theData = (short) this->getInteger();
  return *this;
}

//*#**************************************************************************
//*# 

istream&
istream::operator >> (unsigned short& theData)
{
  theData = (unsigned short) this->getInteger();
  return *this;
}

//*#**************************************************************************
//*# 

istream&
istream::operator >> (int& theData)
{
  theData = (int) this->getInteger();
  return *this;
}

//*#**************************************************************************
//*# 

istream&
istream::operator >> (unsigned int& theData)
{
  theData = (unsigned int) this->getInteger();
  return *this;
}

//*#**************************************************************************
//*# 

istream&
istream::operator >> (long& theData)
{
  theData = (long) this->getInteger();
  return *this;
}

//*#**************************************************************************
//*# 

istream&
istream::operator >> (unsigned long& theData)
{
  theData = (unsigned long) this->getInteger();
  return *this;
}

//*#**************************************************************************
//*# 

istream&
istream::operator >> (char* theData)
{
  this->skipWhiteSpaces();
  int anOffset = 0;
  while (!isspace(this->lastChar())
         && (currentWidth == 0 || anOffset < currentWidth - 1)
         && !this->eof())
  {
    *theData++ = (char) this->lastChar();
    this->next();
    anOffset++;
  }
  *theData = 0;
  currentWidth = 0;
  return *this;
}

//*#**************************************************************************
//*# 

istream&
istream::operator >> (unsigned char* theData)
{
  return operator >> ((char*) theData);
}

//*#**************************************************************************
//*# 

istream&
istream::operator >> (const class iomanip& theManipulator)
{
  theManipulator.doit(this);
  return *this;
} 

//*#**************************************************************************
//*# 

istream&
istream::operator >> (ios::iomanipfunc theFunction)
{
  theFunction(*this);
  return *this;
} 

//*#**************************************************************************
//*# 

istream&
istream::operator >> (istream::imanipfunc theFunction)
{
  theFunction(*this);
  return *this;
} 

//*#**************************************************************************
//*# 

istream&
istream::get(char& theChar)
{
  theChar = this->lastChar();
  this->next();
  return *this;
}

//*#**************************************************************************
//*# 

istream&
istream::get(unsigned char& theChar)
{
  return this->get((char&) theChar);
}

//*#**************************************************************************
//*# 

istream&
istream::get(char* theString, int theLength, char theDelimeter)
{
  while((theLength--) && (this->lastChar() != theDelimeter) && !this->eof())
  {
    *theString++ = this->lastChar();
    this->next();
  }
  *theString = 0;
  return *this;
}


//*#**************************************************************************
//*# 

istream&
istream::get(unsigned char* theString, int theLength, char theDelimeter)
{
  return this->get((char*) theString, theLength, theDelimeter);
}

//*#**************************************************************************
//*# 

istream&
istream::putback(char theChar)
{
  putBackChar = (unsigned char) theChar;
  return *this;
}

//*#**************************************************************************
//*# 

istream&
istream::putback(unsigned char theChar)
{
  putBackChar = (unsigned char) theChar;
  return *this;
}

//*#**************************************************************************
//*# 

istream::operator bool () const
{
  return !this->eof();
}
  
//*#**************************************************************************
//*# 

long
istream::getInteger()
{
  long anInteger = 0;
  bool anIsNegative = false;
  this->skipWhiteSpaces();
  if (this->lastChar() == '-')
  {
    anIsNegative = true;
    this->next();
  }
  while (isdigit(this->lastChar()) ||
         ((currentBase == hex) &&
          (this->lastChar() >= 'a' && this->lastChar() <= 'f')))
  {
    if (this->lastChar() <= '9')
    {
      anInteger = ((long) currentBase) * anInteger + (this->lastChar() - '0');
    }
    else
    {
      anInteger = (((long) currentBase) * anInteger +
                   (this->lastChar() - 'a') + 10);
    }
    this->next();
  }
  if (anIsNegative)
  {
    return -anInteger;
  }
  return anInteger;
}

//*#**************************************************************************
//*# 

void
istream::skipWhiteSpaces()
{
  while (isspace(this->lastChar()))
  {
    this->next();
  }
}


//*#**************************************************************************
//*# 

char
istream::lastChar()
{
  if (putBackChar != -1)
  {
    return (char) (unsigned char) putBackChar;
  }
  if (nextChar == -1)
  {
    this->next();
  }
  return (char) (unsigned char) nextChar;
}

//*#**************************************************************************
//*# 

void
istream::next()
{
  if (putBackChar != -1)
  {
    putBackChar = -1;
    return;
  }
  else
  {
    nextChar = this->get();
  }
}

//*#**************************************************************************
//*# 

ostream&
ostream::operator << (char theData)
{
  this->put(theData);
  return *this;
}

//*#**************************************************************************
//*# 

ostream&
ostream::operator << (unsigned char theData)
{
  this->put((char) theData);
  return *this;
}

//*#**************************************************************************
//*# 

ostream&
ostream::operator << (short theData)
{
  this->putInteger((long) theData);
  return *this;
}

//*#**************************************************************************
//*# 

ostream&
ostream::operator << (unsigned short theData)
{
  this->putInteger((unsigned long) theData);
  return *this;
}

//*#**************************************************************************
//*# 

ostream&
ostream::operator << (int theData)
{
  this->putInteger((long) theData);
  return *this;
}

//*#**************************************************************************
//*# 

ostream&
ostream::operator << (unsigned int theData)
{
  this->putInteger((unsigned long) theData);
  return *this;
}

//*#**************************************************************************
//*# 

ostream&
ostream::operator << (long theData)
{
  this->putInteger(theData);
  return *this;
}

//*#**************************************************************************
//*# 

ostream&
ostream::operator << (unsigned long theData)
{
  this->putInteger(theData);
  return *this;
}

//*#**************************************************************************
//*# 

ostream&
ostream::operator << (const char* theData)
{
  int aLength = strlen(theData);
  if (currentFlush == right)
  {
    for (int i = aLength; i < currentWidth; i++)
    {
      this->put(fillCharacter);
    }
  }
  while (*theData != 0)
  {
    this->put(*theData++);
  }
  if (currentFlush == left)
  {
    for (int i = aLength; i < currentWidth; i++)
    {
      this->put(fillCharacter);
    }
  }
  currentWidth = 0;
  fillCharacter = ' ';
  return *this;
}

//*#**************************************************************************
//*# 

ostream&
ostream::operator << (const unsigned char* theData)
{
  return this->operator << ((const char*) theData);
}

//*#**************************************************************************
//*# 

ostream&
ostream::operator << (const class iomanip& theData)
{
  theData.doit(this);
  return *this;
}

//*#**************************************************************************
//*# 

ostream&
ostream::operator << (ios::iomanipfunc theFunction)
{
  theFunction(*this);
  return *this;
}

//*#**************************************************************************
//*# 

ostream&
ostream::operator << (ostream::omanipfunc theFunction)
{
  theFunction(*this);
  return *this;
}


//*#**************************************************************************
//*# 

static char
charFromValue(int aValue)
{
  if (aValue < 10)
  {
    return '0' + aValue;
  }
  return 'a' + (aValue - 10);
}

//*#**************************************************************************
//*# 

void
ostream::putInteger(unsigned long theInteger, char theSign)
{
  const int maxIntegerSize = 15;
  
  char aString[maxIntegerSize + 1];
  
  if (theInteger != 0)
  {
    int i;
    for (i = maxIntegerSize - 1; theInteger > 0; i--)
    {
      aString[i] = charFromValue(theInteger % (int) currentBase);
      theInteger /= (int) currentBase;
    }
    if (theSign)
    {
      aString[i--] = theSign;
    }
    aString[maxIntegerSize] = 0;
    this->operator << (aString + (i + 1));
    return;
  }
  strcpy(aString, "0");
  this->operator << (aString);
}
  
//*#**************************************************************************
//*# 

void
ostream::putInteger(long theInteger)
{
  char aSign = 0;
  
  if (theInteger < 0)
  {
    aSign = '-';
    theInteger = -theInteger;
  }
  this->putInteger((unsigned long) theInteger, aSign);
}


//-----------------------------------------------------------------------------
// 

istream*
cinIstream::stream = 0;

//*#**************************************************************************
//*# 

cinIstream::cinIstream()
{
  if (!cinIstream::stream)
  {
    cinIstream::stream = new cistream(1);
  }
}

//*#**************************************************************************
//*# 

bool
cinIstream::eof() const
{
  return cinIstream::stream->eof();
}

//*#**************************************************************************
//*# 

streampos
cinIstream::tellg() const
{
  return cinIstream::stream->tellg();
}

//*#**************************************************************************
//*# 

char
cinIstream::get()
{
  return cinIstream::stream->get(); 
}

//-----------------------------------------------------------------------------
// 

ostream*
coutOstream::stream = 0;

//*#**************************************************************************
//*# 

coutOstream::coutOstream()
{
  coutOstream::stream = new costream(1);
}

//*#**************************************************************************
//*# 

streampos
coutOstream::tellp() const
{
  return coutOstream::stream->tellp();
}

//*#**************************************************************************
//*# 

void
coutOstream::put(char theChar)
{
  coutOstream::stream->put(theChar);
}

//*#**************************************************************************
//*# 

setw::setw(int theWidth) 
    : width(theWidth)
{}

//*#**************************************************************************
//*# 

void
setw::doit(ios* theStream) const
{
  theStream->width(width);
}

//*#**************************************************************************
//*#

setiosflags::setiosflags(long theFlags)
    : flags(theFlags)
{}

//*#**************************************************************************
//*# 

void
setiosflags::doit(ios* theStream) const
{
  if ((flags == ios::left) || (flags == ios::right))
  {
    theStream->setFlush((ios::Flush) flags);
  }
}

//*#**************************************************************************
//*# 

resetiosflags::resetiosflags(long theFlags)
    : flags(theFlags)
{}

//*#**************************************************************************
//*# 

void
resetiosflags::doit(ios* theStream) const
{
  if (flags == ios::left)
  {
    theStream->setFlush(ios::right);
  }
  if (flags == ios::right)
  {
    theStream->setFlush(ios::left);    
  }
}

//*#**************************************************************************
//*# 

setfill::setfill(char theFillChar)
    : fillChar(theFillChar)
{}

//*#**************************************************************************
//*# 

void
setfill::doit(ios* theStream) const
{
  theStream->fill(fillChar);
}

//*#**************************************************************************
//*# 

ios&
dec(ios& theStream)
{
  theStream.base(ios::dec);
  return theStream;
}

//*#**************************************************************************
//*# 

ios&
hex(ios& theStream)
{
  theStream.base(ios::hex);
  return theStream;
}

//*#**************************************************************************
//*# 

ostream&
endl(ostream& theStream)
{
  return theStream << "\r\n";
}

//*#**************************************************************************
//*# 

istream&
ws(istream& theStream)
{
  theStream.skipWhiteSpaces();
  return theStream;
}



