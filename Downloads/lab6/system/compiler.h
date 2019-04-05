/*!******************************************************************
*!
*!   FILE NAME:    COMPILER.H
*!
*!   DESCRIPTION:  AXIS GLOBAL COMPILER DEFINITIONS
*!                 VERSION 4.13 950117
*!
*!-------------------------------------------------------------------
*!
*!  HISTORY:
*!
*!  DATE:   VER:  NAME:   CHANGES:
*!  =====   ====  =====   ========
*!  931118  4.0   KJ      - Cleaned up all different compiler.h versions
*!                        - and moved to 4.0.
*!  931208  4.1   AB      - Definitions for Watcom C/C++32
*!  940111  4.2   KJ      - HILO etc. for WATCOM
*!  940324  4.3   MR      - for 32 bit systems: Changed absaddr from
*!                          void* to byte*
*!  940328  4.4   MR      - FLAT_32BIT defined.
*!  940420  4.05  IP      - Added alloc-32 macros to GNU-CRIS
*!  940502  4.06  AB      - Added Borland C++/32 Console
*!  940517  4.07  AB      - Enhanced casting for PUT_xxx
*!                        - HIBYTE: Lint warning 572 disabled
*!                        - Some macros put on several lines (for clarity)
*!                        - Added comment, and warning, to MAX and MIN
*!  940518  4.08  MR      - Macros for interrupt control,
*!                          DISABLE() & RESTORE(). Dummies on all platt-
*!                          forms except WATCOM & CRIS.
*!  940607  4.09  AB      - Fixed awful PUT_-macro bug.
*!  940610  4.10  AB      - Simplified definition of NULL
*!  940704  4.11  AB      - Added GNU C (Sparc) and Quick C (16-bit DOS)
*!                        - Made NULL same for all compilers
*!  950104  4.12  JN      - Added a new segment 'DEFINITIONS FOR LINT' and
*!                          redefined ZERO_LENGTH_ARRAY for LINT.
*!
*!  Jan 17 1995   Mart Roomus       4.13, Adjusted a few absaddr typedefs
*!  Mar 06 1995   Mart Roomus       Adjusted enable/disable protos for watcom
*!  Mar 23 1995   H-P Nilsson       Added ENABLE_ONCE_ONLY()
*!  Apr 18 1995   Mart Roomus       Better interrupt control stuff:
*!                                  DISABLE_SAVE()  save state and disable ints
*!                                  RESTORE()       restore interrupt state
*!                                  DISABLE()       disable interrupt
*!                                  ENABLE()        enable interrupt
*!                                  Removed ENABLE_ONCE_ONLY(); it's guaranteed
*!                                  that a ENABLE()/DISABLE() sequence will
*!                                  momentarily enable interrupts. It's up
*!                                  to the macro constructor to ensure this.
*!  Apr 20 1995   Mart Roomus       Adjusted possibly erroneous ENABLE() macro
*!                                  for GNU_CRIS.
*!  Apr 24 1995   Willy Sagefalk    Fixed swapped enable/disable bug.
*!  Nov 13 1995   Patrik Bannura    ifdef'd out 'bool' typedef for g++.
*!  Mar 14 1996   Fredrik Norrman   Made multiple includable
*!  May 17 1996   Sven Ekstrom      Added definitions for MSVC++ 4.1.
*!  Aug 02 1996   H-P Nilsson       Modified DISABLE_SAVE and RESTORE macro for
*!                                  Etrax.
*!  Oct 02 1996   H-P Nilsson       Updated VC++ for proposed PC-env (_WIN32 +
*!                                  _CONSOLE + _M_IX86)
*!  Oct 24 1996   H-P Nilsson       Modified enable()/disable() etc to ease
*!                                  debugging and defined PC_DEBUG for PC-env.
*!  Nov 04 1996   Fred Jonsson      Modified for Borland C++  
*!  Nov  8 1996   Fredrik Svensson  Changed VC++ word and uword def to 16 bits
*!  Dec 19 1996   Fredrik Svensson  Fixed more stuff for VC++
*!---------------------------------------------------------------------------
*!        (C) 1993-1996 Axis Communications AB, Lund, Sweden
*!**************************************************************************/
/* @(#) compiler.h 1.30 12/19/96 */

