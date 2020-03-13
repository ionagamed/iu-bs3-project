#include <stdio.h>
#include <string.h>

#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/pem.h>

void dump_bn(const BIGNUM *bn) {
  char *hex = BN_bn2hex(bn);
  printf("%s\n", hex);
}

void dump_point(const EC_GROUP *group, const EC_POINT *point, BN_CTX *bn_ctx) {
  char *hex =
      EC_POINT_point2hex(group, point, POINT_CONVERSION_UNCOMPRESSED, bn_ctx);
  printf("%s\n", hex);
}

void print_error() { ERR_print_errors(NULL); }

int main(int argc, char **argv) {
  if (argc < 2) {
    // TODO: usage.
    printf("Usage: \n");
    return -1;
  }

  printf("[+] Trying to load %s as a private key\n", argv[1]);

  EC_KEY *pubkey;
  FILE *pubkey_file = fopen(argv[1], "r");

  // well, for some reason openssl doesn't produce valid params when generating
  // the public key, so i dunno
  // this works well as a PoC, i suppose

  /* PEM_read_EC_PUBKEY(pubkey_file, &pubkey, NULL, NULL); */
  if (!PEM_read_ECPrivateKey(pubkey_file, &pubkey, NULL, NULL)) {
    printf("[-] Could not load private key:\n");
    ERR_print_errors_fp(stdout);
    return -3;
  }

  const EC_GROUP *group = EC_KEY_get0_group(pubkey);
  const EC_POINT *generator_point = EC_GROUP_get0_generator(group);
  const EC_POINT *pubkey_point = EC_KEY_get0_public_key(pubkey);

  BN_CTX *bn_ctx = BN_CTX_new();

  BIGNUM *private_key = BN_new();
  BN_dec2bn(&private_key, "1");

  printf("[+] Everything loaded successfully, trying to linearly brute the "
         "params\n");

  BIGNUM *zero = BN_new();
  BN_dec2bn(&zero, "0");

  BIGNUM *print_every = BN_new();
  BN_dec2bn(&print_every, "10000");

  BIGNUM *rem = BN_new();

  while (1) {
    EC_POINT *possible_pubkey_point = EC_POINT_dup(generator_point, group);
    if (!EC_POINT_mul(group, possible_pubkey_point, NULL, generator_point,
                      private_key, bn_ctx)) {
      printf("[-] EC_POINT_mul failed with:\n");
      print_error();
      return -2;
    }

    if (EC_POINT_cmp(group, possible_pubkey_point, pubkey_point, bn_ctx) == 0) {
      printf("[+] Hey, found the private key!\n");
      printf("[+] private_key: %s\n", BN_bn2hex(private_key));
      return 0;
    }

    BN_add(private_key, private_key, BN_value_one());

    BN_mod(rem, private_key, print_every, bn_ctx);
    if (BN_cmp(rem, zero) == 0) {
      printf("[*] Currently at %s\n", BN_bn2hex(private_key));
    }
  }
}

// vim: set fdm=marker:
