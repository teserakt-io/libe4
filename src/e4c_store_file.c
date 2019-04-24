//  e4c_store_file.c
//  2018-07-06  Markku-Juhani O. Saarinen <markku@teserakt.io>
//  2019-04-15  Antony Vennard <antony@teserakt.io>

//  (c) 2018 Copyright Teserakt AG

//  Persistent key storage for POSIX devices. Really a toy version
//  intended for little embedded devices.

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "e4/e4.h"
#include "e4/internal/e4c_store_file.h"
#include "e4/crypto/sha3.h"

#define ZERO(X) do {\
    memset(&X, 0, sizeof X); \
} while(0)

int e4c_init(e4storage* store) 
{
    ZERO(store->data.id);
    store->data.topiccount = 0;
    ZERO(store->data.topics);
    return 0;
}

int e4c_load(e4storage* store, const char *path) 
{
    int fd, i;
    size_t rlen = 0;
    char mbuf[4];
    
    if (path == NULL)
        path = "/tmp/persistence.e4p";

    store->filepath = strdup(path);

    fd = open(store->filepath, O_RDONLY);
    if (fd < 0) {
        perror(path);
        return E4ERR_PersistenceError;
    }

    memset(mbuf, 0, sizeof mbuf);
    rlen = read(fd, mbuf, sizeof E4V1_MAGIC);
    if ( memcmp(mbuf, E4V1_MAGIC, sizeof E4V1_MAGIC) != 0 ) {
        goto err;
    }

    rlen = read(fd, store->data.id.id, sizeof store->data.id.id);
    if ( rlen != sizeof store->data.id.id ) {
        goto err;
    }

    rlen = read(fd, store->data.id.key, sizeof store->data.id.key);
    if ( rlen != sizeof store->data.id.key ) {
        goto err;
    }

    rlen = read(fd, &store->data.topiccount, sizeof store->data.topiccount);
    if ( rlen != sizeof store->data.topiccount ) {
        goto err;
    }

    for ( i=0; i < store->data.topiccount; i++ ) {

    }

    return 0;
err:
    e4c_init(store);
    perror(path);
    return E4ERR_PersistenceError;
}

int e4c_sync(e4storage* store) 
{   
    int fd = -1;
    uint16_t i = 0;

    if (store->filepath == NULL)
        return E4ERR_PersistenceError;

    fd = open(store->filepath, O_WRONLY | O_CREAT, 0600);
    if (fd < 0) {
        perror(store->filepath);
        return E4ERR_PersistenceError;
    }

    write(fd, E4V1_MAGIC, sizeof E4V1_MAGIC);
    write(fd, store->data.id.id, sizeof store->data.id.id);
    write(fd, store->data.id.key, sizeof store->data.id.key);
    write(fd, &store->data.topiccount, sizeof store->data.topiccount);

    for ( i=0; i < store->data.topiccount; i++ ) {
        topic_key* t = &(store->data.topics[0]) + i;

        write(fd, t->topic, sizeof t->topic);
        write(fd, t->key, sizeof t->key);
    }

    close(fd);

    return 0;
}

int e4c_set_id(e4storage* store, const uint8_t *id)
{
    memmove(store->data.id.id, id, sizeof store->data.id.id);
    return 0;
}
int e4c_set_idkey(e4storage* store, const uint8_t *key)
{
    memmove(store->data.id.key, key, sizeof store->data.id.key);
    return 0;
}

int e4c_getindex(e4storage* store, const char *topic)
{
    int i;
    uint8_t hash[E4_TOPICHASH_LEN];

    // hash the topic
    sha3(topic, strlen(topic), hash, E4_TOPICHASH_LEN);

    // look for it
    for (i = 0; i < store->data.topiccount; i++) {   // find the key
        if (memcmp(store->data.topics[i].topic, hash, E4_TOPICHASH_LEN) == 0) {
            break;
        }
    }
    if (i >= store->data.topiccount)
        return E4ERR_TopicKeyMissing;

    return i;
}

int e4c_gettopickey(uint8_t *key, e4storage* store, const int index) 
{

    if (index < 0 || index >= store->data.topiccount)
        return E4ERR_TopicKeyMissing;

    memcpy(key, store->data.topics[index].key, E4_KEY_LEN);

    return 0;
}

int e4c_set_topic_key(e4storage* store, const uint8_t *topic_hash, const uint8_t *key)
{
    int i;

    for (i = 0; i < store->data.topiccount; i++) {
        if (memcmp(store->data.topics[i].topic, topic_hash, E4_TOPICHASH_LEN) == 0)
            break;
    }
    if (i >= E4_TOPICS_MAX)                // out of space
        return E4ERR_TopicKeyMissing;

    memcpy(store->data.topics[i].topic, topic_hash, E4_TOPICHASH_LEN);
    memcpy(store->data.topics[i].key, key, E4_KEY_LEN);

    if (i == store->data.topiccount) {               // new topic
        store->data.topiccount++;
    }

    return e4c_sync(store);
}

int e4c_remove_topic(e4storage* store, const uint8_t *topic_hash) 
{
    int i, j;
    topic_key* topic_keys = store->data.topics;

    for (i = 0; i < store->data.topiccount; i++) {

        if (memcmp(topic_keys[i].topic, topic_hash, E4_TOPICHASH_LEN) == 0) {
            // remove this item and move list up
            for (j = i + 1; j < store->data.topiccount; j++) {
                memcpy(&topic_keys[j - 1], &topic_keys[j],
                    sizeof(topic_key));
            }
            ZERO(topic_keys[store->data.topiccount]);
            store->data.topiccount--;

            return e4c_sync(store);
        }
    }

    return E4ERR_TopicKeyMissing;
}

#ifdef DEBUG

void e4c_debug_dumpkeys(e4storage* store)
{
    int i, j;

    for (i = 0; i < store->data.topiccount; i++) {
        printf("!!! %2d topic hash = ", i);
        for (j = 0; j < E4_TOPICHASH_LEN; j++)
            printf("%02X", topic_keys[i].topic[j]);
        printf("\n!!! %2d  topic key = ", i);
        for (j = 0; j < E4_KEY_LEN; j++)
            printf("%02X", topic_keys[i].key[j]);
        printf("\n");
    }
}

#endif
