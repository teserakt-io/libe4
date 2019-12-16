#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define E4_MODE_PUBKEY

#include "e4/e4.h"
#include "e4/util.h"
#include "e4/strlcpy.h"
#include "e4/crypto/sha3.h"

#include "pubkey_kat.h"

const size_t ITER_MAX = 100;
const size_t PT_MAX = 100;
const size_t NUM_TOPICS = 10;
const size_t NUM_DEVICES = 10;
const size_t SIZE_TOPICNAME = 8;

int main(int argc, char** argv, char** envp) {
    int returncode = 0;
    int e4retcode = 0;
    int iteration = 0;
    size_t bytes_read = 0;
    FILE* urand_fd = NULL;
    e4storage store;

    unsigned char clientid[E4_ID_LEN];
    unsigned char clientkey[E4_KEY_LEN];
    unsigned char topickey_current[E4_KEY_LEN]; 
    unsigned char topicname_tmp[SIZE_TOPICNAME/2];
    char topicname_current[SIZE_TOPICNAME+1];
    unsigned char topichash[E4_TOPICHASH_LEN];
    char topics[NUM_TOPICS][SIZE_TOPICNAME+1];
    unsigned char deviceid1[E4_ID_LEN];
    unsigned char deviceid2[E4_ID_LEN];
    unsigned char deviceid3[E4_ID_LEN];
    unsigned char deviceid4[E4_ID_LEN];
    unsigned char deviceid5[E4_ID_LEN];
    unsigned char deviceid6[E4_ID_LEN];
    unsigned char deviceid7[E4_ID_LEN];
    unsigned char deviceid8[E4_ID_LEN];
    unsigned char deviceid9[E4_ID_LEN];
    unsigned char deviceid10[E4_ID_LEN];

    memset(&store, 0, sizeof store);

    urand_fd = fopen("/dev/urandom", "r");
    if ( urand_fd == NULL  ) {
        /* can't generate random plaintexts in this case */
        printf("Failed: unable to open /dev/urandom\n");
        returncode = 1;
        goto exit;
    }

    e4retcode = e4c_init(&store);
    if ( e4retcode != 0 ) {
        printf("Failed: unable to init e4store\n");
        returncode = 1;
        goto exit_close;
    }
    
    e4c_set_storagelocation(&store, "/tmp/unittestspk.e4c");

    e4retcode = e4c_set_id(&store, DEVICEID_1);
    e4retcode = e4c_set_idkey(&store, DEVEDWARDS_SECKEY_1);

    for ( iteration = 0; iteration<NUM_TOPICS; iteration++ ) {

        int j=0;

        memset(topics[iteration], 0, SIZE_TOPICNAME);
        memset(topicname_current, 0, sizeof topicname_current);
        memset(topickey_current, 0, sizeof topickey_current);

        bytes_read = fread(topickey_current, 1, sizeof topickey_current, urand_fd);
        if ( bytes_read < sizeof topickey_current ) {
            printf("Failed: generating topickey_current bytes read %lu, expected %lu\n", bytes_read, sizeof topickey_current);
            returncode = 4;
            goto exit_close;
        }
        bytes_read = fread(topicname_tmp, 1, SIZE_TOPICNAME/2, urand_fd);
        if ( bytes_read < sizeof topicname_tmp ) {
            printf("Failed: generating topicname bytes read %lu, expected %lu\n", bytes_read, SIZE_TOPICNAME/2);
            returncode = 5;
            goto exit_close;
        }

        for ( j = 0; j < SIZE_TOPICNAME/2; j++ ) {
            snprintf(&topicname_current[2*j], (sizeof topicname_current)-(2*j), "%02x", topicname_tmp[j]);
        }
        bytes_read = strlcpy(&topics[iteration][0], topicname_current, SIZE_TOPICNAME+1);
        if ( bytes_read < SIZE_TOPICNAME ) {
            printf("Failed: strlcpy topicname->iteration, read %lu, expected %lu\n", bytes_read, SIZE_TOPICNAME);
            returncode = 6;
            goto exit_close;
        }

        e4c_derive_topichash(topichash, E4_TOPICHASH_LEN, topicname_current);
        e4c_set_topic_key(&store, topichash, topickey_current); 
    }

    memset(deviceid1, 0, sizeof deviceid1);
    bytes_read = fread(deviceid1, 1, sizeof deviceid1, urand_fd);
    if ( bytes_read < sizeof deviceid1 ) {
        printf("Failed: generating deviceid bytes read %lu, expected %lu\n", bytes_read, sizeof deviceid1);
        returncode = 4;
        goto exit_close;
    }
    e4c_set_device_key(&store, deviceid1, DEVEDWARDS_PUBKEY_1);
    
    memset(deviceid2, 0, sizeof deviceid2);
    bytes_read = fread(deviceid2, 1, sizeof deviceid2, urand_fd);
    if ( bytes_read < sizeof deviceid2 ) {
        printf("Failed: generating deviceid2 bytes read %lu, expected %lu\n", bytes_read, sizeof deviceid2);
        returncode = 4;
        goto exit_close;
    }
    e4c_set_device_key(&store, deviceid2, DEVEDWARDS_PUBKEY_2);


    /* test sync and reload from file-based storage */
    e4c_sync(&store);
    memset(&store, 0, sizeof store);
    e4c_load(&store, "/tmp/unittestspk.e4c");

exit_close:
    fclose(urand_fd);
exit:
    return returncode;
}
