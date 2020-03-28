#ifndef _EC_H
#define _EC_H

struct EC_GROUP;
struct BIGNUM;
struct BN_CTX;

EC_GROUP *construct_prime_group(
    BIGNUM *p,
    BIGNUM *a,
    BIGNUM *b,
    BIGNUM *gx,
    BIGNUM *gy,
    BN_CTX *bn_ctx
);

EC_GROUP *construct_prime_group_known_order(
    BIGNUM *p,
    BIGNUM *a,
    BIGNUM *b,
    BIGNUM *gx,
    BIGNUM *gy,
    BIGNUM *order,
    BN_CTX *bn_ctx
);

#endif
