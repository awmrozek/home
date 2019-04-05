/*
** NAME:    $Source: /home/grad1/drh/res/srclib/xprintf.sh,v $
** VERSION: $Revision: 1.1 $
** DATE:    $Date: 90/11/29 13:12:44 $
** DATE:    $Date: 91/10/18 MG: Reduced bufsize
**
** ONELINER:   A replacement for formatted printing programs.
**
** COPYRIGHT:
**   Copyright (c) 1990 by D. R. Hipp.  This code is an original work
**   and has been prepared without reference to any prior
**   implementations of similar functions.  No part of this code is
**   subject to licensing restrictions of any telephone company or
**   university.
**
**   Permission is hereby granted for this code to be used by anyone
**   for any purpose under the following restrictions:
**     1.  No attempt shall be made to prevent others (especially the
**         author) from using this code.
**     2.  Changes to this code are to be clearly marked.
**     3.  The origins of this code are not to be misrepresented.
**     4.  The user agrees that the author is in no way responsible for
**         the correctness or usefulness of this program.
**
** DESCRIPTION:
**   This program is an enhanced replacement for the "printf" programs
**   found in the standard library.  The following enhancements are
**   supported:
**
**      +  Additional functions.  The standard set of "printf" functions
**         includes printf, sprintf, vprintf, and vsprintf.
**         This module adds the following:
**
**           *  snprintf -- Works like sprintf, but has an extra argument
**                          which is the size of the buffer written to.
**
**           *  xprintf --  Calls a function to dispose of output.
**
**           *  nprintf --  No output, but returns the number of characters
**                          that would have been output by printf.
**
**           *  A v- version (ex: vsnprintf) of every function is also
**              supplied.
**
**      +  A few extensions to the formatting notation are supported:
**
**           *  The %b field outputs an integer in binary notation.
**
**           *  The %c field now accepts a precision.  The character output
**              is repeated by the number of times the precision specifies.
**
**           *  The %' field works like %c, but takes as its character the
**              next character of the format string, instead of the next
**              argument.  For example,  printf("%.78'-")  prints 78 minus
**              signs, the same as  printf("%.78c",'-').
**
**      +  The user can add a limited number (20) of new conversion fields
**         at run-time using the "converter" function.  Test case 5 below
**         shows an example of how this is done.
**
**      +  When compiled using GCC on a SPARC, this version of printf is
**         faster than the library printf for SUN OS 4.1.
**
**      +  All functions (except converter) are fully reentrant.
**
** TESTING
**   Use the following command sequence to run 5 tests on this code:
**
**     gcc -DTEST1 xprintf.c         Test integer conversion
**     a.out
**     gcc -DTEST2 xprintf.c         Test string and character output
**     a.out
**     gcc -DTEST3 xprintf.c         Test floating point conversions
**     a.out
**     gcc -DTEST4 xprintf.c         Test the library
**     a.out
**     gcc -DTEST5 xprintf.c         Test the "converter" function
**     a.out
**
*/
/* Axis history:
*! Date         Name       Comment
*! 930930  HP      Axis history comment; DEBUG_USING_IRQ for debug texts.
*! 940518  MR      Changed DI \ RESTORE to DISABLE() \ RESTORE()
*! 950310  SJ      Added serial_putch(). Removed hw.w. Added port_hw.h.
*! Apr 19 1995   Willy Sagefalk    Added include of npscfg.h
*! Apr 19 1995   Mart Roomus       Changed DISABLE() to DISABLE_SAVE()
*! Jun 16 1995   Mart Roomus       serial_putch() usage only if DEBUG
*! Oct 11 1995   H-P Nilsson       Made spaces[] in vxprintf const.
*!                                 Now must #define USE_USER_FMT if
*!                                 user formats will be used.
*! Oct 19 1995   Jens Johansson    Sure, HP. And I have to remove the trail
*!                                 of warnings left behind.
*! May 16 1996   Fredrik Norrman   Fixed compiler warning.
*! Sep 18 1996   Willy Sagefalk    Added SERIAL_DEBUG.
*! Sep 30 1996   Jens Johansson    Compilable for PC.
*/
/* @(#) xprintf.c 1.12 09/30/96  */

