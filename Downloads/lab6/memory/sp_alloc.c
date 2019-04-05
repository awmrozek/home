/*!**************************************************************************
*!  
*!  FILE NAME:      sp_alloc.c
*!
*!  DESCRIPTION:    Storage allocation facility that provides a simple form
*!                  of heap memory management. The older object handler heap
*!                  memory management is emulated with the obj_xxx functions.
*!                  This emulation doesn't include defragmentation.
*!
*!  FUNCTIONS:      aheap_init
*!                  amalloc
*!                  afree
*!                  acalloc
*!                  arealloc
*!                  acoreleft
*!                  acoreleft_total
*!
*!                  -- Object handler simulation interface --
*!
*!                  obj_heap_init
*!                  obj_alloc
*!                  obj_size
*!                  obj_calloc
*!                  obj_realloc
*!                  obj_free
*!                  obj_get
*!                  obj_put
*!                  obj_lock
*!                  obj_unlock
*!
*!                  -- Module test --
*!
*!                  alloc_module_test
*!---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! Feb 23 1996  Willy Sagefalk     Initial
*! Apr 15 1996  Willy Sagefalk     Fixed obj_size bug
*! May 23 1996  Willy Sagefalk     Added ifdef debug for some printf's
*! May 23 1996  Willy Sagefalk     Added alloc_statistics
*! Jun 27 1996  Fredrik Norrman    Modified alloc.c so that it can be compiled
*!                                 as a target so that it can be included
*!                                 in the 'files' list even if it's
*!                                 included from another c-file
*! Oct 29 1996 Fred Jonsson        Changed name from alloc.c
*!        (C) 1996 Axis Communications AB, Lund, Sweden
*!**************************************************************************/

/* @(#) sp_alloc.c 1.10 10/29/96 */


/**************************  INCLUDE FILES  ********************************/
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "compiler.h"
#include "system.h"
#include "osys.h"
#include "timr.h"
#include "sp_alloc.h"

/**************************  CONSTANTS  ************************************/

#define BLOCK_ALLOCATED  0x2222
#define BLOCK_FREE       0x3333

#define LDEBUG(X)                  /* Local debug macro */
#define ODEBUG(X)                  /* Local debug macro */
#undef DO_ALLOC_MODULE_TEST       /* Run module test of memhand */

/**************************  TYPE DEFINITIONS  *****************************/
typedef struct free_type     /* Freeblock header */
{
  udword             size;         /* Size of free block including header */
#ifdef DEBUG
  udword             orig_size;    /* Size of block requested by user  */
  udword             time_stamp;   /* System time when block was allocated */
#endif  
  uword              state;        /* State of block "allocated" "free" ... */
  struct free_type  *next;         /* Next free block */
} free_type;

typedef struct obj_header_type
{
  udword size;    /* Real object size */
  bool   locked;
} obj_header_type;

typedef struct heapinfo_type     /* Heap info */
{
  void*        start;         /* Start of heap */
  udword       size;          /* Size of heap */
#ifdef DEBUG
  udword       blocks_allocated;
  udword       bytes_allocated;
#endif  
  free_type    free;          /* 1st entry in heaps freelink */
} heap_info_type;

/**************************  EXTERNALS  ************************************/

/**************************  EXTERNAL FUNCTIONS  ***************************/

/**************************  LOCALS  ***************************************/

#ifdef DO_ALLOC_MODULE_TEST
void alloc_module_test();
#endif
void alloc_print_statistics();
heap_info_type  myheap;

/*#**************************************************************************
*#
*# FUNCTION NAME : aheap_init
*#
*# PARAMETERS    : heap - pointer to heap
*#                 size - size of heap
*#
*# RETURNS       : nothing
*#
*# SIDE EFFECTS  :
*#
*# DESCRIPTION   : Initiates heap
*#
*#--------------------------------------------------------------------------
*#
*# HISTORY
*#
*# DATE          NAME              CHANGES
*# ----          ----              -------
*# Feb 23 1996   Willy Sagefalk    Initial
*#
*#**************************************************************************/

