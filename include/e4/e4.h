/**
 * \brief E4 main header, provides access to the E4 API.
 * @ingroup group_e4
 *
 * 2018-05-01  Markku-Juhani O. Saarinen <markku@teserakt.io>
 * 2019-04-15  Antony Vennard <antony@teserakt.io>
 *  (c) 2018-2019 Copyright Teserakt AG
 */

#ifndef _E4CLIENT_H_
#define _E4CLIENT_H_

#include <stdint.h>
#include <stddef.h>

/** E4 Library Error codes */
#define E4ERR_Ok                    0
#define E4ERR_InvalidTag            -101
#define E4ERR_TimestampInFuture     -102
#define E4ERR_TooOldTimestamp       -103
#define E4ERR_TooShortCiphertext    -104
#define E4ERR_TopicKeyMissing       -105
#define E4ERR_ReplayedMessage       -106
#define E4ERR_InvalidCommand        -107
#define E4ERR_PersistenceError      -108

/** Size of the ID, truncated sha3(alias) */
#define E4_ID_LEN 16

/** Secret key size */
#define E4_KEY_LEN 32

/** Topic Hash Length */
#define E4_TOPICHASH_LEN 32


/** Define control topic length */
#define E4_CTRLTOPIC_LEN (2*E4_ID_LEN) + 3

/* Forward-declared struct and typedef
   We do this to avoid the use of 
   function pointers.

   Testing for this is done by compiling the library with each possible define.
*/
struct _e4storage;
typedef struct _e4storage e4storage;

/** \brief e4c_protect_message produces a protected message for onwards transmission
   \param cptr Pointer to a ciphertext buffer
   \param cmax
   \param clen
   \param mptr Pointer to a message (plaintext) buffer.
   \param mlen Length of message to be encrypted.
   \param topic Pointer to a topic hash?
   \return 0 on success. Non-zero return values indicate errors.
 */
int e4c_protect_message(uint8_t *cptr, size_t cmax, size_t *clen,
    const uint8_t *mptr, size_t mlen, const char *topic, e4storage* storage);

/** \brief e4c_unprotect_message retrieves and authenticates a message that 
was encrypted by E4.
   \param cptr Pointer to ciphertext to be read
   \param cmax
   \param clen
   \param mptr Pointer to plaintext message to be written out.
   \param mlen
   \param topic
   \return 0 on success. Non-zero return values indicate errors.
 */
int e4c_unprotect_message(uint8_t *mptr, size_t mmax, size_t *mlen,
    const uint8_t *cptr, size_t clen, const char *topic, e4storage* storage);

#ifdef E4_STORE_FILE
#include "e4/internal/e4c_store_file.h"
#endif

#endif
