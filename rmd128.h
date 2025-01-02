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

typedef struct rmd128 rmd128_t;
unsigned int rmd128tsize(void);
void rmd128init(rmd128_t *);
void rmd128update(rmd128_t *, const unsigned char *, unsigned int);
void rmd128final(rmd128_t *, unsigned char *); /* 16 unsigned char (128 bits) */
void rmd128hmac(const unsigned char *k, unsigned int kl, const unsigned char *d, unsigned int dl, unsigned char *h); /* 16 unsigned char (128 bits) */
void rmd128hex(const unsigned char *, char *); /* 16 unsigned char (128 bits), 32 char (not null-terminated) */
