/*
 * E4 C Library (c) 2018-2019 Copyright Teserakt AG
 *
 * 2018-05-01  Markku-Juhani O. Saarinen <markku@teserakt.io>
 * 2019-04-15  Antony Vennard <antony@teserakt.io>
 */

#ifndef _E4CLIENT_H_
#define _E4CLIENT_H_

#include <stddef.h>
#include <stdint.h>

/* E4 Library Error codes */
/** Operation succeeded without error \ingroup e4 */
#define E4_RESULT_OK 0
/** A control message was handled. Applications may discard the resulting buffer */
#define E4_RESULT_OK_CONTROL -1
/** Invalid authentication tag indicates corrupted ciphertext \ingroup e4 */
#define E4_ERROR_INVALID_TAG -101
/** Message received outside of error window for clock. \ingroup e4 */
#define E4_ERROR_TIMESTAMP_IN_FUTURE -102
/** Message received outside of error window for clock. \ingroup e4 */
#define E4_ERROR_TIMESTAMP_TOO_OLD -103
/** Ciphertext too short. \ingroup e4 */
#define E4_ERROR_CIPHERTEXT_TOO_SHORT -104
/** Unable to find key for topic; could not decrypt.\ingroup e4 */
#define E4_ERROR_TOPICKEY_MISSING -105
/** Message has already been seen. \ingroup e4 */
#define E4_ERROR_MESSAGE_REPLAYED -106
/** E4 Protocol command invalid. Internal error. \ingroup e4 */
#define E4_ERROR_INVALID_COMMAND -107
/** E4 Persistence layer reported an error. \ingroup e4 */
#define E4_ERROR_PERSISTENCE_ERROR -108

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
#define E4_TOPICHASH_LEN 16

/** Define control topic length
 \ingroup e4
 */
#define E4_CTRLTOPIC_LEN (2 * E4_ID_LEN) + 3

#define E4_TAG_LEN 16
#define E4_TIMESTAMP_LEN 8

#define E4_MSGHDR_LEN (E4_TAG_LEN + E4_TIMESTAMP_LEN)

struct _e4storage;
/** This structure represents storage-specific data to be passed to the e4c
 storage functions. It is forward-declared and implemented by the specific
 choice of storage to be supported, which is chosen based on the configurable
 defines.
 \ingroup e4 */
typedef struct _e4storage e4storage;

/** \brief e4c_protect_message produces a protected message for onwards
   transmission \param[out] cptr Pointer to a ciphertext buffer \param[in] cmax
   Maximum length of ciphertext buffer to be written. \param[out] clen Actual
   length of ciphertext buffer written. \param[in] mptr Pointer to a message
   (plaintext) buffer. \param[in] mlen Length of message to be encrypted.
   \param[in] topic Pointer to a topic hash for topic associated with this
   message. \param[inout] storage Pointer to the structure representing storage.
   \return 0 on success. Non-zero return values indicate errors. \ingroup e4
 */
int e4c_protect_message(uint8_t *cptr,
                        size_t cmax,
                        size_t *clen,
                        const uint8_t *mptr,
                        size_t mlen,
                        const char *topic,
                        e4storage *storage);

/** \brief e4c_unprotect_message retrieves and authenticates a message that
was encrypted by E4.
   \param[out] mptr Pointer to ciphertext to be read
   \param[in] mmax Maximum length of plaintext buffer to which we can write.
   \param[out] mlen Actual length of plaintext buffer written by decryption.
   \param[in] cptr Pointer to ciphertext buffer
   \param[in] clen Length of ciphertext buffer
   \param[in] topic Pointer to a topic hash for topic associated with this
message. \param[inout] storage Pointer to the structure representing storage.
\return 0 on success. Non-zero return values indicate errors. \ingroup e4
 */
int e4c_unprotect_message(uint8_t *mptr,
                          size_t mmax,
                          size_t *mlen,
                          const uint8_t *cptr,
                          size_t clen,
                          const char *topic,
                          e4storage *storage);


/** the e4storage type pre-defined above implements these API calls */
int e4c_init(e4storage *store);
int e4c_set_storagelocation(e4storage *store, const char *path);
int e4c_load(e4storage *store, const char *path);
int e4c_sync(e4storage *store);
int e4c_set_id(e4storage *store, const uint8_t *id);
int e4c_set_idkey(e4storage *store, const uint8_t *key);
int e4c_is_device_ctrltopic(e4storage *store, const char *topic);
int e4c_getindex(e4storage *store, const char *topic);
int e4c_gettopickey(uint8_t *key, e4storage *store, const int index);
int e4c_set_topic_key(e4storage *store, const uint8_t *topic_hash, const uint8_t *key);
int e4c_remove_topic(e4storage *store, const uint8_t *topic_hash);
int e4c_reset_topics(e4storage *store);

//#ifdef DEBUG
void e4c_debug_print(e4storage *store);
//#endif

#ifdef E4_STORE_FILE
#include "e4/internal/e4c_store_file.h"
#endif

#endif
