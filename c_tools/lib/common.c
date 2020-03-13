#include <openssl/bn.h>

#include "log.h"

void remainder_log(BIGNUM *num, BN_CTX *bn_ctx) {
  static int initialized = 0;
  static BIGNUM *zero;
  static BIGNUM *rem;
  static BIGNUM *log_every;

  if (!initialized) {
    initialized = 1;
    rem = BN_new();
    log_every = BN_new();
    BN_dec2bn(&log_every, "100000");
    zero = BN_new();
    BN_dec2bn(&zero, "0");
  }

  BN_mod(rem, num, log_every, bn_ctx);

  if (BN_cmp(rem, zero) == 0) {
    log_debug("Currently at %s", BN_bn2dec(num));
  }
}
