#include <stdio.h>
#include <stdlib.h>

#include "asprintf.h"

int main()
{
  char *s;
  if (asprintf(&s, "Hello, %d in hex padded to 8 digits is: %08x\n", 15, 15) != -1)
  {
    puts(s);
    insane_free(s);
  }
  asprintf(&s, "Numbers!\n0123456789 0123456789 0123456789 0123456789 0123456789 0123456789 0123456789 0123456789 0123456789 0123456789\n"
               "%s",
               "0123456789 0123456789 0123456789 0123456789 0123456789 0123456789 0123456789 0123456789 0123456789 0123456789\n");
  if (s)
  {
    puts(s);
    insane_free(s);
  }
  return(0);
}
