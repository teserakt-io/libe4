//  e4c_store.h
//  2018-07-06  Markku-Juhani O. Saarinen <markku@teserakt.io>

//  (c) 2018 Copyright Teserakt AG

//  Persistent key storage.

#ifndef _E4C_STORE_H_
#define _E4C_STORE_H_

#include <stdint.h>
#include <stddef.h>


#define E4_TOPICS_MAX 100

const char E4V1_MAGIC[] = "E41P";

// In memory structures that represent the file. This may need to become 
typedef struct {
    uint8_t id[E4_ID_LEN];
    uint8_t key[E4_KEY_LEN];
} identity;

typedef struct {
    uint8_t topic[E4C_TOPIC_LEN];
    uint8_t key[E4_KEY_LEN];
} topic_key;

typedef struct {
    identity id;
    uint16_t topiccount;
    topic_key topics[E4_TOPICS_MAX]; // this is a vararg struct.
} e4data;

typedef struct {
    e4data data;
    char* filepath;
} e4storage;


int e4c_init(e4storage* store, const char *path);
int e4c_sync(e4storage* store);
int e4c_free(e4storage* store);
int e4c_reset_topics(e4storage* store);
int e4c_reset_storage(e4storage* stor);

int e4c_set_id(e4storage* store, const uint8_t *id);
int e4c_set_idkey(e4storage* store, const uint8_t *key);

int e4c_getindex(e4storage* store, const char *topic);
int e4c_getkey(uint8_t *key, e4storage* store, const int index);

int e4c_remove_topic(e4storage* store, const uint8_t *topic_hash);
int e4c_set_topic_key(e4storage* store, const uint8_t *, const uint8_t *key);

// == Debug ==

void e4c_debug_dumpkeys();

#endif