#ifndef compiler_h
#define compiler_h

/*********************************************************************
*!
*!      COMPILER SPECIFIC DEFINITIONS
*!      =============================
*!
*!  The definitions are described for Turbo C. The definitions
*!  for the other compilers follow directly after.
*!
*!********************************************************************/
/*********************************************************************
*!  DEFINITIONS FOR BORLAND TURBO C
*********************************************************************/

#ifdef __TURBOC__
#define PROTOTYPES

/*********************************************************************
*!
*!  TYPES
*!  As it is uncertain how long or short the standard types of C are,
*!  special types have been introduced. The length of these types
*!  are guaranteed. They are used as any standard type.
*!
*!********************************************************************/
typedef unsigned char   byte;
typedef signed char     sbyte;

#ifdef __WIN32__
  typedef short           word;
  typedef unsigned short  uword;
  typedef int             dword;
  typedef unsigned        udword;
  typedef char            *absaddr;     /* Absolute linear address */
#else
  typedef int             word;    /* Signed two-byte value (16 bits) */
  typedef unsigned int    uword;   /* Unsigned two-byte value (16 bits) */
  typedef long            dword;   /* Signed four-byte value (32 bits) */
  typedef unsigned long   udword;  /* Unsigned four-byte value (32 bits) */
  typedef byte HUGE       *absaddr;     /* Absolute linear address */
#endif

#if !defined(__cplusplus) && !defined(bool)
typedef char            bool;    /* Boolean value (TRUE or FALSE) */
#define true            1
#define false           0 
#endif

#ifndef __WIN32__
  #undef  FLAT_32BIT                       /* Not 32 bit linear address */
#endif

#define NO_RET          void     /* No return from a function  */
#define NO_PARAMS       void     /* No parameter to a function */
#define VOID            void
#define CONST           const
#define HUGE            
#define FAR
#define NEAR            near
#define VOLATILE        volatile
#define USED(name)      { if (name) { ; } }  /* Dummy use of names to avoid */
                                             /* warnings. Typically for unused */
                                             /* parameters. */

#define INTERRUPT   interrupt      /* Make the function of interrupt type */
#define LOCAL                      /* Variable/function can not be used */
                                   /* from other segments. This is made */
                                   /* for CGA3 banking system... */

/* Constants defining opening of files */
#define WRITE_BINARY    "wb"
#define READ_BINARY     "rb"
#define APPEND_BINARY   "ab"

/* Malloc functions */
#define MALLOC32(size)             farmalloc(size)
#define REALLOC32(old,size)        farrealloc(old,size)
#define FREE32(block)              farfree(block)

#define ZERO_LENGTH_ARRAY  1        /* Used when defining an array with size 0 */

/* Big-endian-network <-> host byte order conversion macros */
#define HILO(a) ( (((a)>>8) & 0xff) | (((a)<<8) & 0xff00) ) /* Set HI-LO order */
#define LHILO(a) ( (HILO((a) & 0xffff)<<16)+HILO((a)>>16) )

/* Little-endian-network <-> host byte order conversion macros */
#define LOHI(a)  (a)    /* Set LO-HI order */
#define LLOHI(a) (a)

#define DISABLE_SAVE()
#define RESTORE()
#define DISABLE()
#define ENABLE()

#define __MINMAX_DEFINED

#endif



/*********************************************************************
*!  DEFINITIONS FOR MICROSOFT QUICK C FOR WINDOWS (DOS PROGRAM)
*********************************************************************/

#if defined(_MSC_VER) && (_MSC_VER < 800)

#define PROTOTYPES
#define CONST       const
#define HUGE        huge
#define FAR         far
#define NEAR        near
#define VOLATILE    volatile
#define USED(name)  { if (name) { ; } }
#define INTERRUPT   interrupt
#define LOCAL

typedef char            bool;
typedef unsigned char   byte;
typedef signed char     sbyte;
typedef int             word;
typedef unsigned int    uword;
typedef long            dword;
typedef unsigned long   udword;

#define NO_RET          void
#define NO_PARAMS       void
#define VOID            void
typedef byte HUGE       *absaddr;

#define WRITE_BINARY    "wb"
#define READ_BINARY     "rb"
#define APPEND_BINARY   "ab"

