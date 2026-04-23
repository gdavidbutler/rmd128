#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rmd128.h"
#include "rmd256.h"

/* Spec vectors from https://homes.esat.kuleuven.be/~bosselae/ripemd160.html */
struct svec {
  const char *in;
  unsigned int inlen;
  const char *h128;
  const char *h256;
};

static const struct svec Svec[] = {
  { "", 0
   ,"cdf26213a150dc3ecb610f18f6b38b46"
   ,"02ba4c4e5f8ecd1877fc52d64d30e37a2d9774fb1e5d026380ae0168e3c5522d" }
 ,{ "a", 1
   ,"86be7afa339d0fc7cfc785e72f578d33"
   ,"f9333e45d857f5d90a91bab70a1eba0cfb1be4b0783c9acfcd883a9134692925" }
 ,{ "abc", 3
   ,"c14a12199c66e4ba84636b0f69144c77"
   ,"afbd6e228b9d8cbbcef5ca2d03e6dba10ac0bc7dcbe4680e1e42d2e975459b65" }
 ,{ "message digest", 14
   ,"9e327b3d6e523062afc1132d7df9d1b8"
   ,"87e971759a1ce47a514d5c914c392c9018c7c46bc14465554afcdf54a5070c0e" }
 ,{ "abcdefghijklmnopqrstuvwxyz", 26
   ,"fd2aa607f71dc8f510714922b371834e"
   ,"649d3034751ea216776bf9a18acc81bc7896118a5197968782dd1fd97d8d5133" }
 ,{ "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56
   ,"a1aa0689d0fafa2ddc22e88b49133a06"
   ,"3843045583aac6c8c8d9128573e7a9809afb2a0f34ccc36ea9e72f16f6368e3f" }
 ,{ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", 62
   ,"d1e959eb179c911faea4624c60c5c702"
   ,"5740a408ac16b720b84424ae931cbb1fe363d1d0bf4017f1a89f7ea6de77a0b8" }
 ,{ "12345678901234567890123456789012345678901234567890123456789012345678901234567890", 80
   ,"3f45ef194732c2dbb2c4a2c769795fa3"
   ,"06fdcc7a409548aaf91368c06a6275b553e3f099bf0ea4edfd6778df89a890dd" }
};

static void
hexof(
  const unsigned char *b
 ,unsigned int n
 ,char *o
){
  static const char m[] = "0123456789abcdef";
  unsigned int q;

  for (q = 0; q < n; ++q) {
    *o++ = m[(b[q] >> 4) & 0xf];
    *o++ = m[b[q]       & 0xf];
  }
  *o = 0;
}

static int
check(
  const char *label
 ,const char *got
 ,const char *want
){
  if (strcmp(got, want) == 0)
    return (0);
  fprintf(stderr, "FAIL %s: got %s want %s\n", label, got, want);
  return (1);
}