/*
** Undefine COMPATIBILITY to make some slight changes in the way things
** work.  I think the changes are an improvement, but they are not
** backwards compatible.
*/
#define COMPATIBILITY       /* Compatible with SUN OS 4.1 */

#ifndef NOINTERFACE
#  if defined(TEST1) || defined(TEST2) || defined(TEST3)
#    define NOINTERFACE
#  else
#    define printf    DUMMY_FUNCTION_1    /* I don't want prototypes */
#    define fprintf   DUMMY_FUNCTION_2    /*    for any of the standard */    
#    define sprintf   DUMMY_FUNCTION_3    /*    functions, because such */
#    define vprintf   DUMMY_FUNCTION_4    /*    prototypes could clash with */
#    define vfprintf  DUMMY_FUNCTION_5    /*    my redefinitions */
#    define vsprintf  DUMMY_FUNCTION_6
#  endif
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "compiler.h"
#include "system.h"
#ifdef __CRIS__
#include "etrax.h"
#endif
#ifdef GNX_CG16
#include "npscg16.h"
#endif

#include "xprintf.h"

#ifndef NOINTERFACE
#  undef printf
#  undef fprintf
#  undef sprintf
#  undef vprintf
#  undef vfprintf
#  undef vsprintf
#endif

/*
** Conversion types fall into various categories as defined by the
** following enumeration.
*/
enum e_type {    /* The type of the format field */
   RADIX,            /* Integer types.  %d, %x, %o, and so forth */
   FLOAT,            /* Floating point.  %f */
   EXP,              /* Exponentional notation. %e and %E */
   GENERIC,          /* Floating or exponential, depending on exponent. %g */
   SIZE,             /* Return number of characters processed so far. %n */
   STRING,           /* Strings. %s */
   PERCENT,          /* Percent symbol. %% */
   CHAR,             /* Characters. %c */
   CHARLIT,          /* Literal characters.  %' */
#ifdef USE_USER_FMT
   USER,             /* User specified conversion */
#endif
   ERROR,            /* Used to indicate no such conversion type */
};

/*
** Each builtin conversion character (ex: the 'd' in "%d") is described
** by an instance of the following structure
*/
typedef struct s_info {   /* Information about each format field */
  int  fmttype;              /* The format field code letter */
  int  base;                 /* The base for radix conversion */
  char *charset;             /* The character set for conversion */
  int  flag_signed;          /* Is the quantity signed? */
  char *prefix;              /* Prefix on non-zero values in alt format */
  enum e_type type;          /* Conversion paradigm */
} info;

/*
** The following table is searched linearly, so it is good to put the
** most frequently used conversion types first.
*/
static const info fmtinfo[] = {
  { 'd',  10,  "0123456789",       1,    0, RADIX, },
  {  's',   0,  0,                  0,    0, STRING, },
  {  'c',   0,  0,                  0,    0, CHAR, },
  {  'o',   8,  "01234567",         0,  "0", RADIX, },
  {  'u',  10,  "0123456789",       0,    0, RADIX, },
  {  'x',  16,  "0123456789abcdef", 0, "x0", RADIX, },
  {  'X',  16,  "0123456789ABCDEF", 0, "X0", RADIX, },
  {  'f',   0,  0,                  1,    0, FLOAT, },
  {  'e',   0,  "e",                1,    0, EXP, },
  {  'E',   0,  "E",                1,    0, EXP, },
  {  'g',   0,  "e",                1,    0, GENERIC, },
  {  'G',   0,  "E",                1,    0, GENERIC, },
  {  'i',  10,  "0123456789",       1,    0, RADIX, },
  {  'n',   0,  0,                  0,    0, SIZE, },
  {  '%',   0,  0,                  0,    0, PERCENT, },
  {  'b',   2,  "01",               0, "b0", RADIX,   /* Binary notation */ },
  {  'p',  10,  "0123456789",       0,    0, RADIX,   /* Pointers */ },
  {  '\'',  0,  0,                  0,    0, CHARLIT, /* Literal character */}
};
#define NINFO  (sizeof(fmtinfo)/sizeof(info))  /* Size of the fmtinfo table */