#define MALLOC32(size)             fmalloc(size)
#define REALLOC32(old,size)        frealloc(old,size)
#define FREE32(block)              ffree(block)

#define ZERO_LENGTH_ARRAY

#define HILO(a) ( (((a)>>8) & 0xff) | (((a)<<8) & 0xff00) ) /* Set HI-LO order */
#define LHILO(a) ( (HILO((a) & 0xffff)<<16)+HILO((a)>>16) )

#define LOHI(a)  (a)    /* Set LO-HI order */
#define LLOHI(a) (a)

#define DISABLE_SAVE()
#define RESTORE()
#define DISABLE()
#define ENABLE()

#endif


/*********************************************************************
*!  DEFINITIONS FOR MICROSOFT VISUAL C++ 4.1
*********************************************************************/

#if defined(_MSC_VER) && (_MSC_VER >= 801)

/*
** Disable warning 4786: 'xxx' : identifier was truncated to '255' characters
** in the browser information
*/
#pragma warning(disable: 4786)

/* Undefine stuff that windows.h defines */
#undef FAR
#undef NEAR
#undef HIBYTE
#undef LOBYTE

#define PROTOTYPES
#define CONST       const
#define HUGE
#define FAR
#define NEAR

#define VOLATILE    volatile
#define USED(name)  { if (name) { ; } }
#define INTERRUPT   
#define LOCAL

#define bool            char
#define false           0
#define true            1


typedef unsigned char   byte;
typedef signed char     sbyte;
typedef short           word;
typedef unsigned short  uword;
typedef long            dword;
typedef unsigned long   udword;

#define NO_RET          void
#define NO_PARAMS       void
#define VOID            void
typedef byte            *absaddr;


#define WRITE_BINARY    "wb"
#define READ_BINARY     "rb"
#define APPEND_BINARY   "ab"

#define MALLOC32(size)             fmalloc(size)
#define REALLOC32(old,size)        frealloc(old,size)
#define FREE32(block)              ffree(block)

#define ZERO_LENGTH_ARRAY

#define HILO(a) ( (((a)>>8) & 0xff) | (((a)<<8) & 0xff00) ) /* Set HI-LO order */
#define LHILO(a) ( (HILO((a) & 0xffff)<<16)+HILO((a)>>16) )

#define LOHI(a)  (a)    /* Set LO-HI order */
#define LLOHI(a) (a)

/* If running in the "PC debug environment" */
#if defined(_WIN32) && defined(_CONSOLE) && defined(_M_IX86)

/* We need to include projos.h, but that would need a change in
   inclusion order etc etc. */
#ifdef __cplusplus
extern "C"
#endif
extern uword disable_save(const char *, unsigned long);
#define DISABLE_SAVE() uword _Int_state = disable_save(__FILE__, __LINE__)

#ifdef __cplusplus
extern "C"
#endif
extern void disable_restore(uword, const char *, unsigned long);
#define RESTORE() disable_restore(_Int_state, __FILE__, __LINE__)

#ifdef __cplusplus
extern "C"
#endif
extern void disable(const char *, unsigned long);
#define DISABLE() disable(__FILE__, __LINE__)

#ifdef __cplusplus
extern "C"
#endif
extern void enable(const char *, unsigned long);
#define ENABLE() enable(__FILE__, __LINE__)

#define PC_DEBUG

#else  /* Not using the PC environment (or change to console app!) */

#define DISABLE_SAVE()
#define RESTORE()
#define DISABLE()
#define ENABLE()
#endif

#endif



/*********************************************************************
*!  DEFINITIONS FOR PARAGON MCC68K
*!  Description: see above for Turbo C
*********************************************************************/

#ifdef  MCC68K

#undef  PROTOTYPES
#define FLAT_32BIT
#define CONST   const
#define HUGE
#define FAR
#define NEAR
#define VOLATILE
#define USED(name)
#define INTERRUPT
#define LOCAL

typedef char            bool;
typedef unsigned char   byte;
typedef char            sbyte;
typedef short           word;
typedef unsigned short  uword;
typedef int             dword;
typedef unsigned        udword;

#define NO_RET          void
#define NO_PARAMS
#define VOID            int
typedef byte            *absaddr;

#define max(a,b)    ((a) > (b) ? (a) : (b))
#define min(a,b)    ((a) < (b) ? (a) : (b))

