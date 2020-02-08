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

#include <stdlib.h>
#include <string.h>

#include "e4/e4.h"
#include "e4/internal/e4c_pk_store_file.h"
#include "e4/strlcpy.h"
#include "e4/util.h"

const char E4V2_MAGIC[4] = "E42P";

uint32_t e4c_pubkey_get_storage_caps(void* s) {
    return E4_STORECAP_PUBKEY;
}

int e4c_pubkey_init(void* s)
{
    e4storage_pubkey* store = (e4storage_pubkey*)s;
    ZERO(store->id);
    ZERO(store->privkey);
    ZERO(store->ctrltopic);
    store->topiccount = 0;
    ZERO(store->topics);
    ZERO(store->filepath);
    return E4_RESULT_OK;
}

int e4c_pubkey_configure_storage(void* s, const void* params)
{
    return E4_RESULT_OK;
}

int e4c_pubkey_load(void* s, const char *path)
{
    return E4_RESULT_OK;
}

int e4c_pubkey_sync(void* s)
{
    return E4_RESULT_OK;
}

int e4c_pubkey_set_id(void* s, const uint8_t *id)
{
    e4storage_pubkey* store = (e4storage_pubkey*)s;
    int r = E4_RESULT_OK;
    char controltopic[E4_CTRLTOPIC_LEN+1];
    ZERO(controltopic);

    r = e4c_derive_control_topic(controltopic, E4_CTRLTOPIC_LEN + 1, id);
    if (r != E4_RESULT_OK) goto exit;

    r = e4c_derive_topichash(store->ctrltopic, E4_TOPICHASH_LEN, controltopic);
    if (r != E4_RESULT_OK) {
        ZERO(store->ctrltopic);
        goto exit;
    }

    memmove(store->id, id, sizeof(store->id));
    r = E4_RESULT_OK;
exit:
    return r;
}

int e4c_pubkey_get_id(void* s, uint8_t* id) {
    e4storage_pubkey* store = (e4storage_pubkey*)s;
    memmove(id, store->id, sizeof(store->id));
    return E4_RESULT_OK;
}

const uint8_t* e4c_pubkey_get_id_cached(void* s) {
    e4storage_pubkey* store = (e4storage_pubkey*)s;
    return store->id;
}

int e4c_pubkey_set_idseckey(void* s, const uint8_t *key)
{
    e4storage_pubkey* store = (e4storage_pubkey*)s;
    size_t c2keynotempty = 0;
    memmove(store->privkey, key, sizeof(store->privkey));
    e4c_pubkey_sync(store);
    c2keynotempty = zerocheck(store->c2key, sizeof(store->c2key));
    if (c2keynotempty) {
        e4c_pubkey_c2sharedsecret_derivestore(store);
    }
    return E4_RESULT_OK;
}

int e4c_pubkey_get_idseckey(void* s, uint8_t *key) {
    e4storage_pubkey* store = (e4storage_pubkey*)s;
    memcpy(key, store->privkey, sizeof(store->privkey));
    return E4_RESULT_OK;
}

int e4c_pubkey_get_idpubkey(void* s, uint8_t *key) {
    e4storage_pubkey* store = (e4storage_pubkey*)s;
    memcpy(key, store->pubkey, sizeof(store->pubkey));
    return E4_RESULT_OK;
}

const uint8_t* e4c_pubkey_get_idseckey_cached(void* s) {
    e4storage_pubkey* store = (e4storage_pubkey*)s;
    return store->privkey;
}

const uint8_t* e4c_pubkey_get_idpubkey_cached(void* s) {
    e4storage_pubkey* store = (e4storage_pubkey*)s;
    return store->pubkey;
}

int e4c_pubkey_gettopicindex(void* s, const char *topic)
{
    int i;
    uint8_t hash[E4_TOPICHASH_LEN];
    e4storage_pubkey* store = (e4storage_pubkey*)s;

    /* hash the topic */
    if (e4c_derive_topichash(hash, E4_TOPICHASH_LEN, topic) != 0) {
        return E4_ERROR_PERSISTENCE_ERROR;
    }
    /* look for it */
    for (i = 0; i < store->topiccount; i++)
    { 
        if (memcmp(store->topics[i].topic, hash, E4_TOPICHASH_LEN) == 0)
        {
            break;
        }
    }
    if (i >= store->topiccount) return E4_ERROR_TOPICKEY_MISSING;

    return i;
}