int
main(
  void
){
  static const unsigned char Hk1[16] = {
    0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b
   ,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b
  };
  static const char Hd1[] = "Hi There";
  static const char Hk2[] = "Jefe";
  static const char Hd2[] = "what do ya want for nothing?";
  static unsigned char Hk6[80];
  static const char Hd6[] = "Test Using Larger Than Block-Size Key - Hash Key First";
  static const char Msg[] =
    "The quick brown fox jumps over the lazy dog."
    "The quick brown fox jumps over the lazy dog."
    "The quick brown fox jumps over the lazy dog."
    "The quick brown fox jumps over the lazy dog.";
  static const unsigned int Chunks[] = { 1, 7, 31, 55, 56, 63, 64, 65, 88, 127, 128 };
  rmd128_t *c128;
  rmd256_t *c256;
  unsigned char h128[RMD128_SZ];
  unsigned char h256[RMD256_SZ];
  char b128[2 * RMD128_SZ + 1];
  char b256[2 * RMD256_SZ + 1];
  char ref128[2 * RMD128_SZ + 1];
  char ref256[2 * RMD256_SZ + 1];
  unsigned char *mega;
  unsigned int msglen;
  unsigned int q;
  unsigned int off;
  unsigned int n;
  int fail;

  if (!(c128 = malloc(rmd128tsize()))) {
    fprintf(stderr, "FAIL: malloc(rmd128tsize)\n");
    return (1);
  }
  if (!(c256 = malloc(rmd256tsize()))) {
    fprintf(stderr, "FAIL: malloc(rmd256tsize)\n");
    free(c128);
    return (1);
  }
  fail = 0;

  /* Spec KATs */
  for (q = 0; q < sizeof (Svec) / sizeof (Svec[0]); ++q) {
    rmd128init(c128);
    rmd128update(c128, (const unsigned char *)Svec[q].in, Svec[q].inlen);
    rmd128final(c128, h128);
    hexof(h128, RMD128_SZ, b128);
    fail += check("rmd128 spec", b128, Svec[q].h128);

    rmd256init(c256);
    rmd256update(c256, (const unsigned char *)Svec[q].in, Svec[q].inlen);
    rmd256final(c256, h256);
    hexof(h256, RMD256_SZ, b256);
    fail += check("rmd256 spec", b256, Svec[q].h256);
  }

  /* Million-'a' */
  if (!(mega = malloc(1000000))) {
    fprintf(stderr, "FAIL: malloc(1000000)\n");
    return (1);
  }
  memset(mega, 'a', 1000000);
  rmd128init(c128);
  rmd128update(c128, mega, 1000000);
  rmd128final(c128, h128);
  hexof(h128, RMD128_SZ, b128);
  fail += check("rmd128 million-a", b128, "4a7f5723f954eba1216c9d8f6320431f");

  rmd256init(c256);
  rmd256update(c256, mega, 1000000);
  rmd256final(c256, h256);
  hexof(h256, RMD256_SZ, b256);
  fail += check("rmd256 million-a", b256, "ac953744e10e31514c150d4d8d7b677342e33399788296e43ae4850ce4f97978");
  free(mega);

  /* Streaming-boundary equivalence: one-shot vs chunk-by-chunk at sizes
     that straddle the 64-byte block buffer */
  msglen = sizeof (Msg) - 1;
  rmd128init(c128);
  rmd128update(c128, (const unsigned char *)Msg, msglen);
  rmd128final(c128, h128);
  hexof(h128, RMD128_SZ, ref128);
  rmd256init(c256);
  rmd256update(c256, (const unsigned char *)Msg, msglen);
  rmd256final(c256, h256);
  hexof(h256, RMD256_SZ, ref256);
  for (q = 0; q < sizeof (Chunks) / sizeof (Chunks[0]); ++q) {
    rmd128init(c128);
    rmd256init(c256);
    for (off = 0; off < msglen; off += n) {
      n = Chunks[q];
      if (off + n > msglen)
        n = msglen - off;
      rmd128update(c128, (const unsigned char *)Msg + off, n);
      rmd256update(c256, (const unsigned char *)Msg + off, n);
    }
    rmd128final(c128, h128);
    rmd256final(c256, h256);
    hexof(h128, RMD128_SZ, b128);
    hexof(h256, RMD256_SZ, b256);
    fail += check("rmd128 stream",  b128, ref128);
    fail += check("rmd256 stream",  b256, ref256);
  }

  /* HMAC-RIPEMD-128 from RFC 2286 */
  rmd128hmac(Hk1, sizeof (Hk1)
            ,(const unsigned char *)Hd1, sizeof (Hd1) - 1, h128);
  hexof(h128, RMD128_SZ, b128);
  fail += check("hmac-rmd128 t1", b128, "fbf61f9492aa4bbf81c172e84e0734db");

  rmd128hmac((const unsigned char *)Hk2, sizeof (Hk2) - 1
            ,(const unsigned char *)Hd2, sizeof (Hd2) - 1, h128);
  hexof(h128, RMD128_SZ, b128);
  fail += check("hmac-rmd128 t2", b128, "875f828862b6b334b427c55f9f7ff09b");

  for (q = 0; q < sizeof (Hk6); ++q)
    Hk6[q] = 0xaa;
  rmd128hmac(Hk6, sizeof (Hk6)
            ,(const unsigned char *)Hd6, sizeof (Hd6) - 1, h128);
  hexof(h128, RMD128_SZ, b128);
  fail += check("hmac-rmd128 t6", b128, "dc732928de98104a1f59d373c150acbb");

  /* HMAC-RIPEMD-256: no RFC vectors exist; regression against this impl */
  rmd256hmac(Hk1, sizeof (Hk1)
            ,(const unsigned char *)Hd1, sizeof (Hd1) - 1, h256);
  hexof(h256, RMD256_SZ, b256);
  fail += check("hmac-rmd256 t1", b256, "3dbfd19cad4f6701814d6083f938d2a116f32d81e9b84c2dfafbbafbad771998");

  rmd256hmac(Hk6, sizeof (Hk6)
            ,(const unsigned char *)Hd6, sizeof (Hd6) - 1, h256);
  hexof(h256, RMD256_SZ, b256);
  fail += check("hmac-rmd256 t6", b256, "821543a295eccf093af9b461f630d7c49206fb2158e5ac89e73a02cf089528ed");

  free(c128);
  free(c256);
  if (fail) {
    fprintf(stderr, "%d test(s) failed\n", fail);
    return (1);
  }
  printf("all tests passed\n");
  return (0);
}
