//  test_aes_siv.c
//  2018-07-02  Markku-Juhani O. Saarinen <markku@teserakt.io>

//  (c) 2018 Copyright Teserakt AG

#include <string.h>

#include "e4/crypto/aes256enc.h"
#include "e4/crypto/aes_siv.h"

// AES_Test triplet lifted from FIPS-197

int test_aes256()
{
    const uint8_t test_pt[16] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
    };

    const uint8_t test_ct[16] = {
        0x8E, 0xA2, 0xB7, 0xCA, 0x51, 0x67, 0x45, 0xBF,
        0xEA, 0xFC, 0x49, 0x90, 0x4B, 0x49, 0x60, 0x89
    };

    const uint8_t test_key[32] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
    };

    size_t i;
    uint8_t v[16], ek[AES256_EXPKEY_LEN];

    aes256_enc_exp_key(ek, test_key);   // expand key

    for (i = 0; i < 16; i++)
        v[i] = test_pt[i];

    aes256_encrypt_ecb(v, ek);          // encrypt

    for (i = 0; i < 16; i++) {
        if (v[i] != test_ct[i])
            return 1;                   // FAIL!
    }

    return 0;
}

#include <stdio.h>

// An AES256-SIV Test vector

int test_aes_siv()
{
    // self generated
    const uint8_t test_key[64] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08,
        0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00,
        0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9, 0xF8,
        0xF7, 0xF6, 0xF5, 0xF4, 0xF3, 0xF2, 0xF1, 0xF0,
        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
        0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
    };

    const uint8_t test_ad[24] = {
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27
    };

    const uint8_t test_pt[14] = {
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
        0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE
    };

    const uint8_t test_ivc[30] = {
        0xC4, 0xF1, 0xF8, 0x45, 0x64, 0xD0, 0xCD, 0x14,
        0x88, 0xD3, 0xC7, 0x0A, 0xB5, 0x82, 0xA4, 0x27,
        0x59, 0xAC, 0xA6, 0xC7, 0x4C, 0x6E, 0x9A, 0x10,
        0xFF, 0xAA, 0xDF, 0x4F, 0xB0, 0x49
    };

    // from crypto_test.go
    const uint8_t test_jp[80] = {
        163, 170, 113, 22,  250, 77,  249, 210, 78,  28,
        160, 45,  237, 93,  164, 200, 239, 32,  170, 161,
        67,  210, 209, 143, 206, 227, 56,  153, 89,  63,
        105, 243, 212, 68,  150, 83,  214, 188, 67,  40,
        124, 247, 11,  3,   36,  146, 111, 176, 104, 213,
        152, 36,  136, 233, 234, 238, 103, 167, 49,  182,
        211, 77,  82,  130, 240, 196, 174, 235, 101, 183,
        104, 189, 60,  240, 96,  15,  71,  147, 9,   43
    };

    uint8_t buf1[256], buf2[256];
    size_t len1, len2;
    int i, fails;

    memset(buf1, 0x55, sizeof(buf1));
    len1 = 0;
    memset(buf2, 0xAA, sizeof(buf2));
    len2 = 0;

    fails = 0;

    // encrypt test
    aes256_encrypt_siv(buf1, &len1, test_ad, 24, test_pt, 14, test_key);
    if (len1 != 30 || memcmp(buf1, test_ivc, len1) != 0) {
        fails++;
    }

    // decrypt test
    if (aes256_decrypt_siv(buf2, &len2, test_ad, 24,
                            buf1, len1, test_key)) {
        fails++;
    }

    if (len2 != 14 || memcmp(buf2, test_pt, len2) != 0) {
        fails++;
    }

    // corrupt test
    buf1[25] ^= 1;
    if (aes256_decrypt_siv(buf2, &len2, test_ad, 24,
                            buf1, len1, test_key) == 0) {
        fails++;
    }

    // encrypt test with JP's test vector
    for (i = 0; i < sizeof(buf1); i++)
        buf1[i] = i;

    len2 = 0;
    aes256_encrypt_siv(buf2, &len2, buf1, 8, buf1, 64, buf1);

    if (len2 != 80 || memcmp(buf2, test_jp, 80) != 0) {
        fails++;
    }

    return fails;
}
