#include <stdio.h>
#include <string.h>

#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/pem.h>

#include "../lib/log.h"

void dump_bn(const BIGNUM *bn) {
  char *hex = BN_bn2hex(bn);
  printf("%s\n", hex);
}

void dump_point(const EC_GROUP *group, const EC_POINT *point) {
  char *hex =
      EC_POINT_point2hex(group, point, POINT_CONVERSION_UNCOMPRESSED, NULL);
  printf("%s\n", hex);
}

void print_error() { ERR_print_errors(NULL); }

// Pollard's rho for group logarithms {{{1 =====================================

// Some useful BN constants
BIGNUM *zero;
BIGNUM *one;
BIGNUM *two;
BIGNUM *three;

void init_bn_consts() {
  BN_dec2bn(&zero, "0");
  BN_dec2bn(&one, "1");
  BN_dec2bn(&two, "2");
  BN_dec2bn(&three, "3");
}

/**
 * Get a new A for the Pollard's rho.
 *
 * \param[in]     group        Group to perform operations on.
 * \param[in,out] point        Current point A from the algorithm.
 * \param[in,out] a            a from A = aP + bQ
 * \param[in,out] b            b from A = aP + bQ
 * \param[in]     target_point Target point (Q from kP=Q).
 */
int pollard_new_xab(const EC_GROUP *group, EC_POINT *point, BIGNUM *a,
                     BIGNUM *b, const EC_POINT *target_point, BN_CTX *bn_ctx) {
  BIGNUM *y = BN_new();
  if (!EC_POINT_get_affine_coordinates(group, point, NULL, y, NULL)) {
    print_error();
    return -1;
  }

  BIGNUM *rem = BN_new();
  BN_mod(rem, y, three, bn_ctx);

  const EC_POINT *generator_point = EC_GROUP_get0_generator(group);

  if (BN_cmp(rem, zero) == 0) {
    EC_POINT_add(group, point, point, generator_point, NULL);
    BN_add(a, a, one);
  } else if (BN_cmp(rem, one) == 0) {
    EC_POINT_dbl(group, point, point, NULL);
    BN_add(a, a, a);
    BN_add(b, b, b);
  } else {
    EC_POINT_add(group, point, point, target_point, NULL);
    BN_add(b, b, one);
  }

  return 0;
}

int pollard(const EC_GROUP *group, const EC_POINT *target_point,
            BIGNUM *result, BN_CTX *bn_ctx) {
  BIGNUM *field_modulo = BN_new();
  EC_GROUP_get_curve(group, field_modulo, NULL, NULL, NULL);

  BIGNUM *baby_step_a = BN_dup(one);
  BIGNUM *baby_step_b = BN_dup(zero);
  EC_POINT *baby_step = EC_POINT_dup(EC_GROUP_get0_generator(group), group);

  BIGNUM *giant_step_a = BN_dup(one);
  BIGNUM *giant_step_b = BN_dup(zero);
  EC_POINT *giant_step = EC_POINT_dup(EC_GROUP_get0_generator(group), group);

  int first_iteration = 1;

  while (first_iteration ||
         EC_POINT_cmp(group, baby_step, giant_step, NULL) != 0) {
    first_iteration = 0;

    pollard_new_xab(group, baby_step, baby_step_a, baby_step_b, target_point, bn_ctx);
    pollard_new_xab(group, giant_step, giant_step_a, giant_step_b,
                    target_point, bn_ctx);
    pollard_new_xab(group, giant_step, giant_step_a, giant_step_b,
                    target_point, bn_ctx);
  }

  BIGNUM *da = BN_new();
  BN_mod_sub(da, baby_step_a, giant_step_b, field_modulo, bn_ctx);
  BIGNUM *db = BN_new();
  BN_mod_sub(db, giant_step_b, baby_step_b, field_modulo, bn_ctx);

  BIGNUM *gcd = BN_new();
  BN_gcd(gcd, db, field_modulo, bn_ctx);

  if (BN_cmp(gcd, one) == 0) {
    BN_mod_inverse(result, db, field_modulo, bn_ctx);
    BN_mod_mul(result, result, da, field_modulo, bn_ctx);
    return 0;
  } else {
    log_error("GCD of pollard params is not 1, not implemented\n");
    log_error("gcd: %s", BN_bn2dec(gcd));
    return -1;
  }
}

// Main function {{{1 ==========================================================

int main(int argc, char **argv) {
  if (argc < 3) {
    // TODO: usage.
    printf("Usage: \n");
    printf("    %s <public_key.pem> <output_private_key.pem>", argv[0]);
    return -1;
  }

  init_bn_consts();

  log_info("Trying to load %s as a private key", argv[1]);

  EC_KEY *pubkey;
  FILE *pubkey_file = fopen(argv[1], "r");

  // well, for some reason openssl doesn't produce valid params when generating
  // the public key, so i dunno
  // this works well as a PoC, i suppose

  PEM_read_EC_PUBKEY(pubkey_file, &pubkey, NULL, NULL);
  // PEM_read_ECPrivateKey(pubkey_file, &pubkey, NULL, NULL);

  BN_CTX *bn_ctx = BN_CTX_new();

  const EC_GROUP *group = EC_KEY_get0_group(pubkey);
  const EC_POINT *pubkey_point = EC_KEY_get0_public_key(pubkey);

  log_info("Everything loaded ok, trying to brute the key using Pollard's rho");

  BIGNUM *private_key_n = BN_new();
  if (pollard(group, pubkey_point, private_key_n, bn_ctx)) {
    log_error("Could not brute for some reasone");
    return -2;
  } else {
    log_info("Found the private key!");
    log_info("private_key: %s", BN_bn2dec(private_key_n));
    log_info("Generating and writing into the output file");

    EC_KEY *private_key = EC_KEY_dup(pubkey);
    EC_KEY_set_private_key(private_key, private_key_n);

    FILE *fp = fopen(argv[2], "w");
    if (!fp) {
      log_error("Could not open output file %s for writing", argv[2]);
      return -4;
    }
    PEM_write_ECPrivateKey(fp, private_key, NULL, NULL, 0, NULL, NULL);
  }
}

// vim: set fdm=marker:
