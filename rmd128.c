/*
 * rmd128 - a small RIPEMD-128 implementation
 * Copyright (C) 2018-2023 G. David Butler <gdb@dbSystems.com>
 *
 * This file is part of rmd128
 *
 * rmd128 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * rmd128 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "rmd128.h"

typedef unsigned int rmd128_bt; /* unsigned 32 bits */
struct rmd128 {
 rmd128_bt h[4];       /* unsigned 32 bits */
 rmd128_bt bh;         /* bytes processed high */
 rmd128_bt bl;         /* bytes processed low */
 unsigned int l;       /* current short data */
 unsigned char d[64];  /* short data */
};

unsigned int
rmd128tsize(
  void
){
  return (sizeof (rmd128_t));
}

void
rmd128init(
  rmd128_t *v
){
  v->h[0] = 0x67452301U;
  v->h[1] = 0xefcdab89U;
  v->h[2] = 0x98badcfeU;
  v->h[3] = 0x10325476U;
  v->bh = v->bl = 0;
  v->l = 0;
}

static void
rmd128mix(
  rmd128_bt h[]
 ,unsigned char x[]
){
  static rmd128_bt k[8] = { /* added constants */
    0x00000000U
   ,0x5a827999U
   ,0x6ed9eba1U
   ,0x8f1bbcdcU

   ,0x50a28be6U
   ,0x5c4dd124U
   ,0x6d703ef3U
   ,0x00000000U
  };
  static unsigned char r[8][16] = { /* message word (from bytes) */
   { 0, 4, 8,12,16,20,24,28,32,36,40,44,48,52,56,60}
  ,{28,16,52, 4,40,24,60,12,48, 0,36,20, 8,56,44,32}
  ,{12,40,56,16,36,60,32, 4, 8,28, 0,24,52,44,20,48}
  ,{ 4,36,44,40, 0,32,48,16,52,12,28,60,56,20,24, 8}

  ,{20,56,28, 0,36, 8,44,16,52,24,60,32, 4,40,12,48}
  ,{24,44,12,28, 0,52,20,40,56,60,32,48,16,36, 4, 8}
  ,{60,20, 4,12,28,56,24,36,44,32,48, 8,40, 0,16,52}
  ,{32,24,16, 4,12,44,60, 0,20,48, 8,52,36,28,40,56}
  };
  static unsigned char s[8][16] = { /* amount to rotate left */
   {11,14,15,12, 5, 8, 7, 9,11,13,14,15, 6, 7, 9, 8}
  ,{ 7, 6, 8,13,11, 9, 7,15, 7,12,15, 9,11, 7,13,12}
  ,{11,13, 6, 7,14, 9,13,15,14, 8,13, 6, 5,12, 7, 5}
  ,{11,12,14,15,14,15, 9, 8, 9,14, 5, 6, 8, 6, 5,12}

  ,{ 8, 9, 9,11,13,15,15, 5, 7, 7, 8,11,14,14,12, 6}
  ,{ 9,13,15, 7,12, 8, 9,11, 7, 7,12, 7, 6,15,13,11}
  ,{ 9, 7,15,11, 8, 6, 6,14,12,13, 5,14,13,13, 7, 5}
  ,{15, 5, 8,11,14,14, 6,14, 6, 9,12, 9,12, 5,15, 8}
  };
  static unsigned char v[8][16][4] = { /* hash rotate */
  {
   {0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0},{0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0}
  ,{0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0},{0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0}
  },{
   {0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0},{0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0}
  ,{0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0},{0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0}
  },{
   {0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0},{0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0}
  ,{0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0},{0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0}
  },{
   {0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0},{0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0}
  ,{0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0},{0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0}
  }
  ,
  {
   {4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4},{4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4}
  ,{4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4},{4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4}
  },{
   {4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4},{4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4}
  ,{4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4},{4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4}
  },{
   {4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4},{4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4}
  ,{4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4},{4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4}
  },{
   {4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4},{4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4}
  ,{4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4},{4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4}
  }
  };
  rmd128_bt t[8]; /* a=0 b=1 c=2 d=3 a'=4 b'=5 c'=6 d'=7 */
  rmd128_bt f;
  unsigned int i;
  unsigned int j;

  for (i = 0; i < 4; ++i)
    t[4 + i] = t[i] = h[i];
  for (i = 0; i < 8; ++i) {
    for (j = 0; j < 16; ++j) {
      switch (i) {
      case 0: case 7: /* x XOR y XOR z */
        f = t[v[i][j][1]] ^ t[v[i][j][2]] ^ t[v[i][j][3]];
        break;
      case 1: case 6: /* (x AND y) OR (NOT(x) AND z) */
        f = (t[v[i][j][1]] & t[v[i][j][2]]) | (~t[v[i][j][1]] & t[v[i][j][3]]);
        break;
      case 2: case 5: /* (x OR NOT(y)) XOR z */
        f = (t[v[i][j][1]] | ~t[v[i][j][2]]) ^ t[v[i][j][3]];
        break;
      case 3: case 4: /* (x AND z) OR (y AND NOT(z)) */
        f = (t[v[i][j][1]] & t[v[i][j][3]]) | (t[v[i][j][2]] & ~t[v[i][j][3]]);
        break;
      }
      f += t[v[i][j][0]]
         + (x[r[i][j] + 0] << 0 | x[r[i][j] + 1] << 8 | x[r[i][j] + 2] << 16 | x[r[i][j] + 3] << 24)
         + k[i];
      t[v[i][j][0]] = (f << s[i][j]) | (f >> (32 - s[i][j])); /* rotate left */
    }
  }
     f = h[1] + t[2] + t[7];
  h[1] = h[2] + t[3] + t[4];
  h[2] = h[3] + t[0] + t[5];
  h[3] = h[0] + t[1] + t[6];
  h[0] = f;
}

