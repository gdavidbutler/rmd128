#include <stdio.h>
#include <stdlib.h>
#include "rmd128.h"

int
main(
  void
){
  rmd128_t *c;
  size_t i;
  unsigned char h[16];
  unsigned char b[32];

  if (!(c = malloc(rmd128tsize())))
    return (1);
  rmd128init(c);
  while ((i = fread(b, 1, sizeof (b), stdin)) == sizeof (b))
    rmd128update(c, b, i);
  if (i > 0)
    rmd128update(c, b, i);
  rmd128final(c, h);
  free(c);
  rmd128hex(h, (char *)b);
  printf("%.32s\n", (char *)b);
  return (0);
}
