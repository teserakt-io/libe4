

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "e4/crypto/aes256enc.h"
#include "e4/crypto/aes_siv.h"
#include "siv_kat.h"

int main(int argc, char** argv, char** envp) {

    size_t failures = 0;

    for (int i = 0; i < SIV_KAT_NUM; i++) {

        uint8_t generated_ct[116] = {0};
        uint8_t recovered_pt[100] = {0};
        size_t ptlen = 100;
        size_t ctlen = 116;

        if (aes256_encrypt_siv(generated_ct, &ctlen, 
                    siv_kats[i].ad, sizeof(siv_kats[i].ad),
                    siv_kats[i].plaintext, sizeof(siv_kats[i].plaintext), 
                    siv_kats[i].key) != 0)
        {
            printf("encrypt-siv func failure ");
            failures += 1;
            continue;
        }
                    
        if (memcmp(generated_ct, siv_kats[i].ciphertext, sizeof(generated_ct)) != 0) {
            printf("encrypt-siv output failure ");
            failures += 1;
            continue;
        }

        if (aes256_decrypt_siv(recovered_pt, &ptlen, 
                    siv_kats[i].ad, sizeof(siv_kats[i].ad),
                    siv_kats[i].ciphertext, sizeof(siv_kats[i].ciphertext),
                    siv_kats[i].key) != 0)
        {
            printf("decrypt-siv func failure ");
            failures += 1;
            continue;
        }

        if (memcmp(recovered_pt, siv_kats[i].plaintext, sizeof(recovered_pt)) != 0) {
            printf("decrypt-siv output failure ");
            failures += 1;
            continue;
        }
    }
    printf("\n");
    printf("SIV test: (executed %d KATs, with %ld divergences) %s\n", SIV_KAT_NUM, failures, failures==0 ? "OK" : "Failed");

    return failures > 0 ? 1 : 0;
}
