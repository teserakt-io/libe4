//  aes_siv.h
//  2018-07-01  Markku-Juhani O. Saarinen <markku@teserakt.io>
//  (c) 2018 Copyright Teserakt AG

#ifndef _AES_SIV_H_
#define _AES_SIV_H_

#include <stddef.h>
#include <stdint.h>

/** AES256-SIV Encrypt (with one associated data item)
 \param[out] ct ciphertext buffer to be written
 \param ctlen length of ciphertext buffer
 \param[in] ad buffer of associated data to be authenticated with the ciphertext
 \param[in] adlen Length of ad buffer
 \param[in] pt Plaintext buffer
 \param[in] ptlen Length of plaintext buffer
 \param[in] key Key to be used
 \ingroup crypto
*/
int aes256_encrypt_siv(uint8_t *ct,
                       size_t *ctlen, // out: ciphertext
                       const uint8_t *ad,
                       size_t adlen, // in: associated data / nonce
                       const uint8_t *pt,
                       size_t ptlen,        // in: plaintext
                       const uint8_t *key); // in: secret key (64 bytes)

/** \brief AES256-SIV Decrypt (with one associated data item)
 \param[out] pt Plaintext buffer
 \param[out] ptlen Length of plaintext buffer
 \param[in] ad Associated data
 \param[in] adlen Associated data length
 \param[in] ct Ciphertext buffer
 \param[in] ctlen Length of ciphertext buffer
 \param[in] key Key to be used.
 \ingroup crypto
 */
int aes256_decrypt_siv(uint8_t *pt,
                       size_t *ptlen, // out: plaintext
                       const uint8_t *ad,
                       size_t adlen, // in: associated data / nonce
                       const uint8_t *ct,
                       size_t ctlen,        // in: ciphertext
                       const uint8_t *key); // in: secret key (64 bytes)

#endif /* _SIV_H_ */
