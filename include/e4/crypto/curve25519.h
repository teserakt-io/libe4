#ifndef E4_CURVE25519_H
#define E4_CURVE25519_H
#ifdef __cplusplus
extern "C" {
#endif

void curve25519(unsigned char *output, const unsigned char *a,
                             const unsigned char *b);

#ifdef __cplusplus
}
#endif
#endif
