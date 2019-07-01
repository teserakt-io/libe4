//  aes256enc.h
//  2018-07-01  Markku-Juhani O. Saarinen <markku@teserakt.io>
//  (c) 2018 Copyright Teserakt AG

#ifndef _AES256ENC_H_
#define _AES256ENC_H_

// expand 256-bit key. ek[] needs 240 bytes of storage
#define AES256_EXPKEY_LEN 240

void aes256_enc_exp_key (void *ek, const void *key);

// encrypt a block
void aes256_encrypt_ecb (void *v, const void *ek);

#endif
