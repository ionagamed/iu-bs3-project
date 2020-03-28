#include <openssl/bn.h>
#include <openssl/ec.h>

#include "common.h"
#include "ec.h"
#include "log.h"

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

EC_GROUP *construct_prime_group(
    BIGNUM *p,
    BIGNUM *a,
    BIGNUM *b,
    BIGNUM *gx,
    BIGNUM *gy,
    BN_CTX *bn_ctx
) {
  EC_GROUP *group = EC_GROUP_new(EC_GFp_simple_method());
  EC_GROUP_set_curve(group, p, a, b, bn_ctx);

  EC_POINT *generator = EC_POINT_new(group);
  EC_POINT_set_affine_coordinates(group, generator, gx, gy, bn_ctx);

  log_info("Trying to find group order");
  BIGNUM *order = BN_new();
  find_order(group, generator, order, bn_ctx);
  log_info("Found the group order");

  BIGNUM *cofactor = BN_new();
  BN_dec2bn(&cofactor, "1");

  EC_GROUP_set_generator(group, generator, order, cofactor);

  EC_GROUP_set_asn1_flag(group, OPENSSL_EC_EXPLICIT_CURVE);

  return group;
}

EC_GROUP *construct_prime_group_known_order(
    BIGNUM *p,
    BIGNUM *a,
    BIGNUM *b,
    BIGNUM *gx,
    BIGNUM *gy,
    BIGNUM *order,
    BN_CTX *bn_ctx
) {
  EC_GROUP *group = EC_GROUP_new(EC_GFp_simple_method());
  EC_GROUP_set_curve(group, p, a, b, bn_ctx);

  EC_POINT *generator = EC_POINT_new(group);
  EC_POINT_set_affine_coordinates(group, generator, gx, gy, bn_ctx);

  BIGNUM *cofactor = BN_new();
  BN_dec2bn(&cofactor, "1");

  EC_GROUP_set_generator(group, generator, order, cofactor);

  EC_GROUP_set_asn1_flag(group, OPENSSL_EC_EXPLICIT_CURVE);

  return group;
}
