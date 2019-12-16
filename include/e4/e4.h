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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _E4CLIENT_H_
#define _E4CLIENT_H_

#include <stddef.h>
#include <stdint.h>

/* E4 Library Error codes */
/* Operation succeeded without error  */
#define E4_RESULT_OK 0
/* A control message was handled. Applications may discard the resulting buffer */
#define E4_RESULT_OK_CONTROL -1
/* Invalid authentication tag indicates corrupted ciphertext  */
#define E4_ERROR_INVALID_TAG -101
/* Message received outside of error window for clock.  */
#define E4_ERROR_TIMESTAMP_IN_FUTURE -102
/* Message received outside of error window for clock.  */
#define E4_ERROR_TIMESTAMP_TOO_OLD -103
/* Ciphertext too short.  */
#define E4_ERROR_CIPHERTEXT_TOO_SHORT -104
/* Unable to find key for topic; could not decrypt. */
#define E4_ERROR_TOPICKEY_MISSING -105
/* Message has already been seen.  */
#define E4_ERROR_MESSAGE_REPLAYED -106
/* E4 Protocol command invalid. Internal error.  */
#define E4_ERROR_INVALID_COMMAND -107
/* E4 Persistence layer reported an error.  */
#define E4_ERROR_PERSISTENCE_ERROR -108

/* Unable to find public key for device;e4 */
#define E4_ERROR_DEVICEPK_MISSING -109

/* Size of the ID, truncated sha3(alias) */
#define E4_ID_LEN 16

/* Secret key size */
#define E4_KEY_LEN 32

/* Topic Hash Length */
#define E4_TOPICHASH_LEN 16

/* Define control topic length */
#define E4_CTRLTOPIC_LEN (2 * E4_ID_LEN) + 3

#define E4_TAG_LEN 16
#define E4_TIMESTAMP_LEN 8
#define E4_MSGHDR_LEN (E4_TAG_LEN + E4_TIMESTAMP_LEN)

/* Public key support */
#define E4_PK_EDDSA_PRIVKEY_LEN 64
#define E4_PK_EDDSA_PUBKEY_LEN 32
#define E4_PK_EDDSA_SIG_LEN 32

struct _e4storage;
/* This structure represents storage-specific data to be passed to the e4c
 storage functions. It is forward-declared and implemented by the specific
 choice of storage to be supported, which is chosen based on the configurable
 defines. */
typedef struct _e4storage e4storage;

/* e4c_protect_message produces a protected message for onwards
   transmission 
   (out) cptr Pointer to a ciphertext buffer 
   (in) cmax  Maximum length of ciphertext buffer to be written. \param[out] clen Actual
   length of ciphertext buffer written. \param[in] mptr Pointer to a message
   (plaintext) buffer. \param[in] mlen Length of message to be encrypted.
   \param[in] topic Pointer to a topic hash for topic associated with this
   message. \param[inout] storage Pointer to the structure representing storage.
   \return 0 on success. Non-zero return values indicate errors. 
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
\return 0 on success. Non-zero return values indicate errors. 
 */
int e4c_unprotect_message(uint8_t *mptr,
                          size_t mmax,
                          size_t *mlen,
                          const uint8_t *cptr,
                          size_t clen,
                          const char *topic,
                          e4storage *storage);


/* the e4storage type pre-defined above implements these API calls */
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

#ifdef E4_MODE_PUBKEY 
/* pubkey storage apis */
int e4c_getdeviceindex(e4storage *store, const uint8_t* id);
int e4c_getdevicekey(uint8_t* key, e4storage *store, const int index);
int e4c_set_device_key(e4storage *store, const uint8_t *id, const uint8_t *key);
int e4c_remove_devices(e4storage* store, const uint8_t* id);
int e4c_reset_devices(e4storage* store);
#endif

/*#ifdef DEBUG*/
void e4c_debug_print(e4storage *store);
/*#endif*/

#ifdef E4_STORE_FILE

  #ifdef E4_MODE_PUBKEY
    #include "e4/internal/e4c_pk_store_file.h"
  #else
    #include "e4/internal/e4c_store_file.h"
  #endif

#endif

#endif

#ifdef __cplusplus
}
#endif
