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
#include "e4/internal/e4c_store_mem.h"
#include "e4/strlcpy.h"
#include "e4/util.h"

const char E4V1_MAGIC[4] = "E41P";

int e4c_init(e4storage *store)
{
    ZERO(store->id);
    ZERO(store->key);
    ZERO(store->ctrltopic);
    store->topiccount = 0;
    ZERO(store->topics);
    ZERO(store->filepath);
    return 0;
}

int e4c_set_storagelocation(e4storage *store, const char *path)
{
    return 0;
}

int e4c_load(e4storage *store, const char *path)
{
    return 0;
}

int e4c_sync(e4storage *store)
{
    return E4_RESULT_OK;
}


int e4c_set_id(e4storage *store, const uint8_t *id)
{
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


int e4c_set_idkey(e4storage *store, const uint8_t *key)
{
    memmove(store->key, key, sizeof(store->key));
    return E4_RESULT_OK;
}

int e4c_getindex(e4storage *store, const char *topic)
{
    int i;
    uint8_t hash[E4_TOPICHASH_LEN];

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

int e4c_is_device_ctrltopic(e4storage *store, const char *topic)
{
    uint8_t hash[E4_TOPICHASH_LEN];

    /* hash the topic */
    if (e4c_derive_topichash(hash, E4_TOPICHASH_LEN, topic) != 0) {
        return E4_ERROR_PERSISTENCE_ERROR;
    }

    return memcmp(store->ctrltopic, hash, E4_TOPICHASH_LEN);
}

int e4c_gettopickey(uint8_t *key, e4storage *store, const int index)
{

    if (index < 0 || index >= store->topiccount)
        return E4_ERROR_TOPICKEY_MISSING;

    memcpy(key, store->topics[index].key, E4_KEY_LEN);

    return 0;
}

int e4c_set_topic_key(e4storage *store, const uint8_t *topic_hash, const uint8_t *key)
{
    int i;

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

    return e4c_sync(store);
}

int e4c_remove_topic(e4storage *store, const uint8_t *topic_hash)
{
    int i, j;
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

            return e4c_sync(store);
        }
    }

    return E4_ERROR_TOPICKEY_MISSING;
}

int e4c_reset_topics(e4storage *store)
{

    if (store->topiccount > 0)
    {
        store->topiccount = 0;
    }

    e4c_sync(store);

    return 0;
}

/*#ifdef DEBUG */

void e4c_debug_print(e4storage *store)
{
    return;
}

/*#endif*/
