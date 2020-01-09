

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "e4/crypto/aes256enc.h"
#include "e4/crypto/aes_siv.h"
#include "siv_kat.h"

int main(int argc, char** argv, char** envp) {

	printf("LIBE4 AES-SIV Compatibility testing.\n\n");

    /*int aes256_encrypt_siv(uint8_t *ct,
                       size_t *ctlen, 
                       const uint8_t *ad,
                       size_t adlen, 
                       const uint8_t *pt,
                       size_t ptlen,  
                       const uint8_t *key);

      int aes256_decrypt_siv(uint8_t *pt,
                       size_t *ptlen, 
                       const uint8_t *ad,
                       size_t adlen, 
                       const uint8_t *ct,
                       size_t ctlen, 
                       const uint8_t *key);

typedef struct _e4_aessiv_kat {
    const uint8_t ciphertext[116];
    const uint8_t plaintext[100];
    const uint8_t ad[32];
	const uint8_t key[32];
} e4_aessiv_kat;


    */ 
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
    printf("SIV test executed %d KATs, with %ld failures\n", SIV_KAT_NUM, failures);

    return failures > 0 ? 1 : 0;
}
