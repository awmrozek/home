/*!**************************************************************************
*!                                                            
*! FILE NAME  : etrax.h
*!
*! DESCRIPTION: Definitions for the ETRAX Chip.
*!
*! FUNCTIONS  : None.
*! (EXPORTED)
*!
*!---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME             CHANGES
*! ----         ----             -------
*! Feb 01 1994  Kenneth Jonsson  Initial version
*! May 03 1995  Stefan Jonsson   Added testbutton stuff.
*! May 03 1995  Stefan Jonsson   Added LED stuff.
*! May 12 1995  Mart Roomus      Removed FP in/out stuff. Moved to FP_HW.H .
*! Sep 15 1995  H-P Nilsson      uword product_hardware_id declared.
*! Sep 22 1994  Jens Johansson   Changed asm to __asm__.
*! Mar 18 1996  Bernt B          Added RXD_BIT_MASK in serial status register.
*! Mar 25 1996  Willy Sagefalk   Added DRAM_MODE2
*! May 02 1996  Jens Johansson   Added R_SCSI_CONFIG.
*! Oct 30 1996  Fred Jonsson     Added Defines for HW_PC
*! Nov 07 1996  Stefan Jonsson   HW_ETRAX is not defined in all pseudo prods.
*!
*!---------------------------------------------------------------------------
*!
*! (C) Copyright 1995, Axis Communications AB, LUND, SWEDEN
*!
*!**************************************************************************/
/* @(#) etrax.h 1.13 11/07/96 */

/********************** INCLUDE FILES SECTION ******************************/

/********************** CONSTANT AND MACRO SECTION *************************/

#define EPROM0_START 0x00000000                         /* Memory segments */
#define EPROM1_START 0x20000000
#define SRAM_START   0x40000000
#define DRAM0_START  0x60000000
#define DRAM1_START  0x70000000
#define CS1_START    0x80000000
#define CS2_START    0xA0000000
#define ETRAX_IOBASE 0xE0000000

                        /* ETRAX Memory mapped registers and I/O addresses */

#define R_LATE_WS    (ETRAX_IOBASE + 0x00) /* Bus interface mode registers */
#define R_EARLY_WS   (ETRAX_IOBASE + 0x01)
#define R_BUS_MODE   (ETRAX_IOBASE + 0x02)
#define R_DRAM_MODE  (ETRAX_IOBASE + 0x04)
#define R_DRAM_MODE2 (ETRAX_IOBASE + 0x05)
#define R_CLOCK_MODE (ETRAX_IOBASE + 0x06)
#define R_DMA_CONFIG (ETRAX_IOBASE + 0x07)
#define R_END_COUNT  (ETRAX_IOBASE + 0x07)



#define R_IO_STATUS  (ETRAX_IOBASE + 0x10)     /* Interrupt mask registers */

#if defined(SIMULATION) && SIMULATION
#define R_IO_MASKS   (ETRAX_IOBASE + 0x09)
#else
#define R_IO_MASKS   (ETRAX_IOBASE + 0x10)
#endif

#define R_IO_MASKC   (ETRAX_IOBASE + 0x11)
#define   PAR1_INT_BIT      0x01
#define   PAR2_INT_BIT      0x02
#define   SER_IN_INT_BIT    0x04
#define   SER_OUT_INT_BIT   0x08
#define   DMA0_INT_BIT      0x10
#define   DMA1_INT_BIT      0x20
#define   TIMER_INT_BIT     0x40
#define   EXT_INT_BIT       0x80

#define R_ETR_STATUS (ETRAX_IOBASE + 0x12)

#if defined(SIMULATION) && SIMULATION
#define R_ETR_MASKS  (ETRAX_IOBASE + 0x16)
#else
#define R_ETR_MASKS  (ETRAX_IOBASE + 0x12)
#endif

#define R_ETR_MASKC  (ETRAX_IOBASE + 0x13)
#define   ETR_FREQ                 0x80
#define   ETR_LOST_FR_ERROR        0x40
#define   ETR_TOKEN_ERROR          0x20
#define   ETR_ABORT_TRANS          0x10
#define   ETR_LINE_ERROR           0x08
#define   ETR_TVX                  0x04
#define   ETR_TRR                  0x02
#define   ETR_BURST                0x01

