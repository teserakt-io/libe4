
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "e4/crypto/selftest.h"

int main(int argc, char** argv, char** envp) {

    int r_sha3, r_shake, r_aes, r_siv, result = 0;

	printf("LIBE4 Crypto Implementation Testing.\n\n");
    
    r_sha3 = test_sha3(); result += r_sha3;
    printf("Testing SHA3 (ALL):\t\t%s\n", r_sha3 != 0 ? "failed" : "ok");
    r_shake = test_shake(); result += r_shake;
    printf("Testing SHAKE (ALL):\t\t%s\n", r_shake != 0 ? "failed" : "ok");
    r_aes = test_aes256(); result += r_aes;
    printf("Testing AES-256:\t\t%s\n", r_aes != 0 ? "failed" : "ok");
    r_siv = test_aes_siv(); result += r_siv;
    printf("Testing SIV-AES-256:\t\t%s\n", r_siv != 0 ? "failed" : "ok");
    
    printf("\n");
	printf("Tests %s\n", result == 0 ? "Passed" : "Failed");
    return result;
}