/*
** If NOFLOATINGPOINT is defined, then none of the floating point
** conversions will work.
*/
#ifndef NOFLOATINGPOINT
/*
** "*val" is a double such that 0.1 <= *val < 10.0
** Return the ascii code for the leading digit of *val, then
** multiply "*val" by 10.0 to renormalize.
**
** Example:
**     input:     *val = 3.14159
**     output:    *val = 1.4159    function return = '3'
*/
static int getdigit(double *val){
  int digit;
  double d;
  digit = (int)*val;
  d = digit;
  digit += '0';
  *val = (*val - d)*10.0;
  return digit;
}
#endif


#ifdef TEST5
#define USE_USER_FMT
#endif

#if defined(USE_USER_FMT)
/*
** User specified conversion fields.
*/

/*
** For internal use only:  This structure remembers the details of a
** user defined conversion.
*/  
typedef struct s_userinfo {
  char format;                 /* The conversion format character */
  int (*func)(convertctrl*);   /* The function to call to do the convert */
  void *arg;                   /* A generic argument to (*func)(...) */
} userinfo;

/*
** Here is an array of the user defined conversions.  There are a fixed
** number.
*/

#define NUSERFMT 20                 /* Max no. of user-defined conversions */
static int nuserfmt = 0;            /* No. of previously defined */
static userinfo userfmt[NUSERFMT];  /* Array of user-defined conversions */

/*
** Call this function in order to install a new user-specified conversion
** field.
**
** The function returns TRUE if the conversion is successfully installed.
*/
int converter(char format, int (*func)(convertctrl*), void *arg){
  int rc;
  if( nuserfmt<NUSERFMT ){
    userfmt[nuserfmt].format = format;
    userfmt[nuserfmt].func = func;
    userfmt[nuserfmt].arg = arg;
    nuserfmt++;
    rc = 1;
  }else{
    rc = 0;
  }
  return rc;
}
#endif /* USE_USER_FMT */


#ifndef TEST4
# define BUFSIZE 200  /* Size of the output buffer */
    /*  NOTE:  No field can be longer than BUFSIZE characters! */
#else
# define BUFSIZE 7     /* Set low to exercise code during TEST4 */
#endif

