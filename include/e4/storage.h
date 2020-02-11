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

#if defined(E4_STORE_NONE)
#define E4LT extern
#else
#define E4LT
#endif /* STORE NONE */



#if defined(E4_MODE_SYMKEY) || defined(E4_MODE_ALL)
E4LT int e4c_symkey_init(void* store);
E4LT int e4c_symkey_configure_storage(void* store, const void* params);

E4LT int e4c_symkey_set_id(void* store, const uint8_t *id);
E4LT int e4c_symkey_get_id(void* store, uint8_t* id);
E4LT const uint8_t* e4c_symkey_get_id_cached(void* store);

E4LT int e4c_symkey_is_device_ctrltopic(void* store, const char *topic);
E4LT int e4c_symkey_gettopicindex(void* store, const char *topic);
E4LT int e4c_symkey_gettopickey(uint8_t *key, void* store, const int index);
E4LT int e4c_symkey_set_topic_key(void* store, const uint8_t *topic_hash, const uint8_t *key);
E4LT int e4c_symkey_remove_topic(void* store, const uint8_t *topic_hash);
E4LT int e4c_symkey_reset_topics(void* store);
E4LT int e4c_symkey_load(void* store, const char *path);
E4LT int e4c_symkey_sync(void* store);
E4LT int e4c_symkey_set_idkey(void* store, const uint8_t *key);
/* This function returns a pointer to the symkey if the 
 * underlying storage supports loading that into main memory */
E4LT const uint8_t* e4c_symkey_get_idkey_cached(void* store);
/* If the above function returns NULL, then the 
 * key can be copied out of storage as needed */
E4LT int e4c_symkey_get_idkey(void* store, uint8_t* key);
#endif /* symkey or all */

#if defined(E4_MODE_PUBKEY) || defined(E4_MODE_ALL)
/* pubkey storage apis */
E4LT int e4c_pubkey_init(void* store);
E4LT int e4c_pubkey_configure_storage(void* store, const void* params);

E4LT int e4c_pubkey_set_id(void* store, const uint8_t *id);
E4LT int e4c_pubkey_get_id(void* store, uint8_t* id);
E4LT const uint8_t* e4c_pubkey_get_id_cached(void* store);

E4LT int e4c_pubkey_is_device_ctrltopic(void* store, const char *topic);
E4LT int e4c_pubkey_gettopicindex(void* store, const char *topic);
E4LT int e4c_pubkey_gettopickey(uint8_t *key, void* store, const int index);
E4LT int e4c_pubkey_set_topic_key(void* store, const uint8_t *topic_hash, const uint8_t *key);
E4LT int e4c_pubkey_remove_topic(void* store, const uint8_t *topic_hash);
E4LT int e4c_pubkey_reset_topics(void* store);
E4LT int e4c_pubkey_load(void* store, const char *path);
E4LT int e4c_pubkey_sync(void* store);
E4LT int e4c_pubkey_set_idpubkey(void* store, const uint8_t *pubkey);
E4LT int e4c_pubkey_set_idseckey(void* store, const uint8_t *key);
E4LT int e4c_pubkey_get_idseckey(void* store, uint8_t *key);
E4LT int e4c_pubkey_get_idpubkey(void* store, uint8_t *key);

/* As with the symmetric key mode, these functions will return a 
 * pointer to the key if available and return NULL if not */
E4LT const uint8_t* e4c_pubkey_get_idseckey_cached(void* store);
E4LT const uint8_t* e4c_pubkey_get_idpubkey_cached(void* store);

/* APIs to store and retrieve the C2 shared secret */
E4LT int e4c_pubkey_set_c2sharedsecret(void* store, const uint8_t* key);
E4LT int e4c_pubkey_get_c2sharedsecret(void* store, uint8_t* key);
E4LT const uint8_t* e4c_pubkey_get_c2sharedsecret_cached(void* store);

E4LT int e4c_pubkey_getdeviceindex(void* store, const uint8_t* id);
E4LT int e4c_pubkey_getdevicekey(uint8_t* key, void* store, const int index);
E4LT int e4c_pubkey_set_device_key(void* store, const uint8_t *id, const uint8_t *key);
E4LT int e4c_pubkey_remove_device(void* store, const uint8_t* id);
E4LT int e4c_pubkey_reset_devices(void* store);
E4LT int e4c_pubkey_set_c2_pubkey(void* store, const uint8_t* key);
E4LT int e4c_pubkey_get_c2_pubkey(void* store, uint8_t* key);
E4LT const uint8_t* e4c_pubkey_get_c2_pubkey_cached(void* store);
#endif /* pubkey or all */

#if defined(E4_MODE_PUBKEY) || defined(E4_MODE_ALL)
  #include "e4/internal/e4c_pk_store_file.h"
#endif /* pubkey or all */
#if defined(E4_MODE_SYMKEY) || defined(E4_MODE_ALL)
  #include "e4/internal/e4c_sym_store_file.h"
#endif /* symkey or all */

#ifdef __cplusplus
}
#endif /* C++ */
#endif /* include guard */

