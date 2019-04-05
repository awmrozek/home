/* ============================================================================
*! FILE NAME  : setjmp.hh
*! DESCRIPTION:
*! The inline definitions of setjmp and longjmp for cris
*! 
*! --------------------------------------------------------------------------
*! HISTORY
*! 
*! DATE         NAME            CHANGES
*! ----         ----            -------
*! 1995         Kenny R         Initial version
*! May 14 1996  Stefan S        int __inline__ => __inline__ int to avoid
*!                              warning
*! Nov 04 1996  Fred Jonsson    Added ifdef over etrax assembler
*! Nov 26 1996  Fredrik Svensson   Don't include this file for PC environment 
*! --------------------------------------------------------------------------
*! (C) Copyright 1995, Axis Technologies AB, LUND, SWEDEN
*! ========================================================================== */
/* @(#) setjmp.hh 1.5 11/26/96 */

#ifndef __setjmp_h
#define __setjmp_h

#include "compiler.h"
#ifndef PC_DEBUG

#ifdef __cplusplus
extern "C" {
#endif

/* jmp_buf is a vector for PC,R14(sp)..R0,SRP,CCR */
/*
   Note that we save and restore CCR to be able to
   correctly handle DI/EI. This could however be a problem
   with DMA. Check this!!!

   jmp_buf[0] - PC
   jmp_buf[1] - SP (R14)
   jmp_buf[2] - R13
   jmp_buf[3] - R12
   jmp_buf[4] - R11
   jmp_buf[5] - R10
   jmp_buf[6] - R9
   jmp_buf[7] - R8
   jmp_buf[8] - R7
   jmp_buf[9] - R6
   jmp_buf[10] - R5
   jmp_buf[11] - R4
   jmp_buf[12] - R3
   jmp_buf[13] - R2
   jmp_buf[14] - R1
   jmp_buf[15] - R0
   jmp_buf[16] - SRP
   jmp_buf[17] - CCR
   */

typedef unsigned long int jmp_buf[18];

/* You better make sure this doesnt change stack pointer before
   returning, if you [cd]are to change it.   */
__inline__ int
setjmp(jmp_buf buf) {
  int ret;
  __asm__ __volatile__
    ("moveq 1,r9\n\t"
     "movem sp,[%1+1*4]\n\t"
     "move.d LL%=,r9\n\t"
     "move.d r9,[%1]\n\t"
     "move srp,[%1+16*4]\n\t"
     "move ccr,[%1+17*4]\n\t"
     "clear.d r9\n"
     "LL%=:\n\t"
     "move.d r9,%0"
     : "=&r" (ret)		/* output */
     : "r" (buf) :		/* input */
     "r9" );			/* slask */
  return ret;
}
  
__inline__ void 
longjmp(jmp_buf buf, int val) {
  __asm__ __volatile__
    ("move [%0+17*4],ccr\n\t"
     "move [%0+16*4],srp\n\t"
     "test.d %1\n\t"
     "beq 0f\n\t"
     "nop\n\t"
     "move.d %1,[%0+6*4]\n"	/* offset for r9 */
     "0:\n\t"
     "movem [%0],pc"
     : /* no outputs */
     : "r" (buf), "r" (val)); /* input */
}

#ifdef __cplusplus
}
#endif

#endif /* PC_DEBUG */

#endif /* __setjmp_h */
