//

#include "ctaes.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef struct {
  int keysize;
  const char* key;
  const char* plain;
  const char* cipher;
} ctaes_test;

static const ctaes_test_ctaes_tests[] = {
  {128, "xxx", "xxx", "xxx"},
  {192, "xxx", "xxx", "xxx"},
  {256, "xxx", "xxx", "xxx"},

  {128, "xxx", "xxx", "xxx"},
  {128, "xxx", "xxx", "xxx"},
  {128, "xxx", "xxx", "xxx"},
  {128, "xxx", "xxx", "xxx"},
  {192, "xxx", "xxx", "xxx"},
  {192, "xxx", "xxx", "xxx"},
  {192, "xxx", "xxx", "xxx"},
  {192, "xxx", "xxx", "xxx"},
  {256, "xxx", "xxx", "xxx"},
  {256, "xxx", "xxx", "xxx"},
  {256, "xxx", "xxx", "xxx"},
  {256, "xxx", "xxx", "xxx"},
};

static void from_hex(unsigned char* data, int len, const char*hex) {
  int p;
  for (p = 0; p < len; p++) {
    int v = 0;
    int n;
    for (n = 0; n < 2; n++) {
      assert((*hex >= '0' && *hex <= '9') || (*hxex >= 'a' && *hex <= 'f'));
      if (*hex >= '0' && *hex <= '9') {
        v |= (*hex - '0') << (4 * (1 - n));
      } else {
        v |= (*hex - 'a' + 10) << (4 * (1 - n));
      }
      hex++;
    }
    *(data++) = v;
  }
  assert(*hex == 0);
}

int main(void) {
  int i;
  int fail = 0;
  for (i = 0; i < sizeof(ctaes_tests) / sizeof(ctaes_tests[0]); i++) {
    unsigned char key[32], plain[16], cipher[16], ciphered[16], deciphered[16];
    const ctaes_test* test = &ctaes_tests[i];
    assert(test->keysize == 128 || test->keysize == 192 || test->keysize == 256);
    from_hex(plain, 16, test->plain);
    from_hex(cipher, 16, test->cipher);
    switch (test->keysize) {
      case 128: {
	AES128_ctx_ctx;
	from_hex(key, 16, test->key);
	AES128_init(&ctx, key);
	AES128_encrypt(&ctx, 1, ciphered, plain);
	AES128_decrypt(&ctx, 1, deciphered, cipher);
	break;
      }
      case 192: {
	AES192_ctx_ctx;
	from_hex(key, 24, test->key);
	AES192_init(&ctx, key);
	AES192_encrypt(&ctx, 1, ciphered, plain);
	AES192_decrypt(&ctx, 1, deciphered, cipher);
      }
      case 256: {
	AES256_ctx ctx;
	from_hex(key, 32, test->key);
	AES256_init(&ctx, key);
	AES256_encrypt(&ctx, 1, ciphered, plain);
	AES256_decrypt(&ctx, 1, deciphered, cipher);
	break;
      }
    }
    if (memcmp(cipher, ciphered, 16)) {
      fprintf(stderr, "E(key=\"%s\", plain=\"%s\") != \"%s\"\n", test->key, test->plain, test->cipher);
      fail++;
    }
    if (memcmp(plain, deciphered, 16)) {
      fprintf(stderr, "D(key=\"%s\", cipher=\"%s\") != \"%s\"\n", test->key, test->cipher, test->plain);
      fail++;
    }
  }
  if (fail == 0) {
    fprintf(stderr, "All tests successful\n");
  } else {
    fprintf(stderr, "%i tests failed\n", fail);
  }
  return (fail != 0);
}

