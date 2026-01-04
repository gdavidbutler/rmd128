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

#ifndef RMD256_H
#define RMD256_H

#define RMD256_SZ 32 /* 256 bits */
typedef struct rmd256 rmd256_t;
unsigned int rmd256tsize(void);
void rmd256init(rmd256_t *);
void rmd256update(rmd256_t *, const unsigned char *, unsigned int);
void rmd256final(rmd256_t *, unsigned char *); /* RMD256_SZ */
void rmd256hmac(const unsigned char *k, unsigned int kl, const unsigned char *d, unsigned int dl, unsigned char *h); /* RMD256_SZ */
void rmd256hex(const unsigned char *, char *); /* RMD256_SZ, 2 * RMD256_SZ (not null-terminated) */

#endif /* RMD256_H */
