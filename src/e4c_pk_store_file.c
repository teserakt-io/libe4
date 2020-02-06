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

#ifndef E4_STORE_NONE

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "e4/e4.h"
#include "e4/internal/e4c_pk_store_file.h"
#include "e4/strlcpy.h"
#include "e4/util.h"

const char E4V2_MAGIC[4] = "E42P";

int e4c_init(e4storage *store)
{
    zeroize((void*)store, sizeof(struct _e4storage));
    ZERO(store->id);
    ZERO(store->privkey);
    ZERO(store->pubkey);
    ZERO(store->c2key);
    ZERO(store->c2sharedkey);
    ZERO(store->ctrltopic);
    ZERO(store->topiccount);
    ZERO(store->devicecount);
    ZERO(store->topics);
    ZERO(store->filepath);
    return 0;
}

int e4c_set_storagelocation(e4storage *store, const char *path)
{
    size_t pathlen = strlen(path);
    size_t copied_bytes = strlcpy(store->filepath, path, E4_MAX_PATH);
    if (pathlen >= copied_bytes)
    {
        return 1;
    }
    return 0;
}

int e4c_load(e4storage *store, const char *path)
{
    int fd, i, r;
    size_t devicekeypresent, c2keypresent;
    size_t rlen = 0;
    char mbuf[4];
    char controltopic[E4_CTRLTOPIC_LEN + 1];
    memset(controltopic, 0, sizeof(controltopic));

    if (path == NULL) path = "/tmp/persistence.e4p";

    e4c_set_storagelocation(store, path);

    fd = open(store->filepath, O_RDONLY);
    if (fd < 0)
    {
        perror(path);
        return E4_ERROR_PERSISTENCE_ERROR;
    }

    memset(mbuf, 0, sizeof(mbuf));
    rlen = read(fd, mbuf, sizeof(E4V2_MAGIC));
    if (rlen != sizeof(E4V2_MAGIC))
    {
        goto err;
    }
    if (memcmp(mbuf, E4V2_MAGIC, sizeof(E4V2_MAGIC)) != 0)
    {
        goto err;
    }

    rlen = read(fd, store->id, sizeof(store->id));
    if (rlen != sizeof(store->id))
    {
        goto err;
    }


    r = e4c_derive_control_topic(controltopic, E4_CTRLTOPIC_LEN + 1, store->id);
    if (r != 0)
    {
        goto err;
    }

    /* derive a topichash for the control topic. */
    e4c_derive_topichash(store->ctrltopic, E4_TOPICHASH_LEN, controltopic);

    /* read in key material */
    rlen = read(fd, store->privkey, sizeof(store->privkey));
    if (rlen != sizeof(store->privkey))
    {
        goto err;
    }
    rlen = read(fd, store->pubkey, sizeof(store->pubkey));
    if (rlen != sizeof(store->pubkey))
    {
        goto err;
    }
    
    rlen = read(fd, store->c2key, sizeof(store->c2key));
    if (rlen != sizeof(store->c2key))
    {
        goto err;
    }

    rlen = read(fd, &store->topiccount, sizeof(store->topiccount));
    if (rlen != sizeof(store->topiccount))
    {
        goto err;
    }
    if (store->topiccount > E4_TOPICS_MAX) {
        /* we must not process more entries than we can handle*/
        goto err;
    }

    /* TODO: detect if we cannot read everything based on the topiccount */


    for (i = 0; i < store->topiccount; i++)
    {
        rlen = read(fd, store->topics[i].topic, E4_TOPICHASH_LEN);
        if (rlen != E4_TOPICHASH_LEN)
        {
            goto err;
        }

        rlen = read(fd, store->topics[i].key, E4_KEY_LEN);
        if (rlen != E4_KEY_LEN)
        {
            goto err;
        }
    }
    
    rlen = read(fd, &store->devicecount, sizeof(store->devicecount));
    if (rlen != sizeof(store->devicecount))
    {
        goto err;
    }
    if (store->devicecount > E4_DEVICES_MAX) {
        /* we must not process more entries than we can handle*/
        goto err;
    }
    
    for (i = 0; i < store->devicecount; i++)
    {
        rlen = read(fd, store->devices[i].id, E4_ID_LEN);
        if (rlen != E4_ID_LEN)
        {
            goto err;
        }

        rlen = read(fd, store->devices[i].pubkey, E4_PK_EDDSA_PUBKEY_LEN);
        if (rlen != E4_PK_EDDSA_PUBKEY_LEN)
        {
            goto err;
        }
    }

#ifdef DEBUG
    e4c_debug_print(store);
#endif

    close(fd);
    ZERO(store->c2sharedkey);
    devicekeypresent = zerocheck(store->privkey, sizeof(store->privkey));
    c2keypresent= zerocheck(store->c2key, sizeof(store->c2key));
    if ( c2keypresent != 0 && devicekeypresent != 0 )
    {
        /* on load from storage we have both a c2 public key and 
         * a device key. We can therefore derive the shared secret 
         * for KEX for control messsages.
         * If this condition is not triggered, it is because key material 
         * is missing. Setting both the device key and the C2 public key 
         * will trigger shared point generation.
         */
        e4c_pubkey_c2sharedsecret_derivestore(store);
    }
    return 0;
err:
    perror(path);
    close(fd);

    return E4_ERROR_PERSISTENCE_ERROR;
}