#define WRITE_BINARY    "w"
#define READ_BINARY     "r"
#define APPEND_BINARY   "a"

#define MALLOC32(size)             malloc(size)
#define REALLOC32(old,size)        realloc(old,size)
#define FREE32(block)              free(block)

#define ZERO_LENGTH_ARRAY

#define DISABLE_SAVE()
#define RESTORE()
#define DISABLE()
#define ENABLE()

#endif

/*********************************************************************
*!  DEFINITIONS FOR GNX FOR CG16
*!  Description: see above for Turbo C
*!********************************************************************/

#ifdef  GNX_CG16

#define PROTOTYPES
#define FLAT_32BIT
#define CONST           const
#define HUGE
#define NEAR
#define FAR
#define VOLATILE        volatile
#define USED(name)
#define INTERRUPT
#define LOCAL

typedef char            bool;
typedef unsigned char   byte;
typedef char            sbyte;
typedef short           word;
typedef unsigned short  uword;
typedef int             dword;
typedef unsigned int    udword;

#define NO_RET          void
#define NO_PARAMS       void
#define VOID            void
typedef byte            *absaddr;

#define   WRITE_BINARY  "w"
#define   READ_BINARY   "r"
#define   APPEND_BINARY "a"

#define MALLOC32(size)             malloc(size)
#define REALLOC32(old,size)        realloc(old,size)
#define FREE32(block)              free(block)

#define ZERO_LENGTH_ARRAY

#define HILO(a) ( (((a)>>8) & 0xff) | (((a)<<8) & 0xff00) ) /* Set HI-LO order */
#define LHILO(a) ( (HILO((a) & 0xffff)<<16)+HILO((a)>>16) )

#define LOHI(a)  (a)    /* Set LO-HI order */
#define LLOHI(a) (a)

/* It is optimizational suicide to use asm() statements with GNX4.4 */
extern uword int_disable_save(void);
extern void  int_restore(uword flags);
extern void  int_enable(void);
extern void  int_disable(void);

#define DISABLE_SAVE()  uword  _Flags = int_disable_save()
#define RESTORE()       int_restore(_Flags)
#define DISABLE()       int_disable()
#define ENABLE()        int_enable()

#endif


/*********************************************************************
*!  DEFINITIONS FOR ZORTECH DOS 386/486 C++ COMPILER
*!  Description: see above for Turbo C
*!  Note: <char> is assumed unsigned.
*!********************************************************************/

#ifdef __ZTC__
#if __I86__==3 || __I86__==4
#undef  PROTOTYPES
#define FLAT_32BIT
#define CONST      const
#define HUGE
#define NEAR       near
#define FAR        far
#define VOLATILE   volatile
#define USED(name)
#define INTERRUPT  interrupt
#define LOCAL

typedef char            bool;
typedef char            byte;
typedef signed char     sbyte;
typedef short           word;
typedef unsigned short  uword;
typedef int             dword;
typedef unsigned int    udword;

#define NO_RET          void
#define NO_PARAMS       void
#define VOID            void
typedef byte            *absaddr;

#define WRITE_BINARY  "wb"
#define READ_BINARY   "rb"
#define APPEND_BINARY "ab"

#define MALLOC32(size)             malloc(size)
#define REALLOC32(old,size)        realloc(old,size)
#define FREE32(block)              free(block)

#define ZERO_LENGTH_ARRAY

#define HILO(a) ( (((a)>>8) & 0xff) | (((a)<<8) & 0xff00) ) /* Set HI-LO order */
#define LHILO(a) ( (HILO((a) & 0xffff)<<16)+HILO((a)>>16) )

#define LOHI(a)  (a)    /* Set LO-HI order */
#define LLOHI(a) (a)

#define DISABLE_SAVE()
#define RESTORE()
#define DISABLE()
#define ENABLE()

#endif
#endif

/*********************************************************************
*!  DEFINITIONS FOR UNIX CC
*!  Description: see above for Turbo C
*!********************************************************************/

#ifdef  _CC_

#undef  PROTOTYPES
#define FLAT_32BIT
#define CONST
#define HUGE
#define NEAR
#define FAR
#define VOLATILE
#define USED(name)
#define INTERRUPT
#define LOCAL

