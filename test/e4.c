

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e4/e4.h"
#include "e4/util.h"
#include "e4/strlcpy.h"
#include "e4/crypto/sha3.h"

const size_t ITER_MAX = 100;
const size_t PT_MAX = 100;
const size_t NUM_TOPICS = 10;
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
    
    e4c_set_storagelocation(&store, "/tmp/unittests.e4c");

    /* set the identity key */
    bytes_read = fread(&clientid, 1, sizeof clientid, urand_fd);
    if ( bytes_read < sizeof clientid ) {
        printf("Failed: generating clientid bytes read %lu, expected %lu\n", bytes_read, sizeof clientid);
        returncode = 2;
        goto exit_close;
    }
    bytes_read = fread(clientkey, 1, sizeof clientkey, urand_fd);
    if ( bytes_read < sizeof clientkey ) {
        printf("Failed: generating clientkey bytes read %lu, expected %lu\n", bytes_read, sizeof clientid);
        returncode = 3;
        goto exit_close;
    }
    
    e4retcode = e4c_set_id(&store, clientid);
    e4retcode = e4c_set_idkey(&store, clientkey);

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

        sha3(topicname_current, strlen(topicname_current), topichash, E4_TOPICHASH_LEN);
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
        size_t recovered_len = 0;
        unsigned char plaintext_buffer[PT_MAX+1];
        unsigned char ciphertext_buffer[PT_MAX + E4_MSGHDR_LEN + 1];
        unsigned char recovered_buffer[PT_MAX+1];
        uint8_t topicindex = 0;

        char* topicname;

        bytes_read = fread(&topicindex, 1, sizeof topicindex, urand_fd);
        if ( bytes_read < sizeof topicindex ) {
            printf("Failed: unable to read byte for topicindex");
            returncode = 7;
            goto exit_close;
        }
        topicindex = topicindex % NUM_TOPICS;
        topicname = topics[topicindex];

        memset(plaintext_buffer, 0, PT_MAX+1);
        memset(ciphertext_buffer, 0, PT_MAX+E4_MSGHDR_LEN+1);
        memset(recovered_buffer, 0, PT_MAX+1);

        bytes_read = fread(plaintext_buffer, 1, PT_MAX, urand_fd);
        if ( bytes_read < PT_MAX ) {
            printf("Failed: unable to read random plaintext. Asked for %lu bytes, got %lu\n", PT_MAX, bytes_read);
            returncode = 8;
            goto exit_close;
        }

        e4retcode = e4c_protect_message(ciphertext_buffer, PT_MAX+E4_MSGHDR_LEN, &ciphertext_len,
            plaintext_buffer, PT_MAX, topicname, &store);

        if (e4retcode != E4_ERROR_OK) {
            returncode = 12;
            printf("Failed: E4 Error %d\n", e4retcode);
            goto exit_close;
        }

        if ( ciphertext_len != PT_MAX + E4_MSGHDR_LEN ) {
            printf("Failed: decrypted ciphertext has length %lu, should be %lu\n", ciphertext_len, PT_MAX+E4_MSGHDR_LEN);
            returncode = 9;
            goto exit_close;
        }

		/* e4c_unprotect_message zero-pads the output buffer. Perhaps we should 
           get rid of this functionality and leave it to the user. For now, 
           we fix it by passing the correct length of the recovered buffer. */
        e4retcode = e4c_unprotect_message(recovered_buffer, PT_MAX+1, &recovered_len,
            ciphertext_buffer, PT_MAX+E4_MSGHDR_LEN, topicname, &store);

        if (e4retcode != E4_ERROR_OK) {
            returncode = 13;
            printf("Failed: E4 Error %d\n", e4retcode);
            goto exit_close;
        }

        if ( recovered_len != PT_MAX ) {
            printf("Failed: decrypted plaintext has length %lu, should be %lu\n", recovered_len, PT_MAX);
            returncode = 10;
            goto exit_close;
        }


        if ( memcmp(recovered_buffer, plaintext_buffer, sizeof plaintext_buffer) != 0 ) {
            printf("Failed: recovered buffer not equal to plaintext buffer.\n");
            returncode = 11;
            goto exit_close;
        }

    }

exit_close:
    fclose(urand_fd);
exit:
    return returncode;
}