int e4c_sync(e4storage *store)
{
    int fd = -1;
    uint16_t i = 0;

    if (strlen(store->filepath) == 0)
    {
        return E4_ERROR_PERSISTENCE_ERROR;
    }

    fd = open(store->filepath, O_WRONLY | O_CREAT, 0600);
    if (fd < 0)
    {
        perror(store->filepath);
        return E4_ERROR_PERSISTENCE_ERROR;
    }

    write(fd, E4V2_MAGIC, sizeof(E4V2_MAGIC));
    write(fd, store->id, sizeof(store->id));
    write(fd, store->privkey, sizeof(store->privkey));
    write(fd, store->pubkey, sizeof(store->pubkey));
    write(fd, store->c2key, sizeof(store->c2key));
    write(fd, &store->topiccount, sizeof(store->topiccount));

    for (i = 0; i < store->topiccount; i++)
    {
        topic_key *t = &(store->topics[0]) + i;

        write(fd, t->topic, sizeof(t->topic));
        write(fd, t->key, sizeof(t->key));
    }
    write(fd, &store->devicecount, sizeof(store->devicecount));

    for (i = 0; i < store->devicecount; i++)
    {
        device_key *d = &(store->devices[0]) + i;

        write(fd, d->id, sizeof(d->id));
        write(fd, d->pubkey, sizeof(d->pubkey));
    }
    close(fd);

    return 0;
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
int e4c_set_idseckey(e4storage *store, const uint8_t *key)
{
    size_t c2keynotempty = 0;
    memmove(store->privkey, key, sizeof(store->privkey));
    e4c_sync(store);
    c2keynotempty = zerocheck(store->c2key, sizeof(store->c2key));
    if (c2keynotempty) {
        e4c_pubkey_c2sharedsecret_derivestore(store);
    }
    return E4_RESULT_OK;
}

int e4c_get_idseckey(e4storage* store, uint8_t *key) {
    memcpy(key, store->privkey, sizeof(store->privkey));
    return 0;
}

int e4c_get_idpubkey(e4storage* store, uint8_t *key) {
    memcpy(key, store->pubkey, sizeof(store->pubkey));
    return 0;
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
    for (i=0 ; i < E4_TOPICS_MAX ; i++) {
        ZERO(store->topics[i]);
    }

    e4c_sync(store);
    return 0;
}

int e4c_set_idpubkey(e4storage *store, const uint8_t *pubkey) {
    memmove(store->pubkey, pubkey, sizeof(store->pubkey));
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
    for (i=0 ; i < E4_DEVICES_MAX ; i++) {
        ZERO(store->devices[i]);
    }

    e4c_sync(store);
    return 0;
}

int e4c_set_c2_pubkey(e4storage* store, const uint8_t* key) {
    size_t devicekeynotempty = 0;
    memcpy(store->c2key, key, E4_PK_X25519_PUBKEY_LEN);
    e4c_sync(store);
    devicekeynotempty = zerocheck(store->privkey, sizeof(store->privkey));
    if (devicekeynotempty) {
        e4c_pubkey_c2sharedsecret_derivestore(store);
    }
    return E4_RESULT_OK;
}

int e4c_get_c2_pubkey(e4storage* store, uint8_t* key) {
    size_t empty = zerocheck(store->c2key, sizeof(store->c2key));
    if (empty == 0) {
        return E4_ERROR_PERSISTENCE_ERROR;
    }
    memcpy(key, store->c2key, E4_PK_X25519_PUBKEY_LEN);
    return E4_RESULT_OK;
}

int e4c_set_c2sharedsecret(e4storage* store, const uint8_t* key) {
    memcpy(store->c2sharedkey, key, E4_KEY_LEN);
    return E4_RESULT_OK;
}

int e4c_get_c2sharedsecret(e4storage* store, uint8_t* key) {
    size_t empty = zerocheck(store->c2sharedkey, sizeof(store->c2sharedkey));
    if (empty == 0) {
        return E4_ERROR_PERSISTENCE_ERROR;
    }
    memcpy(key, store->c2sharedkey, E4_KEY_LEN);
    return E4_RESULT_OK;
}

#ifdef DEBUG
void e4c_debug_print(e4storage *store)
{
    int i, j;
    char controltopic[E4_CTRLTOPIC_LEN + 1];

    printf("Client\n");
    printf("  ID=");
    for (j = 0; j < E4_ID_LEN; j++)
    {
        printf("%02x", store->id[j]);
    }
    printf("\n");
    printf("  PrivKey=");
    for (j = 0; j < E4_PK_EDDSA_PRIVKEY_LEN; j++)
    {
        printf("%02x", store->privkey[j]);
    }
    printf("\n");
    printf("  PubKey=");
    for (j = 0; j < E4_PK_EDDSA_PUBKEY_LEN; j++)
    {
        printf("%02x", store->pubkey[j]);
    }
    printf("\n");
    printf("  C2PubKey=");
    for (j = 0; j < E4_PK_X25519_PUBKEY_LEN; j++)
    {
        printf("%02x", store->c2key[j]);
    }
    printf("\n");
    e4c_derive_control_topic(controltopic, E4_CTRLTOPIC_LEN + 1, store->id);
    printf("  ControlTopic=%s\n", controltopic);

    for (i = 0; i < store->devicecount; i++)
    {
        printf("  Device %d\n", i);
        printf("    ID=");
        for (j = 0; j < E4_ID_LEN; j++)
        {
            printf("%02x", store->devices[i].id[j]);
        }
        printf("\n");
        printf("    Key=");
        for (j = 0; j < E4_PK_EDDSA_PUBKEY_LEN; j++)
            printf("%02x", store->devices[i].pubkey[j]);
        printf("\n");
    }

    for (i = 0; i < store->topiccount; i++)
    {
        printf("  Topic %d\n", i);
        printf("    Hash=");
        for (j = 0; j < E4_TOPICHASH_LEN; j++)
        {
            printf("%02x", store->topics[i].topic[j]);
        }
        printf("\n");
        printf("    Key=");
        for (j = 0; j < E4_KEY_LEN; j++)
            printf("%02x", store->topics[i].key[j]);
        printf("\n");
    }
}
#endif
#endif