int e4c_pubkey_is_device_ctrltopic(void* s, const char *topic)
{
    uint8_t hash[E4_TOPICHASH_LEN];
    e4storage_pubkey* store = (e4storage_pubkey*)s;

    /* hash the topic */
    if (e4c_derive_topichash(hash, E4_TOPICHASH_LEN, topic) != 0) {
        return E4_ERROR_PERSISTENCE_ERROR;
    }

    return memcmp(store->ctrltopic, hash, E4_TOPICHASH_LEN);
}

int e4c_pubkey_gettopickey(uint8_t *key, void* s, const int index)
{
    e4storage_pubkey* store = (e4storage_pubkey*)s;

    if (index < 0 || index >= store->topiccount)
        return E4_ERROR_TOPICKEY_MISSING;

    memcpy(key, store->topics[index].key, E4_KEY_LEN);

    return E4_RESULT_OK;
}

int e4c_pubkey_set_topic_key(void* s, const uint8_t *topic_hash, const uint8_t *key)
{
    int i;
    e4storage_pubkey* store = (e4storage_pubkey*)s;

    for (i = 0; i < store->topiccount; i++)
    {
        if (memcmp(store->topics[i].topic, topic_hash, E4_TOPICHASH_LEN) == 0)
            break;
    }
    if (i >= E4_TOPICS_MAX) /* out of space */
        return E4_ERROR_TOPICKEY_MISSING;

    memcpy(store->topics[i].topic, topic_hash, E4_TOPICHASH_LEN);
    memcpy(store->topics[i].key, key, E4_KEY_LEN);

    if (i == store->topiccount)
    { 
        /* new topic */
        store->topiccount++;
    }

    return e4c_pubkey_sync(store);
}

int e4c_pubkey_remove_topic(void* s, const uint8_t *topic_hash)
{
    int i, j;
    e4storage_pubkey* store = (e4storage_pubkey*)s;
    topic_key *topic_keys = store->topics;


    for (i = 0; i < store->topiccount; i++)
    {

        if (memcmp(topic_keys[i].topic, topic_hash, E4_TOPICHASH_LEN) == 0)
        {
            /* remove this item and move list up */
            for (j = i + 1; j < store->topiccount; j++)
            {
                memcpy(&topic_keys[j - 1], &topic_keys[j], sizeof(topic_key));
            }
            ZERO(topic_keys[store->topiccount]);
            store->topiccount--;

            return e4c_pubkey_sync(store);
        }
    }

    return E4_ERROR_TOPICKEY_MISSING;
}

int e4c_pubkey_reset_topics(void* s)
{
    int i = 0;
    e4storage_pubkey* store = (e4storage_pubkey*)s;

    if (store->topiccount > 0)
    {
        store->topiccount = 0;
    }
    for (i=0 ; i < E4_TOPICS_MAX ; i++) {
        ZERO(store->topics[i]);
    }

    e4c_pubkey_sync(store);
    return E4_RESULT_OK;
}

int e4c_pubkey_set_idpubkey(void* s, const uint8_t *pubkey) {
    e4storage_pubkey* store = (e4storage_pubkey*)s;
    memmove(store->pubkey, pubkey, sizeof(store->pubkey));
    e4c_pubkey_sync(store);
    return E4_RESULT_OK;
}

int e4c_pubkey_getdeviceindex(void* s, const uint8_t* id) 
{
    int i;
    e4storage_pubkey* store = (e4storage_pubkey*)s;

    /* look for it */
    for (i = 0; i < store->devicecount; i++)
    { 
        if (memcmp(store->devices[i].id, id, E4_ID_LEN) == 0)
        {
            break;
        }
    }
    if (i >= store->devicecount) return E4_ERROR_DEVICEPK_MISSING;

    return i;
}

int e4c_pubkey_getdevicekey(uint8_t* pubkey, void* s, const int index)
{
    e4storage_pubkey* store = (e4storage_pubkey*)s;
    if (index < 0 || index >= store->devicecount)
        return E4_ERROR_TOPICKEY_MISSING;

    memcpy(pubkey, store->devices[index].pubkey, E4_PK_EDDSA_PUBKEY_LEN);

    return E4_RESULT_OK;
}

