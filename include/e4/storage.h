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

#define E4_STORECAP_SYMKEY 0x1
#define E4_STORECAP_PUBKEY 0x2
#define E4_STORECAP_FIPS   0x4
#define E4_STORECAP_PQ     0x8

struct _e4storage;
/* This structure represents storage-specific data to be passed to the e4c
 storage functions. It is forward-declared and implemented by the specific
 choice of storage to be supported, which is chosen based on the configurable
 defines. */
typedef struct _e4storage e4storage;

#if defined(__APPLE__) && defined(E4_MODE_ALL)
#define E4LT extern
#else
#define E4LT
#endif

/* the e4storage type pre-defined above implements these API calls */
E4LT uint32_t e4c_get_storage_caps(e4storage* store);
E4LT int e4c_init(e4storage *store);
E4LT int e4c_set_storagelocation(e4storage *store, const char *path);
E4LT int e4c_load(e4storage *store, const char *path);
E4LT int e4c_sync(e4storage *store);
E4LT int e4c_set_id(e4storage *store, const uint8_t *id);
E4LT int e4c_get_id(e4storage *store, uint8_t* id);
E4LT const uint8_t* e4c_get_id_cached(e4storage* store);

E4LT int e4c_is_device_ctrltopic(e4storage *store, const char *topic);
E4LT int e4c_getindex(e4storage *store, const char *topic);
E4LT int e4c_gettopickey(uint8_t *key, e4storage *store, const int index);
E4LT int e4c_set_topic_key(e4storage *store, const uint8_t *topic_hash, const uint8_t *key);
E4LT int e4c_remove_topic(e4storage *store, const uint8_t *topic_hash);
E4LT int e4c_reset_topics(e4storage *store);

#if defined(E4_MODE_SYMKEY) || defined(E4_MODE_ALL)
E4LT int e4c_set_idsymkey(e4storage *store, const uint8_t *key);
/* This function returns a pointer to the symkey if the 
 * underlying storage supports loading that into main memory */
E4LT const uint8_t* e4c_get_idsymkey_cached(e4storage *store);
/* If the above function returns NULL, then the 
 * key can be copied out of storage as needed */
E4LT int e4c_get_idsymkey(e4storage* store, uint8_t* key);
#endif
#if defined(E4_MODE_PUBKEY) || defined(E4_MODE_ALL)
/* pubkey storage apis */
E4LT int e4c_set_idpubkey(e4storage *store, const uint8_t *pubkey);
E4LT int e4c_set_idseckey(e4storage *store, const uint8_t *key);
E4LT int e4c_get_idseckey(e4storage* store, uint8_t *key);
E4LT int e4c_get_idpubkey(e4storage* store, uint8_t *key);

/* As with the symmetric key mode, these functions will return a 
 * pointer to the key if available and return NULL if not */
E4LT const uint8_t* e4c_get_idseckey_cached(e4storage* store);
E4LT const uint8_t* e4c_get_idpubkey_cached(e4storage* store);

/* APIs to store and retrieve the C2 shared secret */
E4LT int e4c_set_c2sharedsecret(e4storage* store, const uint8_t* key);
E4LT int e4c_get_c2sharedsecret(e4storage* store, uint8_t* key);
E4LT const uint8_t* e4c_get_c2sharedsecret_cached(e4storage* store);

E4LT int e4c_getdeviceindex(e4storage *store, const uint8_t* id);
E4LT int e4c_getdevicekey(uint8_t* key, e4storage *store, const int index);
E4LT int e4c_set_device_key(e4storage *store, const uint8_t *id, const uint8_t *key);
E4LT int e4c_remove_device(e4storage* store, const uint8_t* id);
E4LT int e4c_reset_devices(e4storage* store);
E4LT int e4c_set_c2_pubkey(e4storage* store, const uint8_t* key);
E4LT int e4c_get_c2_pubkey(e4storage* store, uint8_t* key);
E4LT const uint8_t* e4c_get_c2_pubkey_cached(e4storage* store);
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

