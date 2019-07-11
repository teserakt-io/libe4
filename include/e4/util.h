
#ifndef E4UTIL_H
#define E4UTIL_H

// Takes a client name or some other value identifying the client and produces
// a unique ID by taking the sha3 of this value.
int e4c_derive_clientid(char *clientid,
                        const size_t clientidlen,
                        const char *clientname,
                        const size_t clientnamelen);

// Produces a control topic from a E4_ID_LEN byte clientid. Length of
// clientid is assumed to be E4_ID_LEN bytes. Control topics are
// e4/<hex of clientid>.
int e4c_derive_control_topic(char *topic, const size_t topiclen, const uint8_t *clientid);

int e4c_hex_decode(char *bytes, const size_t byteslen, const char *hexstring, const size_t hexstringlen);
#endif
