//  e4c_store_file.c
//  2018-07-06  Markku-Juhani O. Saarinen <markku@teserakt.io>
//  2019-04-15  Antony Vennard <antony@teserakt.io>

//  (c) 2018 Copyright Teserakt AG

//  Persistent key storage for POSIX devices. Really a toy version
//  intended for little embedded devices.

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "e4/crypto/sha3.h"
#include "e4/e4.h"
#include "e4/internal/e4c_store_file.h"
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
    ZERO(store->key);
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


    // derive a topichash for the control topic.
    sha3(controltopic, E4_CTRLTOPIC_LEN, store->ctrltopic, E4_ID_LEN);

    rlen = read(fd, store->key, sizeof store->key);
    if (rlen != sizeof store->key)
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
    // TODO: detect if we cannot read everything based on the topiccount


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
    write(fd, store->key, sizeof store->key);
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
    close(fd);

    return 0;
}

int e4c_set_id(e4storage *store, const uint8_t *id)
{
    memmove(store->id, id, sizeof store->id);
    return E4_ERROR_OK;
}
int e4c_set_idkey(e4storage *store, const uint8_t *key)
{
    memmove(store->key, key, sizeof store->key);
    return E4_ERROR_OK;
}

int e4c_getindex(e4storage *store, const char *topic)
{
    int i;
    uint8_t hash[E4_TOPICHASH_LEN];

    // hash the topic
    sha3(topic, strlen(topic), hash, E4_TOPICHASH_LEN);
    // look for it
    for (i = 0; i < store->topiccount; i++)
    { // find the key
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

    // hash the topic
    sha3(topic, strlen(topic), hash, E4_TOPICHASH_LEN);
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
    if (i >= E4_TOPICS_MAX) // out of space
        return E4_ERROR_TOPICKEY_MISSING;

    memcpy(store->topics[i].topic, topic_hash, E4_TOPICHASH_LEN);
    memcpy(store->topics[i].key, key, E4_KEY_LEN);

    if (i == store->topiccount)
    { // new topic
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
            // remove this item and move list up
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

//#ifdef DEBUG

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
    printf("  Key=");
    for (j = 0; j < E4_KEY_LEN; j++)
    {
        printf("%02x", store->key[j]);
    }
    printf("\n");
    e4c_derive_control_topic(controltopic, E4_CTRLTOPIC_LEN + 1, store->id);
    printf("  ControlTopic=%s\n", controltopic);

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

//#endif