/*
** The root program.  All variations call this core.
**
** INPUTS:
**   func   This is a pointer to a function taking three arguments
**            1. A pointer to the list of characters to be output
**               (Note, this list is NOT null terminated.)
**            2. An integer number of characters to be output.
**               (Note: This number might be zero.)
**            3. A pointer to anything.  Same as the "arg" parameter.
**
**   arg    This is the pointer to anything which will be passed as the
**          third argument to "func".  Use it for whatever you like.
**
**   fmt    This is the format string, as in the usual printf.
**
**   ap     This is a pointer to a list of arguments.  Same as in
**          vfprintf.
**
** OUTPUTS:
**          The return value is the total number of characters sent to
**          the function "func".  Returns EOF on a error.
**
** Note that the order in which automatic variables are declared below
** seems to make a big difference in determining how fast this beast
** will run.
*/
int vxprintf(void (*func)(char*,int,void*),
void *arg, const char *format, va_list ap){
  register const char *fmt; /* The format string. */
  register int c;           /* Next character in the format string */
  register char *bufpt;     /* Pointer to the conversion buffer */
  register int  precision;  /* Precision of the current field */
  register int  length;     /* Length of the field */
  register int  idx;        /* A general purpose loop counter */
  int count;                /* Total number of characters output */
  int width;                /* Width of the current field */
  int flag_leftjustify;     /* True if "-" flag is present */
  int flag_plussign;        /* True if "+" flag is present */
  int flag_blanksign;       /* True if " " flag is present */
  int flag_alternateform;   /* True if "#" flag is present */
  int flag_zeropad;         /* True if field width constant starts with zero */
  int flag_long;            /* True if "l" flag is present */
  unsigned long longvalue;  /* Value for integer types */
  double realvalue;         /* Value for real types */
  const info *infop;        /* Pointer to the appropriate info structure */
  char buf[BUFSIZE];        /* Conversion buffer */
  char prefix;              /* Prefix character.  "+" or "-" or " " or 0. */
  int  errorflag = 0;       /* True if an error is encountered */
  enum e_type xtype;        /* Which of 3 different FP formats */
#ifdef USE_USER_FMT
  convertctrl v;            /* Used for calling user conversion routines */
  userinfo *ufp = 0;        /* Pointer to info about a user conversion */
#endif
  static const char spaces[]="                                                    ";
#define SPACESIZE (sizeof(spaces)-1)
#ifndef NOFLOATINGPOINT
  int  exp;                 /* exponent of real numbers */
  double rounder;           /* Used for rounding floating point values */
  int flag_dp;              /* True if decimal point should be shown */
  int flag_rtz;             /* True if trailing zeros should be removed */
#endif

  fmt = format;                     /* Put in a register for speed */
  count = length = 0;
  bufpt = 0;
  for(; (c=(*fmt))!=0; ++fmt){
    if( c!='%' ){
      register int amt;
      bufpt = (char *)fmt;
      amt = 1;
      while( (c=*++fmt)!='%' && c!=0 ) amt++;
      (*func)(bufpt,amt,arg);
      count += amt;
      if( c==0 ) break;
    }
    if( (c=(*++fmt))==0 ){
      errorflag = 1;
      (*func)("%",1,arg);
      count++;
      break;
    }
    /* Find out what flags are present */
    flag_leftjustify = flag_plussign = flag_blanksign = 
     flag_alternateform = flag_zeropad = 0;
    do{
      switch( c ){
        case '-':   flag_leftjustify = 1;     c = 0;   break;
        case '+':   flag_plussign = 1;        c = 0;   break;
        case ' ':   flag_blanksign = 1;       c = 0;   break;
        case '#':   flag_alternateform = 1;   c = 0;   break;
        case '0':   flag_zeropad = 1;         c = 0;   break;
        default:                                       break;
      }
    }while( c==0 && (c=*++fmt)!=0 );
    /* Get the field width */
    width = 0;
    if( c=='*' ){
      width = va_arg(ap,int);
      if( width<0 ){
        flag_leftjustify = 1;
        width = -width;
      }
      c = *++fmt;
    }else{
      while( isdigit(c) ){
        width = width*10 + c - '0';
        c = *++fmt;
      }
    }
    /* Get the precision */
    if( c=='.' ){
      precision = 0;
      c = *++fmt;
      if( c=='*' ){
        precision = va_arg(ap,int);
#ifndef COMPATIBILITY
        /* This is sensible, but SUN OS 4.1 doesn't do it. */
        if( precision<0 ) precision = -precision;
#endif
        c = *++fmt;
      }else{
        while( isdigit(c) ){
          precision = precision*10 + c - '0';
          c = *++fmt;
        }
      }
      /* Limit the precision to prevent overflowing buf[] during conversion */
      if( precision>BUFSIZE-40 ) precision = BUFSIZE-40;
    }else{
      precision = -1;
    }
    /* Get the conversion type modifier */
    if( c=='l' ){
      flag_long = 1;
      c = *++fmt;
    }else{
      flag_long = 0;
    }
    /* Fetch the info entry for the field */
    infop = 0;
    for(idx=0; idx<NINFO; idx++){
      if( c==fmtinfo[idx].fmttype ){
        infop = &fmtinfo[idx];
        break;
      }
    }
    /* No info entry found.  Perhaps this is a user-specified conversion.
    ** Otherwise, it must be an error.  Check it out. */
    if( infop==0 ){
#ifdef USE_USER_FMT
      int i;
#endif
      xtype = ERROR;
#ifdef USE_USER_FMT
      for(i=0; i<nuserfmt; i++){
        if( userfmt[i].format==c ){
          ufp = &userfmt[i];
          xtype = USER;
          break;
        }
      }
#endif
    }else{
      xtype = infop->type;
    }
    /*
    ** At this point, variables are initialized as follows:
    **
    **   flag_alternateform          TRUE if a '#' is present.
    **   flag_plussign               TRUE if a '+' is present.
    **   flag_leftjustify            TRUE if a '-' is present or if the
    **                               field width was negative.
    **   flag_zeropad                TRUE if the width began with 0.
    **   flag_long                   TRUE if the letter 'l' (ell) prefixed
    **                               the conversion character.
    **   flag_blanksign              TRUE if a ' ' is present.
    **   width                       The specified field width.  This is
    **                               always non-negative.  Zero is the default.
    **   precision                   The specified precision.  The default
    **                               is -1.
    **   xtype                       The class of the conversion.
    **   infop                       Pointer to the appropriate info struct.
    **   ufp                         Pointer to an approariate userinfo struct.
    */
    switch( xtype ){
      case RADIX:
        if( flag_long )  longvalue = va_arg(ap,long);
  else             longvalue = va_arg(ap,int);
#ifdef COMPATIBILITY
        /* For the format %#x, the value zero is printed "0" not "0x0".
        ** I think this is stupid. */
        if( longvalue==0 ) flag_alternateform = 0;
#else
        /* More sensible: turn off the prefix for octal (to prevent "00"),
        ** but leave the prefix for hex. */
        if( longvalue==0 && infop->base==8 ) flag_alternateform = 0;
#endif
        if( infop->flag_signed ){
          if( *(long*)&longvalue<0 ){
            longvalue = -*(long*)&longvalue;
            prefix = '-';
          }else if( flag_plussign )  prefix = '+';
          else if( flag_blanksign )  prefix = ' ';
          else                       prefix = 0;
        }else                        prefix = 0;
        if( flag_zeropad && precision<width-(prefix!=0) ){
          precision = width-(prefix!=0);
  }
        {
          register char *cset;      /* Use registers for speed */
          register int base;
          cset = infop->charset;
          base = infop->base;
          bufpt = &buf[BUFSIZE];
          do{                                           /* Convert to ascii */
            *(--bufpt) = cset[longvalue%base];
            longvalue = longvalue/base;
          }while( longvalue>0 );
  }
        length = (int)&buf[BUFSIZE]-(int)bufpt;
        for(idx=precision-length; idx>0; idx--){
          *(--bufpt) = '0';                             /* Zero pad */
  }
        if( prefix ) *(--bufpt) = prefix;               /* Add sign */
        if( flag_alternateform && infop->prefix ){      /* Add "0" or "0x" */
          char *pre, x;
          pre = infop->prefix;
          if( *bufpt!=pre[0] ){
            for(pre=infop->prefix; (x=*pre)!=0; pre++) *(--bufpt) = x;
    }
        }
        length = (int)&buf[BUFSIZE]-(int)bufpt;
        break;
      case FLOAT:
      case EXP:
      case GENERIC:
        realvalue = va_arg(ap,double);
#ifndef NOFLOATINGPOINT
        if( precision<0 ) precision = 6;         /* Set default precision */
        if( precision>BUFSIZE-10 ) precision = BUFSIZE-10;
        if( realvalue<0.0 ){
          realvalue = -realvalue;
          prefix = '-';
  }else{
          if( flag_plussign )          prefix = '+';
          else if( flag_blanksign )    prefix = ' ';
          else                         prefix = 0;
  }
        if( infop->type==GENERIC && precision>0 ) precision--;
        for(idx=precision, rounder=0.5; idx>0; idx--, rounder*=0.1);
        if( infop->type==FLOAT ) realvalue += rounder;
        /* Normalize realvalue to within 10.0 > realvalue >= 1.0 */
        exp = 0;
        while( realvalue>=1e8 ){ realvalue *= 1e-8; exp+=8; }
        while( realvalue>=10.0 ){ realvalue *= 0.1; exp++; }
        while( realvalue<1e-8 ){ realvalue *= 1e8; exp-=8; }
        while( realvalue<1.0 ){ realvalue *= 10.0; exp--; }
        bufpt = buf;
        /*
        ** If the field type is GENERIC, then convert to either EXP
        ** or FLOAT, as appropriate.
        */
        if( xtype==GENERIC ){
          flag_rtz = !flag_alternateform;
            if( exp<-4 || exp>precision ){
            xtype = EXP;
          }else{
            precision = precision - exp;
            realvalue += rounder;
            xtype = FLOAT;
          }
  }
        /*
        ** The "exp+precision" test causes output to be of type EXP if
        ** the precision is too large to fit in buf[].
        */
        if( xtype==FLOAT && exp+precision<BUFSIZE-30 ){
          flag_rtz = 0;
          flag_dp = (precision>0 || flag_alternateform);
          if( prefix ) *(bufpt++) = prefix;         /* Sign */
          if( exp<0 )  *(bufpt++) = '0';            /* Digits before "." */
          else for(; exp>=0; exp--) *(bufpt++) = getdigit(&realvalue);
          if( flag_dp ) *(bufpt++) = '.';           /* The decimal point */
          for(exp++; exp<0 && precision>0; precision--, exp++){
            *(bufpt++) = '0';
          }
          while( (precision--)>0 ) *(bufpt++) = getdigit(&realvalue);
          *(bufpt--) = 0;                           /* Null terminate */
          if( flag_rtz && flag_dp ){     /* Remove trailing zeros and "." */
            while( bufpt>=buf && *bufpt=='0' ) *(bufpt--) = 0;
            if( bufpt>=buf && *bufpt=='.' ) *(bufpt--) = 0;
          }
          bufpt++;                            /* point to next free slot */
  }else{    /* EXP */
          flag_rtz = 0;
          flag_dp = (precision>0 || flag_alternateform);
          realvalue += rounder;
          if( prefix ) *(bufpt++) = prefix;   /* Sign */
          *(bufpt++) = getdigit(&realvalue);  /* First digit */
          if( flag_dp ) *(bufpt++) = '.';     /* Decimal point */
          while( (precision--)>0 ) *(bufpt++) = getdigit(&realvalue);
          bufpt--;                            /* point to last digit */
          if( flag_rtz && flag_dp ){          /* Remove tail zeros */
            while( bufpt>=buf && *bufpt=='0' ) *(bufpt--) = 0;
            if( bufpt>=buf && *bufpt=='.' ) *(bufpt--) = 0;
          }
          bufpt++;                            /* point to next free slot */
          *(bufpt++) = infop->charset[0];
          if( exp<0 ){ *(bufpt++) = '-'; exp = -exp; } /* sign of exp */
          else       { *(bufpt++) = '+'; }
          if( exp>=100 ) *(bufpt++) = (exp/100)+'0';   /* 100's digit */
          *(bufpt++) = exp/10+'0';                     /* 10's digit */
          *(bufpt++) = exp%10+'0';                     /* 1's digit */
  }
        /* The converted number is in buf[] and zero terminated. Output it.
        ** Note that the number is in the usual order, not reversed as with
        ** integer conversions. */
        length = (int)bufpt-(int)buf;
        bufpt = buf;
#endif
        break;
      case SIZE:
        *(va_arg(ap,int*)) = count;
        length = width = 0;
        break;
      case PERCENT:
        buf[0] = '%';
        bufpt = buf;
        length = 1;
        break;
      case CHARLIT:
      case CHAR:
        c = buf[0] = (xtype==CHAR ? va_arg(ap,int) : *++fmt);
        if( precision>=0 ){
          for(idx=1; idx<precision; idx++) buf[idx] = c;
          length = precision;
  }else{
          length =1;
  }
        bufpt = buf;
        break;
      case STRING:
        bufpt = va_arg(ap,char*);
        if( bufpt==0 ) bufpt = "(null)";
        length = strlen(bufpt);
        if( precision>=0 && precision<length ) length = precision;
        break;
#ifdef USE_USER_FMT
      case USER:
        v.bufsize = BUFSIZE;
        v.buf = buf;
        v.precision = precision;
        v.flag_sharp = flag_alternateform;
        v.flag_plus = flag_plussign;
        v.flag_blank = flag_blanksign;
        v.ap = ap;
        v.arg = ufp->arg;
        length = (*(ufp->func))(&v);
        ap = v.ap;
        bufpt = buf;
        break; 
#endif
      case ERROR:
        buf[0] = '%';
        buf[1] = c;
        errorflag = 0;
        idx = 1+(c!=0);
        (*func)("%",idx,arg);
        count += idx;
        if( c==0 ) fmt--;
        break;
    }/* End switch over the format type */
    /*
    ** The text of the conversion is pointed to by "bufpt" and is
    ** "length" characters long.  The field width is "width".  Do
    ** the output.
    */
    if( !flag_leftjustify ){
      register int nspace;
      nspace = width-length;
      if( nspace>0 ){
        count += nspace;
        while( nspace>=SPACESIZE ){
          (*func)((char *) spaces,SPACESIZE,arg);
          nspace -= SPACESIZE;
        }
        if( nspace>0 ) (*func)((char *) spaces,nspace,arg);
      }
    }
    if( length>0 ){
      (*func)(bufpt,length,arg);
      count += length;
    }
    if( flag_leftjustify ){
      register int nspace;
      nspace = width-length;
      if( nspace>0 ){
        count += nspace;
        while( nspace>=SPACESIZE ){
          (*func)((char *) spaces,SPACESIZE,arg);
          nspace -= SPACESIZE;
        }
        if( nspace>0 ) (*func)((char *)spaces,nspace,arg);
      }
    }
  }/* End for loop over the format string */
  return errorflag ? EOF : count;
} /* End of function */

