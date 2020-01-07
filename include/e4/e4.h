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
#include "e4config/e4_config.h"

/* E4 Library Error codes */
/* Operation succeeded without error  */
#define E4_RESULT_OK 0
/* A control message was handled. Applications may discard the resulting buffer */
#define E4_RESULT_OK_CONTROL -1

/* Internal error: for exception conditions that indicate the code has a bug */
#define E4_ERROR_INTERNAL -100
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
/* Signature verification failed */
#define E4_ERROR_PK_SIGVERIF_FAILED -110
/* Overflow detected */
#define E4_ERROR_PARAMETER_OVERFLOW -111
/* Invalid parameters, e.g. NULL pointers */
#define E4_ERROR_PARAMETER_INVALID -112

/* Size of the timestamp field */
#define E4_TIMESTAMP_LEN 8
/* Size of the ID, truncated sha3(alias) */
#define E4_ID_LEN 16

/* Secret key size */
#define E4_KEY_LEN 32

/* Topic Hash Length */
#define E4_TOPICHASH_LEN 16

/* Define control topic length */
#define E4_CTRLTOPIC_LEN (2 * E4_ID_LEN) + 3

#define E4_TAG_LEN 16

#define E4_MSGHDR_LEN (E4_TAG_LEN + E4_TIMESTAMP_LEN)

/* Public key support */
#define E4_PK_EDDSA_PRIVKEY_LEN 64
#define E4_PK_EDDSA_PUBKEY_LEN 32
#define E4_PK_EDDSA_SIG_LEN 64
#define E4_PK_X25519_PUBKEY_LEN 32
#define E4_PK_X25519_PRIVKEY_LEN 32

#define E4_PK_TOPICMSGHDR_LEN (E4_TAG_LEN + E4_TIMESTAMP_LEN + E4_ID_LEN)

struct _e4storage;
/* This structure represents storage-specific data to be passed to the e4c
 storage functions. It is forward-declared and implemented by the specific
 choice of storage to be supported, which is chosen based on the configurable
 defines. */
typedef struct _e4storage e4storage;

/* Options bits for e4c crypto functions: */

/* Disable checks on message timestamp within acceptable window 
 * Packet timestamps are still covered by authenticated encryption, 
 * but older messages are allowed */
#define E4_OPTION_IGNORE_TIMESTAMP         0x01
/* Disable failure if we do not have a client public key to validate 
 * signatures. Corrupt signatures will ALWAYS fail, this is only 
 * for the case where too many clients exist for us to store all of 
 * their keys. */
#define E4_OPTION_IGNORE_MISSING_PUBKEY    0x02


/* e4c_protect_message produces a protected message for onwards
   transmission. 

Parameters:

 * ciphertext: (inout) a pointer to a buffer to receive the output ciphertext.
 * ciphertext_max_len: the maximum length of the ciphertext buffer to write. 
   If this value is insufficient, an error will be returned.
 * ciphertext_len: (out) the number of bytes written to the ciphertext buffer.
 * message: (in) the plaintext (to be encrypted data).
 * message_len: (in) the length of the message buffer, in bytes.
 * topic_name: (in) the C-style zero-terminated string identifying the "topic" 
   name to be used. In MQTT implementations this corresponds to the topic name 
   used in the protocol; more generally this is any valid identifier for the 
   group channel.
 * storage: (inout) pointer to a context structure for the backend key storage 
   in use.

Returns:

   E4_RESULT_OK (and out parameters) on success. An appropriate E4_ERROR 
   variable on failure. These correspond to 0 on success and other values on 
   failure.

 */
int e4c_protect_message(uint8_t *ciphertext,
                        size_t ciphertext_max_len,
                        size_t *ciphertext_len,
                        const uint8_t *message,
                        size_t message_len,
                        const char *topic_name,
                        e4storage *storage,
                        const uint32_t proto_opts);

/* e4c_unprotect_message retrieves and authenticates a message that was 
encrypted by E4.

Parameters:

 * message: (out) Pointer to ciphertext to be read
 * message_max_len: (in) Maximum length of plaintext buffer to which we can write.
 * message_len: (in) Actual length of plaintext buffer written by decryption.
 * ciphertext: (in) Pointer to ciphertext buffer
 * ciphertext_len: (in) clen Length of ciphertext buffer
 * topic_name: (in) Pointer to a topic hash for topic associated with this
message.
 * storage: (inout) Pointer to the structure representing storage.
 
Returns:

   E4_RESULT_OK if the message was successfully decrypted.
   E4_RESULT_OK_CONTROL if the message was a control message. Such messages are 
   commands for the E4 protocol and do not need to be processed by the 
   application. Applications should not rely on the plaintext buffer in this 
   case.
   E4_ERROR_... on error.

*/
int e4c_unprotect_message(uint8_t *message,
                          size_t message_max_len,
                          size_t *message_len,
                          const uint8_t *ciphertext,
                          size_t ciphertext_len,
                          const char *topic_name,
                          e4storage *storage,
                          const uint32_t proto_opts);


/* the e4storage type pre-defined above implements these API calls */
int e4c_init(e4storage *store);
int e4c_set_storagelocation(e4storage *store, const char *path);
int e4c_load(e4storage *store, const char *path);
int e4c_sync(e4storage *store);
int e4c_set_id(e4storage *store, const uint8_t *id);
int e4c_is_device_ctrltopic(e4storage *store, const char *topic);
int e4c_getindex(e4storage *store, const char *topic);
int e4c_gettopickey(uint8_t *key, e4storage *store, const int index);
int e4c_set_topic_key(e4storage *store, const uint8_t *topic_hash, const uint8_t *key);
int e4c_remove_topic(e4storage *store, const uint8_t *topic_hash);
int e4c_reset_topics(e4storage *store);

#ifdef E4_MODE_SYMKEY
int e4c_set_idkey(e4storage *store, const uint8_t *key);
#endif
#ifdef E4_MODE_PUBKEY 
/* pubkey storage apis */
int e4c_set_idpubkey(e4storage *store, const uint8_t *pubkey);
int e4c_set_idseckey(e4storage *store, const uint8_t *key);
int e4c_get_idseckey(e4storage* store, uint8_t *key);
int e4c_get_idpubkey(e4storage* store, uint8_t *key);
int e4c_getdeviceindex(e4storage *store, const uint8_t* id);
int e4c_getdevicekey(uint8_t* key, e4storage *store, const int index);
int e4c_set_device_key(e4storage *store, const uint8_t *id, const uint8_t *key);
int e4c_remove_device(e4storage* store, const uint8_t* id);
int e4c_reset_devices(e4storage* store);
int e4c_set_c2_pubkey(e4storage* store, const uint8_t* key);
int e4c_get_c2_pubkey(e4storage* store, uint8_t* key);
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

#ifdef E4_STORE_MEM
  #ifdef E4_MODE_PUBKEY
    #include "e4/internal/e4c_pk_store_mem.h"
  #else
    #include "e4/internal/e4c_store_mem.h"
  #endif

#endif

#ifdef __cplusplus
}
#endif

#endif