void aheap_init(byte *heap, udword size)
{
  byte *aligned_heap_start;

  /* Align size and start of heap to avoid complexity in allocate and free */
  aligned_heap_start = heap;
  UHALIGN((udword)aligned_heap_start);
  size -= ((udword)aligned_heap_start - (udword)heap);
  ULALIGN(size);
    
  myheap.start      = aligned_heap_start;
  myheap.size       = size;

#ifdef DEBUG  
  myheap.blocks_allocated = 0;
  myheap.bytes_allocated  = 0;
#endif
  
  myheap.free.size  = 0;
  myheap.free.next  = (free_type*)heap;

  myheap.free.next->size  = size;
  myheap.free.next->next  = NULL;
  myheap.free.next->state = BLOCK_FREE;
#ifdef DEBUG  
  myheap.free.next->orig_size = 0;
#endif  
  LDEBUG(printf("Init heap with %u bytes heapbase %08X\n",size,aligned_heap_start));
  
#ifdef DO_ALLOC_MODULE_TEST
  alloc_module_test();
#endif  
}

/*#**************************************************************************
*#
*# FUNCTION NAME : amalloc
*#
*# PARAMETERS    : size - size of block to allocate
*#
*# RETURNS       : nothing
*#
*# SIDE EFFECTS  :
*#
*# DESCRIPTION   : Allocates block with best fit method. Block size is
*#                 aligned to 32 byte boundry
*#
*#--------------------------------------------------------------------------
*#
*# HISTORY
*#
*# DATE          NAME              CHANGES
*# ----          ----              -------
*# Feb 23 1996   Willy Sagefalk    Initial
*#
*#**************************************************************************/

void* amalloc(udword size)
{
  void      *p        = NULL;
  free_type *freeblk  = myheap.free.next;
  free_type *previous = &(myheap.free);
  free_type *best     = NULL;
  free_type *bestp    = NULL; /* best previous */
  uword      loopcount = 0;
#ifdef DEBUG
  udword     orig_size = size;
#endif  
  LDEBUG(ax_printf("Malloc %u bytes \n",size));
  
  /* Add space for block header */
  size += sizeof(free_type);

  /* Align size to boundry */
  UHALIGN(size);

#ifdef DEBUG
  myheap.blocks_allocated++;
  myheap.bytes_allocated += size;
#endif

  LDEBUG(ax_printf("Allocate %u bytes blocks %u tbytes %u",size,myheap.blocks_allocated,myheap.bytes_allocated));

  /* Search for block large enough. Stop searching if block with correct size is found */
  while (freeblk != NULL) 
  {
    loopcount++;
    if (freeblk->size >= size)
    {
      /* Free block is large enough */
      if ((best == NULL)  ||  (freeblk->size < best->size))
      {
        /* Best fit so far */
        best  = freeblk;
        bestp = previous;
      }
    }
    previous = freeblk;
    if (freeblk->size == size)
    {
      break;
    }
    freeblk = freeblk->next;
  }
  if (best != NULL)
  {
    LDEBUG(ax_printf("Best = %u bytes loops %u\n",best->size,loopcount));
    
    /* Decrease block size with size */
    best->size -= size;
    if (best->size < sizeof(free_type))
    {
      /* The remaining block is too small to keep track of. Increase requested size */
      bestp->next = best->next;                  /* Unlink allocated block from free list */
      p = (void*)((byte*)best + sizeof(free_type));
      best->state = BLOCK_ALLOCATED;
      best->size = size + best->size;
    }
    else
    {
      /* Use part of block */
      p = (void*)((byte*)best + best->size);
      ((free_type*)p)->size = size;
      ((free_type*)p)->state = BLOCK_ALLOCATED;
      ((free_type*)p)++; 
    }      
  }
#ifdef DEBUG
  if (p != NULL)
  {
    ((free_type*)p - 1)->orig_size  = orig_size;
    ((free_type*)p - 1)->time_stamp = get_time();
  }
#endif
  return p;
}

