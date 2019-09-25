//  e4c_store_eeprom.c
//  2018-07-06  Markku-Juhani O. Saarinen <markku@teserakt.io>

//  (c) 2018 Copyright Teserakt AG

//  Persistent key storage for Arduino -- uses EEPROM

#include <string.h>
#include <avr/eeprom.h>

#include "e4client.h"
#include "e4c_store.h"
#include "sha3.h"

// number of topic keys

#define E4C_TOPICS_MAX 10

static int topic_keys_no = 0;

// This is the topic-key structure

typedef struct {
    uint8_t used;
    uint8_t reserved[3];
    uint8_t topic[E4C_TOPIC_LEN];
    uint8_t key[E4C_KEY_LEN];
} topic_key_t;

static topic_key_t eeprom_keys[E4C_TOPICS_MAX] EEMEM;

// Utility function .. compare with eeprom

static int memcmp_eeprom(const uint8_t *eeprom, const uint8_t *sram, size_t n)
{
    size_t i;

    for (i = 0; i < n; i++) {
        if (eeprom_read_byte(eeprom + i) != sram[i])
            return 1;
    }
        
    return 0;
}

// This is the persistent storage format -- invoked with a physical button
// or something

int e4c_reset_storage()
{
    size_t i;

    for (i = 0; i < E4C_TOPICS_MAX; i++)
        eeprom_update_byte(&eeprom_keys[i].used, 0x00);

    topic_keys_no = 0;

    return 0;
}

// Initialize and check if persistent storage is valid. The path is 
// optional -- default filename is used if set to NULL. Ignored with EEPROM.

int e4c_init(const char *path)
{
    size_t i;

    topic_keys_no = 0;

    for (i = 0; i < E4C_TOPICS_MAX; i++) {
        if (eeprom_read_byte(&eeprom_keys[i].used) != 0)
            topic_keys_no++;
    }   

    return topic_keys_no;
}

// Synchronize the persistence

int e4c_sync()
{
    return 0;
}

// Free all resources

int e4c_free()
{   
    return 0;
}

// Fetch an index of a key of given hash. The command channel is 0.
// Returns a negative on failure.

int e4c_getindex(const char *topic)
{
    int i;
    uint8_t hash[E4C_TOPIC_LEN];

    // hash the topic
    sha3(topic, strlen(topic), hash, E4C_TOPIC_LEN);

    // look for it
    for (i = 0; i < E4C_TOPICS_MAX; i++) {   // find the key
        if (eeprom_read_byte(&eeprom_keys[i].used) == 0)
            continue;
        if (memcmp_eeprom(eeprom_keys[i].topic, hash, E4C_TOPIC_LEN) == 0) {
            break;
        }
    }
    if (i >= E4C_TOPICS_MAX)
        return E4ERR_TopicKeyMissing;

    return i;
}

// get a key by index

int e4c_getkey(uint8_t * key, int index)
{
    if (index < 0 || index >= E4C_TOPICS_MAX || 
        eeprom_read_byte(&eeprom_keys[index].used) == 0)
        return E4ERR_TopicKeyMissing;       
        
    eeprom_read_block(key, eeprom_keys[index].key, E4C_KEY_LEN);

    return 0;
}

// Remove topic (hash)

int e4c_remove_topic(const uint8_t *topic_hash)
{
    size_t i;

    for (i = 0; i < E4C_TOPICS_MAX; i++) {
        if (eeprom_read_byte(&eeprom_keys[i].used) == 0)
            continue;
        if (memcmp_eeprom(eeprom_keys[i].topic, topic_hash, 
            E4C_TOPIC_LEN) == 0) {
            // remove this item by marking it free
            eeprom_write_byte(&eeprom_keys[i].used, 0);
            topic_keys_no--;
        }
    }

    return E4ERR_TopicKeyMissing;
}

// Clear all topics except ID key

int e4c_reset_topics()
{
    size_t i;

    for (i = 1; i < E4C_TOPICS_MAX; i++) {
        if (eeprom_read_byte(&eeprom_keys[i].used) == 0)
            continue;
        eeprom_write_byte(&eeprom_keys[i].used, 0);
        topic_keys_no--;
    }

    return 0;
}

// set id key for this instance index 0

int e4c_set_id_key(const uint8_t *topic_hash, const uint8_t *key)
{
    if (topic_hash != NULL)
        eeprom_update_block(topic_hash, eeprom_keys[0].topic, E4C_TOPIC_LEN);
    if (key != NULL)    
        eeprom_update_block(key, eeprom_keys[0].key, E4C_KEY_LEN);

    if (topic_hash != NULL && key != NULL) {
        eeprom_update_byte(&eeprom_keys[0].used, 0xFF);
        if (topic_keys_no < 1)
            topic_keys_no = 1;
    }

    return 0;
}

// set key for given topic (hash)

int e4c_set_topic_key(const uint8_t *topic_hash, const uint8_t *key)
{
    int i;

    for (i = 0; i < E4C_TOPICS_MAX; i++) {
        if (eeprom_read_byte(&eeprom_keys[i].used) == 0)
            continue;
        if (memcmp_eeprom(eeprom_keys[i].topic, topic_hash, 
            E4C_TOPIC_LEN) == 0) {
            // update key only
            eeprom_update_block(key, eeprom_keys[i].key, E4C_KEY_LEN);
            return 0;
        }
    }

    // must be new, look for a free spot
    for (i = 0; i < E4C_TOPICS_MAX; i++) {
        if (eeprom_read_byte(&eeprom_keys[i].used) == 0) {

            // mark used, update key and hash
            eeprom_update_byte(&eeprom_keys[i].used, 0xFF);
            eeprom_update_block(topic_hash, eeprom_keys[i].topic, 
                E4C_TOPIC_LEN);
            eeprom_update_block(key, eeprom_keys[i].key, E4C_KEY_LEN);
            return 0;
        }
    }

    return E4ERR_TopicKeyMissing;
}

