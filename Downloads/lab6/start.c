/*!*************************************************************************
*!
*! FILE NAME   : strt_ina.c
*!
*! DESCRIPTION : Start of ETRAX products, C initialization.
*!
*! FUNCTIONS   : startc           (global)
*!               flashled         (local)
*!               mem_compare      (global, debugging only)
*!
*!-------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME             CHANGES
*! ----         ----             -------
*! Oct 17 1995  Jens Johansson   Initial version for CD Server.
*! Oct 20 1995  Jens Johansson   No DISABLE/RESTORE around pb_retrieve.
*! Oct 25 1995  Jens Johansson   Less debug printouts.
*! Nov 30 1995  Sven Ekstrom     Modified startc.
*! May 08 1996  Willy Sagefalk   Modified for HD.
*! May 31 1996  Jens Johansson   Fixed Daisy blink.
*! Jul 17 1996  Jens Johansson   This file for INA.
*! Aug 28 1996  Jens Johansson   Moved scsi enable to scsi module.
*! Sep 25 1996  Jens Johansson   SNMP variables after parameters created.
*!
*!-------------------------------------------------------------------------
*!
*!       (C) 1996   Axis Communications AB, Lund, Sweden
*!
*!**************************************************************************/
/* @(#) start_etrax_ina.c 1.4 09/25/96 */

/*********** INCLUDE FILE SECTION ***********************************/
#include "compiler.h"
#include "sp_alloc.h"
#include "system.h"
#include "osys.h"

/*********** CONSTANT AND MACRO SECTION  ****************************/


#define PACKET_LED 1
#define STATUS_LED 2
#define CD_LED     3

/*********** TYPE DEFINITION SECTION  *******************************/

/*********** FUNCTION DECLARATION SECTION ***************************/

static void flashled(void);

/*********** VARIABLE DECLARATION SECTION ***************************/

extern  byte etext[];  /* end of .text segment */
extern  byte _Sdata[]; /* .data start */
extern  byte edata[];  /* end of .data segment */
extern  byte _Stext[]; /* .text start */
extern  byte _end[];   /* .bss end */
extern void __do_global_ctors();

byte    *__heaptop,
        *__heapbase,
        *__brklvl;

uword   dimm_presence;
udword  dram_size;


/*#**************************************************************************
*#
*# FUNCTION NAME: startc
*#
*# PARAMETERS   : None
*#
*# RETURNS      : Nothing
*#
*# SIDE EFFECTS :
*#
*# DESCRIPTION  : Is called from the startup assembler file of the
*#                box, after initialization of registers, stack pointers etc.
*#                Performs a copy of data from PROM into RAM, then starts
*#                the OS.
*#
*#---------------------------------------------------------------------------
*# HISTORY
*#
*# DATE          NAME             CHANGES
*# ----          ----             -------
*# Jun 12 1995   Jens Johansson   Added 530 fix. After the analog ethernet
*#                                circuit was replaced with the same circuit
*#                                as in the 550r, the reset signal was NOT
*#                                taken from the same pin (P15), but from TXD2
*#                                instead. It is set to low here.
*# Jun 20 1995    Jens Johansson  Hardware is the same now. The above fix
*#                                removed.
*# Nov 30 1995   Sven Ekstrom     Added unconditional printout of product
*#                                name, version, and creation information.
*#
*#**************************************************************************/
void startc(void)
{
  flashled();
  ax_printf("\r\n\r\nServer boot.\r\n");
  ax_printf("heapbase %08x heaptop %08x heapsize %08x\n",
            __heapbase,__heaptop,__heaptop - __heapbase);

  aheap_init(__heapbase, __heaptop - __heapbase);

  /* Now after we've shown that we're alive, we can let in
     interrupts.  */
  __asm__ ("ei");

  __do_global_ctors();

  ax_printf("\n\nStarting OSYS.\n");
  os_start();
  // will never return here!
} /* startc */


/*#**************************************************************************
*#
*#  FUNCTION NAME : led_on, led_off
*#
*#  PARAMETERS    : none
*#
*#  RETURNS       : nothing
*#
*#  DESCRIPTION   : Flash the LED.
*#
*#************************************************************************#*/
static byte write_out_register_shadow = 0x78;

void led_on(uword ledNumber)
{
  uword led = 4 << ledNumber;  /* convert LED number to bit weight */
  *(VOLATILE byte*)0x80000000 = write_out_register_shadow &= ~led ;
}

void led_off(uword ledNumber)
{
  uword led = 4 << ledNumber;  /* convert LED number to bit weight */
  *(VOLATILE byte*)0x80000000 = write_out_register_shadow |= led ;
}

/*#**************************************************************************
*#
*#  FUNCTION NAME : flashled
*#
*#  PARAMETERS    : none
*#
*#  RETURNS       : nothing
*#
*#  DESCRIPTION   : Flash the LED a few times, just to show that the
*#                  main routine has started, or show running lights,
*#                  depending on how many leds are available in the product...
*#
*#************************************************************************#*/
static void flashled(void)
{
  int i,j;
  for (j =0; j < 4; j++)
  {
    for (i = 0; i < 60000; i++)
    {
      led_on(STATUS_LED);
    }
    for (i = 0; i < 60000; i++)
    {
      led_off(STATUS_LED);
    }
  }


}

/****************** END OF FILE strt_ina.c ****************************/
