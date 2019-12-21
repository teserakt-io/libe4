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
    uint8_t* signaturep = NULL;
    size_t signeddatalen=0;

    /* pubkey messages are: Timestamp (8) | id (16) | IV (16) | Ciphertext (n) | sig (64) */

    if (mlen + E4_PK_TOPICMSGHDR_LEN + E4_PK_EDDSA_SIG_LEN > cmax) /* actually: not enough space */
        return E4_ERROR_CIPHERTEXT_TOO_SHORT;
    *clen = mlen + E4_MSGHDR_LEN;

    /* get the key */
    i = e4c_getindex(storage, topic);
    if (i >= 0)
    {
        e4c_gettopickey(key, storage, i);
    }
    else
    {
        /* TODO: we do not implement sending back to the C2 at present */
        return E4_ERROR_TOPICKEY_MISSING;
    }

#ifdef __AVR__
    time_now = secs1970; /* externally incremented */
#else
    time_now = time(NULL); /* timestamp */
#endif

    for (i = 0; i < 8; i++)
    {
        cptr[i] = time_now & 0xFF;
        time_now >>= 8;
    }

    /* set our ID in the output buffer
     * if Avi is reading this code, hope you are enjoying this memcpy.
     */
    memcpy(cptr + E4_TS_LEN, storage->id, E4_ID_LEN);

    /* encrypt */
    clen2 = 0;
    aes256_encrypt_siv(cptr + E4_TS_LEN + E4_ID_LEN, &clen2, cptr, 8, mptr, mlen, key);

    /* sign the result */
    signeddatalen = E4_TS_LEN + E4_ID_LEN + E4_TAG_LEN + mlen;
    signaturep = &cptr[0] + signeddatalen;

    /*
    ed25519_sign(unsigned char *signature,
                 const unsigned char *message,
                 size_t message_len,
                 const unsigned char *public_key,
                 const unsigned char *private_key);
     */
    ed25519_sign(signaturep,
                 cptr,
                 signeddatalen,
                 storage->pubkey,
                 storage->privkey);

    return E4_RESULT_OK;
}

/* Unprotect message */
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

    uint8_t* assocdata;
    size_t assocdatalen = 0;
    size_t sivpayloadlen = 0;

#ifndef __AVR__
    uint64_t secs1970;

    secs1970 = (uint64_t)time(NULL); /* this system has a RTC */
