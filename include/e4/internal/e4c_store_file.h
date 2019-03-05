//  e4c_store.h
//  2018-07-06  Markku-Juhani O. Saarinen <markku@teserakt.io>

//  (c) 2018 Copyright Teserakt AG

//  Persistent key storage.

#ifndef _E4C_STORE_H_
#define _E4C_STORE_H_

#include <stdint.h>
#include <stddef.h>

// Initialize and check if persistent storage is valid. The path is 
// optional -- default filename is used if set to NULL. Ignored with EEPROM.

int e4c_init(const char *path);

// Synchronize with persistent storage

int e4c_sync();

// Free all resources

int e4c_free();

// Internal functions

int e4c_getindex(const char *topic);

int e4c_getkey(uint8_t *key, int index);

int e4c_remove_topic(const uint8_t *topic_hash);

int e4c_reset_topics();

int e4c_reset_storage();

int e4c_set_id_key(const uint8_t *cmd_topic_hash, const uint8_t *key);

int e4c_set_topic_key(const uint8_t *topic_hash, const uint8_t *key);

// == Debug ==

void e4c_debug_dumpkeys();

#endif
