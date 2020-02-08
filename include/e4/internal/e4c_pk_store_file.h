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

#ifndef _E4C_STORE_PK_FILE_H_
#define _E4C_STORE_PK_FILE_H_

#include <stddef.h>
#include <stdint.h>
#include <fcntl.h>

#ifndef E4_DEVICES_MAX
#define E4_DEVICES_MAX 100
#endif

#ifndef E4_TOPICS_MAX
#define E4_TOPICS_MAX 100
#endif

#define E4_MAX_PATH 255

/* In memory structures that represent the file. */

#include "e4c_common_storage.h"

typedef struct _e4_device_key
{
    uint8_t id[E4_ID_LEN];
    uint8_t pubkey[E4_PK_EDDSA_PUBKEY_LEN];
} device_key;

typedef struct _e4storage_pubkey
{
    /* These fields are persisted by the sync command */
    uint8_t id[E4_ID_LEN];
    uint8_t privkey[E4_PK_EDDSA_PRIVKEY_LEN];
    uint8_t pubkey[E4_PK_EDDSA_PUBKEY_LEN];
    uint8_t c2key[E4_PK_X25519_PUBKEY_LEN];

    uint8_t c2sharedkey[E4_KEY_LEN];

    uint16_t devicecount;
    uint16_t topiccount;

    device_key devices[E4_DEVICES_MAX];
    topic_key topics[E4_TOPICS_MAX];

    /* These fields are set at run time only */
    char filepath[E4_MAX_PATH + 1];
    uint8_t ctrltopic[E4_TOPICHASH_LEN];
} e4storage_pubkey;

#endif