/*#**************************************************************************
*#
*# FUNCTION NAME : afree
*#
*# PARAMETERS    : p - pointer to block to free
*#
*# RETURNS       : nothing
*#
*# SIDE EFFECTS  :
*#
*# DESCRIPTION   : Returns block to free list. Concatenate with other free
*#                 blocks if possible.
*#
*#--------------------------------------------------------------------------
*#
*# HISTORY
*#
*# DATE          NAME              CHANGES
*# ----          ----              -------
*# Feb 23 1996   Willy Sagefalk    Initial
*#
*#**************************************************************************/

void afree(void *p)
{
  free_type       *freeblk = &(myheap.free);
  free_type       *allocated_block = NULL;

  /* Don't care about null pointers */
  if (p == NULL)
  {
    return;
  }
  
  /* Adjust pointer to start of freeblock */
  allocated_block = (free_type *)((byte*)p - sizeof(free_type));

  LDEBUG(ax_printf("Free pointer = %08X size = %u\n",p,allocated_block->size));
  
  /* Check if block is allocated to avoid corruption of list of free blocks */ 
  if (allocated_block->state != BLOCK_ALLOCATED)
  {
    /*ax_printf("Block not allocated !!!!!!!!!! state %04x size %u\n",allocated_block->state,allocated_block->size);*/
    return;
  }
  else
  {
    allocated_block->state     = BLOCK_FREE;
#ifdef DEBUG
    allocated_block->orig_size = 0;
#endif    
  }
  
#ifdef DEBUG
  myheap.blocks_allocated--;
  myheap.bytes_allocated -= allocated_block->size;
#endif

  LDEBUG(ax_printf("Free %u bytes state = %04X \n",allocated_block->size,allocated_block->state));
  
  if (allocated_block->size > 0)
  {
    /* Search for insertion point */
    while ((freeblk->next != NULL) && ((void*)allocated_block > (void*)freeblk->next))
    {
      LDEBUG(ax_printf("Free block pointer = %08X size = %u\n",freeblk,freeblk->size));
      freeblk = freeblk->next;
    }

    if (freeblk != NULL)
    {
      LDEBUG(ax_printf("Free block pointer = %08X size = %u\n",freeblk,freeblk->size));
      /*
      ** Found insertion point.
      ** Insert between freeblk & freeblk->next, possibly concatenate.
      */

      if ((void*)((byte*)allocated_block + allocated_block->size) == (void*)freeblk->next)
      {
        LDEBUG(ax_printf("Concatenate with high block\n"));
        /* Concatenate with high block */
        allocated_block->next = freeblk->next->next;
        allocated_block->size += freeblk->next->size;
      }
      else
      {
        LDEBUG(ax_printf("Do not concatenate with high block\n"));
        allocated_block->next = freeblk->next;
      }

      if ((byte*)allocated_block == ((byte*)freeblk + freeblk->size))
      {
        LDEBUG(ax_printf("Concatenate with low block\n"));
        
        /* Concatenate with low block */
        freeblk->size += allocated_block->size;
        freeblk->next = allocated_block->next;
      }
      else
      {
        LDEBUG(ax_printf("Do not concatenate with low block\n"));
        
        freeblk->next = allocated_block;
      }
    }
  }  /* If valid ptr */
}

/*#**************************************************************************
*#
*# FUNCTION NAME : acoreleft
*#
*# PARAMETERS    : 
*#
*# RETURNS       : Returns size of largest block in free list
*#
*# SIDE EFFECTS  :
*#
*# DESCRIPTION   : 
*#
*#--------------------------------------------------------------------------
*#
*# HISTORY
*#
*# DATE          NAME              CHANGES
*# ----          ----              -------
*# Feb 23 1996   Willy Sagefalk    Initial
*#
*#**************************************************************************/

udword acoreleft(void)
{
  udword maxsize = 0;
  free_type *freeblk  = myheap.free.next;

  /*alloc_print_statistics();*/

  /* Find largest block */
  while (freeblk != NULL)
  {
    if (freeblk->size > maxsize)
    {
      maxsize = freeblk->size - sizeof(free_type);
    }    
    freeblk = freeblk->next;
  }
  return maxsize;
}