#define R_BUF_STATUS (ETRAX_IOBASE + 0x14)

#if defined (SIMULATION) && SIMULATION
#define R_BUF_MASKS  (ETRAX_IOBASE + 0x08)
#else
#define R_BUF_MASKS  (ETRAX_IOBASE + 0x14)
#endif

#define R_BUF_MASKC  (ETRAX_IOBASE + 0x15)
#define   BUF__ABORT_INT           0x08
#define   BUF__BUFFER_FULL         0x04
#define   BUF__PACKET_TR           0x02
#define   BUF__PACKET_REC          0x01

#define R_EXT_MASKS  (ETRAX_IOBASE + 0x16)
#define R_EXT_MASKC  (ETRAX_IOBASE + 0x17)

#define R_MIO_ADDR   (ETRAX_IOBASE + 0x28)                /* MIO registers */
#define R_MIO_MODE   (ETRAX_IOBASE + 0x29)

#define R_P1_DATA_IN  (ETRAX_IOBASE + 0x30)            /* Port 1 registers */
#define R_P1_DATA_OUT (ETRAX_IOBASE + 0x30)
#define R_P1_DIR      (ETRAX_IOBASE + 0x31)

#define R_P2_DATA     (ETRAX_IOBASE + 0x34)            /* Port 2 registers */
#define R_SER_CONNECT (ETRAX_IOBASE + 0x34)
#define R_P2_CONFIG   (ETRAX_IOBASE + 0x35)

#define R_P3_DATA     (ETRAX_IOBASE + 0x38)            /* Port 3 registers */

#define DMA1_END_BIT  0x40
#define DMA0_END_BIT  0x20
#define SRAM_W_BIT    0x10
#define EPROM_W_BIT   0x08
#define IRQ_BIT       0x04
#define PAR_POL_BIT   0x02
#define EX_DREQ_BIT   0x01

#define R_PAR_BACKCH  (ETRAX_IOBASE + 0x40)        /* Parallel 1 registers */
#define R_PAR_DATA    (ETRAX_IOBASE + 0x40)
#define R_PAR_STAT    (ETRAX_IOBASE + 0x41)
#define R_PAR_DSETUP  (ETRAX_IOBASE + 0x41)
#define R_PAR_STROBE  (ETRAX_IOBASE + 0x42)
#define R_PAR_DHOLD   (ETRAX_IOBASE + 0x43)
#define R_PAR_CTRL    (ETRAX_IOBASE + 0x44)
#define R_PAR_CONFIG  (ETRAX_IOBASE + 0x45)

#define R_PCH_DATA    (ETRAX_IOBASE + 0x48)        /* Parallel 2 registers */
#define R_PAR2_DATA   (ETRAX_IOBASE + 0x48)
#define R_PCH_BACKCH  (ETRAX_IOBASE + 0x48)
#define R_PCH_STAT    (ETRAX_IOBASE + 0x49)
#define R_PCH_ACKTIME (ETRAX_IOBASE + 0x49)
#define R_PAR2_STROBE (ETRAX_IOBASE + 0x49)
#define R_PAR2_DHOLD  (ETRAX_IOBASE + 0x4A)
#define R_PAR2_BACKCH (ETRAX_IOBASE + 0x4B)
#define R_PAR2_DSETUP (ETRAX_IOBASE + 0x4B)
#define R_PAR2_CTRL   (ETRAX_IOBASE + 0x4C)
#define R_PCH_CTRL    (ETRAX_IOBASE + 0x4C)
#define R_PAR2_CONFIG (ETRAX_IOBASE + 0x4D)
#define R_PCH_CONFIG  (ETRAX_IOBASE + 0x4D)

#define R_SER1_DIN     (ETRAX_IOBASE + 0x50)    /* Serial port 1 registers */
#define R_SER1_DOUT    (ETRAX_IOBASE + 0x50)
#define R_SER1_STAT    (ETRAX_IOBASE + 0x51)
#define   DAV_BIT_MASK   0x01
#define   DAV_BIT_NUM    0
#define   TRE_BIT_MASK   0x20
#define   TRE_BIT_NUM    5
#define   CTS_BIT_MASK   0x40
#define   CTS_BIT_NUM    6
#define   RXD_BIT_MASK   0x10
#define   RXD_BIT_NUM    4

