/* e4client.c
 * 2018-05-01  Markku-Juhani O. Saarinen <markku@teserakt.io>
 * 2018-12-01  Antony Vennard <antony@teserakt.io
 * (c) 2018-2019 Copyright Teserakt AG
 */

#include <string.h>
#ifndef __AVR__
#include <stdio.h>
#include <time.h>
#endif

#include "e4/e4.h"

#include "e4/crypto/aes_siv.h"
#include "e4/crypto/sha3.h"

#ifdef __AVR__
// Wall-clock time, incremented exernally, calibrated from control message
uint64_t secs1970 = 0;
#endif

// Currently allow 10 min slack in either direction in timestamps
#define E4C_TIME_FUTURE (10 * 60)
#define E4C_TIME_TOO_OLD (10 * 60)

// Protect message
int e4c_protect_message(uint8_t *cptr,
                        size_t cmax,
                        size_t *clen,
                        const uint8_t *mptr,
                        size_t mlen,
                        const char *topic,
                        e4storage *storage)
{
    int i = 0;
    size_t clen2 = 0;
    uint8_t key[E4_KEY_LEN];
    uint64_t time_now = 0;

    if (mlen + E4_MSGHDR_LEN > cmax) // actually: not enough space
        return E4_ERROR_CIPHERTEXT_TOO_SHORT;
    *clen = mlen + E4_MSGHDR_LEN;

    // get the key
    i = e4c_getindex(storage, topic);
    if (i >= 0)
    {
        e4c_gettopickey(key, storage, i);
    }
    else
    {
        if (e4c_is_device_ctrltopic(storage, topic) != 0)
        {
            return E4_ERROR_TOPICKEY_MISSING;
        }
        // control topic being used:
        memcpy(key, storage->key, E4_KEY_LEN);
    }

#ifdef __AVR__
    time_now = secs1970; // externally incremented
#else
    time_now = time(NULL); // timestamp
#endif

    for (i = 0; i < 8; i++)
    {
        cptr[i] = time_now & 0xFF;
        time_now >>= 8;
    }

    // encrypt
    clen2 = 0;
    aes256_encrypt_siv(cptr + 8, &clen2, cptr, 8, mptr, mlen, key);

    return E4_RESULT_OK;
}


// Unprotect message

int e4c_unprotect_message(uint8_t *mptr,
                          size_t mmax,
                          size_t *mlen,
                          const uint8_t *cptr,
                          size_t clen,
                          const char *topic,
                          e4storage *storage)
{
    uint8_t control = 0;
    int i = 0, j = 0, r = 0;
    uint8_t key[E4_KEY_LEN];
    uint64_t tstamp;
#ifndef __AVR__
    uint64_t secs1970;

    secs1970 = (uint64_t)time(NULL); // this system has a RTC
#endif

    // bounds checking

    if (clen < E4_MSGHDR_LEN || mmax < clen - E4_MSGHDR_LEN)
    {
        return E4_ERROR_CIPHERTEXT_TOO_SHORT;
    }

    // get the key
    i = e4c_getindex(storage, topic);
    if (i >= 0)
    {
        e4c_gettopickey(key, storage, i);
    }
    else
    {
        if (e4c_is_device_ctrltopic(storage, topic) != 0)
        {
            return E4_ERROR_TOPICKEY_MISSING;
        }
        // control topic being used:
        memcpy(key, storage->key, E4_KEY_LEN);
        control = 1;
    }

    // check timestamp
    tstamp = 0;
    for (j = 7; j >= 0; j--)
    {
        tstamp <<= 8;
        tstamp += (uint64_t)cptr[j];
    }

    // decrypt
    if (aes256_decrypt_siv(mptr, mlen, cptr, 8, cptr + 8, clen - 8, key) != 0)
    {
        return E4_ERROR_INVALID_TAG;
    }

    // TODO: this is only valuable for string-type data
    // we should consider removing it, as it requires that
    // the plaintext buffer be 1 byte bigger than that which was
    // encrypted, which is very unnecessary.
    if (*mlen + 1 > mmax) // zero-pad it in place..
        return E4_ERROR_CIPHERTEXT_TOO_SHORT;
    mptr[*mlen] = 0;


    // Since AVR has no real time clock, time is initially unknown.
    if (secs1970 < 946684800)
    {
        if (i == 0)            // command channel ?
            secs1970 = tstamp; // calibrate message
    }
    else
    {

        if (tstamp >= secs1970)
        {
            if (tstamp - secs1970 > E4C_TIME_FUTURE)
                return E4_ERROR_TIMESTAMP_IN_FUTURE;
        }
        else
        {
            if (secs1970 - tstamp > E4C_TIME_TOO_OLD)
                return E4_ERROR_TIMESTAMP_TOO_OLD;
        }
    }

    // if not control channel, we can exit now; no command to process.
    if (!(control)) return E4_RESULT_OK;

    // execute commands

    if (*mlen == 0) return E4_ERROR_INVALID_COMMAND;

    switch (mptr[0])
    {
    case 0x00: // RemoveTopic(topic);
        r = e4c_remove_topic(storage, (const uint8_t *)mptr + 1);
        return r == 0 ? E4_RESULT_OK_CONTROL : r;

    case 0x01: // ResetTopics();
        if (*mlen != 1) return E4_ERROR_INVALID_COMMAND;
        r = e4c_reset_topics(storage);
        return r == 0 ? E4_RESULT_OK_CONTROL : r;

    case 0x02: // SetIdKey(key)
        if (*mlen != (1 + E4_KEY_LEN)) return E4_ERROR_INVALID_COMMAND;
        r = e4c_set_idkey(storage, mptr + 1);
        return r == 0 ? E4_RESULT_OK_CONTROL : r;

    case 0x03: // SetTopicKey(topic, key)
        if (*mlen != (1 + E4_KEY_LEN + E4_ID_LEN))
            return E4_ERROR_INVALID_COMMAND;
        r = e4c_set_topic_key(storage, (const uint8_t *)mptr + E4_KEY_LEN + 1, mptr + 1);
        return r == 0 ? E4_RESULT_OK_CONTROL : r;
    }

    return E4_ERROR_INVALID_COMMAND;
}
