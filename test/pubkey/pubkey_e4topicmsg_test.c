#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    //size_t bytes_read = 0;
    FILE* urand_fd = NULL;
    e4storage store;

    size_t messagelen = 0;
    uint8_t messagebuffer[33]; /* e4c_unprotect leaves an additional 0 */
    char ctrltopic[E4_CTRLTOPIC_LEN+1] = {0};

    memset(&store, 0, sizeof(store));

    urand_fd = fopen("/dev/urandom", "r");
    if (urand_fd == NULL) {
        /* can't generate random plaintexts in this case */
        printf("Failed: unable to open /dev/urandom\n");
        returncode = 1;
        goto exit;
    }

    e4retcode = e4c_init(&store);
        if (e4retcode != 0) {
            printf("Failed: unable to init e4store\n");
            returncode = 1;
            goto exit_close;
        }

        e4c_set_storagelocation(&store, "/tmp/unittestspk_topic.e4c");

        e4retcode = e4c_set_id(&store, pkkat[0].deviceid);
        if (e4retcode != 0) {
            printf("Failed: unable to set id\n");
            returncode = 1;
            goto exit_close;
        }

        e4retcode = e4c_set_idseckey(&store, pkkat[0].dev_edwards_seckey);
        if (e4retcode != 0) {
            printf("Failed: unable to set idseckey\n");
            returncode = 1;
            goto exit_close;
        }
        e4retcode = e4c_set_idpubkey(&store, pkkat[0].dev_edwards_pubkey);
        if (e4retcode != 0) {
            printf("Failed: unable to set idpubkey\n");
            returncode = 1;
            goto exit_close;
        }

        e4retcode = e4c_set_c2_pubkey(&store, pkkat[0].c2_montgom_pubkey);
        if (e4retcode != 0) {
            printf("Failed: unable to set c2 pubkey\n");
            returncode = 1;
            goto exit_close;
        }

        uint8_t c2pk[E4_PK_X25519_PUBKEY_LEN] = {0};
        e4retcode = e4c_get_c2_pubkey(&store, c2pk);
        if (e4retcode != 0) {
            printf("Failed: unable to set c2 pubkey\n");
            returncode = 1;
            goto exit_close;
        }
        if (memcmp(c2pk, pkkat[0].c2_montgom_pubkey, sizeof(pkkat[0].c2_montgom_pubkey)) != 0) {
            printf("Failed: unable to retrieve c2 pubkey we set. Data is corrupted.\n");
            returncode = 1;
            goto exit_close;
        }

        if (e4c_derive_control_topic(ctrltopic, E4_CTRLTOPIC_LEN+1, pkkat[0].deviceid) != 0) {
            printf("Failed: unable to derive control topic\n");
            returncode = 1;
            goto exit_close;
        }

    for (int i = 0; i < NUM_TOPICCATS; i++) {


        unsigned char topichash[E4_TOPICHASH_LEN] = {0};

        if (e4c_set_device_key(&store, topickat[i].otherdevice_id, topickat[i].otherdevice_pubkey) != E4_RESULT_OK) {
            printf("Unable to set device key");
            returncode = 1;
            goto exit_close;
        }

        e4c_derive_topichash(topichash, E4_TOPICHASH_LEN, topickat[i].topicname);
        e4c_set_topic_key(&store, topichash, topickat[i].topickey);
        
        memset(messagebuffer, 0, sizeof(messagebuffer));
        e4retcode = e4c_unprotect_message(messagebuffer,
                          sizeof(messagebuffer),
                          &messagelen,
                          topickat[i].e4_ciphertext,
                          sizeof(topickat[i].e4_ciphertext),
                          topickat[i].topicname,
                          &store,
                          E4_OPTION_IGNORE_TIMESTAMP);

        if (e4retcode != E4_RESULT_OK) {
            printf("Failed: e4c_unprotect_message returned error code other than 'OK'\n");
            printf("      : return code was %d\n", e4retcode);
            printf("      : test instance is %d\n", i);
            returncode = 1;
            goto exit_close;
        }
        
        if (memcmp(messagebuffer, topickat[i].plaintext, sizeof(topickat[i].plaintext)) != 0) {
            printf("Failed: decrypted plaintext not equal to KAT.\n");
            returncode = 1;
            goto exit_close;
        }
        
        e4retcode = e4c_remove_device(&store, topickat[i].otherdevice_id);
        if (e4retcode != E4_RESULT_OK) {
#ifdef DEBUG
            e4c_debug_print(&store);
#endif
            printf("ID: ");
            for (int j = 0; j < E4_ID_LEN; j++) {
                printf("%02x ", topickat[i].otherdevice_id[j]);
            }
            printf("\n");
            printf("Failed: e4c_remove_device could not remove a devicekey\n");
            returncode = 1;
            goto exit_close;
        }
    }

exit_close:
    fclose(urand_fd);
exit:
    return returncode;
}