int xprintf(void (*func)(char*,int,void*), void *arg, const char *fmt, ...){
  va_list ap;
  va_start(ap,fmt);
  return vxprintf(func,arg,fmt,ap);
}

#ifndef NOINTERFACE
/*
** The following set of routines implement convenient interfaces to the
** xprintf functions.  These are as follows:
**
**     [v]printf             The usual library routines.
**     [v]sprintf            For generating strings.
**     [v]snprintf           Like sprintf, but the strings are limited in
**                           length to avoid overflowing buffers.
**     [v]nprintf            Do no printing.  Measure the size of the
**                           output string if it were to be printed.
**
** First, the regular print, and its variants, as found in any standard
** library.
*/
/*#**************************************************************************
*#
*#  FUNCTION NAME : serial_putch
*#
*#  PARAMETERS    : ch : Character to be put on the debug port.
*#
*#  RETURNS       : Character that was put on the debug port.
*#
*#  DESCRIPTION   : This function puts one character on the serial port.
*#
*#************************************************************************#*/

word serial_putch(ch)
word ch;
{
#ifndef HW_PC
  while (!BITTST(*((volatile byte *)R_SER1_STAT), TRE_BIT_MASK))
    ;
  *((volatile byte *)R_SER1_DOUT) = ch;
#endif
  return ch;
}

