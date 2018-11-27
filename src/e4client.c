//  e4client.c
//  2018-05-01  Markku-Juhani O. Saarinen <markku@teserakt.io>
//  (c) 2018 Copyright Teserakt AG

#include <string.h>
#ifndef __AVR__
#include <stdio.h>
#include <time.h>
#endif

#include "e4/e4.h"

#include "sha3.h"
#include "aes_siv.h"

#ifdef __AVR__
// Wall-clock time, incremented exernally, calibrated from control message
uint64_t secs1970 = 0;
#endif

// Currently allow 10 min slack in either direction in timestamps
#define E4C_TIME_FUTURE   (10 * 60)
#define E4C_TIME_TOO_OLD  (10 * 60)

// Protect message

int e4c_protect_message(uint8_t *cptr, size_t cmax, size_t *clen,
    const uint8_t *mptr, size_t mlen, const char *topic)
{
    int i;
    size_t clen2;
    uint8_t key[E4C_KEY_LEN];
    uint64_t time_now = 0;

    if (mlen + 24 > cmax)                   // actually: not enough space
        return E4ERR_TooShortCiphertext;
    *clen = mlen + 24;

    // get the key
    /*i = e4c_getindex(topic);
    if (i < 0)
        return i;
    e4c_getkey(key, i);*/

#ifdef __AVR__
    time_now = secs1970;                    // externally incremented
#else
    time_now = time(NULL);                  // timestamp
#endif

    for (i = 0; i < 8; i++) {
        cptr[i] = time_now & 0xFF;
        time_now >>= 8;
    }

    // encrypt
    clen2 = 0;
    aes256_encrypt_siv(cptr + 8, &clen2, cptr, 8, mptr, mlen, key);

    return 0;
}


// Unprotect message

int e4c_unprotect_message(uint8_t *mptr, size_t mmax, size_t *mlen,
    const uint8_t *cptr, size_t clen, const char *topic)
{
    int i, j;
    uint8_t key[E4C_KEY_LEN];
    uint64_t tstamp;
#ifndef __AVR__
    uint64_t secs1970;

    secs1970 = (uint64_t) time(NULL);       // this system has a RTC
#endif

    // bounds checking
    if (clen < 24 || mmax < clen - 24)
        return E4ERR_TooShortCiphertext;

    // get the key
    /*i = e4c_getindex(topic);
    if (i < 0)
        return i;
    e4c_getkey(key, i);*/

    // check timestamp
    tstamp = 0;
    for (j = 7; j >= 0; j--) {
        tstamp <<= 8;
        tstamp += (uint64_t) cptr[j];
    }

    // decrypt
    if (aes256_decrypt_siv(mptr, mlen, cptr, 8, cptr + 8, clen - 8,  key) != 0)
        return E4ERR_InvalidTag;

    if (*mlen + 1 > mmax)                   // zero-pad it in place..
        return E4ERR_TooShortCiphertext;
    mptr[*mlen] = 0;


    // Since AVR has no real time clock, time is initially unknown. 
    if (secs1970 < 946684800) {
        if (i == 0)                         // command channel ?
            secs1970 = tstamp;              // calibrate message
    } else {

        if (tstamp >= secs1970) {
            if (tstamp - secs1970 > E4C_TIME_FUTURE)
                return E4ERR_TimestampInFuture;
        } else {
            if (secs1970 - tstamp > E4C_TIME_TOO_OLD)
                return E4ERR_TooOldTimestamp; 
        }
    }

    if (i != 0)                             // if not command, success
        return 0;

    // execute commands

    if (*mlen == 0)
        return E4ERR_InvalidCommand;

    switch (mptr[0]) {
        case 0x00:                          // RemoveTopic(topic);
            return e4c_remove_topic((const uint8_t *) mptr + 1);

        case 0x01:                          // ResetTopics();
            if (*mlen != 1)
                return E4ERR_InvalidCommand;
            return e4c_reset_topics();

        case 0x02:                          // SetIdKey(key)
            if (*mlen != (1 + E4C_KEY_LEN))
                return E4ERR_InvalidCommand;
            return e4c_set_id_key(NULL, mptr + 1);

        case 0x03:                          // SetTopicKey(topic, key)
            if (*mlen != (1 + E4C_KEY_LEN + E4C_TOPIC_LEN))
                return E4ERR_InvalidCommand;
            return e4c_set_topic_key((const uint8_t *) 
                mptr + E4C_KEY_LEN + 1, mptr + 1);
    }

    return E4ERR_InvalidCommand;
}

