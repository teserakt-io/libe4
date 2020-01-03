#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define E4_MODE_PUBKEY
#include "e4/crypto/ed25519.h"
#include "e4/crypto/curve25519.h"
#include "e4/crypto/sha3.h"
#include "e4/crypto/xed25519.h"
#include "e4/e4.h"


#include "pubkey_kat.h"

void printhex(const uint8_t* buffer, size_t len)
{
    for (size_t j = 0; j <len; j++)
    {
        printf("%02X", (uint8_t)buffer[j]);
    }
}

int main(int argc, char** argv) {

    for ( int i = 0; i < NUM_PKCATS; i++ )
    {
    // point conversion tests:
    
    uint8_t c2_c255_private[32] = {0};
    xed25519_convert_ed2c_private(c2_c255_private, pkkat[0].c2_edwards_seckey);
    if (memcmp(c2_c255_private, pkkat[0].c2_montgom_seckey, 32) != 0) {
        printf("c25519_privkey_convert failed.\n");
        printhex(c2_c255_private, sizeof c2_c255_private);
        printf("\n");
        printhex(pkkat[0].c2_montgom_seckey, sizeof pkkat[0].c2_montgom_seckey);
        printf("\n");

        //return 1;
    } else {
        printf("c25519_privkey_convert: OK.\n");
    }

    uint8_t c2_c255_pubkey[32] = {0};

    if (xed25519_convert_ed2c_public(c2_c255_pubkey, pkkat[0].c2_edwards_pubkey) != XED25519_RESULT_OK ) {
        
        printf("c25519_pubkey_convert error reported from decode.\n");
    }
    if (memcmp(c2_c255_pubkey, pkkat[0].c2_montgom_pubkey, 32) != 0) {
        printf("c25519_privkey_convert failed.\n");
        printhex(c2_c255_pubkey, sizeof c2_c255_pubkey);
        printf("\n");
        printhex(pkkat[0].c2_montgom_pubkey, sizeof pkkat[0].c2_montgom_pubkey);
        printf("\n");

        //return 1;
    } else {
        printf("c25519_pubkey_convert: OK.\n");
    }

    // curve25519 variant c2->device:
    
    uint8_t c255_shared_point[32] = {0};
    uint8_t c255_shared_secret[32] = {0};

    curve25519(c255_shared_point, pkkat[0].c2_montgom_seckey, pkkat[0].dev_montgom_pubkey);
    sha3(c255_shared_point, sizeof c255_shared_point, c255_shared_secret, sizeof c255_shared_secret);
    
    if (memcmp(c255_shared_secret, pkkat[0].c2_sharedkey, 32) != 0) {
        printf("c25519_kex failed.\n");
        printhex(c255_shared_secret, sizeof c255_shared_secret);
        printf("\n");
        printhex(pkkat[0].c2_sharedkey, sizeof pkkat[0].c2_sharedkey);
        printf("\n");
        printhex(pkkat[0].dev_sharedkey, sizeof pkkat[0].dev_sharedkey);
        printf("\n");

        //return 1;
    } else {
        printf("c25519_kex_c2d: OK.\n");
    }
    
    // curve25519 variant c2->device:
    
    memset(c255_shared_secret, 0, sizeof c255_shared_secret);
    memset(c255_shared_point, 0, sizeof c255_shared_point);

    curve25519(c255_shared_point, pkkat[0].dev_montgom_seckey, pkkat[0].c2_montgom_pubkey);
    sha3(c255_shared_point, sizeof c255_shared_point, c255_shared_secret, sizeof c255_shared_secret);
    
    if (memcmp(c255_shared_secret, pkkat[0].dev_sharedkey, 32) != 0) {
        printf("c25519_kex failed.\n");
        printhex(c255_shared_secret, sizeof c255_shared_secret);
        printf("\n");
        printhex(pkkat[0].c2_sharedkey, sizeof pkkat[0].c2_sharedkey);
        printf("\n");
        printhex(pkkat[0].dev_sharedkey, sizeof pkkat[0].dev_sharedkey);
        printf("\n");

        //return 1;
    } else {
        printf("c25519_kex_d2c: OK.\n");
    }
    }

    return 0;
}
