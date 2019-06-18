

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e4/e4.h"
#include "e4/util.h"

const size_t ITER_MAX = 100;
const size_t PT_MAX = 100;
const size_t NUM_TOPICS = 10;
const size_t SIZE_TOPICNAME = 8;

int main(int argc, char** argv, char** envp) {
    int returncode = 0;
    int e4retcode = 0;
    int iteration = 0;
    FILE* urand_fd = NULL;
    e4storage store;

    char clientid[E4_ID_LEN];
    char clientkey[E4_KEY_LEN];
    char topickey_current[E4_KEY_LEN]; 
    char topicname_tmp[SIZE_TOPICNAME/2];
    char topicname_current[SIZE_TOPICNAME+1];
    char topichash[E4_TOPICHASH_LEN];

    char topics[NUM_TOPICS][SIZE_TOPICNAME+1];

    memset(&store, 0, sizeof e4storage);

    urand_fd = fopen("/dev/urandom", "r");
    if ( urand_fd == NULL  ) {
        /* can't generate random plaintexts in this case */
        returncode = 1;
        goto exit;
    }

    e4retcode = e4c_init(&store);
    if ( e4retcode != 0 ) {
        returncode = 1;
        goto exit_close;
    }
    
    e4c_set_storagelocation(&store, "/tmp/unittests.e4c");

    /* set the identity key */
    bytes_read = fread(clientid, sizeof clientid, 1, urand_fd);
    if ( bytes_read < sizeof clientid ) {
        returncode = 2;
        goto exit_close;
    }
    bytes_read = fread(clientkey, sizeof clientkey, 1, urand_fd);
    if ( bytes_read < sizeof clientkey ) {
        returncode = 2;
        goto exit_close;
    }
    
    e4retcode = e4c_set_id(store, clientid);
    e4retcode = e4c_set_idkey(store, clientkey);

    for ( iteration = 0; iteration<NUM_TOPICS; iteration++ ) {

        int j=0;

        memset(topics[iteration], 0, SIZE_TOPICNAME);
        memset(topicname_current, 0, sizeof topickey_current);

        bytes_read = fread(topickey_current, sizeof topickey_current, 1, urand_fd);
        if ( bytes_read < sizeof topickey_current ) {
            returncode = 2;
            goto exit_close;
        }
        bytes_read = fread(topicname_tmp, SIZE_TOPICNAME/2, 1, urand_fd);
        if ( bytes_read < sizeof topicname_tmp ) {
            returncode = 2;
            goto exit_close;
        }

        for ( j = 0; j < SIZE_TOPICNAME/2; j++ ) {
            snprintf(&topicname_current[2*j], "%02X", topicname_tmp[j]);
        }

        bytes_read = strlcpy(topics[iteration], topicname_current, SIZE_TOPICNAME);
        if ( bytes_read < SIZE_TOPICNAME ) {
            returncode = 2;
            goto exit_close;
        }

        sha3(topicname_current, sizeof topicname_current, topichash, E4_TOPICHASH_LEN);
        e4c_set_topic_key(&store, topichash, topickey_current); 
    }

    /* test sync and reload from file-based storage */
    e4c_sync(&store);
    memset(&store, 0, sizeof store);
    e4c_load(&store, "/tmp/unittests.e4c");

    // TODO: test control messages have their intended effect.

    /* Test E4 encryption using random topic keys */
    for ( iteration = 0; iteration<ITER_MAX; iteration++) {


        size_t bytes_read = 0;
        size_t ciphertext_len = 0;
        size_t plaintext_len = 0;
        char plaintext_buffer[PT_MAX+1];
        char ciphertext_buffer[PT_MAX + E4_MSGHDR_LEN + 1];
        char recovered_buffer[PT_MAX + + 1];
        uint8_t topicindex = 0;

        char* topicname;

        bytes_read = fread(&topicindex, sizeof topicindex, 1, urand_fd);
        if ( bytes_read < sizeof topicindex ) {
            returncode = 2;
            goto exit_close;
        }
        topicindex = topicindex % NUM_TOPICS;
        topicname = topics[topicindex];

        memset(plaintext_buffer, 0, sizeof plaintext_buffer);
        memset(ciphertext_buffer, 0, sizeof ciphertext_buffer);
        memset(recovered_buffer, 0, sizeof recovered_buffer);

        bytes_read = fread(plaintext_buffer, PT_MAX, 1, urand_fd);
        if ( bytes_read < PT_MAX ) {
            returncode = 2;
            goto exit_close;
        }

        e4retcode = e4c_protect_message(ciphertext_buffer, PT_MAX+E4_MSGHDR_LEN, ciphertext_len,
            plaintext_buffer, PT_MAX, topicname, &store);

        e4retcode = e4c_unprotect_message(recovered_buffer, PT_MAX, recovered_len,
            ciphertext_buffer, PT_MAX+E4_MSGHDR_LEN, topicname, &store);

        if ( memcmp(recovered_buffer, plaintext_buffer, sizeof plaintext_buffer) != 0 ) {
            returncode = 3;
            goto exit_close;
        }

    }

exit_close:
    fclose(urand_fd);
exit:
    return returncode;
}
