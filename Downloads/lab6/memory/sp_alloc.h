/*!**************************************************************************
*!
*!  FILE NAME:      sp_alloc.h
*!
*!  DESCRIPTION:    Storage allocation facility that provides a simple form
*!                  of heap memory management. The older object handler heap
*!                  memory management is emulated with the obj_xxx functions.
*!                  This emulation doesn't include defragmentation.
*!
*!  FUNCTIONS:
*!
*!
*!
*!---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE          NAME              CHANGES
*! ----          ----              -------
*! Feb 23 1996   Willy Sagefalk    Initial
*! May 23 1996   Willy Sagefalk    Added alloc_statistics
*! Oct 29 1996 Fred Jonsson        Changed name from alloc.c
*!
*!        (C) 1996 Axis Communications AB, Lund, Sweden
*!**************************************************************************/

/* @(#) sp_alloc.h 1.5 10/29/96 */

#ifndef sp_alloc_h
#define sp_alloc_h

/*
** memory alignment for heaps
** _HMEMALIGN must be n**2 - 1
*/
#define _HMEMALIGN  (32-1)

#define UHALIGN(x)   {if (x &_HMEMALIGN) x = (x+_HMEMALIGN) & ~(udword)_HMEMALIGN; }
#define ULALIGN(x)   {if (x &_HMEMALIGN) x = (x-_HMEMALIGN) & ~(udword)_HMEMALIGN; }


void  aheap_init(byte *heap, udword size);
void* amalloc(udword size);
void  afree(void *p);
udword acoreleft(void);
udword acoreleft_total(void);
void *acalloc(udword items, udword size);
void *arealloc(void *p,udword size);

void *obj_alloc(udword size);
void *obj_calloc(udword size,byte n);
void *obj_realloc(void *p,udword size);
void *obj_free(void *p);
void *obj_put(void *id, void *dest, void *src, udword size);
void *obj_get(void *id, void *src, void *dest, udword size);
void *obj_open(void *id);
void *obj_lock(void *id);
void *obj_unlock(void *id);
udword obj_size(void *id);
void obj_heap_init(byte *heap, udword size);
udword alloc_statistics(udword min, udword max, udword stats[]);

#endif /* sp_alloc_h */