void
rmd128update(
  rmd128_t *v
 ,unsigned char *d
 ,unsigned int l
){
  unsigned char *s;

  if (v->l) {
    unsigned int i;

    for (i = v->l, s = v->d + i; l && i < 64; --l, ++i, ++s, ++d)
      *s = *d;
    if (i == 64) {
      rmd128mix(v->h, v->d);
      if ((v->bl += 64) < 64)
        ++v->bh;
      v->l = 0;
    } else {
      v->l = i;
      return;
    }
  }
  for (; l >= 64; l -= 64, d += 64) {
    rmd128mix(v->h, d);
    if ((v->bl += 64) < 64)
      ++v->bh;
  }
  if (l) {
    v->l = l;
    for (s = v->d; l; --l, ++s, ++d)
      *s = *d;
  }
}

void
rmd128final(
  rmd128_t *v
 ,unsigned char *h
){
  unsigned char *s;
  unsigned int i;

  if ((i = v->l))
    if ((v->bl += i) < i)
      ++v->bh;
  s = v->d + i++;
  *s++ = 0x80;
  if (i > 64 - 8) {
    for (; i < 64; ++i, ++s)
      *s = 0x00;
    rmd128mix(v->h, v->d);
    i = 0;
    s = v->d;
  }
  for (; i < 64 - 8; ++i, ++s)
    *s = 0x00;
  /* bytes to bits * 8=2^3 */
  *s++ = v->bl << 3;
  *s++ = v->bl >> (1 * 8 - 3);
  *s++ = v->bl >> (2 * 8 - 3);
  *s++ = v->bl >> (3 * 8 - 3);
  *s++ = v->bh << 3;
  *s++ = v->bh >> (1 * 8 - 3);
  *s++ = v->bh >> (2 * 8 - 3);
  *s   = v->bh >> (3 * 8 - 3);
  rmd128mix(v->h, v->d);
  for (i = 0; i < 4; ++i) {
    *h++ = v->h[i] >> (0 * 8);
    *h++ = v->h[i] >> (1 * 8);
    *h++ = v->h[i] >> (2 * 8);
    *h++ = v->h[i] >> (3 * 8);
  }
}

void
rmd128hex(
  unsigned char *h
 ,char *o
){
  unsigned int i;

  for (i = 0; i < 16; ++i, ++h) {
    static char m[] = "0123456789abcdef";

    *o++ = m[(*h >> 4) & 0xf];
    *o++ = m[(*h >> 0) & 0xf];
  }
}
