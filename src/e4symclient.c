/*
 * Teserakt AG LIBE4 C Library
 *
 * Copyright 2018-2020 Teserakt AG, Lausanne, Switzerland
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
/* Wall-clock time, incremented exernally, calibrated from control message */
uint64_t secs1970 = 0;
#endif

/* Currently allow 10 min slack in either direction in timestamps */
#define E4C_TIME_FUTURE (10 * 60)
#define E4C_TIME_TOO_OLD (10 * 60)

/* Protect message */
int e4c_symkey_protect_message(uint8_t *cptr,
                        size_t cmax,
                        size_t *clen,
                        const uint8_t *mptr,
                        size_t mlen,
                        const char *topic,
                        e4storage *storage,
                        const uint32_t proto_opts)
{
    int i = 0;
    uint8_t key[E4_KEY_LEN];
    uint64_t time_now = 0;

    if (mlen + E4_MSGHDR_LEN > cmax) /* actually: not enough space */
        return E4_ERROR_CIPHERTEXT_TOO_SHORT;
    if (mlen + E4_MSGHDR_LEN < mlen) /* overflow */
        return E4_ERROR_PARAMETER_OVERFLOW;
    
    if (cptr == NULL ||
        mptr == NULL ||
        topic == NULL ||
        storage == NULL)
    {
        return E4_ERROR_PARAMETER_INVALID;
    }

    /* get the key */
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
        /* control topic being used: */
        e4c_get_idsymkey(storage, key);
    }

#ifdef __AVR__
    time_now = secs1970; /* externally incremented */
#else
    time_now = time(NULL); /* timestamp */
#endif

    for (i = 0; i < E4_TIMESTAMP_LEN; i++)
    {
        cptr[i] = time_now & 0xFF;
        time_now >>= 8;
    }

    /* encrypt */
    aes256_encrypt_siv(cptr + E4_TIMESTAMP_LEN, clen, cptr, E4_TIMESTAMP_LEN, mptr, mlen, key);
    /* add associated data */
    *clen += E4_TIMESTAMP_LEN;

    /* safety check */
    if (*clen != mlen + E4_MSGHDR_LEN) {
        return E4_ERROR_INTERNAL;
    }

    return E4_RESULT_OK;
}


/* Unprotect message */

int e4c_symkey_unprotect_message(uint8_t *mptr,
                          size_t mmax,
                          size_t *mlen,
                          const uint8_t *cptr,
                          size_t clen,
                          const char *topic,
                          e4storage *storage,
                          const uint32_t proto_opts)
{
    uint8_t control = 0;
    int i = 0, j = 0, r = 0;
    uint8_t key[E4_KEY_LEN];
    uint64_t tstamp;
#ifndef __AVR__
    uint64_t secs1970;

    secs1970 = (uint64_t)time(NULL); /* this system has a RTC */
#endif

    /* bounds checking */

    if (clen < E4_MSGHDR_LEN || mmax < clen - E4_MSGHDR_LEN)
    {
        return E4_ERROR_CIPHERTEXT_TOO_SHORT;
    }
    
    if (cptr == NULL ||
        mptr == NULL ||
        topic == NULL ||
        storage == NULL)
    {
        return E4_ERROR_PARAMETER_INVALID;
    }


    /* get the key */
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
        /* control topic being used: */
        e4c_get_idsymkey(storage, key);
        control = 1;
    }

    /* check timestamp */
    tstamp = 0;
    for (j = 7; j >= 0; j--)
    {
        tstamp <<= 8;
        tstamp += (uint64_t)cptr[j];
    }

    /* decrypt */
    if (aes256_decrypt_siv(mptr, mlen, cptr, E4_TIMESTAMP_LEN, cptr + E4_TIMESTAMP_LEN, clen - E4_TIMESTAMP_LEN, key) != 0)
    {
        return E4_ERROR_INVALID_TAG;
    }

    if (*mlen > mmax) 
        return E4_ERROR_CIPHERTEXT_TOO_SHORT;

    /* Since AVR has no real time clock, time is initially unknown. */
    if (secs1970 < 946684800)
    {
        /* calibrate message if this is a control message */
        if (control) {         
            secs1970 = tstamp; 
        }
    }
    else
    {
        if (!(proto_opts & E4_OPTION_IGNORE_TIMESTAMP)) {   
            if (tstamp >= secs1970)
            {
                if (tstamp - secs1970 > E4C_TIME_FUTURE)
                {
                    return E4_ERROR_TIMESTAMP_IN_FUTURE;
                }
            }
            else
            {
                if (secs1970 - tstamp > E4C_TIME_TOO_OLD)
                {
                    return E4_ERROR_TIMESTAMP_TOO_OLD;
                }
            }
        }
    }

    /* if not control channel, we can exit now; no command to process. */
    if (!(control)) return E4_RESULT_OK;

    /* execute commands */

    if (*mlen == 0) return E4_ERROR_INVALID_COMMAND;

    switch (mptr[0])
    {
    case 0x00: /* RemoveTopic(topic); */
        r = e4c_remove_topic(storage, (const uint8_t *)mptr + 1);
        return r == 0 ? E4_RESULT_OK_CONTROL : r;

    case 0x01: /* ResetTopics(); */
        if (*mlen != 1) return E4_ERROR_INVALID_COMMAND;
        r = e4c_reset_topics(storage);
        return r == 0 ? E4_RESULT_OK_CONTROL : r;

    case 0x02: /* SetIdKey(key) */
        if (*mlen != (1 + E4_KEY_LEN)) return E4_ERROR_INVALID_COMMAND;
        r = e4c_set_idsymkey(storage, mptr + 1);
        return r == 0 ? E4_RESULT_OK_CONTROL : r;

    case 0x03: /* SetTopicKey(topic, key) */
        if (*mlen != (1 + E4_KEY_LEN + E4_TOPICHASH_LEN))
            return E4_ERROR_INVALID_COMMAND;
        r = e4c_set_topic_key(storage, (const uint8_t *)mptr + E4_KEY_LEN + 1, mptr + 1);
        return r == 0 ? E4_RESULT_OK_CONTROL : r;
    }

    return E4_ERROR_INVALID_COMMAND;
}

#ifndef E4_MODE_ALL

int e4c_protect_message(uint8_t *cptr,
    size_t cmax,
    size_t *clen,
    const uint8_t *mptr,
    size_t mlen,
    const char *topic,
    e4storage *storage,
    const uint32_t proto_opts
) __attribute__((alias ("e4c_symkey_protect_message")));

int e4c_unprotect_message(uint8_t *mptr,
    size_t mmax,
    size_t *mlen,
    const uint8_t *cptr,
    size_t clen,
    const char *topic,
    e4storage *storage,
    const uint32_t proto_opts
) __attribute__((alias ("e4c_symkey_unprotect_message")));


#endif