#define R_SER1_BAUD    (ETRAX_IOBASE + 0x51)
#define R_SER1_IN_CFG  (ETRAX_IOBASE + 0x52)
#define RTS_BIT        0x20
#define R_SER1_OUT_CFG (ETRAX_IOBASE + 0x53)

#define R_TIMER_DATA   (ETRAX_IOBASE + 0x58)            /* Timer registers */
#define R_TIMER_MODE   (ETRAX_IOBASE + 0x59)
#define R_TIMER_INTA   (ETRAX_IOBASE + 0x5A)

#define R_SCSI_CONFIG    (ETRAX_IOBASE + 0x45)     /* Same as R_PAR_CONFIG */
#define R_SCSI_INTR_MASK (ETRAX_IOBASE + 0x30)    /* Same as R_P1_DATA_OUT */

#define R_MA0   (ETRAX_IOBASE + 0x80) /* Ethernet and token-ring registers */
#define R_MA1   (ETRAX_IOBASE + 0x81)
#define R_MA2   (ETRAX_IOBASE + 0x82)
#define R_MA3   (ETRAX_IOBASE + 0x83)
#define R_MA4   (ETRAX_IOBASE + 0x84)
#define R_MA5   (ETRAX_IOBASE + 0x85)

#define R_GA0   (ETRAX_IOBASE + 0x88)
#define R_GA1   (ETRAX_IOBASE + 0x89)
#define R_GA2   (ETRAX_IOBASE + 0x8A)
#define R_GA3   (ETRAX_IOBASE + 0x8B)
#define R_GA4   (ETRAX_IOBASE + 0x8C)
#define R_GA5   (ETRAX_IOBASE + 0x8D)
#define R_GA6   (ETRAX_IOBASE + 0x8E)
#define R_GA7   (ETRAX_IOBASE + 0x8F)

#define STATION_ADDRESS    (R_MA0)
#define GROUP_ADDRESS      (R_GA0)
#define FUNCTIONAL_ADDRESS (R_GA4)

#define R_TR_MODE1     (ETRAX_IOBASE + 0x90)
#define   ETR_TX_TOKEN               0x80
#define   ETR_TX_NO_TOKEN            0x40
#define   ETR_ETHERNET_MODE          0x20
#define   ETR_ETR_MODE               0x10
#define   ETR_TX_FILL                0x08
#define   ETR_SPY_MODE               0x04
#define   ETR_ACTIVATE_OPER          0x02
#define   ETR_A0_STATE               0x01

#define R_TR_MODE2     (ETRAX_IOBASE + 0x91)
#define   ETR_ENABLE_ADDR_RECOG      0x10
#define   ETR_INSERT                 0x08
#define   ETR_NOT_RESET              0x04
#define   ETR_ELASTIC_BUFFER         0x02
#define   ETR_RD_CLR_INT             0x01

#define R_TVX          (ETRAX_IOBASE + 0x92)

#define R_TRR          (ETRAX_IOBASE + 0x93)

#define R_ANALOG       (ETRAX_IOBASE + 0x94)
#define   ETR_CLR_TOK                0x04
#define   ETR_MODE_4                 0x01
#define   ETR_FRAQ                   0x02

#define R_REC_MODE     (ETRAX_IOBASE + 0x98)   /* Buffer handler registers */
#define   REC_RESET                  0x01
#define   REC__C_BUFF_FULL           0x02
#define   REC__C_PACKET_INT          0x04

#if defined(SIMULATION) && SIMULATION
#define R_REC_STATUS   (ETRAX_IOBASE + 0xA0)
#else
#define R_REC_STATUS   (ETRAX_IOBASE + 0x98)
#endif
#define   REC_STATUS_BUF_FULL_BIT    0x02
#define   REC_STATUS_SYNC_BIT        0x04
#define   REC_STATUS_FRAQ_BIT        0x08
#define   REC_STATUS_TOKEN_BIT       0x10

