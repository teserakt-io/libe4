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

#ifndef E4UTIL_H
#define E4UTIL_H
#ifdef __cplusplus
extern "C" {
#endif

#ifndef ZERO
#define ZERO(X)                   \
    do                            \
    {                             \
        zeroize(&X, sizeof(X));    \
    } while (0)
#endif

#if __STDC_VERSION__ >= 199901L
#define INLINE static inline
#else
#define INLINE static
#endif

/* void zeroize */
void zeroize(void *v, size_t n);

/* check buffer is zero */
size_t zerocheck(void* v, size_t n);

/* Derives a ClientID. Depends on the constant E4_ID_LEN */
int e4c_derive_clientid(uint8_t *clientid, const size_t clientidlen, const char *clientname, const size_t clientnamelen);

/* Derives a TopicHash. Depends on the constant E4_TOPICHASH_LEN */
int e4c_derive_topichash(uint8_t* topichash, const size_t topichash_len, const char* topic);

/* Produces a control topic from a E4_ID_LEN byte clientid. Length of
 * clientid is assumed to be E4_ID_LEN bytes. Control topics are
 * e4/<hex of clientid>. */
int e4c_derive_control_topic(char *topic, const size_t topiclen, const uint8_t *clientid);

int e4c_hex_decode(char *bytes, const size_t byteslen, const char *hexstring, const size_t hexstringlen);
#ifdef __cplusplus
}
#endif /* C++ */
#endif /* E4UTIL_H */
