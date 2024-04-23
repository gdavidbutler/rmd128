## RMD128
A small C language implementation of RIPEMD-128

SRIPEMD-128 is a member of the RIPEMD [(RIPE Message Digest)](https://en.wikipedia.org/wiki/RIPEMD) cryptographic hash functions developed in 1996 generating a 128 bit digest.
See [pdf](https://homes.esat.kuleuven.be/~bosselae/ripemd160/pdf/AB-9601/AB-9601.pdf) and [pseudo-code](https://homes.esat.kuleuven.be/~bosselae/ripemd/rmd128.txt).

This implementation was created to provide small code to fit in a memory constrained 32 bit microcontroller.
If your compiler does not implement "unsigned int" as 32 bits, change "unsigned int" in rmd128.c @ typedef unsigned int rmd128_bt; to, perhaps, "unsigned long".

Included is an example driver program, main.c, that reads standard input till end-of-file and writes on standard output a hex representation of the hash.
