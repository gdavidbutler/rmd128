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

#ifndef RMD128_H
#define RMD128_H

#define RMD128_SZ 16 /* 128 bits */
typedef struct rmd128 rmd128_t;
unsigned int rmd128tsize(void);
void rmd128init(rmd128_t *);
void rmd128update(rmd128_t *, const unsigned char *, unsigned int);
void rmd128final(rmd128_t *, unsigned char *); /* RMD128_SZ */
void rmd128hmac(const unsigned char *k, unsigned int kl, const unsigned char *d, unsigned int dl, unsigned char *h); /* RMD128_SZ */
void rmd128hex(const unsigned char *, char *); /* RMD128_SZ, 2 * RMD128_SZ (not null-terminated) */

#endif /* RMD128_H */
