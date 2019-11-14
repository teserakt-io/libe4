
/* TODO: remove this stdint.h */
#include <stdint.h>
#include "curve25519.h"

/* Field Element implementation for Curve25519
 *
 * This implementation uses the same field element representation as 
 * suggested in the paper and popularly used, namely that if x is an 
 * array of curve25519_words then:
 *
 * x[0] + 2^26 * x[1] + 2^51 * x[2] + ...
 *
 * the limbs are thus 26 or 25 bits wide.
 */

/* 255 / 25.5 = 10 limbs required. */
const size_t CURVE25519_FE_LIMBS = 10;

/* We define some types for various parts of our implementation. */ 
typedef uint32_t curve25519_word;
typedef uint64_t curve25519_dword;
#define CURVE25519_WORDBITS sizeof(curve25519_word)
typedef curve25519_word[CURVE25519_FE_LIMBS] curve25519_fe;
typedef curve25519_word[2*CURVE25519_FE_LIMBS] curve25519_double_fe;


static inline curve25519_dword le64_decode4() {

}

static inline curve25519_dword le64_decode4() {

}

/* Encoding/decoding functions */
void curve25519_fe_encode(uint8_t* binary, const size_t maxbytes, 
        cpnst curve25519_fe* c25519fe, const size_t fesize) {

}

size_t curve25519_fe_decode(curve25519_fe* c25519fe, const size_t fesize, 
        const uint8_t* binary, const size_t maxbytes) {


}


/* Constant-time field element move. */
void curve25519_fe_cmov(curve25519_fe out, const curve25519_fe src) {

    out[0] = src[0];
    out[1] = src[1];
    out[2] = src[2];
    out[3] = src[3];
    out[4] = src[4];
    out[5] = src[5];
    out[6] = src[6];
    out[7] = src[7];
    out[8] = src[8];
    out[9] = src[9];
}

/* Constant-time field element swap. Required for ladder. */
void curve25519_fe_cswap(const curve25519_word swap, 
        curve25519_fe f, curve25519_fe g) {

    curve25519_word ctl = -swap;

    curve25519_word tmp0 = ctl & (f[0] ^ g[0])
    curve25519_word tmp1 = ctl & (f[1] ^ g[1])
    curve25519_word tmp2 = ctl & (f[2] ^ g[2])
    curve25519_word tmp3 = ctl & (f[3] ^ g[3])
    curve25519_word tmp4 = ctl & (f[4] ^ g[4])
    curve25519_word tmp5 = ctl & (f[5] ^ g[5])
    curve25519_word tmp6 = ctl & (f[6] ^ g[6])
    curve25519_word tmp7 = ctl & (f[7] ^ g[7])
    curve25519_word tmp8 = ctl & (f[8] ^ g[8])
    curve25519_word tmp9 = ctl & (f[9] ^ g[9])
    f[0] = f[0] & ctl;
    g[0] = g[0] & ctl;
    f[1] = f[1] & ctl;
    g[1] = g[1] & ctl;
    f[2] = f[2] & ctl;
    g[2] = g[2] & ctl;
    f[3] = f[3] & ctl;
    g[3] = g[3] & ctl;
    f[4] = f[4] & ctl;
    g[4] = g[4] & ctl;
    f[5] = f[5] & ctl;
    g[5] = g[5] & ctl;
    f[6] = f[6] & ctl;
    g[6] = g[6] & ctl;
    f[7] = f[7] & ctl;
    g[7] = g[7] & ctl;
    f[8] = f[8] & ctl;
    g[8] = g[8] & ctl;
    f[9] = f[9] & ctl;
    g[9] = g[9] & ctl;
}

/* out = a + b */
void curve25519_fe_add(curve25519_fe out, 
                       const curve25519_fe a, const curve25519_fe b) {

    out[0] = a[0] + b[0];
    out[1] = a[1] + b[1];
    out[2] = a[2] + b[2];
    out[3] = a[3] + b[3];
    out[4] = a[4] + b[4];
    out[5] = a[5] + b[5];
    out[6] = a[6] + b[6];
    out[7] = a[7] + b[7];
    out[8] = a[8] + b[8];
    out[9] = a[9] + b[9];
}

/* out = a-b */
void curve25519_fe_sub(curve25519_fe out, const curve25519_fe a, const curve25519_fe b) {
    
    out[0] = a[0] - b[0];
    out[1] = a[1] - b[1];
    out[2] = a[2] - b[2];
    out[3] = a[3] - b[3];
    out[4] = a[4] - b[4];
    out[5] = a[5] - b[5];
    out[6] = a[6] - b[6];
    out[7] = a[7] - b[7];
    out[8] = a[8] - b[8];
    out[9] = a[9] - b[9];
}

void curve25519_fe_product_notfe(curve25519_double_fe* out, 
        curve25519_fe* a, curve25519_fe* b) 
{
    // TODO: review casts etc)
    out[0] = (curve25519_dword)(a[0]*b[0]);

    out[1] = (curve25519_dword)(a[0]*b[1] + a[1]*b[0]);

    out[2]
}



void curve25519_scalarmult(curve25519_fe)
