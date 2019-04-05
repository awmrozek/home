;!**************************************************************************
;!    
;! FILE NAME   : boot_ina.s
;!
;! DESCRIPTION : Start of ETRAX CD-ROM Server (Assembler part)
;!
;!--------------------------------------------------------------------------
;!
;!       (C) 1996  Axis Communications AB, Lund, Sweden
;!
;!**************************************************************************

;********** INCLUDE FILE SECTION ***********************************
#define ASSEMBLER_MACROS_ONLY
#include "etrax.h"

;********** CONSTANT AND MACRO SECTION *****************************

; Set this to 1 if serial output should use 19200 baud instead of 38400
#define DEBUG_SER_AT_19200 0
	
.data

  .set   SYSTEM_ROM_SIZE,     0x100000 ; 1 Mbyte PROM
  .set   SRAM_SIZE,           0x10000
  .set   DRAM_SIZE,           0x200000
  .set   RXTX_BUF_SIZE,       0xa000
  .set   SYS_STACK_SIZE,      4000     ; System stack used by OSYS
  .set   EARLY_WAITSTATES,    0x00     ; No early waitstates
  .set   INITIAL_CLOCK_MODE,  0x7c     ; Sync clock mode, Ethernet 40MHz
  .set   RUNTIME_CLOCK_MODE,  0x78     ; Real clock mode, Ethernet 40MHz
  .set   SER_CONNECT,         0xfa     ; Use serial port 1 pins,
                                       ; bit 7 controls TERM_ENABLE_

;********** VARIABLE DECLARATION SECTION ***************************

  .lcomm _ram_low, 4                    ; Variable space in .bss
  .lcomm _ram_high, 4                   ; Variable space in .bss
  .lcomm _eprom_size, 4 
  .comm _product_hardware_id, 2         ; Variable space in .bss
  .comm _write_out_register_version, 2  ; Variable space in .bss
  .comm _enable_process_swapping, 2     ; True when MAC allows proc swap.
                                        ; Only uses one byte, but keep 
                                        ; addresses word-aligned.

; Add 32 32-byte-aligned bytes in a section placed after all other
; read-only-data, to work around a bug in cburn / cris-burn /
; axis-binstrip in that the read-only data isn't padded up to the
; alignment of .data, when stitching initialized data after
; read-only-data.

  .section .rodata1
  .p2align 5
  .fill 32,1,0

; We need a symbol at the start of the .eh_frame section.  This file
; is a suitable place as it must be linked in first.  No other files
; have ordering requirements.

  .section .eh_frame
  .globl ___EH_FRAME_START
___EH_FRAME_START:

; We also need to terminate it, which as it happens (as the default
; linker script is laid out) we can do by adding a 0 at the top of the
; .ctors section (which is traversed in the opposite direction).

  .section .ctors
  .dword 0

;********** EXTERNALS **********************************************

  .text

  .globl   start                  ; reset point; required by linker
  .globl   _master_reset          ; When all else fails...

  .globl   _get_ram_size          ; Returns size of RAM.
  .globl   _get_eprom_size        ; Returns size of EPROM.
  .globl   _eprom_csum            ; Performs an EPROM checksum.
  .globl   tx_debug_char
  .globl   tx_debug_string
  .globl   tx_debug_value
  .globl   rx_debug_char

  .globl   io_intr
  .globl   io_interrupt
  .globl   spur_intr
  .globl   return_from_intr
  .globl   int_nmi                ; interface variables for UMON
  .globl   after_nmi

  .globl   display_error

  .globl   dimm_presence
  .globl   dram_size 

;********** TYPE DEFINITION SECTION ********************************

;********** FUNCTION DECLARATION SECTION ***************************

;********** MACRO DEFINITIONS **************************************

;********** PROGRAM SECTION ****************************************

  .text

  .space 2,0         ; Dummy bus width configuration word, not used
  ba start           ; Jump to init code after interrupt vectors
