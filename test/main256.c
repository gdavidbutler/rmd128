#include <stdio.h>
#include <stdlib.h>
#include "rmd256.h"

int
main(
  void
){
  rmd256_t *c;
  size_t i;
  unsigned char h[32];
  unsigned char b[64];

  if (!(c = malloc(rmd256tsize())))
    return (1);
  rmd256init(c);
  while ((i = fread(b, 1, sizeof (b), stdin)) == sizeof (b))
    rmd256update(c, b, i);
  if (i > 0)
    rmd256update(c, b, i);
  rmd256final(c, h);
  free(c);
  rmd256hex(h, (char *)b);
  printf("%.64s\n", (char *)b);
  return (0);
}
