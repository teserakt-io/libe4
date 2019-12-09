
#ifndef E4_SELFTEST_H
#define E4_SELFTEST_H
#ifdef __cplusplus
extern "C" {
#endif

int test_aes256();
int test_aes_siv();
int test_sha3();
int test_shake();

#ifdef __cplusplus
}
#endif
#endif
