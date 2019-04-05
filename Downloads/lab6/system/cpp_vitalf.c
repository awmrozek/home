/* @(#) cpp_vitalf.c 1.1 10/18/95 */
/* dummy functions */
#include "compiler.h"
#include "system.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

void *malloc(size_t siz)
{
  return ax_malloc(siz);
}

void free(void *p)
{
  ax_free(p);
}

/* Silence gcc warnings from abort returning. */
__attribute__ ((__noreturn__))
void _exit(int a)
{
  loop:
  goto loop;
}

void abort(void)
{
  _exit (-1);
}

ssize_t write(int a, const void* b, unsigned int c)
{
  /* Maybe call ax_printf here, for a == 1 and a == 2? */
  return c;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  /* Don't use fileno(), it has further dependencies. */
  int fno = stream == stdout ? 1 : (stream == stderr ? 2 : -1);

  return (size_t) write (fno, ptr, nmemb*size);
}

char* getenv(const char* a)
{
  return 0;  
}

/* Because we don't link in the crtfiles that register exception
   tables and handle constructors, we have to roll our own.  We don't
   handle anything put in the .init section, but thankfully all known
   (ELF) gcc versions use .ctors for its constructors.  In the future
   we may be required to handle .init_array as well.  */

typedef void (*fptr) (void);

/* Two linker-defined symbols.  The __elf_ctors_dtors_end symbol is
   brittle: it alludes to containing .dtors as well, but doesn't.  */
extern fptr __elf_ctors_dtors_end[];
extern fptr __ctors[];

/* This one is provided by boot.s.  */
extern const char __EH_FRAME_START[];

struct ehobject
{
  void *p[6];
};

extern void __register_frame_info(const void *, struct ehobject *)
  __attribute__ ((__weak__));

void __do_global_ctors(void)
{
  static char beenhere;
  static struct ehobject ehob;
  fptr *p;

  if (beenhere)
    return;
  beenhere = 1;


  /* First, we register EH info.  No files use any exceptions, though
     the libraries do.  If the user happens to invoke and/or define
     any exception, we should attempt to work or at least fail
     gracefully.  */
  __register_frame_info (__EH_FRAME_START, &ehob);

  /* Working backwards, as expected and done in the gcc code.  */
  for (p = __elf_ctors_dtors_end - 1;
       p != __ctors && *p != (fptr) -1 && *p;
       p--)
    (*p) ();
}
