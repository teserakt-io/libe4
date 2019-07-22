
#ifndef E4UTIL_H
#define E4UTIL_H

/* Derives a ClientID. Depends on the constant E4_ID_LEN */
int e4c_derive_clientid(uint8_t *clientid, const size_t clientidlen, const char *clientname, const size_t clientnamelen);

/* Derives a TopicHash. Depends on the constant E4_TOPICHASH_LEN */
int e4c_derive_topichash(uint8_t* topichash, const size_t topichash_len, const char* topic);

// Produces a control topic from a E4_ID_LEN byte clientid. Length of
// clientid is assumed to be E4_ID_LEN bytes. Control topics are
// e4/<hex of clientid>.
int e4c_derive_control_topic(char *topic, const size_t topiclen, const uint8_t *clientid);

int e4c_hex_decode(char *bytes, const size_t byteslen, const char *hexstring, const size_t hexstringlen);
#endif
