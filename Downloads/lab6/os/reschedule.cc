/*****************************************************************************
*! FILE NAME  : reschedule.cc
*!                                                            
*! DESCRIPTION: The implementation of reschedule function to be called from
*!              C code
*!              
*!---------------------------------------------------------------------------
*! HISTORY                                                    
*!                                                            
*! DATE         NAME               CHANGES                       
*! ----         ----               -------                       
*! Jul 27 1995  Stefan S           Initial version
*! Dec  4 1996  Fredrik Svensson   Included compiler.h
*!
*!---------------------------------------------------------------------------
*! (C) Copyright 1995, Axis Technologies AB, LUND, SWEDEN
*****************************************************************************/
// @(#) reschedule.cc 1.2 12/04/96

extern "C"
{
#include "compiler.h"
#include "reschedule.h"
}
#include "threads.hh"

extern "C" void
reschedule()
{
  Thread::current()->reschedule();
}



