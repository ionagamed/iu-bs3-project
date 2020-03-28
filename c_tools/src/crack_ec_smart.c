#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/pem.h>

#include "../lib/log.h"

void print_error() { ERR_print_errors(NULL); }

int main(int argc, char **argv) {
  if (argc < 3) {
    printf("Usage:\n");
    printf("    %s public_key private_key\n", argv[0]);
    return -1;
  }

  FILE *key_file = fopen(argv[1], "r");
  if (!key_file) {
    log_error("Could not open public key");
    perror("fopen");
    return -1;
  }

  EC_KEY *key;
  if (!PEM_read_EC_PUBKEY(key_file, &key, NULL, NULL)) {
    log_error("Could not read public key");
    print_error();
    return -1;
  }

  if (access("scripts/smart_attack.sage", F_OK) == -1) {
    log_error("Could not locate the attack script");
    log_error("Please run from the directory in which the `scripts` directory is located");
    return -1;
  }

  const EC_GROUP *group = EC_KEY_get0_group(key);

  BIGNUM *p = BN_new();
  BIGNUM *a = BN_new();
  BIGNUM *b = BN_new();

  BN_CTX *bn_ctx = BN_CTX_new();

  EC_GROUP_get_curve(group, p, a, b, bn_ctx);

  const EC_POINT *generator = EC_GROUP_get0_generator(group);

  BIGNUM *gx = BN_new();
  BIGNUM *gy = BN_new();

  EC_POINT_get_affine_coordinates(group, generator, gx, gy, bn_ctx);

  BIGNUM *px = BN_new();
  BIGNUM *py = BN_new();

  const EC_POINT *key_point = EC_KEY_get0_public_key(key);
  EC_POINT_get_affine_coordinates(group, key_point, px, py, bn_ctx);

  char script[1024] = "sage ";
  strcat(script, "scripts/smart_attack.sage ");
  strcat(script, BN_bn2dec(p));
  strcat(script, " ");
  strcat(script, BN_bn2dec(BN_new()));
  strcat(script, " ");
  strcat(script, BN_bn2dec(BN_new()));
  strcat(script, " ");
  strcat(script, BN_bn2dec(BN_new()));
  strcat(script, " ");
  strcat(script, BN_bn2dec(a));
  strcat(script, " ");
  strcat(script, BN_bn2dec(b));
  strcat(script, " ");
  strcat(script, BN_bn2dec(gx));
  strcat(script, " ");
  strcat(script, BN_bn2dec(gy));
  strcat(script, " ");
  strcat(script, BN_bn2dec(px));
  strcat(script, " ");
  strcat(script, BN_bn2dec(py));

  FILE *script_output = popen(script, "r");

  char attack_result[1024];
  fgets(attack_result, sizeof(attack_result), script_output);

  BIGNUM *private_key = NULL;
  BN_dec2bn(&private_key, attack_result);

  EC_KEY_set_private_key(key, private_key);

  FILE *output_key = fopen(argv[2], "w");
  if (!output_key) {
    log_error("Could not open output file for writing");
    perror("fopen");
    return -1;
  }

  PEM_write_ECPrivateKey(output_key, key, NULL, NULL, 0, NULL, NULL);

  return 0;
}
