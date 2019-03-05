//  e4client.h
//  2018-05-01  Markku-Juhani O. Saarinen <markku@teserakt.io>

//  (c) 2018 Copyright Teserakt AG

#ifndef _E4CLIENT_H_
#define _E4CLIENT_H_

#include <stdint.h>
#include <stddef.h>

// Error codes
#define E4ERR_Ok                    0
#define E4ERR_InvalidTag            -101
#define E4ERR_TimestampInFuture     -102
#define E4ERR_TooOldTimestamp       -103
#define E4ERR_TooShortCiphertext    -104
#define E4ERR_TopicKeyMissing       -105
#define E4ERR_ReplayedMessage       -106
#define E4ERR_InvalidCommand        -107
#define E4ERR_PersistenceError      -108

// Size of the topic hash
#define E4C_TOPIC_LEN 32

// Secret key size
#define E4C_KEY_LEN 64

// == Local Client API ==

// Protect message mptr[mlen] and place it to cptr[cmax] (length in *clen)
int e4c_protect_message(uint8_t *cptr, size_t cmax, size_t *clen,
    const uint8_t *mptr, size_t mlen, const char *topic);

// Unprotect message cptr[clen] and place it to mptr[mmax] (length in *mlen)
int e4c_unprotect_message(uint8_t *mptr, size_t mmax, size_t *mlen,
    const uint8_t *cptr, size_t clen, const char *topic);

#ifdef E4_STORE_FILE
#include "e4/internal/e4c_store_file.h"
#endif

#endif
