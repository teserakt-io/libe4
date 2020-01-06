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
#include "e4/internal/e4c_pk_store_mem.h"
#include "e4/strlcpy.h"
#include "e4/util.h"

const char E4V2_MAGIC[4] = "E42P";

int e4c_init(e4storage *store)
{
    ZERO(store->id);
    ZERO(store->privkey);
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
    return 0;
}

int e4c_set_id(e4storage *store, const uint8_t *id)
{
    int r = E4_RESULT_OK;
    char controltopic[E4_CTRLTOPIC_LEN+1];
    ZERO(controltopic);

    r = e4c_derive_control_topic(controltopic, E4_CTRLTOPIC_LEN + 1, id);
    if ( r != E4_RESULT_OK ) goto exit;

    r = e4c_derive_topichash(store->ctrltopic, E4_TOPICHASH_LEN, controltopic);
    if ( r != E4_RESULT_OK ) {
        ZERO(store->ctrltopic);
        goto exit;
    }

    memmove(store->id, id, sizeof store->id);
    r = E4_RESULT_OK;
exit:
    return r;
}

int e4c_set_idseckey(e4storage *store, const uint8_t *key)
{
    memmove(store->privkey, key, sizeof store->privkey);
    e4c_sync(store);
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
    int i = 0;

    if (store->topiccount > 0)
    {
        store->topiccount = 0;
    }
    for ( i=0 ; i < E4_TOPICS_MAX ; i++ ) {
        ZERO(store->topics[i]);
    }

    e4c_sync(store);
    return 0;
}

int e4c_set_idpubkey(e4storage *store, const uint8_t *pubkey) {
    memmove(store->pubkey, pubkey, sizeof store->pubkey);
    e4c_sync(store);
    return E4_RESULT_OK;
}

int e4c_getdeviceindex(e4storage *store, const uint8_t* id) 
{
    int i;

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

int e4c_getdevicekey(uint8_t* pubkey, e4storage *store, const int index)
{
    if (index < 0 || index >= store->devicecount)
        return E4_ERROR_TOPICKEY_MISSING;

    memcpy(pubkey, store->devices[index].pubkey, E4_PK_EDDSA_PUBKEY_LEN);

    return 0;
}

int e4c_set_device_key(e4storage *store, const uint8_t *id, const uint8_t *pubkey)
{
    int i;

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

    return e4c_sync(store);
}

int e4c_remove_device(e4storage* store, const uint8_t* id)
{
    int i, j;
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

            return e4c_sync(store);
        }
    }

    return E4_ERROR_DEVICEPK_MISSING;
}

int e4c_reset_devices(e4storage* store)
{
    int i = 0;
    if (store->devicecount > 0)
    {
        store->devicecount = 0;
    }
    /* Attempt to zero memory. This may not work depending on the platform */
    for ( i=0 ; i < E4_DEVICES_MAX ; i++ ) {
        ZERO(store->devices[i]);
    }

    e4c_sync(store);
    return 0;
}

int e4c_set_c2_pubkey(e4storage* store, const uint8_t* key) {
    memcpy(store->c2key, key, E4_PK_X25519_PUBKEY_LEN);
    e4c_sync(store);
    return 0;
}

int e4c_get_c2_pubkey(e4storage* store, uint8_t* key) {
    memcpy(key, store->c2key, E4_PK_X25519_PUBKEY_LEN);
    return 0;
}

/*#ifdef DEBUG */

void e4c_debug_print(e4storage *store)
{
    return;
}

/*#endif*/
