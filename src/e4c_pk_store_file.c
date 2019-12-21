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

const char E4V1_MAGIC[4] = "E41P";

#define ZERO(X)                                                                \
    do                                                                         \
    {                                                                          \
        memset(&X, 0, sizeof X);                                               \
    } while (0)

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

    /*size_t filesize = */ lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    memset(mbuf, 0, sizeof mbuf);
    rlen = read(fd, mbuf, sizeof E4V1_MAGIC);
    if (rlen != sizeof E4V1_MAGIC)
    {
        goto err;
    }
    if (memcmp(mbuf, E4V1_MAGIC, sizeof E4V1_MAGIC) != 0)
    {
        goto err;
    }

    lseek(fd, 4, SEEK_CUR);

    rlen = read(fd, store->id, sizeof store->id);
    if (rlen != sizeof store->id)
    {
        goto err;
    }

    lseek(fd, 4, SEEK_CUR);

    r = e4c_derive_control_topic(controltopic, E4_CTRLTOPIC_LEN + 1, store->id);
    if (r != 0)
    {
        goto err;
    }

    /* derive a topichash for the control topic. */
    e4c_derive_topichash(store->ctrltopic, E4_TOPICHASH_LEN, controltopic);

    /* read in key material */
    rlen = read(fd, store->privkey, sizeof store->privkey);
    if (rlen != sizeof store->privkey)
    {
        goto err;
    }
    lseek(fd, 4, SEEK_CUR);
    
    rlen = read(fd, store->pubkey, sizeof store->pubkey);
    if (rlen != sizeof store->pubkey)
    {
        goto err;
    }
    lseek(fd, 4, SEEK_CUR);

    rlen = read(fd, &store->topiccount, sizeof store->topiccount);
    if (rlen != sizeof store->topiccount)
    {
        goto err;
    }

    lseek(fd, 4, SEEK_CUR);
    /* TODO: detect if we cannot read everything based on the topiccount */


    for (i = 0; i < store->topiccount; i++)
    {
        rlen = read(fd, store->topics[i].topic, E4_TOPICHASH_LEN);
        if (rlen != E4_TOPICHASH_LEN)
        {
            goto err;
        }

        lseek(fd, 4, SEEK_CUR);
        rlen = read(fd, store->topics[i].key, E4_KEY_LEN);
        if (rlen != E4_KEY_LEN)
        {
            goto err;
        }
        lseek(fd, 4, SEEK_CUR);
    }
    
    rlen = read(fd, &store->devicecount, sizeof store->devicecount);
    if (rlen != sizeof store->devicecount)
    {
        goto err;
    }
    lseek(fd, 4, SEEK_CUR);
    
    for (i = 0; i < store->devicecount; i++)
    {
        rlen = read(fd, store->devices[i].id, E4_ID_LEN);
        if (rlen != E4_ID_LEN)
        {
            goto err;
        }

        lseek(fd, 4, SEEK_CUR);
        rlen = read(fd, store->devices[i].pubkey, E4_PK_EDDSA_PUBKEY_LEN);
        if (rlen != E4_PK_EDDSA_PUBKEY_LEN)
        {
            goto err;
        }
        lseek(fd, 4, SEEK_CUR);
    }

    e4c_debug_print(store);

    close(fd);
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

    uint32_t zero = 0;

    write(fd, E4V1_MAGIC, sizeof E4V1_MAGIC);
    write(fd, &zero, sizeof zero);
    write(fd, store->id, sizeof store->id);
    write(fd, &zero, sizeof zero);
    write(fd, store->privkey, sizeof store->privkey);
    write(fd, &zero, sizeof zero);
    write(fd, store->pubkey, sizeof store->pubkey);
    write(fd, &zero, sizeof zero);
    write(fd, &store->topiccount, sizeof store->topiccount);
    write(fd, &zero, sizeof zero);

    for (i = 0; i < store->topiccount; i++)
    {
        topic_key *t = &(store->topics[0]) + i;

        write(fd, t->topic, sizeof t->topic);
        write(fd, &zero, sizeof zero);
        write(fd, t->key, sizeof t->key);
        write(fd, &zero, sizeof zero);
    }
    write(fd, &store->devicecount, sizeof store->devicecount);
    write(fd, &zero, sizeof zero);

    for (i = 0; i < store->devicecount; i++)
    {
        device_key *d = &(store->devices[0]) + i;

        write(fd, d->id, sizeof d->id);
        write(fd, &zero, sizeof zero);
        write(fd, d->pubkey, sizeof d->pubkey);
        write(fd, &zero, sizeof zero);
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
int e4c_set_idkey(e4storage *store, const uint8_t *key)
{
    memmove(store->privkey, key, sizeof store->privkey);
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
        printf("%02x", store->privkey[j]);
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

/*#endif*/
