//  aes256enc_openssl.c
//  2018-07-01  Markku-Juhani O. Saarinen <markku@teserakt.io>

//  (c) 2018 Copyright Teserakt AG

//  OpenSSL wrapper

#include "e4/crypto/aes256enc.h"
#include <openssl/aes.h>

void aes256_enc_exp_key(void *eky, const void *key)
{
    AES_set_encrypt_key(key, 256, eky);
}

void aes256_encrypt_ecb(void *v, const void *eky) { AES_encrypt(v, v, eky); }