int e4c_pubkey_set_device_key(void* s, const uint8_t *id, const uint8_t *pubkey)
{
    int i;
    e4storage_pubkey* store = (e4storage_pubkey*)s;

    for (i = 0; i < store->devicecount; i++)
    {
        if (memcmp(store->devices[i].id, id, E4_ID_LEN) == 0)
            break;
    }
    if (i >= E4_DEVICES_MAX) /* out of space */
        return E4_ERROR_DEVICEPK_MISSING;

    memcpy(store->devices[i].id, id, E4_TOPICHASH_LEN);
    memcpy(store->devices[i].pubkey, pubkey, E4_PK_EDDSA_PUBKEY_LEN);

    if (i == store->devicecount)
    { 
        /* new topic */
        store->devicecount++;
    }

    return e4c_pubkey_sync(store);
}

int e4c_pubkey_remove_device(void* s, const uint8_t* id)
{
    int i, j;
    e4storage_pubkey* store = (e4storage_pubkey*)s;
    device_key *devicekeys = store->devices;

    for (i = 0; i < store->devicecount; i++)
    {

        if (memcmp(devicekeys[i].id, id, E4_ID_LEN) == 0)
        {
            /* remove this item and move list up */
            for (j = i + 1; j < store->topiccount; j++)
            {
                memcpy(&devicekeys[j - 1], &devicekeys[j], sizeof(device_key));
            }
            ZERO(devicekeys[store->devicecount]);
            store->devicecount--;

            return e4c_pubkey_sync(store);
        }
    }

    return E4_ERROR_DEVICEPK_MISSING;
}

int e4c_pubkey_reset_devices(void* s)
{
    int i = 0;
    e4storage_pubkey* store = (e4storage_pubkey*)s;
    if (store->devicecount > 0)
    {
        store->devicecount = 0;
    }
    /* Attempt to zero memory. This may not work depending on the platform */
    for (i=0 ; i < E4_DEVICES_MAX ; i++) {
        ZERO(store->devices[i]);
    }

    e4c_pubkey_sync(store);
    return E4_RESULT_OK;
}

int e4c_set_c2_pubkey(void* s, const uint8_t* key) {
    size_t devicekeynotempty = 0;
    e4storage_pubkey* store = (e4storage_pubkey*)s;
    memcpy(store->c2key, key, E4_PK_X25519_PUBKEY_LEN);
    e4c_pubkey_sync(store);
    devicekeynotempty = zerocheck(store->privkey, sizeof(store->privkey));
    if (devicekeynotempty) {
        e4c_pubkey_c2sharedsecret_derivestore(store);
    }
    return E4_RESULT_OK;
}

int e4c_pubkey_get_c2_pubkey(void* s, uint8_t* key) {
    e4storage_pubkey* store = (e4storage_pubkey*)s;
    size_t empty = zerocheck(store->c2key, sizeof(store->c2key));
    if (empty == 0) {
        return E4_ERROR_PERSISTENCE_ERROR;
    }
    memcpy(key, store->c2key, E4_PK_X25519_PUBKEY_LEN);
    return E4_RESULT_OK;
}

const uint8_t* e4c_pubkey_get_c2_pubkey_cached(void* s) {
    e4storage_pubkey* store = (e4storage_pubkey*)s;
    size_t empty = zerocheck(store->c2key, sizeof(store->c2key));
    if (empty == 0) {
        return NULL;
    }
    return E4_RESULT_OK;
}

int e4c_pubkey_set_c2sharedsecret(void* s, const uint8_t* key) {
    e4storage_pubkey* store = (e4storage_pubkey*)s;
    memcpy(store->c2sharedkey, key, E4_KEY_LEN);
    return E4_RESULT_OK;
}

int e4c_pubkey_get_c2sharedsecret(void* s, uint8_t* key) {
    e4storage_pubkey* store = (e4storage_pubkey*)s;
    size_t empty = zerocheck(store->c2sharedkey, sizeof(store->c2sharedkey));
    if (empty == 0) {
        return E4_ERROR_PERSISTENCE_ERROR;
    }
    memcpy(key, store->c2sharedkey, E4_KEY_LEN);
    return E4_RESULT_OK;
}

const uint8_t* e4c_pubkey_get_c2sharedsecret_cached(void* s)
{
    e4storage_pubkey* store = (e4storage_pubkey*)s;
    size_t empty = zerocheck(store->c2sharedkey, sizeof(store->c2sharedkey));
    if (empty == 0) {
        return NULL;
    }
    return store->c2sharedkey;
}

#ifdef DEBUG 
void e4c_pubkey_debug_print(void* s)
{
    return;
}
#endif
