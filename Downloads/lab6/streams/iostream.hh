/*!**************************************************************************
*! FILE NAME  : iostream.hh
*!                                                            
*! DESCRIPTION: The interface to (incomplete) iostream implementeation
*!              for AXIS products
*!              
*!---------------------------------------------------------------------------
*! HISTORY                                                    
*!
*! DATE         NAME            CHANGES
*! ----         ----            -------
*! Apr 27 1995  Stefan S        Initial version
*! June 5 1996  Stefan S        Made cin, cout and cerr work independent of
*!                              link order.
*! Oct 28 1996  Fred Jonsson    Include <iostream.h> if __GNU_CRIS__ not def.
*! Nov 25 1996  Fredrik Svensson   Never include <iostream.h>, use these
*!                                 streams even for PC environment 
*!---------------------------------------------------------------------------
*! (C) Copyright 1995, 1996, Axis Technologies AB, LUND, SWEDEN
*!**************************************************************************/
// @(#) iostream.hh 1.5 11/25/96

#ifndef iostreams_hh
#define iostreams_hh

#include "compiler.h"

typedef unsigned long streampos;

//*#**************************************************************************
//*# CLASS NAME       : ios
//*# BASE CLASSES     : none
//*#                    
//*# DESCRIPTION      : base class for streams
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

class ios
{
 public:
  ios();
  virtual ~ios() {}

  int width(int);
  int width() const;

  char fill(char);
  char fill() const;

  enum Base { dec = 10, hex = 16 };
  void base(Base);

  typedef ios& (*iomanipfunc)(ios&);

  enum Flush
  {
    left   = 02,
    right  = 04
  };
  void setFlush(Flush);
  
 protected:
  int   currentWidth;
  int   currentBase;
  char  fillCharacter;
  Flush currentFlush;
};


//*#**************************************************************************
//*# CLASS NAME       : istream
//*# BASE CLASSES     : ios
//*#                    
//*# DESCRIPTION      : input character stream
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

class istream : public ios
{
  friend class cinIstream; 
  friend istream& ws(istream&);
  
 public:
  istream();

  typedef istream& (*imanipfunc)(istream&);
  
  istream& operator >> (char&);
  istream& operator >> (unsigned char&);
  istream& operator >> (short&);
  istream& operator >> (unsigned short&);
  istream& operator >> (int&);
  istream& operator >> (unsigned int&);
  istream& operator >> (long&);
  istream& operator >> (unsigned long&);
  istream& operator >> (char*);
  istream& operator >> (unsigned char*);
  istream& operator >> (const class iomanip&);
  istream& operator >> (iomanipfunc);
  istream& operator >> (imanipfunc);

  istream& get(char&);
  istream& get(unsigned char&);
  istream& get(char*, int theLength, char theDelimeter = '\n');
  istream& get(unsigned char*, int theLength, char theDelimeter = '\n');

  istream& putback(char);
  istream& putback(unsigned char);

  operator bool() const;
  virtual bool eof() const = 0;
  virtual streampos tellg() const = 0;
  
 protected:
  long getInteger();
  void skipWhiteSpaces();

  char    lastChar();
  void    next();
  virtual char get() = 0;

 private:
  int  nextChar;
  int  putBackChar;
};

//*#**************************************************************************
//*# CLASS NAME       : ostream
//*# BASE CLASSES     : ios
//*#                    
//*# DESCRIPTION      : output stream for characters
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

class ostream : public ios
{
  friend class coutOstream;
  
 public:

  typedef ostream& (*omanipfunc)(ostream&);
  
  ostream& operator << (char);
  ostream& operator << (unsigned char);
  ostream& operator << (short);
  ostream& operator << (unsigned short);
  ostream& operator << (int);
  ostream& operator << (unsigned int);
  ostream& operator << (long);
  ostream& operator << (unsigned long);
  ostream& operator << (const char*);
  ostream& operator << (const unsigned char*);
  ostream& operator << (const class iomanip&);
  ostream& operator << (iomanipfunc);
  ostream& operator << (omanipfunc);

  virtual streampos tellp() const = 0;
  
 protected:
  void putInteger(unsigned long, char = 0);
  void putInteger(long);
  
  virtual void put(char) = 0;
};

//*#**************************************************************************
//*# CLASS NAME       : cinIstream
//*# BASE CLASSES     : istream
//*#                    
//*# DESCRIPTION      : cin
//*#                    
//*# RESPONSIBILITIES : as istream
//*#                    
//*#**************************************************************************/

class cinIstream : public istream
{
 public:
  cinIstream();

  bool eof() const;
  streampos tellg() const;
  
 protected:
  static istream* stream;
  
  char get();
};

static cinIstream cin;

//*#**************************************************************************
//*# CLASS NAME       : ostreamAssigned
//*# BASE CLASSES     : ostream
//*#                    
//*# DESCRIPTION      : cout
//*#                    
//*# RESPONSIBILITIES : as ostream
//*#                    
//*#---------------------------------------------------------------------------
//*# HISTORY                                                    
//*#                                                            
//*# DATE         NAME            CHANGES                       
//*# ----         ----            -------                       
//*# May 26 1995  Stefan S        Initial version
//*#**************************************************************************/

class coutOstream : public ostream
{
 public:
  coutOstream();

  streampos tellp() const;
  
 protected:
  static ostream* stream;
  
  void put(char);
};
  
static coutOstream cout;
static coutOstream cerr;

//*#**************************************************************************
//*# CLASS NAME       : iomanip
//*# BASE CLASSES     : none
//*#                    
//*# DESCRIPTION      : manipulator base class
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

class iomanip
{
  friend class ios;
  friend class istream;
  friend class ostream;

 private:
  virtual void doit(ios*) const = 0;
};


//*#**************************************************************************
//*# CLASS NAME       : setw
//*# BASE CLASSES     : iomanip
//*#                    
//*# DESCRIPTION      : sets width for stream
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

class setw : public iomanip
{
 public:
  setw(int);

 private:
  void doit(ios*) const;

  int width;
};

//*#**************************************************************************
//*# CLASS NAME       : setiosflags
//*# BASE CLASSES     : iomanip
//*#                    
//*# DESCRIPTION      : sets flush left or right for stream
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

class setiosflags : public iomanip
{
 public:
  setiosflags(long);

 private:
  void doit(ios*) const;

  long flags;
};

//*#**************************************************************************
//*# CLASS NAME       : resetiosflags
//*# BASE CLASSES     : iomanip
//*#                    
//*# DESCRIPTION      : resets flush left or right for stream
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

class resetiosflags : public iomanip
{
 public:
  resetiosflags(long);

 private:
  void doit(ios*) const;

  long flags;
};

//*#**************************************************************************
//*# CLASS NAME       : setfill
//*# BASE CLASSES     : iomanip
//*#                    
//*# DESCRIPTION      : set fill character for stream
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

class setfill : public iomanip
{
 public:
  setfill(char);

 private:
  void doit(ios*) const;

  char fillChar;
};



//*#**************************************************************************
//*# Sets stream in decimal mode

extern ios& dec(ios&);

//*#**************************************************************************
//*# Sets stream in hexadecimal mode

extern ios& hex(ios&);

//*#**************************************************************************
//*# 

extern ostream& endl(ostream&);

//*#**************************************************************************
//*# 

extern istream& ws(istream&);

// end of iostreams.hh
#endif

