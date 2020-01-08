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

#include "e4/crypto/sha3.h"
#include "e4/e4.h"
#include <e4/stdint.h>
#include <e4/util.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* #ifdef DEBUG */

/* #endif */

void zeroize( void *v, size_t n )
{
    volatile unsigned char *p = ( volatile unsigned char * )v;
    while( n-- ) *p++ = 0;
}

int e4c_derive_clientid(uint8_t *clientid, const size_t clientidlen, const char *clientname, const size_t clientnamelen)
{
    if (clientidlen != E4_ID_LEN ) {
        return E4_ID_LEN;
    }
    sha3_256_trunc((char*)clientid, clientidlen, clientname, clientnamelen);
    return 0;
}

int e4c_derive_control_topic(char *topic, const size_t topiclen, const uint8_t *clientid)
{

    int i = 0;

    if (topiclen < E4_CTRLTOPIC_LEN + 1)
    {
        return E4_CTRLTOPIC_LEN + 1;
    }

#if __STDC_VERSION__ >= 199901L
    snprintf(topic, topiclen, "e4/");
#else
    if ( topiclen < 3 ) {
        return -1;
    }
    sprintf(topic, "e4/");
#endif
    for (i = 0; i < E4_ID_LEN; i++)
    {
        const size_t adjust = 3 + 2 * i;
    #if __STDC_VERSION__ >= 199901L
        snprintf((char *)(topic + adjust), topiclen - adjust, "%02x", clientid[i]);
    #else
        if ( adjust >= topiclen ) {
            return -1;
        }
        sprintf((char *)(topic + adjust), "%02x", clientid[i]);
    #endif
    }

    return 0;
}

/* Computes the topichash, which is currently SHA3-256/128. */
int e4c_derive_topichash(uint8_t* topichash, const size_t topichash_len, const char* topic) {
    size_t topiclen = strlen(topic);
    if (topichash_len != E4_ID_LEN ) {
        return E4_ID_LEN;
    }
    sha3_256_trunc((char*)topichash, topichash_len, topic, topiclen);
    return 0;
}

INLINE int8_t from_hex(const char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'a' && c <= 'f') {
        return (c - 'a') + 10;
    } else if (c >= 'A' && c <= 'F') {
        return (c - 'A') + 10;
    } else {
        return -1;
    }
}


int e4c_hex_decode(char *dst, const size_t dst_len, const char *encoded, const size_t encoded_len)
{
    char *walk;
    int i;
    size_t decoded_len;
    int8_t h1, h2;

    /* can't decode empty string; can't decode odd bytes, */
    if (encoded_len == 0 || encoded_len % 2 != 0)
    {
        printf("e4c_hex_decode: encoded_len=%ld but must be non-zero and even.\n", encoded_len);
        return 0;
    }

    decoded_len = encoded_len >> 1;
    if (decoded_len > dst_len)
    {
        printf("e4c_hex_decode: decoded_len is wrong. r=%ld, hl=%ld, bl=%ld\n",
                                                decoded_len, encoded_len, dst_len);
        return 0;
    }

    walk = dst;
    for (i = 0; i < encoded_len; i += 2)
    {
        if ((h1 = from_hex(encoded[i])) < 0)
            return 0;
        if ((h2 = from_hex(encoded[i+1])) < 0)
            return 0;
        *walk++ = (char)(h1<<4 | h2);
    }

    return i;
}