;  nop                ; Delay slot
  di                 ; MC 2015-02-02
                     ;
                     ; Start of internal interrupt vector table
                     ; The interrupt vector nr looks like this:
                     ;   0b00100ieb
                     ; i is set when there is an I/O port intr
                     ; e is set when there is a network intr
                     ; b is set when there is a buf handler intr (right)
                     ; 
                     ; The combination of i,e and b tells if there
                     ; is one or several interrupts generated at
                     ; the same time. 
  .org 0x80
intr_vector_base:               ;                           interrupt type:
  .dword spur_intr2             ; interrupt vector nr 0x20  no        
  .dword buf_intr               ;                     0x21  buf       
  .dword etr_intr               ;                     0x22  etr       
  .dword buf_intr               ;                     0x23  buf + etr 
  .dword io_intr2               ;                     0x24  io        
  .dword io_intr2               ;                     0x25  io  + buf 
  .dword io_intr2               ;                     0x26  io  + etr 
  .dword io_intr2               ;                     0x27  io  + etr + buf
                                ; Fill out rest of interrupt vector table 
                                ; up to address 0x3ff, if external interrupts
                                ; are used.
                                ;
                                ; This is where we start after reset
  .align 1                      ;
start:
_master_reset:
  move.b 0x00, r0               ; Reset network interface
  move.b r0, [R_TR_MODE2]
  move.b 0x01, r0               ; Reset receive and transmit buffer handler
  move.b r0, [R_REC_MODE]
  move.b r0, [R_TR_CMD]

;
; Set Waitstates 
;
  move.b 0x54, r0               ; 1 Waitstate for flash
  move.b r0, [R_LATE_WS]
  move.b EARLY_WAITSTATES, r0
  move.b r0, [R_EARLY_WS]       ; Zero early waitstates.
;
; Read HW Id and store read in buffer in r0
;
  move.b 0x00, r0                ; Disable SCSI
  move.b r0, [0x80000000]        ;
  move.b r0, [0x40140000]        ;

  move.b 0x00,r0                 ; DRAM Mode register
  move.d DRAM_SIZE, r13          ; DRAM size   
  move.b 0xB4, r1                ; Etrax DRAM mode to 3 and 1 RAS cycle waitstate 16 bit bus width

  move.b r0, [0x40040000]        ; Set DRAM Mode register

  move.b 0x01, r0                ; Use Dual CAS DRAM
  move.b r0, [R_DRAM_MODE2]      ;  

  move.b r1, [R_DRAM_MODE]       ; Set Etrax DRAM mode  


;
; Set Etrax Clock Mode
;
  move.b 0x7c, r0               ; Prescaler 0-40Mhz, Divide by 6
  move.b r0, [R_CLOCK_MODE]     ; Sync mode clock and Ethernet.
  move.d 15, r0                 ; Must wait 60 cycles for this to
timer_wait1:                    ; work in the verilog simulator.
  nop
  subq 1, r0
  bne timer_wait1
  nop                           ; Delay slot.
  move.b 0x78, r0
  move.b r0, [R_CLOCK_MODE]     ; Now set the real clock mode to 
                                ; 40 MHz clock and Ethernet.
;
; Set Up Serial Port
;
#if DEBUG_SER_AT_19200
  move.b 0x66, r0
#else
  move.b 0x77, r0
#endif
  move.b r0, [R_SER1_BAUD]      ; Serial port baud rate. 38400 in and out.
  move.b 0x40, r0
  move.b r0, [R_SER1_IN_CFG]    ; 8 bits, no parity, RTS_ low
  move.b 0x50,r0
  move.b r0, [R_SER1_OUT_CFG]   ; 8 bits, no parity, 2 stop bits, no auto CTS_
  move.b SER_CONNECT, r0
  move.b r0, [R_SER_CONNECT]    ; do CONNECT after CFG to avoid clutter on
                                ; the pins.
;
  move.b 192, r0
  move.b r0, [R_TIMER_DATA]     ; 192 timer ticks per timer interrupt.
                                ;
  move.d 10, r0                 ; Must wait 40 cycles before
timer_wait2:                    ; starting timer and baud rate.
  nop
  subq 1, r0
  bne timer_wait2
  nop                           ; Delay slot.