/*#**************************************************************************
*#
*# FUNCTION NAME : acoreleft_total
*#
*# PARAMETERS    : 
*#
*# RETURNS       : Returns sum of all blocks in free list
*#
*# SIDE EFFECTS  :
*#
*# DESCRIPTION   : 
*#
*#--------------------------------------------------------------------------
*#
*# HISTORY
*#
*# DATE          NAME              CHANGES
*# ----          ----              -------
*# Feb 23 1996   Willy Sagefalk    Initial
*#
*#**************************************************************************/

udword acoreleft_total(void)
{
  udword size = 0;
  free_type *freeblk  = myheap.free.next;

  /* Add all free block sizes minus block header */
  while (freeblk != NULL)
  {
    size += freeblk->size - sizeof(free_type);
    freeblk = freeblk->next;
  }
  return size;
}

/*#**************************************************************************
*#
*# FUNCTION NAME : arealloc
*#
*# PARAMETERS    : p    - Pointer to allocated block
*#                 size - Increase block to this size
*#
*# RETURNS       : pointer to reallocated block
*#
*# SIDE EFFECTS  :
*#
*# DESCRIPTION   : 
*#
*#--------------------------------------------------------------------------
*#
*# HISTORY
*#
*# DATE          NAME              CHANGES
*# ----          ----              -------
*# Feb 23 1996   Willy Sagefalk    Initial
*#
*#**************************************************************************/

void *arealloc(void *p,udword size)
{
  free_type       *freeblk      = &(myheap.free);
  free_type       *prevblk      = NULL;
  free_type       *pruned_block = NULL;
  free_type       *allocated_block = NULL;
  udword           oldsize;

  LDEBUG(ax_printf("Realloc pointer = %08X size = %u\n",p,size));

  /* Adjust pointer to start of freeblock */
  allocated_block = (free_type*)((byte*)p - sizeof(free_type));
  
  /* Get size of block to realloc */
  oldsize = allocated_block->size;

  /* Temporary  solution */
  {
    void *p1;
    p1 = amalloc(size);
    memcpy(p1,p,MIN(oldsize,size));
    afree(p);
    p = p1;
    return p;
  }


  /* Add space for header */
  size += sizeof(free_type);

  /* Align size to boundry */
  UHALIGN(size);

  /* Decrease block size */
  if (size <= oldsize)
  {
    if ((oldsize - size) <= sizeof(free_type))
    {
      /* Cannot resize block. Differens is to small */    
    }
    else
    {
      /* Split block in two and free the high block in the normal way */
      free_type *free_block;
      
      allocated_block->size = size;
      free_block = (free_type*)((byte*)allocated_block + size);
      free_block->size   = oldsize - size;
      free_block->state  = BLOCK_ALLOCATED;
      afree((void*)((byte*)free_block + sizeof(free_type)));
    }
    return p;
  }
  else
  {
    /* Find first freeblk after allocated one. */
    while (freeblk->next != NULL && (void*)allocated_block  > (void*)freeblk->next)
    {
      prevblk = freeblk;
      freeblk = freeblk->next;
    }

    /* Is the freeblk directly after the allocated one and is it big enough ?
       Header for freeblock is also used and added to size */
  /*  if ( (((byte*)allocated_block + oldsize) == (byte*)freeblk->next) &&
           ((freeblk->size + sizeof(free_type)) >= (size - oldsize)) )*/
    if (FALSE)
    {
      /* resize allocated block */
      if ((freeblk->size + sizeof(free_type)) == (size - oldsize))
      {
        /* Concatenate with high block */
        prevblk->next  = freeblk->next->next;
        prevblk->size += freeblk->next->size;

       /* Adjust size of allocated block. Must include header */
        allocated_block->size += freeblk->size; 
        allocated_block->size += sizeof(free_type); 
      }
      else
      {
         if ((freeblk->size - (size - oldsize)) > sizeof(free_type))
         {
           /* Use part of block. Call rest of block pruned_block */
           pruned_block = (free_type*)((byte*)freeblk + freeblk->size);
           pruned_block->size  = freeblk->size - (size - oldsize);
           pruned_block->state = BLOCK_FREE;

           /* Link pruned block to free list */
           pruned_block->next = freeblk->next;
           prevblk->next      = pruned_block;
         }
         else
         {
           /* Concatenate with high block */
           prevblk->next  = freeblk->next->next;
           prevblk->size += freeblk->next->size;
         }           
      }    
    }
    else
    {
      /* Cannot resize block so try to allocate a new one */
      void *p1;
      p1 = amalloc(size);
      memcpy(p1,p,oldsize);
      afree(p);
      p = p1;
    }
  }
  return p;
}

