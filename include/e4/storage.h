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

#ifndef E4_STORAGE_H
#define E4_STORAGE_H
#ifdef __cplusplus
extern "C" {
#endif

struct _e4storage;
/* This structure represents storage-specific data to be passed to the e4c
 storage functions. It is forward-declared and implemented by the specific
 choice of storage to be supported, which is chosen based on the configurable
 defines. */
typedef struct _e4storage e4storage;

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

const uint8_t* e4c_get_id(e4storage *store);

#ifdef E4_MODE_SYMKEY
int e4c_set_idkey(e4storage *store, const uint8_t *key);
const uint8_t* e4c_get_idkey(e4storage *store);
#endif
#ifdef E4_MODE_PUBKEY 
/* pubkey storage apis */
int e4c_set_idpubkey(e4storage *store, const uint8_t *pubkey);
int e4c_set_idseckey(e4storage *store, const uint8_t *key);
int e4c_get_idseckey(e4storage* store, uint8_t *key);
int e4c_get_idpubkey(e4storage* store, uint8_t *key);

/* APIs to store and retrieve the C2 shared secret */
int e4c_set_c2sharedsecret(e4storage* store, const uint8_t* key);
int e4c_get_c2sharedsecret(e4storage* store, uint8_t* key);

int e4c_getdeviceindex(e4storage *store, const uint8_t* id);
int e4c_getdevicekey(uint8_t* key, e4storage *store, const int index);
int e4c_set_device_key(e4storage *store, const uint8_t *id, const uint8_t *key);
int e4c_remove_device(e4storage* store, const uint8_t* id);
int e4c_reset_devices(e4storage* store);
int e4c_set_c2_pubkey(e4storage* store, const uint8_t* key);
int e4c_get_c2_pubkey(e4storage* store, uint8_t* key);
#endif

#ifdef DEBUG
void e4c_debug_print(e4storage *store);
#endif

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