typedef char            bool;
typedef unsigned char   byte;
typedef char            sbyte;
typedef short           word;
typedef unsigned short  uword;
typedef int             dword;
typedef unsigned int    udword;
typedef byte            *absaddr;

#define NO_RET    void
#define NO_PARAMS
#define VOID     int

#define   WRITE_BINARY  "w"
#define   READ_BINARY   "r"
#define   APPEND_BINARY "a"

#define MALLOC32(size)             malloc(size)
#define REALLOC32(old,size)        realloc(old,size)
#define FREE32(block)              free(block)

#define ZERO_LENGTH_ARRAY

#define DISABLE_SAVE()
#define RESTORE()
#define DISABLE()
#define ENABLE()

#endif

/*********************************************************************
*!  DEFINITIONS FOR GNU CRIS COMPILER
*!  Description: see above for Turbo C
*!********************************************************************/

#ifdef  __GNU_CRIS__

#define HW_ETRAX

#define PROTOTYPES
#define FLAT_32BIT
#define CONST           const
#define HUGE
#define NEAR
#define FAR
#define VOLATILE        volatile
#define USED(name)

#define INTERRUPT
#define LOCAL

#ifndef __cplusplus 
typedef char            bool; /* valid keyword in C++ */
#endif
typedef unsigned char   byte;
typedef signed char     sbyte;
typedef short           word;
typedef unsigned short  uword;
typedef int             dword;
typedef unsigned int    udword;

#define NO_RET          void
#define NO_PARAMS       void
#define VOID            void
typedef byte            *absaddr;

#define   WRITE_BINARY  "wb"
#define   READ_BINARY   "rb"
#define   APPEND_BINARY "ab"

#define MALLOC32(size)             malloc(size)
#define REALLOC32(old,size)        realloc(old,size)
#define FREE32(block)              free(block)

#define ZERO_LENGTH_ARRAY    0

#define HILO(a) ( (((a)>>8) & 0xff) | (((a)<<8) & 0xff00) ) /* Set HI-LO order */
#define LHILO(a) ( (HILO((a) & 0xffff)<<16)+HILO((a)>>16) )

#define LOHI(a)  (a)    /* Set LO-HI order */
#define LLOHI(a) (a)

#define DISABLE_SAVE() uword _Flags; __asm__ volatile ("move ccr,%0\n\tdi" : "=rm" (_Flags))
#define RESTORE() __asm__ volatile ("move %0,ccr" : : "rm" (_Flags))

#define ENABLE()   __asm__ volatile (" ei\n"           \
                                     " nop")
#define DISABLE()  __asm__ volatile ("di")

#endif

/*********************************************************************
*!  DEFINITIONS FOR GNU C SPARC COMPILER
*!  Description: see above for Turbo C
*!********************************************************************/

#ifdef __GNUC__
#ifdef __sparc__

#define PROTOTYPES
#define FLAT_32BIT
#define CONST           const
#define HUGE
#define NEAR
#define FAR
#define VOLATILE        volatile
#define USED(name)

#define INTERRUPT
#define LOCAL

#ifndef __cplusplus
typedef char            bool; /* valid keyword in C++ */
#endif
typedef unsigned char   byte;
typedef signed char     sbyte;
typedef short           word;
typedef unsigned short  uword;
typedef int             dword;
typedef unsigned int    udword;

#define NO_RET          void
#define NO_PARAMS       void
#define VOID            void
typedef byte            *absaddr;

#define WRITE_BINARY    "wb"
#define READ_BINARY     "rb"
#define APPEND_BINARY   "ab"

#define MALLOC32(size)             malloc(size)
#define REALLOC32(old,size)        realloc(old,size)
#define FREE32(block)              free(block)

#define ZERO_LENGTH_ARRAY    0

#define HILO(a) ( (((a)>>8) & 0xff) | (((a)<<8) & 0xff00) ) /* Set HI-LO order */
#define LHILO(a) ( (HILO((a) & 0xffff)<<16)+HILO((a)>>16) )

#define LOHI(a)  (a)    /* Set LO-HI order */
#define LLOHI(a) (a)

#define DISABLE_SAVE()
#define RESTORE()
#define DISABLE()
#define ENABLE()

#endif
#endif