/*#**************************************************************************
*#
*# FUNCTION NAME : acalloc
*#
*# PARAMETERS    : size - 
*#
*# RETURNS       : nothing
*#
*# SIDE EFFECTS  :
*#
*# DESCRIPTION   : 
*#
*#--------------------------------------------------------------------------
*#
*# HISTORY
*#
*# DATE          NAME              CHANGES
*# ----          ----              -------
*# Feb 23 1996   Willy Sagefalk    Initial
*#
*#**************************************************************************/

void *acalloc(udword items, udword size)
{
  void *p;  
  p = amalloc(size*items);
  memset(p,size*items,0);
  return p;
}

#ifdef DEBUG
/*#**************************************************************************
*#
*# FUNCTION NAME : alloc_statistics
*#
*# PARAMETERS    : min - minimum block size to look at
*#                 max - maximum block size to look at
*#                 stats - udword[] where statistics are gathered.
*#                 Must have place for max-min+1 udwords.
*#
*# RETURNS       : Zero if no LARGER memory blocks than
*#                 max are allocated. 
*#                 The size of the next larger block if there are more.
*#
*# SIDE EFFECTS  : 
*#
*# DESCRIPTION   : Stores the number of allocated memory blocks of
*#                 that size in stats[that_size-min]
*#
*#--------------------------------------------------------------------------
*#
*# HISTORY
*#
*# DATE          NAME              CHANGES
*# ----          ----              -------
*# May 23 1996   H-P Nilsson       Initial
*# May 23 1996   Willy Sagefalk    Rewritten
*#**************************************************************************/
udword
alloc_statistics(udword min, udword max, udword stats[])
{    
  free_type  *p_header    = myheap.start;
  udword      next_larger = 0;
  uword       i;
  
  LDEBUG(ax_printf("Min = %u Max = %u \n",min,max));

  /* Reset return statistics area  */
  for (i=0; i <= (max-min);i++)
  {
    stats[i] = 0;
  }

  /* Walk through whole heap */
  while ((byte*)p_header < ((byte*)myheap.start + myheap.size))
  {
    switch (p_header->state)  
    {
      case BLOCK_ALLOCATED:
        LDEBUG(ax_printf("Allocated block osize = %u size = %u addr %08X\n",
                  p_header->orig_size,p_header->size,(byte*)p_header));
        
        if ((p_header->orig_size >= min) && (p_header->orig_size <= max))
        {
          stats[p_header->orig_size - min]++;
        }
        if (p_header->orig_size > max)
        {
          if ((next_larger == 0) || (p_header->orig_size < next_larger))
          {
            next_larger = p_header->orig_size;
          }
        }
        break;
      case BLOCK_FREE:
        LDEBUG(ax_printf("Free block size = %u\n",p_header->size));
        break;
      default :
        LDEBUG(ax_printf("Unknown state %04x\n",p_header->state));
        /* Unknown state */
        assert(0);
        break;
    }
    /* Goto next header */
    p_header = (free_type*)((byte*)p_header + p_header->size);
  }

  return next_larger;
}
#endif /* DEBUG */

/*#**************************************************************************
*#
*# FUNCTION NAME : obj_xxx
*#
*# PARAMETERS    : 
*#
*# RETURNS       : 
*#
*# SIDE EFFECTS  :
*#
*# DESCRIPTION   : Object handler interface to malloc and free
*#
*#--------------------------------------------------------------------------
*#
*# HISTORY
*#
*# DATE          NAME              CHANGES
*# ----          ----              -------
*# Feb 23 1996   Willy Sagefalk    Initial
*#
*#**************************************************************************/

void obj_heap_init(byte *heap, udword size)
{
  aheap_init(heap, size);
}