static void fout(register char *txt, register int amt, register void *arg){

#if defined(DEBUG) || defined(SERIAL_DEBUG) 

#ifdef DEBUG_USING_IRQ
  serial_chunk_print(txt,amt);  /* No, this doesnt have a prototype either... */
#else
  register int c;
  while( amt-->0 ){
    c = *txt++;
    if (c==10)
      serial_putch(13);
    serial_putch(c);
  }
#endif

#else

  USED(txt);
  USED(amt);
  USED(arg);

#endif
}
int printf(const char *fmt, ...){

#ifdef DEBUG_USING_IRQ
  /* Stop other interrupts during this critical period.
   * Note that you cannot do this in "fout()" because then your fields
   * may get mixed-up with other printouts and the normal printdata. */
  DISABLE_SAVE();
#endif

  int retval;
  va_list ap;
  va_start(ap,fmt);
  retval = vxprintf(fout,(void*)NULL,fmt,ap);

#ifdef DEBUG_USING_IRQ
  RESTORE();
#endif

  return retval;
}
int vprintf(const char *fmt, va_list ap){

#ifdef DEBUG_USING_IRQ
  /* Stop other interrupts during this critical period.
   * Note that you cannot do this in "fout()" because then your fields
   * may get mixed-up with other printouts and the normal printdata. */
  DISABLE_SAVE();
#endif

  int retval;
  retval = vxprintf(fout,(void*)NULL,fmt,ap);

#ifdef DEBUG_USING_IRQ
  RESTORE();
#endif

  return retval;
}
int fprintf(FILE *fp, const char *fmt, ...){

#ifdef DEBUG_USING_IRQ
  /* Stop other interrupts during this critical period.
   * Note that you cannot do this in "fout()" because then your fields
   * may get mixed-up with other printouts and the normal printdata. */
  DISABLE_SAVE();
#endif

  int retval;
  va_list ap;
  va_start(ap,fmt);
  retval = vxprintf(fout,(void*)fp,fmt,ap);

#ifdef DEBUG_USING_IRQ
  RESTORE();
#endif

  return retval;
}
int vfprintf(FILE *fp, const char *fmt, va_list ap){

#ifdef DEBUG_USING_IRQ
  /* Stop other interrupts during this critical period.
   * Note that you cannot do this in "fout()" because then your fields
   * may get mixed-up with other printouts and the normal printdata. */
  DISABLE_SAVE();
#endif

  int retval;
  retval = vxprintf(fout,(void*)fp,fmt,ap);

#ifdef DEBUG_USING_IRQ
  RESTORE();
#endif

  return retval;
}