;
  move.b 0x2f, r0
  move.b r0, [R_TIMER_MODE]     ; 19200 Hz timer tick, en timer and baud rate.
                                ; 192/19200 => 10ms timer interrupt.
  move.b 0x06, r0   ;
  move.b r0, [R_BUS_MODE]       ; Max CPU speed. 13us refresh.
                                ;
  move.b 0xff, r0               ;
  move.b r0, [R_IO_MASKC]       ; Disable all I/O interrupts.
  move.b r0, [R_ETR_MASKC]      ; Disable all etr interrupts.
  move.b r0, [R_BUF_MASKC]      ; Disable all buf interrupts.
  move.b r0, [R_EXT_MASKC]      ; Disable external interrupts.
  move.b 0x00, r0               ;
  move.b r0, [R_SCSI_CONFIG]    ; Disable SCSI
  move.b r0, [R_SCSI_INTR_MASK] ; Disable SCSI interrupts.
  
  move.b 0x1f, r0               ; DMA configuration:
                                ; chan 0:parallel input, EX_DACK active low,
                                ; EX_DREQ active high, chan 1:external I/O,
                                ; 16 bits, output
  move.b r0, [R_DMA_CONFIG]

  move.b 0x70, r0               ; Network ON
  move.b r0, [0x80000000]           ;
  move.d 0x200000, r0            ; leave them on for a while...
led_loop9:
  subq 1, r0
  bne led_loop9
  nop

; Test and clear SRAM
  move.d 0x40000000, r1              ; RAM low
  move.d 0x40000000+SRAM_SIZE, r2    ; RAM hi:

; RAM is ok. Now clear RAM, and return

  move.d r1, r10                ; RAM low
ram_loop:
  clear.d [r10]                 ; clear four bytes
  addq 4, r10                   ; step up four bytes
  cmp.d r10, r2                 ; reached RAM hi?
  bne ram_loop                  ; no, clear some more
  nop

  move.d 0x60000000, r1              ; RAM low 
  move.d 0x60000000, r2             
  add.d  r13,r2                     ; Calculate RAM high

; RAM is ok. Now clear RAM, and return

  move.d r1, r10                ; RAM low
ram_loop1:
  clear.d [r10]                 ; clear four bytes
  addq 4, r10                   ; step up four bytes
  cmp.d r10, r2                 ; reached RAM hi?
  bne ram_loop1                 ; no, clear some more
  nop

  move.d r13, r5
  move.w r14, r6
  move.d r2, sp

;
; Initiate initiated variables.
;
  move.d __Sdata,r10
  move.d __Etext,r11
  move.d __Edata,r12            ; You cannot "move.d _edata-__Sdata,r12"
  sub.d __Sdata,r12             ; because as+ld has got a hole in the head.
  jsr _memcpy

  move.d r5, [_dram_size]
  move.w r6, [_dimm_presence]

  move.d r0, [_eprom_size]           ;
  move.d r1, [_ram_low]              ;
  move.d r2, [_ram_high]             ;
  move.w r4, [_write_out_register_version];  Store wout version                                     ;

;
; Adjust for rx/tx buffers at end of ram
; 
; Do not forget to change link.def.$product, eth_etr.c and/or etr_addr.h
; if you change this.
;
  move.d [_ram_high], r0
  sub.d SYS_STACK_SIZE, r0      ; Allocate space for system stack.

                                ; Initiate heapbase and heaptop.
                                ; You may have to change this if you
                                ; want to have the heap at other place
                                ; than where stack/normal variables/SRAM is.
  move.d r0,[___heaptop]

  move.d __Ebss,r0              ; End of zero-initialized variables
                                ; (total end of variable area)
  move.d r0,[___heapbase]
  move.d r0,[___brklvl]         ; Current low of used dynamic variables.

  moveq  1, r0                  ; enable proc swap (MAC allows stack switch)
  move.b r0, [_enable_process_swapping]
;
; Enable intr and jump to the C start up function. It will never return.
;
  move.b 0xc0, r0               ; Enable timer and ext interrupt
  move.b r0, [R_IO_MASKS]       ;
  move intr_vector_base,ibr     ; Tell CRIS where the int vectors are
