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

#include "rmd128.h"

typedef unsigned int rmd128_bt; /* unsigned 32 bits */
typedef int rmd128_bt_size_check[sizeof (rmd128_bt) == 4 ? 1 : -1];
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
 ,const unsigned char x[]
){
  static const rmd128_bt k[8] = { /* added constants */
    0x00000000U
   ,0x50a28be6U
   ,0x5a827999U
   ,0x5c4dd124U
   ,0x6ed9eba1U
   ,0x6d703ef3U
   ,0x8f1bbcdcU
   ,0x00000000U
  };
  static const unsigned char r[8][16] = { /* message word */
   { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15}
  ,{ 5,14, 7, 0, 9, 2,11, 4,13, 6,15, 8, 1,10, 3,12}
  ,{ 7, 4,13, 1,10, 6,15, 3,12, 0, 9, 5, 2,14,11, 8}
  ,{ 6,11, 3, 7, 0,13, 5,10,14,15, 8,12, 4, 9, 1, 2}
  ,{ 3,10,14, 4, 9,15, 8, 1, 2, 7, 0, 6,13,11, 5,12}
  ,{15, 5, 1, 3, 7,14, 6, 9,11, 8,12, 2,10, 0, 4,13}
  ,{ 1, 9,11,10, 0, 8,12, 4,13, 3, 7,15,14, 5, 6, 2}
  ,{ 8, 6, 4, 1, 3,11,15, 0, 5,12, 2,13, 9, 7,10,14}
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
  rmd128_bt w[16]; /* precomputed message words */
  rmd128_bt t[8]; /* a=0 b=1 c=2 d=3 a'=4 b'=5 c'=6 d'=7 */
  rmd128_bt f;
  unsigned int i;
  unsigned int j;
  unsigned int base;
  unsigned int a, b, c, d;

  for (i = 0; i < 16; ++i)
    w[i] = (rmd128_bt)x[i * 4] | (rmd128_bt)x[i * 4 + 1] << 8
         | (rmd128_bt)x[i * 4 + 2] << 16 | (rmd128_bt)x[i * 4 + 3] << 24;
  for (i = 0; i < 4; ++i)
    t[4 + i] = t[i] = h[i];
  for (i = 0; i < 8; ++i) {
    base = (i & 1) << 2;
    for (j = 0; j < 16; ++j) {
      a = base | ((4u - (j & 3)) & 3);
      b = base | ((5u - (j & 3)) & 3);
      c = base | ((6u - (j & 3)) & 3);
      d = base | ((7u - (j & 3)) & 3);
      switch (i) {
      case 0: case 7: /* x XOR y XOR z */
        f = t[b] ^ t[c] ^ t[d];
        break;
      case 2: case 5: /* (x AND y) OR (NOT(x) AND z) */
        f = t[d] ^ (t[b] & (t[c] ^ t[d]));
        break;
      case 4: case 3: /* (x OR NOT(y)) XOR z */
        f = (t[b] | ~t[c]) ^ t[d];
        break;
      case 6: case 1: /* (x AND z) OR (y AND NOT(z)) */
        f = t[c] ^ (t[d] & (t[b] ^ t[c]));
        break;
      }
      f += t[a] + w[r[i][j]] + k[i];
      t[a] = (f << s[i][j]) | (f >> (sizeof (rmd128_bt) * 8 - s[i][j])); /* rotate left */
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
 ,const unsigned char *d
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
  *s++ = (unsigned char)(v->bl << 3);
  *s++ = (unsigned char)(v->bl >> (1 * 8 - 3));
  *s++ = (unsigned char)(v->bl >> (2 * 8 - 3));
  *s++ = (unsigned char)(v->bl >> (3 * 8 - 3));
  *s++ = (unsigned char)((v->bh << 3) | (v->bl >> (4 * 8 - 3)));
  *s++ = (unsigned char)(v->bh >> (1 * 8 - 3));
  *s++ = (unsigned char)(v->bh >> (2 * 8 - 3));
  *s   = (unsigned char)(v->bh >> (3 * 8 - 3));
  rmd128mix(v->h, v->d);
  for (i = 0; i < 4; ++i) {
    *h++ = (unsigned char)(v->h[i] >> (0 * 8));
    *h++ = (unsigned char)(v->h[i] >> (1 * 8));
    *h++ = (unsigned char)(v->h[i] >> (2 * 8));
    *h++ = (unsigned char)(v->h[i] >> (3 * 8));
  }
}

void
rmd128hmac(
  const unsigned char *k
 ,unsigned int kl
 ,const unsigned char *d
 ,unsigned int dl
 ,unsigned char *h
){
  rmd128_t c;
  unsigned char i[64];
  unsigned char o[64];
  unsigned int l;

  if (kl > 64) {
    rmd128init(&c);
    rmd128update(&c, k, kl);
    rmd128final(&c, h);
    k = h;
    kl = RMD128_SZ;
  }
  for (l = 0; l < kl; ++l) {
    i[l] = *(k + l) ^ 0x36;
    o[l] = *(k + l) ^ 0x5c;
  }
  for (; l < 64; ++l) {
    i[l] = 0x00 ^ 0x36;
    o[l] = 0x00 ^ 0x5c;
  }
  rmd128init(&c);
  rmd128update(&c, i, sizeof (i));
  rmd128update(&c, d, dl);
  rmd128final(&c, h);
  rmd128init(&c);
  rmd128update(&c, o, sizeof (o));
  rmd128update(&c, h, RMD128_SZ);
  rmd128final(&c, h);
  /* wipe stack residue; volatile defeats dead-store elimination */
  {
    volatile unsigned char *p;
    unsigned int n;

    p = (volatile unsigned char *)&c;
    for (n = 0; n < sizeof (c); ++n)
      *p++ = 0;
    p = (volatile unsigned char *)i;
    for (n = 0; n < sizeof (i); ++n)
      *p++ = 0;
    p = (volatile unsigned char *)o;
    for (n = 0; n < sizeof (o); ++n)
      *p++ = 0;
  }
}

void
rmd128hex(
  const unsigned char *h
 ,char *o
){
  unsigned int i;

  for (i = 0; i < RMD128_SZ; ++i, ++h) {
    static const char m[] = "0123456789abcdef";

    *o++ = m[(*h >> 4) & 0xf];
    *o++ = m[(*h >> 0) & 0xf];
  }
}
