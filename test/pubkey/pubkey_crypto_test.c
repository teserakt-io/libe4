#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define E4_MODE_PUBKEY
#include "e4/crypto/ed25519.h"
#include "e4/crypto/curve25519.h"
#include "e4/crypto/sha3.h"
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

    uint8_t shared_point[32] = {0};
    uint8_t shared_secret[32] = {0};

    ed25519_key_exchange((unsigned char*) shared_point, DEVEDWARDS_PUBKEY_1, C2EDWARDS_SECKEY_1);

    sha3(shared_point, sizeof shared_point, shared_secret, sizeof shared_secret);
    if (sizeof shared_secret != sizeof DEVSHAREDKEY_1) {
        printf("ed25519_kex shared secret size mismatch");
        return 1;
    }

    if (memcmp(shared_secret, DEVSHAREDKEY_1, 32) != 0) {
        printf("ed25519_kex failed.\n");
        printhex(shared_secret, sizeof shared_secret);
        printf("\n");
        printhex(DEVSHAREDKEY_1, sizeof DEVSHAREDKEY_1);
        printf("\n");
        printhex(C2SHAREDKEY_1, sizeof DEVSHAREDKEY_1);
        printf("\n");

        return 1;
    }
    return 0;
}
