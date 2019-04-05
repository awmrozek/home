/*
** Extra printf routines implemented by xprintf.c which are not found
** in standard libraries are declared here.
**
*! 950310  SJ   Added serial_putch.
*/
/* @(#) xprintf.h 1.2 03/10/95 */
#ifndef XPRINTFH

#define XPRINTFH
#include <stdarg.h>
int xprintf(void (*)(char*,int,void*), void*, const char*, ...);
int vxprintf(void (*)(char*,int,void*), void*, const char*, va_list);
int snprintf(char*, size_t, const char*, ...);
int vsnprintf(char*, size_t, const char*, va_list);
char *mprintf(const char*, ...);
char *vmprintf(const char*, va_list);
int nprintf(const char*, ...);
int vnprintf(const char*, va_list);
word serial_putch(word ch);

/*
** The following structure is used to pass information from vxprintf to
** the user specified conversion routine for user specified conversions.
**
** The user conversion routine should read the next argument using
** the "ap" field of this structure, convert this argument into text
** of "bufsize" or fewer characters, place this text in "*buf", then
** return the length of the text.  Information about the precision and
** values of flags are supplied.  (If no precision is specified, then
** a value of -1 is put in the "precision" field.)
*/
typedef struct s_cvertctrl {
  int bufsize;         /* Size of buf[] */
  char *buf;           /* Start of buffer into which conversion is written */
  int precision;       /* Precision.  -1 if none specified */
  int flag_sharp;      /* True if the "#" flag is present */
  int flag_plus;       /* True if the "+" flag is present */
  int flag_blank;      /* True if the " " flag is present */
  va_list ap;          /* Pointer to the argument to be converted */
  void *arg;           /* Same as 3rd argument to converter() */
} convertctrl;
int converter(char, int (*)(convertctrl*), void *);

#endif