/*
** Now for string-printf, also as found in any standard library.
** Add to this the snprintf function which stops added characters
** to the string at a given length.
**
** Note that snprintf returns the length of the string as it would
** be if there were no limit on the output.
*/
typedef struct s_strargument {    /* Describes the string being written to */
  char *next;                        /* Next free slot in the string */
  char *last;                        /* Last available slot in the string */
} sarg;

static void sout(char *txt, int amt, void *arg){
  register char *head;
  register char *t;  
  register int a;
  register char *tail;
  a = amt;
  t = txt;
  head = ((sarg*)arg)->next;
  tail = ((sarg*)arg)->last;
  if( tail ){
    while( a-->0 && head<tail ) *(head++) = *(t++);
  }else{
    while( a-->0 ) *(head++) = *(t++);
  }
  *head = 0;
  ((sarg*)arg)->next = head;
}
int sprintf(char *buf, const char *fmt, ...){
  sarg arg;
  va_list ap;
  va_start(ap,fmt);
  arg.next = buf;
  arg.last = 0;
  return vxprintf(sout,(void*)&arg,fmt,ap);
}
int vsprintf(char *buf, const char *fmt, va_list ap){
  sarg arg;
  arg.next = buf;
  arg.last = 0;
  return vxprintf(sout,(void*)&arg,fmt,ap);
}
int snprintf(char *buf, size_t n, const char *fmt, ...){
  va_list ap;
  sarg arg;
  va_start(ap,fmt);
  arg.next = buf;
  arg.last = &buf[n-1];
  return vxprintf(sout,(void*)&arg,fmt,ap);
}
int vsnprintf(char *buf, size_t n, const char *fmt, va_list ap){
  sarg arg;
  arg.next = buf;
  arg.last = &buf[n-1];
  return vxprintf(sout,(void*)&arg,fmt,ap);
}

/*
** Null-printf does nothing but measure the size of the output
** string.
*/
static void nout(void){
  return;
}
int nprintf(const char *fmt, ...){
  va_list ap;
  va_start(ap,fmt);
  return vxprintf((void(*)(char*,int,void*))nout,(void*)0,fmt,ap);
}
int vnprintf(const char *fmt, va_list ap){
  return vxprintf((void(*)(char*,int,void*))nout,(void*)0,fmt,ap);
}
#endif