; Interrupts are enabled in startc.
  jump _startc                  ; C start up in strt_etr.c

;
; Note that the following constructs take advantage of the fact that
; the jump instruction turns off interrupts until after the next instruction.
;  All this because of slight braindamage in the linker or assembler.
; See sys_650.s for discussion.

;# 
;# Spurious interrupt.
;#
spur_intr2:
  jump spur_intr
;#
;# This should never happen, but one never knows.
;#
spur_intr:
  reti                          ; Interrupt without cause
  nop

;#
;# Decode buffer handler interrupts
;#
buf_intr:
  push    irp
  push    srp
  push    ccr                   ; Save ccr with interrupts enabled,
  di                            ; and disable.
  subq    56, sp                ; Reserve space for r0-r13
  movem   r13, [sp]             ; and save them.
  jump os_other_int_buf
  nop
;

;#
;# Token ring interrupts, not used in ethernet mode.
;#
;# Note that all token ring interrupts are cleared at
;# the same time, i.e. they all have to be serviced here.
;#
etr_intr:
  reti
  nop

;# 
;# I/O interrupt.
;#
io_intr2:
  jump io_intr

;#
;# Decode and service I/O interrupts
;# Service most important interrupts first.
;#
io_intr:
  push irp
  push srp
  push ccr                      ; Save ccr with interrupts enabled,
  di                            ; and disable.
  subq 56, sp                   ; Reserve space for r0-r13
  movem r13, [sp]               ; and save them.
  move.b [R_IO_STATUS], r13     ; Read io interrupt status

  btstq 6, r13                  ; Timer interrupt
  bmi timer_intr                ; (bit 6 copied to N flag)
  nop                           ; Delay slot

  btstq 2, r13                  ; Serial 1 input interrupt
  bpl not_ser1_in_intr
  nop
  move.b [R_SER1_DIN], r13

;  move.b r13, [R_SER1_DOUT]     ; Echo the key.

  cmp.b 0x75, r13               ; If key pressed = 'u',
  bne return_from_intr
  nop
;  jump umon_int                 ; jump to monitor here!

not_ser1_in_intr:

  btstq 1, r13                  ; Par2 / pch interrupt
  bmi return_from_intr
  nop                           ; Delay slot

  btstq 3, r13                  ; Serial 1 output interrupt
  bmi return_from_intr
  nop                           ; Delay slot

  btstq 4, r13                  ; DMA channel 0 interrupt
  bmi return_from_intr
  nop                           ; Delay slot

  btstq 5, r13                  ; DMA channel 1 interrupt
  bmi return_from_intr          ; 
  nop                           ; Delay slot

  btstq 7, r13                  ; External interrupt
  bmi return_from_intr
  nop                           ; Delay slot

  jump os_other_int_io          ; Switch stack
                
;#
;# Called from os_other_int_io
;# Service most important interrupts first.
;#
io_interrupt:   
  push srp                      ; Save return address
  move.b [R_IO_STATUS], r13     ; Read io interrupt status
        
  btstq 0, r13                  ; SCSI interrupt
  bmi scsi_intr                 ; 
  nop                           ; Delay slot

  ret                           ; This should never happen.
  nop                           ; Delay slot

;#
;# Jump here after the interrupt service routine
;#
return_from_intr:
  movem [sp+], r13              ; Restore r0-r13
  pop ccr                       ; Restore int flag to original status
  pop srp
  jump [sp+]                    ; Ints will be enabled after this instr

;#
;# Timer interrupt
;#
timer_intr:
  move    irp,r0                ;
;  move.d  r0,[__taskpc]         ;

  clear.b [R_TIMER_INTA]        ; Acknowledge timer interrupt
  jsr _timer_interrupt          ; C interrupt routine
  jump osys_timer_int           ; Exit through OSYS

;#
;# External I/O interrupt (from SCSI controller)
;#
scsi_intr:
;  jsr _controllerInt            ; C interrupt routine
  jump [sp+]                    ; Return to where io_interrupt was called

; ********************** END OF FILE boot_ina.s ****************************