void *obj_alloc(udword size)
{
  void            *retval;
  obj_header_type *obj_header;
  
  obj_header = (obj_header_type*)amalloc(size + sizeof(obj_header_type));
  
  if (obj_header != NULL)
  {
    obj_header->size = size;
    retval = (void*)((byte*)obj_header + sizeof(obj_header_type));
  }
  else
  {
    retval = NULL;
  }
  ODEBUG(ax_printf("OBJ alloc size %u pointer %08X %08X\n",size,retval,obj_header));
  return retval;
}

void *obj_calloc(udword size,byte n)
{
  obj_header_type *obj_header;
  void            *retval;
  
  obj_header = (obj_header_type*)acalloc(n,size + sizeof(obj_header_type));
  if (obj_header != NULL)
  {
    obj_header->size = size;
    retval = (void*)((byte*)obj_header + sizeof(obj_header_type));
  }
  else
  {
    retval = NULL;
  }
  
  ODEBUG(ax_printf("OBJ calloc\n"));
  return retval;
}

void *obj_realloc(void *p,udword size)
{
  obj_header_type *obj_header;
  void            *retval;
  
  obj_header = (obj_header_type*)((byte*)p - sizeof(obj_header_type));
  
  ODEBUG(ax_printf("OBJ realloc %08X %08X\n",p,obj_header));
  
  obj_header = (obj_header_type*)arealloc((void*)obj_header,size + sizeof(obj_header_type));
  
  if (obj_header != NULL)
  {
    obj_header->size = size;
    retval = (void*)((byte*)obj_header + sizeof(obj_header_type));
  }
  else
  {
    retval = NULL;
  }
  ODEBUG(ax_printf("OBJ realloc %08X\n",p));
  return retval;
}

void *obj_free(void *p)
{
  ODEBUG(ax_printf("OBJ free %08X\n",p));
  afree((void*)((byte*)p - sizeof(obj_header_type)));
  return p;
}

void *obj_put(void *id, void *dest, void *src, udword size)
{
  ODEBUG(ax_printf("OBJ put id = %08X size = %u src = %08X dest = %08X %02X \n",id,size,src,dest,*((byte*)id)));
  memcpy(dest,src,size);
  return id;
}

void *obj_get(void *id, void *src, void *dest, udword size)
{
  ODEBUG(ax_printf("OBJ get id = %08X size = %u src = %08X dst = %08X %02X \n",id,size,src,dest,*((byte*)id)));
  memcpy(dest,src,size);
  return id;
}

void *obj_open(void *id)
{
  ODEBUG(ax_printf("OBJ open\n"));
  return id;
}

void *obj_lock(void *id)
{
  ODEBUG(ax_printf("OBJ lock %08X\n",id));
  return id;
}

void *obj_unlock(void *id)
{
  ODEBUG(ax_printf("OBJ unlock %08X\n",id));
  return id;
}

udword obj_size(void *id)
{
  obj_header_type *obj_header;
  ODEBUG(ax_printf("OBJ size %08X\n",id));

  obj_header = (obj_header_type*)((byte*)id - sizeof(obj_header_type));
  
  return obj_header->size;
}

/*#**************************************************************************
*#
*# FUNCTION NAME : alloc_print_statistics
*#
*# PARAMETERS    : 
*#
*# RETURNS       : 
*#
*# SIDE EFFECTS  :
*#
*# DESCRIPTION   : 
*#
*#--------------------------------------------------------------------------
*#
*# HISTORY
*#
*# DATE          NAME              CHANGES
*# ----          ----              -------
*# Apr 04 1996   Willy Sagefalk    Initial
*#
*#**************************************************************************/

void alloc_print_statistics()
{
  free_type *freeblk  = myheap.free.next;
  free_type *prevblk  = NULL;
  byte *temp;
  byte *lastend = 0;
  
  /* Search for block large enough. Stop searching if block with correct size is found */
  while (freeblk != NULL) 
  {
    prevblk = freeblk;
    freeblk = freeblk->next;
    temp = (byte*)freeblk;
    temp += freeblk->size;
    /*ax_printf("Free blk size %08u start %08X end %08X space %08X\n",freeblk->size,freeblk,temp,temp-lastend);*/
    lastend = temp;
  }
}
