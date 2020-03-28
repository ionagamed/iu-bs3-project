#include <stdio.h>

#include <openssl/bn.h>
#include <openssl/bio.h>
#include <openssl/ec.h>
#include <openssl/pem.h>

#include "../lib/common.h"
#include "../lib/log.h"
#include "../lib/ec.h"

int main(int argc, char **argv) {
  // p a b gx gy output_file
  if (argc < 8) {
    printf("Usage:\n    %s output_file p a b gx gy ord\n\n", argv[0]);
    printf("Would generate an ecparam file with the following form:\n");
    printf("    y^2 = x^3 + ax + b\n");
    printf("    over GF(p)\n");
    printf("    with generator (gx, gy)\n");
    printf("    with order ord\n");
    printf("Will output to the output_file\n");
    return -1;
  }

  BIGNUM *p = NULL;
  BN_dec2bn(&p, argv[2]);

  BIGNUM *a = NULL;
  BN_dec2bn(&a, argv[3]);

  BIGNUM *b = NULL;
  BN_dec2bn(&b, argv[4]);

  BIGNUM *gx = NULL;
  BN_dec2bn(&gx, argv[5]);

  BIGNUM *gy = NULL;
  BN_dec2bn(&gy, argv[6]);

  BIGNUM *order = NULL;
  BN_dec2bn(&order, argv[7]);

  BN_CTX *bn_ctx = BN_CTX_new();

  EC_GROUP *group = construct_prime_group_known_order(p, a, b, gx, gy, order, bn_ctx);

  FILE *output = fopen(argv[1], "w");
  PEM_write_ECPKParameters(output, group);
}