#endif

    /* There are two possible message formats:

       From other clients: Timestamp (8) | id (16) | IV (16) | Ciphertext (n) | sig (64)
       From the C2:        Timestamp (8) | IV (16) | Ciphertext (n)
    */

    assocdata = cptr;

    if (e4c_is_device_ctrltopic(storage, topic) == 0)
    {
        uint8_t c2pk[E4_PK_X25519_PUBKEY_LEN];
        uint8_t devicesk[E4_PK_X25519_PUBKEY_LEN];
        uint8_t sharedpoint[E4_PK_X25519_PUBKEY_LEN];
        /* control topic being used: */
        control = 1;

        /* bounds checking */
        if (clen < E4_MSGHDR_LEN || mmax < clen - E4_MSGHDR_LEN)
        {
            return E4_ERROR_CIPHERTEXT_TOO_SHORT;
        }

        e4c_get_c2_pubkey(store, c2pk);

        /* convert our key to X25519 */
        xed25519_convert_ed2c_private(devicesk, store->privkey);

        /* key=sha3(X25519(c2, device)) */

        curve25519(sharedpoint, devicesk, c2pk);
        sha3(sharedpoint, sizeof sharedpoint, key, sizeof key);

        /* set things up for symmetric decryption: */
        /* From the C2:        Timestamp (8) | IV (16) | Ciphertext (n) */
        assocdatalen = E4_TS_LEN;
        sivpayloadlen = clen - E4_TS_LEN;
    }
    else
    {
        int i;
        uint8_t sender_pk[E4_PK_EDDSA_PUBKEY_LEN];
        uint8_t* idptr = cptr[8];

        control = 0;

        /* bounds checking */
        if (clen < E4_PKTOPICMSGHDR_LEN+E4_PK_EDDSA_SIG_LEN ||
            mmax < clen - (E4_PKTOPICMSGHDR_LEN+E4_PK_EDDSA_SIG_LEN))
        {
            return E4_ERROR_CIPHERTEXT_TOO_SHORT;
        }

        i = e4c_getdeviceindex(store, idptr);
        if (i >= 0)
        {
            e4c_getdevicekey(sender_pk, store, i);
        }
        else
        {
            /* TODO: policies to not validate sigs if key not found
               due to storage constraints.

               NOTE: this MUST NOT be conflated with signature verif
               failure. If signatures fail to verify we will _always_
               reject the message.
             */

            return E4_ERROR_DEVICEPK_MISSING;
        }

        /* check signature attached to end */
        signverifresult = ed25519_verify(cptr[clen-E4_PK_EDDSA_SIG_LEN],
                cptr, clen-E4_PK_EDDSA_SIG_LEN, sender_pk);

        if (signverifresult != 1)
        {
            return E4_ERROR_PK_SIGVERIF_FAILED;
        }

        /* find the topic key and set it */
        i = e4c_getindex(storage, topic);
        if (i >= 0)
        {
            e4c_gettopickey(key, storage, i);
        }
        else
        {
            return E4_ERROR_TOPICKEY_MISSING;
        }

        /* set things up for symmetric decryption: */
        /* From other clients: Timestamp (8) | id (16) | IV (16) | Ciphertext (n) | sig (64) */
        assocdatalen = E4_ID_LEN + E4_TS_LEN;
        sivpayloadlen = clen - (E4_TS_LEN + E4_ID_LEN + E4_PK_EDDSA_SIG_LEN);
    }

    /* Retrieve timestamp encoded as little endian */
    tstamp = 0;
    for (j = 7; j >= 0; j--)
    {
        tstamp <<= 8;
        tstamp += (uint64_t)cptr[j];
    }

    /* decrypt
       int aes256_decrypt_siv(uint8_t *pt,
                       size_t *ptlen, /-* out: plaintext *-/
                       const uint8_t *ad,
                       size_t adlen, /-* in: associated data / nonce *-/
                       const uint8_t *ct,
                       size_t ctlen,        /-* in: ciphertext *-/
                       const uint8_t *key); /-* in: secret key (32 bytes) *-/
     */

    if (aes256_decrypt_siv(mptr, mlen, assocdata, assocdatalen,
                cptr + assocdatalen, sivpayloadlen, key) != 0)
    {
        return E4_ERROR_INVALID_TAG;
    }

    /* TODO: this is only valuable for string-type data
     * we should consider removing it, as it requires that
     * the plaintext buffer be 1 byte bigger than that which was
     * encrypted, which is very unnecessary. */
    if (*mlen + 1 > mmax) /* zero-pad it in place. */
        return E4_ERROR_CIPHERTEXT_TOO_SHORT;
    mptr[*mlen] = 0;


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
        r = e4c_set_idkey(storage, mptr + 1);
        return r == 0 ? E4_RESULT_OK_CONTROL : r;

    case 0x03: /* SetTopicKey(topic, key) */
        if (*mlen != (1 + E4_KEY_LEN + E4_ID_LEN))
            return E4_ERROR_INVALID_COMMAND;
        r = e4c_set_topic_key(storage, (const uint8_t *)mptr + E4_KEY_LEN + 1, mptr + 1);
        return r == 0 ? E4_RESULT_OK_CONTROL : r;
    case 0x04: /* RemovePubKey(id) */
        if (*mlen != (1 + E4_ID_LEN))
        r = e4c_remove_devices(storage, mptr+1);
        return r == 0 ? E4_RESULT_OK_CONTROL : r;
    case 0x05: /* RemovePubKeys() */
        if (*mlen != 1) return E4_ERROR_INVALID_COMMAND;
        r = e4c_reset_devices(storage);
        return r == 0 ? E4_RESULT_OK_CONTROL : r;
    case 0x06: /* SetPubKey(pubkey, id) */
        if (*mlen != (1 + E4_ID_LEN + E4_PK_EDDSA_PUBKEY_LEN))
            return E4_ERROR_INVALID_COMMAND;
        r = e4c_set_device_key(storage, mptr+1+E4_PK_EDDSA_PUBKEY_LEN, mptr+1);
        return r == 0 ? E4_RESULT_OK_CONTROL : r;
    }

    return E4_ERROR_INVALID_COMMAND;
}
