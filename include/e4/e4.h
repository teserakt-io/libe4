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

#ifndef E4_H
#define E4_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/* Build-generated configuration. */
#include "e4config/e4_config.h"

/* Headers for protocol defines */
#include "e4/proto.h"
/* Headers for error codes */
#include "e4/error.h"
/* Headers for optional features of protect/unprotect apis */
#include "e4/options.h"
/* Headers for the storage APIs */
#include "e4/storage.h"

#ifndef E4_MODE_ALL
/* If we are not using the "all" mode, we redefine the appropriate 
 * API for public or private key to match one of these functions
 *
 * NOTE: we might want to consider dropping this behaviour entirely and 
 * using just the symkey or pubkey names, to get rid of the ugly linker trick 
 * and developers are more than smart enough to figure this stuff out anyway.
 */

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
#ifdef E4_MODE_PUBKEY
int e4c_pubkey_c2sharedsecret_derivestore(e4storage* storage);
#endif

#else
/* include forward declarations for specific implementations. */
#include "e4/e4pubkey.h"
#include "e4/e4symkey.h"
#endif



#ifdef __cplusplus
}
#endif

#endif
