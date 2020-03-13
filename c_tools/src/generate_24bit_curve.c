#include <stdio.h>

#include <openssl/bio.h>
#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/pem.h>

#include "../lib/common.h"
#include "../lib/log.h"

int find_group_size(const EC_GROUP *group, const BIGNUM *field_size,
                    const EC_POINT *generator, BIGNUM *group_size,
                    BN_CTX *bn_ctx) {
  BIGNUM *iterator = BN_new();
  BN_dec2bn(&iterator, "0");

  BN_dec2bn(&group_size, "1");
  BN_add(group_size, group_size, field_size);

  BN_dec2bn(&group_size, "0");

  BIGNUM *three = BN_new();
  BN_dec2bn(&three, "3");

  // TODO: assuming a fixed curve
  while (BN_cmp(iterator, field_size) != 0) {
    BIGNUM *tmp = BN_dup(iterator);
    BN_mul(tmp, tmp, iterator, bn_ctx);
    BN_mul(tmp, tmp, iterator, bn_ctx);

    BN_add(group_size, group_size, tmp);
    BN_add(group_size, group_size, three);

    BN_free(tmp);
    BN_add(iterator, iterator, BN_value_one());
  }

  printf("%s\n", BN_bn2dec(group_size));

  return 0;
}

int find_order(const EC_GROUP *group, const EC_POINT *generator, BIGNUM *order,
               BN_CTX *bn_ctx) {
  EC_POINT *point = EC_POINT_dup(generator, group);
  BN_dec2bn(&order, "1");

  EC_POINT_add(group, point, point, generator, bn_ctx);

  while (EC_POINT_cmp(group, point, generator, bn_ctx) != 0) {
    BN_add(order, order, BN_value_one());
    EC_POINT_add(group, point, point, generator, bn_ctx);

    remainder_log(order, bn_ctx);
  }

  return 0;
}

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

  EC_GROUP *group = EC_GROUP_new(EC_GFp_simple_method());
  EC_GROUP_set_curve(group, p, a, b, bn_ctx);

  EC_POINT *generator = EC_POINT_new(group);
  EC_POINT_set_affine_coordinates(group, generator, generator_x, generator_y,
                                  bn_ctx);

  log_info("Trying to find group order");
  BIGNUM *order = BN_new();
  find_order(group, generator, order, bn_ctx);
  log_info("Found the group order");

  BIGNUM *cofactor = BN_new();
  BN_dec2bn(&cofactor, "1");

  EC_GROUP_set_generator(group, generator, order, cofactor);

  EC_GROUP_set_asn1_flag(group, OPENSSL_EC_EXPLICIT_CURVE);

  printf("%s\n", BN_bn2dec(order));

  FILE *output_fp = fopen(argv[1], "w");
  PEM_write_ECPKParameters(output_fp, group);

  ERR_print_errors_fp(stdout);

  return 0;
}
