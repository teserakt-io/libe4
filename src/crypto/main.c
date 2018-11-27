#include <stdio.h>

int test_sha3();                            // test_sha3.c
int test_shake();

int test_aes256();                          // in test_aes_siv.c
int test_aes_siv();

int main(int argc, char **argv)
{
    printf("test_aes256() = %d\n", test_aes256());
    printf("test_aes_siv() = %d\n", test_aes_siv());

    printf("test_sha3() = %d\n", test_sha3());
    printf("test_shake() = %d\n", test_shake());

    return 0;
}
