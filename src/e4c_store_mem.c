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
#include "e4/internal/e4c_sym_store_file.h"
#include "e4/strlcpy.h"
#include "e4/util.h"

const char E4V1_MAGIC[4] = "E41P";

uint32_t e4c_symkey_get_storage_caps(void* s) {
    return E4_STORECAP_SYMKEY;
}

int e4c_symkey_init(void* s)
{
    e4storage_symkey* store = (e4storage_symkey*)s;
    ZERO(store->id);
    ZERO(store->key);
    ZERO(store->ctrltopic);
    store->topiccount = 0;
    ZERO(store->topics);
    ZERO(store->filepath);
    return E4_RESULT_OK;
}

int e4c_symkey_configure_storage(void* s, const void* params)
{
    return E4_RESULT_OK;
}

int e4c_symkey_load(void* s, const char *path)
{
    return E4_RESULT_OK;
}

int e4c_symkey_sync(void* s)
{
    return E4_RESULT_OK;
}


int e4c_symkey_set_id(void* s, const uint8_t *id)
{
    int r = E4_RESULT_OK;
    char controltopic[E4_CTRLTOPIC_LEN+1];
    e4storage_symkey* store = (e4storage_symkey*)s;
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

int e4c_symkey_get_id(void* s, uint8_t* id) {
    e4storage_symkey* store = (e4storage_symkey*)s;
    memmove(id, store->id, sizeof(store->id));
    return E4_RESULT_OK;
}

const uint8_t* e4c_symkey_get_id_cached(void* s) {
    e4storage_symkey* store = (e4storage_symkey*)s;
    return store->id;
}

int e4c_symkey_set_idkey(void* s, const uint8_t *key)
{
    e4storage_symkey* store = (e4storage_symkey*)s;
    memmove(store->key, key, sizeof(store->key));
    return E4_RESULT_OK;
}

int e4c_symkey_get_idkey(void* s, uint8_t *key)
{
    e4storage_symkey* store = (e4storage_symkey*)s;
    memmove(key, store->key, sizeof(store->key));
    return E4_RESULT_OK;
}

const uint8_t* e4c_symkey_get_idkey_cached(void* s)
{
    e4storage_symkey* store = (e4storage_symkey*)s;
    return store->key;
}

int e4c_symkey_gettopicindex(void* s, const char *topic)
{
    int i;
    uint8_t hash[E4_TOPICHASH_LEN];
    e4storage_symkey* store = (e4storage_symkey*)s;

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

int e4c_symkey_is_device_ctrltopic(void* s, const char *topic)
{
    uint8_t hash[E4_TOPICHASH_LEN];
    e4storage_symkey* store = (e4storage_symkey*)s;

    /* hash the topic */
    if (e4c_derive_topichash(hash, E4_TOPICHASH_LEN, topic) != 0) {
        return E4_ERROR_PERSISTENCE_ERROR;
    }

    return memcmp(store->ctrltopic, hash, E4_TOPICHASH_LEN);
}

int e4c_symkey_gettopickey(uint8_t *key, void* s, const int index)
{
    e4storage_symkey* store = (e4storage_symkey*)s;

    if (index < 0 || index >= store->topiccount)
        return E4_ERROR_TOPICKEY_MISSING;

    memcpy(key, store->topics[index].key, E4_KEY_LEN);

    return E4_RESULT_OK;
}

int e4c_symkey_set_topic_key(void* s, const uint8_t *topic_hash, const uint8_t *key)
{
    int i;
    e4storage_symkey* store = (e4storage_symkey*)s;

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

    return e4c_symkey_sync(store);
}

int e4c_symkey_remove_topic(void* s, const uint8_t *topic_hash)
{
    int i, j;
    e4storage_symkey* store = (e4storage_symkey*)s;
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

            return e4c_symkey_sync(store);
        }
    }

    return E4_ERROR_TOPICKEY_MISSING;
}

int e4c_symkey_reset_topics(void* s)
{
    e4storage_symkey* store = (e4storage_symkey*)s;

    if (store->topiccount > 0)
    {
        store->topiccount = 0;
    }

    e4c_symkey_sync(store);

    return E4_RESULT_OK;
}

#ifdef DEBUG

void e4c_symkey_debug_print(void* s)
{
    return;
}

#endif
