#include "e4/crypto/sha3.h"
#include "e4/e4.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int e4c_derive_clientid(char *clientid, const size_t clientidlen, const char *clientname, const size_t clientnamelen)
{

    if (clientidlen < E4_ID_LEN)
    {
        return E4_ID_LEN;
    }

    sha3(clientname, clientnamelen, clientid, E4_ID_LEN);
    return 0;
}

int e4c_derive_control_topic(char *topic, const size_t topiclen, const uint8_t *clientid)
{

    int i = 0;

    if (topiclen < E4_CTRLTOPIC_LEN + 1)
    {
        return E4_CTRLTOPIC_LEN + 1;
    }

    snprintf(topic, topiclen, "e4/");
    for (i = 0; i < E4_ID_LEN; i++)
    {
        const size_t adjust = 3 + 2 * i;
        snprintf((char *)(topic + adjust), topiclen - adjust, "%02x", clientid[i]);
    }

    return 0;
}

char hex_lower[] = "0123456789abcdef";
char hex_upper[] = "0123456789ABCDEF";

int e4c_hex_decode(char *bytes, const size_t byteslen, char *hexstring, const size_t hexstringlen)
{

    size_t reqbytes = 0;
    int i = 0;

    // can't decode empty string; can't decode odd bytes,
    if (hexstringlen == 0 || hexstringlen % 2 == 1)
    {
        printf("length condition failed.\n");
        return 0;
    }
    reqbytes = hexstringlen >> 1;

    if (reqbytes > byteslen)
    {
        printf("reqbytes is wrong. r=%ld, hl=%ld, bl=%ld\n", reqbytes, hexstringlen, byteslen);
        return 0;
    }

    for (i = 0; i < reqbytes; i++)
    {

        unsigned char byte = 0;
        char hbits = hexstring[2 * i];
        char lbits = hexstring[2 * i + 1];
        char *pos = NULL;
        int val = -1;

        pos = strchr(hex_upper, hbits);
        if (pos != NULL)
        {
            val = (int)(pos - hex_upper);
        }
        pos = strchr(hex_lower, hbits);
        if (pos != NULL)
        {
            val = (int)(pos - hex_lower);
        }
        if (val < 0)
        {
            printf("val is %d\n", val);
            return 0;
        }

        byte |= val << 4;
        pos = NULL;
        val = -1;

        pos = strchr(hex_upper, lbits);
        if (pos != NULL)
        {
            val = (int)(pos - hex_upper);
        }
        pos = strchr(hex_lower, lbits);
        if (pos != NULL)
        {
            val = (int)(pos - hex_lower);
        }
        if (val < 0)
        {
            printf("val is %d\n", val);
            return 0;
        }

        byte |= val;

        memcpy(bytes + i, &byte, 1);
    }

    return (int)reqbytes;
}
