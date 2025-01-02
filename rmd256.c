/*
 * rmd128 - a small RIPEMD-128 / RIPEMD-256 implementation
 * Copyright (C) 2018-2024 G. David Butler <gdb@dbSystems.com>
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

#include "rmd256.h"

typedef unsigned int rmd256_bt; /* unsigned 32 bits */
struct rmd256 {
 rmd256_bt h[8];       /* unsigned 32 bits */
 rmd256_bt bh;         /* bytes processed high */
 rmd256_bt bl;         /* bytes processed low */
 unsigned int l;       /* current short data */
 unsigned char d[64];  /* short data */
};

unsigned int
rmd256tsize(
  void
){
  return (sizeof (rmd256_t));
}

void
rmd256init(
  rmd256_t *v
){
  v->h[0] = 0x67452301U;
  v->h[1] = 0xefcdab89U;
  v->h[2] = 0x98badcfeU;
  v->h[3] = 0x10325476U;
  v->h[4] = 0x76543210U;
  v->h[5] = 0xfedcba98U;
  v->h[6] = 0x89abcdefU;
  v->h[7] = 0x01234567U;
  v->bh = v->bl = 0;
  v->l = 0;
}

static void
rmd256mix(
  rmd256_bt h[]
 ,const unsigned char x[]
){
  static const rmd256_bt k[8] = { /* added constants */
    0x00000000U
   ,0x50a28be6U
   ,0x5a827999U
   ,0x5c4dd124U
   ,0x6ed9eba1U
   ,0x6d703ef3U
   ,0x8f1bbcdcU
   ,0x00000000U
  };
  static const unsigned char r[8][16] = { /* message word (from bytes) */
   { 0, 4, 8,12,16,20,24,28,32,36,40,44,48,52,56,60}
  ,{20,56,28, 0,36, 8,44,16,52,24,60,32, 4,40,12,48}
  ,{28,16,52, 4,40,24,60,12,48, 0,36,20, 8,56,44,32}
  ,{24,44,12,28, 0,52,20,40,56,60,32,48,16,36, 4, 8}
  ,{12,40,56,16,36,60,32, 4, 8,28, 0,24,52,44,20,48}
  ,{60,20, 4,12,28,56,24,36,44,32,48, 8,40, 0,16,52}
  ,{ 4,36,44,40, 0,32,48,16,52,12,28,60,56,20,24, 8}
  ,{32,24,16, 4,12,44,60, 0,20,48, 8,52,36,28,40,56}
  };
  static const unsigned char s[8][16] = { /* amount to rotate left */
   {11,14,15,12, 5, 8, 7, 9,11,13,14,15, 6, 7, 9, 8}
  ,{ 8, 9, 9,11,13,15,15, 5, 7, 7, 8,11,14,14,12, 6}
  ,{ 7, 6, 8,13,11, 9, 7,15, 7,12,15, 9,11, 7,13,12}
  ,{ 9,13,15, 7,12, 8, 9,11, 7, 7,12, 7, 6,15,13,11}
  ,{11,13, 6, 7,14, 9,13,15,14, 8,13, 6, 5,12, 7, 5}
  ,{ 9, 7,15,11, 8, 6, 6,14,12,13, 5,14,13,13, 7, 5}
  ,{11,12,14,15,14,15, 9, 8, 9,14, 5, 6, 8, 6, 5,12}
  ,{15, 5, 8,11,14,14, 6,14, 6, 9,12, 9,12, 5,15, 8}
  };
  static const unsigned char v[8][16][4] = { /* hash rotate */
  {
   {0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0},{0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0}
  ,{0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0},{0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0}
  },{
   {4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4},{4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4}
  ,{4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4},{4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4}
  },{
   {0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0},{0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0}
  ,{0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0},{0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0}
  },{
   {4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4},{4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4}
  ,{4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4},{4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4}
  },{
   {0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0},{0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0}
  ,{0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0},{0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0}
  },{
   {4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4},{4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4}
  ,{4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4},{4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4}
  },{
   {0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0},{0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0}
  ,{0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0},{0,1,2,3},{3,0,1,2},{2,3,0,1},{1,2,3,0}
  },{
   {4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4},{4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4}
  ,{4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4},{4,5,6,7},{7,4,5,6},{6,7,4,5},{5,6,7,4}
  }
  };
  rmd256_bt t[8]; /* a=0 b=1 c=2 d=3 a'=4 b'=5 c'=6 d'=7 */
  rmd256_bt f;
  unsigned int i;
  unsigned int j;

  for (i = 0; i < 8; ++i)
    t[i] = h[i];
  for (i = 0; i < 8; ++i) {
    for (j = 0; j < 16; ++j) {
      switch (i) {
      case 0: case 7: /* x XOR y XOR z */
        f = t[v[i][j][1]] ^ t[v[i][j][2]] ^ t[v[i][j][3]];
        break;
      case 2: case 5: /* (x AND y) OR (NOT(x) AND z) */
        f = (t[v[i][j][1]] & t[v[i][j][2]]) | (~t[v[i][j][1]] & t[v[i][j][3]]);
        break;
      case 4: case 3: /* (x OR NOT(y)) XOR z */
        f = (t[v[i][j][1]] | ~t[v[i][j][2]]) ^ t[v[i][j][3]];
        break;
      case 6: case 1: /* (x AND z) OR (y AND NOT(z)) */
        f = (t[v[i][j][1]] & t[v[i][j][3]]) | (t[v[i][j][2]] & ~t[v[i][j][3]]);
        break;
      }
      f += t[v[i][j][0]]
         + (x[r[i][j] + 0] << 0 | x[r[i][j] + 1] << 8 | x[r[i][j] + 2] << 16 | x[r[i][j] + 3] << 24)
         + k[i];
      t[v[i][j][0]] = (f << s[i][j]) | (f >> (32 - s[i][j])); /* rotate left */
    }
    switch (i) {
    case 1:
      f = t[0];
      t[0] = t[4];
      t[4] = f;
      break;
    case 3:
      f = t[1];
      t[1] = t[5];
      t[5] = f;
      break;
    case 5:
      f = t[2];
      t[2] = t[6];
      t[6] = f;
      break;
    case 7:
      f = t[3];
      t[3] = t[7];
      t[7] = f;
      break;
    default:
      break;
    }
  }
  for (i = 0; i < 8; ++i)
    h[i] += t[i];
}

