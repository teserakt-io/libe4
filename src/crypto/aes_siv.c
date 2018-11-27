//  aes_siv.c
//  2018-07-01  Markku-Juhani O. Saarinen <markku@teserakt.io>

//  (c) 2018 Copyright Teserakt AG

#include <string.h>

#include "aes256enc.h"
#include "aes_siv.h"

static void cmac_dbl(uint8_t v[16])
{
    int i;
    uint16_t x;

    x = 0;
    for (i = 15; i >= 0; i--) {
        x += ((uint16_t) v[i]) << 1;
        v[i] = (uint8_t) x;
        x >>= 8;
    }
    v[15] ^= (-x) & 0x87;
}

static void siv_cmac(uint8_t res[16], const uint8_t *data, size_t len,
          const uint8_t xorend[16], const void *eky)
{
    size_t i, j;
    uint8_t k12[16], blk[16];

    memset(blk, 0x00, 16);
    memset(k12, 0x00, 16);
    aes256_encrypt_ecb(k12, eky);

    cmac_dbl(k12);

    j = 0;

    for (i = 0; i + 16 < len; i++) {        // process blocks
        if (j >= 16) {
            aes256_encrypt_ecb(blk, eky);
            j = 0;
        }
        blk[j++] ^= data[i];
    }

    for (; i < len; i++) {                  // last 16 bytes
        if (j >= 16) {
            aes256_encrypt_ecb(blk, eky);
            j = 0;
        }
        if (xorend != NULL)                 // xor 16 chaining bytes at end
            blk[j] ^= xorend[i + 16 - len];
        blk[j++] ^= data[i];
    }

    if (j < 16) {                           // padding
        blk[j] ^= 0x80;
        cmac_dbl(k12);
    }
    for (i = 0; i < 16; i++)
        blk[i] ^= k12[i];

    aes256_encrypt_ecb(blk, eky);

    memcpy(res, blk, 16);
}

// counter mode encryption / decryption

static void siv_ctr(uint8_t *dst, const uint8_t *src, size_t len,
         const uint8_t iv[16], void *eky)
{
    size_t i;
    int j;
    uint8_t cnt[16], blk[16];

    memcpy(cnt, iv, 16);
    cnt[8] &= 0x7F;
    cnt[12] &= 0x7F;

    j = 16;
    for (i = 0; i < len; i++) {
        if (j >= 16) {
            memcpy(blk, cnt, 16);
            aes256_encrypt_ecb(blk, eky);

            for (j = 15; j >= 0; j--) {     // will actually terminate early
                cnt[j]++;
                if (cnt[j] != 0xFF)
                    break;
            }
            j = 0;
        }
        dst[i] = src[i] ^ blk[j++];
    }
}

// AES256-SIV Encrypt

int aes256_encrypt_siv(
        uint8_t *ct, size_t *ctlen,         // out: ciphertext
        const uint8_t *ad, size_t adlen,    // in: associated data / nonce
        const uint8_t *pt, size_t ptlen,    // in: plaintext
        const uint8_t *key)                 // in: secret key (64 bytes)
{
    size_t i;
    uint8_t d[16], x[16];

    uint8_t eky[AES256_EXPKEY_LEN];

    // CMAC part

    aes256_enc_exp_key(eky, key);

    memset(d, 0, 16);                       // zero
    siv_cmac(d, d, 16, NULL, eky);

    cmac_dbl(d);
    siv_cmac(x, ad, adlen, NULL, eky);      // cmac associated data
    for (i = 0; i < 16; i++)
        d[i] ^= x[i];

    if (ptlen >= 16) {                      // cmac plaintext
        siv_cmac(ct, pt, ptlen, d, eky);
    } else {
        cmac_dbl(d);
        for (i = 0; i < ptlen; i++)
            d[i] ^= pt[i];
        d[i] ^= 0x80;
        siv_cmac(ct, d, 16, NULL, eky);
    }

    // CTR part

    aes256_enc_exp_key(eky, key + 32);

    siv_ctr(ct + 16, pt, ptlen, ct, eky);   // (siv) counter mode

    *ctlen = ptlen + 16;                    // set the length

    return 0;
}

// AES256-SIV Decrypt

int aes256_decrypt_siv(
        uint8_t *pt, size_t *ptlen,         // out: plaintext
        const uint8_t *ad, size_t adlen,    // in: associated data / nonce
        const uint8_t *ct, size_t ctlen,    // in: ciphertext
        const uint8_t *key)                 // in: secret key (64 bytes)
{
    size_t i;
    uint8_t d[16], x[16];

    uint8_t eky[AES256_EXPKEY_LEN];

    aes256_enc_exp_key(eky, key + 32);

    ctlen -= 16;                            // ctlen is ptlen now
    *ptlen = ctlen;

    siv_ctr(pt, ct + 16, ctlen, ct, eky);

    aes256_enc_exp_key(eky, key);           // CMAC part

    memset(d, 0, 16);                       // zero
    siv_cmac(d, d, 16, NULL, eky);

    cmac_dbl(d);
    siv_cmac(x, ad, adlen, NULL, eky);      // cmac associated data
    for (i = 0; i < 16; i++)
        d[i] ^= x[i];

    if (ctlen >= 16) {                      // cmac plaintext
        siv_cmac(x, pt, ctlen, d, eky);
    } else {
        cmac_dbl(d);
        for (i = 0; i < ctlen; i++)
            d[i] ^= pt[i];
        d[i] ^= 0x80;
        siv_cmac(x, d, 16, NULL, eky);
    }

    return memcmp(ct, x, 16);
}

