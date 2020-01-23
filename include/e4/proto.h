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

#ifndef E4_PROTO_H
#define E4_PROTO_H

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

#endif