void
rmd256update(
  rmd256_t *v
 ,const unsigned char *d
 ,unsigned int l
){
  unsigned char *s;

  if (v->l) {
    unsigned int i;

    for (i = v->l, s = v->d + i; l && i < 64; --l, ++i, ++s, ++d)
      *s = *d;
    if (i == 64) {
      rmd256mix(v->h, v->d);
      if ((v->bl += 64) < 64)
        ++v->bh;
      v->l = 0;
    } else {
      v->l = i;
      return;
    }
  }
  for (; l >= 64; l -= 64, d += 64) {
    rmd256mix(v->h, d);
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
rmd256final(
  rmd256_t *v
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
    rmd256mix(v->h, v->d);
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
  rmd256mix(v->h, v->d);
  for (i = 0; i < 8; ++i) {
    *h++ = v->h[i] >> (0 * 8);
    *h++ = v->h[i] >> (1 * 8);
    *h++ = v->h[i] >> (2 * 8);
    *h++ = v->h[i] >> (3 * 8);
  }
}

void
rmd256hmac(
  const unsigned char *k
 ,unsigned int kl
 ,const unsigned char *d
 ,unsigned int dl
 ,unsigned char *h
){
  rmd256_t c;
  unsigned char i[64];
  unsigned char o[64];
  unsigned int l;

  if (kl > 64) {
    rmd256init(&c);
    rmd256update(&c, k, kl);
    rmd256final(&c, h);
    k = h;
    kl = 32;
  }
  for (l = 0; l < sizeof (i); ++l)
    i[l] = (l < kl ? *(k + l) : 0x00) ^ 0x36;
  for (l = 0; l < sizeof (o); ++l)
    o[l] = (l < kl ? *(k + l) : 0x00) ^ 0x5c;
  rmd256init(&c);
  rmd256update(&c, i, sizeof (i));
  rmd256update(&c, d, dl);
  rmd256final(&c, h);
  rmd256init(&c);
  rmd256update(&c, o, sizeof (o));
  rmd256update(&c, h, 32);
  rmd256final(&c, h);
}

void
rmd256hex(
  const unsigned char *h
 ,char *o
){
  unsigned int i;

  for (i = 0; i < 32; ++i, ++h) {
    static const char m[] = "0123456789abcdef";

    *o++ = m[(*h >> 4) & 0xf];
    *o++ = m[(*h >> 0) & 0xf];
  }
}
