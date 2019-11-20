/*
 (c) 2018 Copyright Teserakt AG
*/

#ifndef _SHA3_H_
#define _SHA3_H_

#include <stddef.h>
#include "e4/stdint.h"

#ifndef KECCAKF_ROUNDS
#define KECCAKF_ROUNDS 24
#endif

/* state context */
typedef struct _sha3_ctx_tag
{
#ifdef __AVR__
    uint8_t b[240]; /* state + work bytes */
#else
    uint8_t b[200]; /* has big registers */
#endif
    uint8_t pt, rsiz, mdlen; /* these don't overflow */
} sha3_ctx_t;

/* this is the common context that should be used */
#ifndef E4_SHA3_INTERNAL
extern sha3_ctx_t common_sha3_ctx;
#endif

/* compression function, rc rounds */
void keccak_f1600(void *st, uint8_t rc);

/* OpenSSL - like interfece */
int sha3_init(sha3_ctx_t *c, int mdlen); /* mdlen = hash output in bytes */
int sha3_update(sha3_ctx_t *c, const void *data, size_t len);
int sha3_final(void *md, sha3_ctx_t *c); /* digest goes to md */

/** \brief compute a SHA3 hash
 \param[in] in input buffer to be hashed
 \param[in] inlen length of input buffer to be hashed
 \param[out] md message digest buffer to which the hash is written.
 \param[in] mdlen length of message digest buffer to be written
 \ingroup crypto
*/
void *sha3(const void *in, size_t inlen, void *md, int mdlen);

#define SHA3_256_DIGEST_LEN 32
void sha3_256_trunc(char* output, const size_t outputlen, const char* input, const size_t inputlen);

/* SHAKE128 and SHAKE256 extensible-output functions */
#define shake128_init(c) sha3_init(c, 16)
#define shake256_init(c) sha3_init(c, 32)
#define shake_update sha3_update

void shake_xof(sha3_ctx_t *c);
void shake_out(sha3_ctx_t *c, void *out, size_t len);

#endif
