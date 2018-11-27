//  aes_siv.h
//  2018-07-01  Markku-Juhani O. Saarinen <markku@teserakt.io>
//  (c) 2018 Copyright Teserakt AG

#ifndef _AES_SIV_H_
#define _AES_SIV_H_

#include <stdint.h>
#include <stddef.h>

// AES256-SIV Encrypt (with one associated data item)

int aes256_encrypt_siv(
        uint8_t *ct, size_t *ctlen,         // out: ciphertext
        const uint8_t *ad, size_t adlen,    // in: associated data / nonce
        const uint8_t *pt, size_t ptlen,    // in: plaintext
        const uint8_t *key);                // in: secret key (64 bytes)

// AES256-SIV Decrypt (with one associated data item)

int aes256_decrypt_siv(
        uint8_t *pt, size_t *ptlen,         // out: plaintext
        const uint8_t *ad, size_t adlen,    // in: associated data / nonce
        const uint8_t *ct, size_t ctlen,    // in: ciphertext
        const uint8_t *key);                // in: secret key (64 bytes)

#endif /* _SIV_H_ */
