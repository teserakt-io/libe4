//  test_aes_siv.c
//  2018-07-02  Markku-Juhani O. Saarinen <markku@teserakt.io>

//  (c) 2018 Copyright Teserakt AG

#include <string.h>

#include "e4/crypto/aes256enc.h"
#include "e4/crypto/aes_siv.h"

// AES_Test triplet lifted from FIPS-197

int test_aes256()
{
    const uint8_t test_pt[16] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
                                  0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB,
                                  0xCC, 0xDD, 0xEE, 0xFF };

    const uint8_t test_ct[16] = { 0x8E, 0xA2, 0xB7, 0xCA, 0x51, 0x67,
                                  0x45, 0xBF, 0xEA, 0xFC, 0x49, 0x90,
                                  0x4B, 0x49, 0x60, 0x89 };

    const uint8_t test_key[32] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                                   0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,
                                   0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14,
                                   0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B,
                                   0x1C, 0x1D, 0x1E, 0x1F };

    size_t i;
    uint8_t v[16], ek[AES256_EXPKEY_LEN];

    aes256_enc_exp_key(ek, test_key); // expand key

    for (i = 0; i < 16; i++) v[i] = test_pt[i];

    aes256_encrypt_ecb(v, ek); // encrypt

    for (i = 0; i < 16; i++)
    {
        if (v[i] != test_ct[i]) return 1; // FAIL!
    }

    return 0;
}

#include <stdio.h>

// An AES256-SIV Test vector

int test_aes_siv()
{
    // self generated
    const uint8_t test_key[64] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
        0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A,
        0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00,
    };

    const uint8_t test_ad[24] = { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
                                  0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B,
                                  0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21,
                                  0x22, 0x23, 0x24, 0x25, 0x26, 0x27 };

    const uint8_t test_pt[14] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                                  0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE };

    const uint8_t test_ivc[30] = { 0xc4, 0xf1, 0xf8, 0x45, 0x64, 0xd0,
                                   0xcd, 0x14, 0x88, 0xd3, 0xc7, 0x0a,
                                   0xb5, 0x82, 0xa4, 0x27, 0xbb, 0xc9,
                                   0x35, 0xb2, 0xd2, 0x8d, 0x10, 0xd1,
                                   0x9b, 0x08, 0x96, 0x38, 0x20, 0xba };

    // from crypto_test.go
    const uint8_t test_jp[80] = {
        163, 170, 113, 22,  250, 77,  249, 210, 78,  28,  160, 45,  237, 93,
        164, 200, 69,  177, 144, 88,  25,  34,  203, 0,   222, 9,   31,  200,
        251, 127, 6,   91,  145, 230, 145, 187, 85,  154, 214, 154, 130, 152,
        98,  74,  163, 29,  244, 187, 138, 58,  140, 254, 85,  107, 236, 245,
        212, 233, 150, 187, 147, 172, 20,  22,  177, 76,  75,  137, 57,  249,
        110, 197, 218, 174, 34,  208, 235, 228, 175, 83
    };

    uint8_t buf1[256], buf2[256];
    size_t len1;
    size_t len2;
    int i, fails;

    memset(buf1, 0x55, sizeof(buf1));
    len1 = 0;
    memset(buf2, 0xAA, sizeof(buf2));
    len2 = 0;

    fails = 0;

    // encrypt test
    aes256_encrypt_siv(buf1, &len1, test_ad, 24, test_pt, 14, test_key);
    if (len1 != 30 || memcmp(buf1, test_ivc, len1) != 0)
    {
        fails++;
    }

    // decrypt test
    if (aes256_decrypt_siv(buf2, &len2, test_ad, 24, buf1, len1, test_key))
    {
        fails++;
    }

    if (len2 != 14 || memcmp(buf2, test_pt, len2) != 0)
    {
        fails++;
    }

    // corrupt test
    buf1[25] ^= 1;
    if (aes256_decrypt_siv(buf2, &len2, test_ad, 24, buf1, len1, test_key) == 0)
    {
        fails++;
    }

    // encrypt test with JP's test vector
    for (i = 0; i < sizeof(buf1); i++) buf1[i] = i;

    len2 = 0;
    aes256_encrypt_siv(buf2, &len2, buf1, 8, buf1, 64, buf1);

    if (len2 != 80 || memcmp(buf2, test_jp, 80) != 0)
    {
        fails++;
    }

    return fails;
}
