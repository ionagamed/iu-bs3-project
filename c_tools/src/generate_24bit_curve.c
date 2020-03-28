#include <stdio.h>

#include <openssl/bio.h>
#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/pem.h>

#include "../lib/common.h"
#include "../lib/ec.h"
#include "../lib/log.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage:\n    %s <output_file.pem>\n", argv[0]);
    return -1;
  }

  log_info("Initializing stuff");

  BN_CTX *bn_ctx = BN_CTX_new();

  BIGNUM *p = BN_new();
  // BN_dec2bn(&p, "31"); // a small prime for tests
  BN_dec2bn(&p, "16777213");

  BIGNUM *a = BN_new();
  BN_dec2bn(&a, "0");

  BIGNUM *b = BN_new();
  BN_dec2bn(&b, "3");

  BIGNUM *generator_x = BN_new();
  BN_dec2bn(&generator_x, "1");

  BIGNUM *generator_y = BN_new();
  BN_dec2bn(&generator_y, "2");

  EC_GROUP *group = construct_prime_group(p, a, b, generator_x, generator_y, bn_ctx);

  FILE *output_fp = fopen(argv[1], "w");
  PEM_write_ECPKParameters(output_fp, group);

  ERR_print_errors_fp(stdout);

  return 0;
}