/*********************************************************************
*!  DEFINITIONS FOR CGA3/6809 INTROL COMPILER
*!  Description: see above for Turbo C
*!********************************************************************/

#ifdef  __CC09__

#define PROTOTYPES
#undef  FLAT_32BIT
#define CONST   const
#define HUGE
#define NEAR
#define FAR
#define VOLATILE        volatile
#define USED(name)
#define INTERRUPT
#ifdef _lint
#define LOCAL
#else
#define LOCAL           __mod1__
#endif

typedef char            bool;
typedef unsigned char   byte;
typedef char            sbyte;
typedef short           word;
typedef unsigned short  uword;
typedef long            dword;
typedef unsigned long   udword;

#define NO_RET          void
#define NO_PARAMS       void
#define VOID            void
typedef udword          absaddr;

#define WRITE_BINARY    "w"
#define READ_BINARY     "r"
#define APPEND_BINARY   "a"

/* The CPU can only address 64k */
#define MALLOC32(size)             malloc(size)
#define REALLOC32(old,size)        realloc(old,size)
#define FREE32(block)              free(block)

#define ZERO_LENGTH_ARRAY

#define DISABLE_SAVE()
#define RESTORE()
#define DISABLE()
#define ENABLE()

#endif

/*********************************************************************
*!  DEFINITIONS FOR C Set/2 for OS/2 2.0
*!  Description: see above for Turbo C
*!********************************************************************/

#if defined(__IBMC__) && defined(__32BIT__)
#undef  PROTOTYPES
#define FLAT_32BIT
#define CONST       const
#define HUGE
#define FAR
#define NEAR
#define VOLATILE    volatile
#define USED(name)  { if (name) { ; } }
#define INTERRUPT
#define LOCAL

typedef char            bool;
typedef unsigned char   byte;
typedef signed char     sbyte;
typedef short           word;
typedef unsigned short  uword;
typedef int             dword;
typedef unsigned int    udword;

#define NO_RET          void
#define NO_PARAMS       void
#define VOID            void
typedef byte            *absaddr;

#define WRITE_BINARY    "wb"
#define READ_BINARY     "rb"
#define APPEND_BINARY   "ab"

#define MALLOC32(size)             malloc(size)
#define REALLOC32(old,size)        realloc(old,size)
#define FREE32(block)              free(block)

#define ZERO_LENGTH_ARRAY

#define DISABLE_SAVE()
#define RESTORE()
#define DISABLE()
#define ENABLE()

#endif

/*********************************************************************
*!  DEFINITIONS FOR Watcom C/C++32
*!  Description: see above for Turbo C
*!********************************************************************/

#if defined(__WATCOMC__)
#define PROTOTYPES
#define FLAT_32BIT
#define CONST       const
#define HUGE
#define FAR
#define NEAR
#define VOLATILE    volatile
#define USED(name)  { if (name) { ; } }
#define INTERRUPT   __interrupt
#define LOCAL

typedef char            bool;
typedef unsigned char   byte;
typedef signed char     sbyte;
typedef short           word;
typedef unsigned short  uword;
typedef int             dword;
typedef unsigned int    udword;

#define NO_RET          void
#define NO_PARAMS       void
#define VOID            void
typedef byte            *absaddr;

#define WRITE_BINARY    "wb"
#define READ_BINARY     "rb"
#define APPEND_BINARY   "ab"

#define MALLOC32(size)             malloc(size)
#define REALLOC32(old,size)        realloc(old,size)
#define FREE32(block)              free(block)

#define ZERO_LENGTH_ARRAY

/* Big-endian-network <-> host byte order conversion macros */
#define HILO(a) ( (((a)>>8) & 0xff) | (((a)<<8) & 0xff00) ) /* Set HI-LO order */
#define LHILO(a) ( (HILO((a) & 0xffff)<<16)+HILO((a)>>16) )

/* Little-endian-network <-> host byte order conversion macros */
#define LOHI(a)  (a)    /* Set LO-HI order */
#define LLOHI(a) (a)

/* enable() / disable() requires PROJOS.H inclusion */

/* Fancy watcom inline stuff */
extern uword disable_save_code(void);
#pragma aux disable_save_code = \
        "pushf"                 \
        "cli"                   \
        "pop    ax"             \
        value [ax];

