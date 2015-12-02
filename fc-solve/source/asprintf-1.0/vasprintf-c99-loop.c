/*
Copyright (C) 2014 insane coder (http://insanecoding.blogspot.com/, http://asprintf.insanecoding.org/)

Permission to use, copy, modify, and distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include "asprintf.h"

/*
//Define the following, and this will work with platforms that have a proper vsnprintf(), but lack va_copy and a simple copying of va_list does the trick
#ifndef va_copy
#define va_copy(dest, src) dest = src
#endif
*/

#define insane_free(ptr) { free(ptr); ptr = 0; }

int vasprintf(char **strp, const char *fmt, va_list ap)
{
  int r, size = 128; //128 is a decent starting value
  char *buffer = 0;

  do
  {
    if ((*strp = (char *)realloc(buffer, size)))
    {
      va_list ap2;

      va_copy(ap2, ap);
      r = vsnprintf(*strp, size, fmt, ap2);
      va_end(ap2);

      if (r != -1)
      {
        if ((r >= 0) && (r < INT_MAX)) //If we reach or exceed INT_MAX, we can't use vsnprintf() anymore
        {
          if (r < size) { break; } //Success!
          size = r+1; //+1 for null
          buffer = *strp; //Setup buffer for next iteration
        }
        else //vsnprintf() overflowed its return value
        {
          insane_free(*strp);
          r = -1;
          errno = ENOMEM;
        }
      }
      else { insane_free(*strp); } //This platform has a broken vsnprintf() or some internal error occured
    }
    else
    {
      insane_free(buffer);
      r = -1;
    }
  } while (r != -1);

  return(r);
}
