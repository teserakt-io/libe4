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

    size_t messagelen = 0;
    uint8_t messagebuffer[100];
    char ctrltopic[E4_CTRLTOPIC_LEN+1] = {0};

    memset(&store, 0, sizeof store);

    urand_fd = fopen("/dev/urandom", "r");
    if ( urand_fd == NULL  ) {
        /* can't generate random plaintexts in this case */
        printf("Failed: unable to open /dev/urandom\n");
        returncode = 1;
        goto exit;
    }

    for ( int i = 0; i < NUM_PKCATS; i++ ) {

        e4retcode = e4c_init(&store);
        if ( e4retcode != 0 ) {
            printf("Failed: unable to init e4store\n");
            returncode = 1;
            goto exit_close;
        }

        e4c_set_storagelocation(&store, "/tmp/unittestspk_cmd.e4c");

        e4retcode = e4c_set_id(&store, pkkat[i].deviceid);
        if ( e4retcode != 0 ) {
            printf("Failed: unable to set id\n");
            returncode = 1;
            goto exit_close;
        }

        e4retcode = e4c_set_idkey(&store, pkkat[i].dev_edwards_seckey);
        if ( e4retcode != 0 ) {
            printf("Failed: unable to set idkey\n");
            returncode = 1;
            goto exit_close;
        }
        e4retcode = e4c_set_idpubkey(&store, pkkat[i].dev_edwards_pubkey);
        if ( e4retcode != 0 ) {
            printf("Failed: unable to set idpubkey\n");
            returncode = 1;
            goto exit_close;
        }

        e4retcode = e4c_set_c2_pubkey(&store, pkkat[i].c2_montgom_pubkey);
        if ( e4retcode != 0 ) {
            printf("Failed: unable to set c2 pubkey\n");
            returncode = 1;
            goto exit_close;
        }

        uint8_t c2pk[E4_PK_X25519_PUBKEY_LEN] = {0};
        e4retcode = e4c_get_c2_pubkey(&store, c2pk);
        if ( e4retcode != 0 ) {
            printf("Failed: unable to set c2 pubkey\n");
            returncode = 1;
            goto exit_close;
        }
        if ( memcmp(c2pk, pkkat[i].c2_montgom_pubkey, sizeof pkkat[i].c2_montgom_pubkey) != 0 ) {
            printf("Failed: unable to retrieve c2 pubkey we set. Data is corrupted.\n");
            returncode = 1;
            goto exit_close;
        }

        if ( e4c_derive_control_topic(ctrltopic, E4_CTRLTOPIC_LEN+1, pkkat[i].deviceid) != 0 ) {
            printf("Failed: unable to derive control topic\n");
            returncode = 1;
            goto exit_close;
        }
        /* now we are set up, let's try processing those command messages */

        memset(messagebuffer, 0, sizeof messagebuffer);
        e4retcode = e4c_unprotect_message(messagebuffer,
                          sizeof messagebuffer,
                          &messagelen,
                          pkkat[i].cmd_resettopics,
                          pkkat[i].cmd_resettopics_len,
                          ctrltopic,
                          &store);

        if ( e4retcode != E4_RESULT_OK_CONTROL ) {
            printf("Failed: e4c_unprotect_message returned error code other than 'this is a command'\n");
            printf("      : return code was %d\n", e4retcode);
            printf("      : command is resettopics\n");
            printf("      : test instance is %d\n", i);
            returncode = 1;
            goto exit_close;
        }

        memset(messagebuffer, 0, sizeof messagebuffer);
        e4retcode = e4c_unprotect_message(messagebuffer,
                          sizeof messagebuffer,
                          &messagelen,
                          pkkat[i].cmd_settopickey,
                          pkkat[i].cmd_settopickey_len,
                          ctrltopic,
                          &store);

        if ( e4retcode != E4_RESULT_OK_CONTROL ) {
            printf("Failed: e4c_unprotect_message returned error code other than 'this is a command'\n");
            printf("      : return code was %d\n", e4retcode);
            printf("      : command is settopickey\n");
            printf("      : test instance is %d\n", i);
            returncode = 1;
            goto exit_close;
        }

        memset(messagebuffer, 0, sizeof messagebuffer);
        e4retcode = e4c_unprotect_message(messagebuffer,
                          sizeof messagebuffer,
                          &messagelen,
                          pkkat[i].cmd_removetopic,
                          pkkat[i].cmd_removetopic_len,
                          ctrltopic,
                          &store);

        if ( e4retcode != E4_RESULT_OK_CONTROL ) {
            printf("Failed: e4c_unprotect_message returned error code other than 'this is a command'\n");
            printf("      : return code was %d\n", e4retcode);
            printf("      : command is removetopic\n");
            printf("      : test instance is %d\n", i);
            returncode = 1;
            goto exit_close;
        }

        memset(messagebuffer, 0, sizeof messagebuffer);
        e4retcode = e4c_unprotect_message(messagebuffer,
                          sizeof messagebuffer,
                          &messagelen,
                          pkkat[i].cmd_resetpubkeys,
                          pkkat[i].cmd_resetpubkeys_len,
                          ctrltopic,
                          &store);

        if ( e4retcode != E4_RESULT_OK_CONTROL ) {
            printf("Failed: e4c_unprotect_message returned error code other than 'this is a command'\n");
            printf("      : return code was %d\n", e4retcode);
            printf("      : command is resetpubkeys\n");
            printf("      : test instance is %d\n", i);
            returncode = 1;
            goto exit_close;
        }

        memset(messagebuffer, 0, sizeof messagebuffer);
        e4retcode = e4c_unprotect_message(messagebuffer,
                          sizeof messagebuffer,
                          &messagelen,
                          pkkat[i].cmd_setpubkey,
                          pkkat[i].cmd_setpubkey_len,
                          ctrltopic,
                          &store);

        if ( e4retcode != E4_RESULT_OK_CONTROL ) {
            printf("Failed: e4c_unprotect_message returned error code other than 'this is a command'\n");
            printf("      : return code was %d\n", e4retcode);
            printf("      : command is setpubkey\n");
            printf("      : test instance is %d\n", i);
            returncode = 1;
            goto exit_close;
        }

        memset(messagebuffer, 0, sizeof messagebuffer);
        e4retcode = e4c_unprotect_message(messagebuffer,
                          sizeof messagebuffer,
                          &messagelen,
                          pkkat[i].cmd_removepubkey,
                          pkkat[i].cmd_removepubkey_len,
                          ctrltopic,
                          &store);

        if ( e4retcode != E4_RESULT_OK_CONTROL ) {
            printf("Failed: e4c_unprotect_message returned error code other than 'this is a command'\n");
            printf("      : return code was %d\n", e4retcode);
            printf("      : command is removepubkey\n");
            printf("      : test instance is %d\n", i);
            returncode = 1;
            goto exit_close;
        }
    }

exit_close:
    fclose(urand_fd);
exit:
    return returncode;
}