extern void restore_code(uword flags);
#pragma aux restore_code = \
        "push   ax"        \
        "popf"             \
        parm [ax];

extern void enable_code(void);
#pragma aux enable_code = \
            "sti"         \
            "nop";

extern void disable_code(void);
#pragma aux disable_code = "cli";

#define DISABLE_SAVE()   uword _Flags = disable_save_code()
#define RESTORE()        restore_code(_Flags)
#define ENABLE()         enable_code()
#define DISABLE()        disable_code()

#endif

/*********************************************************************
*!
*!      COMPILER GENERAL DEFINITIONS
*!      =============================
*!
*!********************************************************************/

#ifndef NULL
#define NULL 0
#endif

#define ABSNULL (absaddr)0

/***************************************************************
*
*    BOOLEAN (LOGICAL) CONSTANTS
*
*      Defined as logical expressions instead of assuming that
*      TRUE is 1 and FALSE is 0. More portable that way. Use these
*      instead of 1 and 0 directly.
*
*  Basic boolean values
***************************************************************/

#ifndef TRUE
#define TRUE    (1)
#endif
#ifndef FALSE
#define FALSE   (0)
#endif

/*  Extra boolean values (mainly for the sake of convenience) */

#ifndef YES
#define YES     TRUE
#endif

#ifndef NO
#define NO      FALSE
#endif

#define ON      TRUE
#define OFF     FALSE

/****************************************************************
* MACRO TO DEFINE GLOBAL DATA
****************************************************************/

#ifdef  DEF_VARS
#define EXTERNAL            /* Declare/export variable */
#else
#define EXTERNAL    extern  /* Import variable */
#endif

/****************************************************************
*
*  MACROS FOR EXTRACTING/APPENDING VALUES FROM ONE TYPE TO ANOTHER
*
*  The types used are defined later on for each compiler.
*  These macros guarantees correct function despite a strange CPU.
*  Also they make the Lint keep quiet.
*********************************************************************/

/*  Extract high byte from a uword   */

#define HIBYTE(val)               /*lint -save -e572*/ ((byte)((val)>>8)) /*lint -restore*/

/*  Extract low byte from a uword  */
#define LOBYTE(val)               ((byte)((val) & 0xFF))

/*  Append two byte/sbyte values into a word value  */
#define BYTE_WORD(b1,b2)          (((word)b1<<8) | \
                                    (word)b2)
/*  Append two byte/sbyte values into a uword value  */
#define BYTE_UWORD(b1,b2)         (((uword)b1<<8) | \
                                    (uword)b2)

/*  Append four byte/sbyte values into a dword value  */
#define BYTE_DWORD(b1,b2,b3,b4)   (((dword)b1<<24) | \
                                   ((dword)b2<<16) | \
                                   ((dword)b3<<8) | \
                                    (dword)b4)

/*  Append four byte/sbyte values into a udword value  */
#define BYTE_UDWORD(b1,b2,b3,b4)  (((udword)b1<<24) | \
                                   ((udword)b2<<16) | \
                                   ((udword)b3<<8) | \
                                    (udword)b4)

/*  Append two word/uword values into a dword value  */
#define WORD_DWORD(w1,w2)         (((dword)w1<<16) | \
                                    (dword)w2)

/*  Append two word/uword values into a udword value  */
#define WORD_UDWORD(w1,w2)        (((udword)w1<<16) | \
                                    (udword)w2)


/***************************************************************
*
*  MACROS FOR EXTRACTING PARAMETERS FROM UNSIGNED BYTE VECTORS
*    ptr:    Pointer to start of byte vector
*    ix:     Index to where the data starts
*  Note that an unsigned byte may also be extracted by "ptr[ix]" directly.
*  The macros assume ptr points to a byte vector.
*
*****************************************************************/

/*  Extract one unsigned byte  */
#define GET_BYTE(ptr,ix)              (ptr[ix])

/*  Extract one signed byte */
#define GET_SBYTE(ptr,ix)      ((sbyte)ptr[ix])

/*  Extract one unsigned word (two bytes)  */
#define GET_UWORD(ptr,ix)    ((uword)((ptr[ix] << 8) | \
                                       ptr[ix+1]))

/*  Extracting one signed word (two bytes)  */
#define GET_WORD(ptr,ix)      ((word)((ptr[ix] << 8) | \
                                       ptr[ix+1]))

