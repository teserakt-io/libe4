/*
 * E4 C Library (c) 2018-2019 Copyright Teserakt AG
 *
 * 2018-05-01  Markku-Juhani O. Saarinen <markku@teserakt.io>
 * 2019-04-15  Antony Vennard <antony@teserakt.io>
 */

#ifndef _E4CLIENT_H_
#define _E4CLIENT_H_

#include <stdint.h>
#include <stddef.h>

/* E4 Library Error codes */
/** Operation succeeded without error \ingroup e4 */
#define E4ERR_Ok                    0
/** Invalid authentication tag indicates corrupted ciphertext \ingroup e4 */
#define E4ERR_InvalidTag            -101
/** Message received outside of error window for clock. \ingroup e4 */
#define E4ERR_TimestampInFuture     -102
/** Message received outside of error window for clock. \ingroup e4 */
#define E4ERR_TooOldTimestamp       -103
/** Ciphertext too short. \ingroup e4 */
#define E4ERR_TooShortCiphertext    -104
/** Unable to find key for topic; could not decrypt.\ingroup e4 */
#define E4ERR_TopicKeyMissing       -105
/** Message has already been seen. \ingroup e4 */
#define E4ERR_ReplayedMessage       -106
/** E4 Protocol command invalid. Internal error. \ingroup e4 */
#define E4ERR_InvalidCommand        -107
/** E4 Persistence layer reported an error. \ingroup e4 */
#define E4ERR_PersistenceError      -108

/** Size of the ID, truncated sha3(alias)
 \ingroup e4
 */
#define E4_ID_LEN 16

/** Secret key size
 \ingroup e4
 */
#define E4_KEY_LEN 32

/** Topic Hash Length
 \ingroup e4
 */
#define E4_TOPICHASH_LEN 32

/** Define control topic length 
 \ingroup e4
 */
#define E4_CTRLTOPIC_LEN (2*E4_ID_LEN) + 3


#define E4_MSGHDR_LEN 24

struct _e4storage;
/** This structure represents storage-specific data to be passed to the e4c 
 storage functions. It is forward-declared and implemented by the specific 
 choice of storage to be supported, which is chosen based on the configurable 
 defines.
 \ingroup e4 */
typedef struct _e4storage e4storage;

/** \brief e4c_protect_message produces a protected message for onwards transmission
   \param[out] cptr Pointer to a ciphertext buffer
   \param[in] cmax Maximum length of ciphertext buffer to be written.
   \param[out] clen Actual length of ciphertext buffer written.
   \param[in] mptr Pointer to a message (plaintext) buffer.
   \param[in] mlen Length of message to be encrypted.
   \param[in] topic Pointer to a topic hash for topic associated with this message.
   \param[inout] storage Pointer to the structure representing storage.
   \return 0 on success. Non-zero return values indicate errors.
   \ingroup e4
 */
int e4c_protect_message(uint8_t *cptr, size_t cmax, size_t *clen,
    const uint8_t *mptr, size_t mlen, const char *topic, e4storage* storage);

/** \brief e4c_unprotect_message retrieves and authenticates a message that 
was encrypted by E4.
   \param[out] mptr Pointer to ciphertext to be read
   \param[in] mmax Maximum length of plaintext buffer to which we can write.
   \param[out] mlen Actual length of plaintext buffer written by decryption.
   \param[in] cptr Pointer to ciphertext buffer
   \param[in] clen Length of ciphertext buffer
   \param[in] topic Pointer to a topic hash for topic associated with this message.
   \param[inout] storage Pointer to the structure representing storage.
   \return 0 on success. Non-zero return values indicate errors.
   \ingroup e4
 */
int e4c_unprotect_message(uint8_t *mptr, size_t mmax, size_t *mlen,
    const uint8_t *cptr, size_t clen, const char *topic, e4storage* storage);

#ifdef E4_STORE_FILE
#include "e4/internal/e4c_store_file.h"
#endif

#endif