#define R_REC_END      (ETRAX_IOBASE + 0x99)
#define R_REC_POS      (ETRAX_IOBASE + 0x9A)
#define R_RT_SIZE      (ETRAX_IOBASE + 0x9B)

#define R_TR_CMD       (ETRAX_IOBASE + 0x9C)
#define   TR_RESET                   0x01
#define   TR_RETRY                   0x02
#define   TR_NEW_TOKEN               0x04
#define   TR_PACKET_INT              0x08
#define   TR_TRANSMIT_STOP           0x10
#define   TR_CANCEL                  0x20

#if defined(SIMULATION) && SIMULATION
#define R_TR_STATUS    (ETRAX_IOBASE + 0xA1)
#else
#define R_TR_STATUS    (ETRAX_IOBASE + 0x9C)
#endif
#define TRANSMITTING_BIT   0x01
#define ABORT_BIT          0x02

#define R_TR_START     (ETRAX_IOBASE + 0x9D)
#define R_TR_POS       (ETRAX_IOBASE + 0x9E)

                                                     /* DMA control macros */
#ifdef HW_PC

#define START_DMA0()
#define STOP_DMA0()
#define DMA0_STAT()
#define SET_DMA0_ADDR(addr)
#define SET_DMA0_LEN(len)
#define GET_DMA0_ADDR(addr)
#define GET_DMA0_LEN(len)

#define START_DMA1()
#define STOP_DMA1()
#define DMA1_STAT()
#define SET_DMA1_ADDR(addr)
#define SET_DMA1_LEN(len)
#define GET_DMA1_ADDR(addr)
#define GET_DMA1_LEN(len)

#define DMA_DRIVES_PAR_CFG  0x45           /* channel 0 and 1 out, 8 bits */
#define DMA_CONFIG(data)

extern uword product_hardware_id; /* What are we running on REALLY? */

#else

#define START_DMA0()        __asm__ volatile("setf e")
#define STOP_DMA0()         __asm__ volatile("clearf e")
#define DMA0_STAT()         BITTST(*(volatile byte*)R_P3_DATA, DMA0_END_BIT)
#define SET_DMA0_ADDR(addr) __asm__ volatile("move %0, dtp0" : /* no output */ : "g" (addr));
#define SET_DMA0_LEN(len)   __asm__ volatile("move %0, dcr0" : /* no output */ : "g" (len));
#define GET_DMA0_ADDR(addr) __asm__ volatile("move dtp0, %0" : "=g" (addr) : /* no input */);
#define GET_DMA0_LEN(len)   __asm__ volatile("move dcr0, %0" : "=g" (len) : /* no input */);

#define START_DMA1()        __asm__ volatile("setf d")
#define STOP_DMA1()         __asm__ volatile("clearf d")
#define DMA1_STAT()         BITTST(*(volatile byte*)R_P3_DATA, DMA1_END_BIT)
#define SET_DMA1_ADDR(addr) __asm__ volatile("move %0, dtp1" : /* no output */ : "g" (addr));
#define SET_DMA1_LEN(len)   __asm__ volatile("move %0, dcr1" : /* no output */ : "g" (len));
#define GET_DMA1_ADDR(addr) __asm__ volatile("move dtp1, %0" : "=g" (addr) : /* no input */);
#define GET_DMA1_LEN(len)   __asm__ volatile("move dcr1, %0" : "=g" (len) : /* no input */);

#define DMA_DRIVES_PAR_CFG  0x45           /* channel 0 and 1 out, 8 bits */
#define DMA_CONFIG(data)    *((volatile byte *) R_DMA_CONFIG) = (data)

#endif 


#ifndef ASSEMBLER_MACROS_ONLY
/********************** TYPE DEFINITION SECTION ****************************/

/********************** EXPORTED FUNCTION DECLARATION SECTION **************/

/********************** EXPORTED VARIABLE DECLARATION SECTION **************/
extern uword product_hardware_id; /* What are we running on REALLY? */

#endif /* ASSEMBLER_MACROS_ONLY */

/********************** END OF FILE etrax.h ********************************/