/*  Extracting three unsigned bytes  */                 
#define GET_3B_UWORD(ptr,ix) (((udword)ptr[ix] << 16) | \
                              ((udword)ptr[ix+1] << 8) | \
                                       ptr[ix+2])

/*  Extracting four unsigned bytes  */
#define GET_UDWORD(ptr,ix)   (((udword)ptr[ix] << 24) | \
                              ((udword)ptr[ix+1] << 16) | \
                              ((udword)ptr[ix+2] << 8) | \
                                       ptr[ix+3])


/****************************************************************
*
*  MACROS FOR PUTTING PARAMETERS IN UNSIGNED BYTE VECTORS
*    ptr:    Pointer to start of byte vector
*    ix:     Index to where the data should be stored
*    val:    Value to store
*  Note that an unsigned byte may also be put by "ptr[ix] = val" directly.
*
****************************************************************/

/*  Store an unsigned or signed one-byte (byte/sbyte) value  */
#define PUT_BYTE(ptr,ix,val)    ( ptr[ix] = (byte)(val) )

/*  Store an unsigned two-byte (uword) value  */
#define PUT_UWORD(ptr,ix,val)   ( ptr[ix]   = (byte)((val)>>8), \
                                  ptr[ix+1] = (byte)(val) )

/*  Store a signed two-byte (word) value  */
#define PUT_WORD(ptr,ix,val)    ( ptr[ix]   = (byte)((val)>>8), \
                                  ptr[ix+1] = (byte)(val) )

/*  Store three unsigned bytes */
#define PUT_3B_WORD(ptr,ix,val) ( ptr[ix]   = (byte)((val)>>16), \
                                  ptr[ix+1] = (byte)((val)>>8), \
                                  ptr[ix+2] = (byte)(val) )

/*  Store four unsigned bytes */
#define PUT_DWORD(ptr,ix,val)   ( ptr[ix]   = (byte)((val)>>24), \
                                  ptr[ix+1] = (byte)((val)>>16), \
                                  ptr[ix+2] = (byte)((val)>>8), \
                                  ptr[ix+3] = (byte)(val) )


/***************************************************************
*
*  MACROS FOR MAXIMUM AND MINIMUM VALUES. BEWARE OF SIDE EFFECTS!
*
****************************************************************/


/* MAX and MIN */
#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

/***************************************************************
*
*  MACROS FOR SETTING/CLEARING BOOLEAN VARIABLES
*
****************************************************************/

/*  Set boolean variable:  */
#define SET(var)  (var = TRUE)

/*  Clear boolean variable:  */
#define CLR(var)  (var = FALSE)


/***************************************************************
*
*  MACRO FOR CONVERTING A BOOLEAN VALUE TO A STRING
*
***************************************************************/

#define BOOL_STR(var)  ((var) ? "TRUE" : "FALSE")


/***************************************************************
*
*  MACROS FOR SETTING/CLEARING/TESTING BITS
*
*  The macros works with any simple type as long as the types of <var> 
*  and <bits> do not conflict. It is possible to set, clear, or 
*  test, any number of bits at the same time.
*
****************************************************************/

/*  Set bit(s) in the variable <var>  */
#define BITSET(var,bits)    ((var) |= (bits))

/*  Clear bit(s) in the variable <var>  */
#define BITCLR(var,bits)    ((var) &= ~(bits))

/*  Test bit(s) in the variable <var>
*  The result is TRUE if any of the tested bits are non-zero.
*  Can be used in two ways:
*    Control condition:
*      if/while (BITTST(buffer_flags, empty | within_limits)) ....
*    Boolean expression:
*      arq_set = BITTST(ipds_cmd[0x04], 0x80);
*  Boolean <not> can be used:
*    if (!BITTST(duck_attributes, dead) ...
*/
#define BITTST(var,bits)    (((var) & (bits)) != 0)

/*********************************************************************
*!  DEFINITIONS FOR LINT
*********************************************************************/

/*lint -uZERO_LENGTH_ARRAY   */
/*lint -dZERO_LENGTH_ARRAY=1 */  /* LINT does not like empty arrays */

/*********************** END OF FILE COMPILER.H **********************/
#endif /* compiler_h */
