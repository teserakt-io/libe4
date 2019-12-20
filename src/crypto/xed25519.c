/*
 * Teserakt AG LIBE4 C Library
 *
 * Copyright 2018-2020 Teserakt AG, Lausanne, Switzerland
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <string.h>
#include "e4/stdint.h"
#include "e4/crypto/ed25519.h"
#include "e4/crypto/sha512.h"
#include "e4/crypto/xed25519.h"

#include "ed25519/fe.h"
#include "ed25519/ge.h"

void xed25519_convert_ed2c_private(uint8_t* curve25519_privkey, 
        const uint8_t* ed25519_privkey) {

    uint8_t hashedkey[64] = {0};

    sha512(ed25519_privkey, XED25519_PRIVKEY_DIGEST_BYTES, hashedkey);

    /* clamping */
    hashedkey[0]  &= 248;
    hashedkey[31] &= 127;
    hashedkey[31] |= 64;
    
    /* truncate to only the lower 256-bits */
    memcpy(curve25519_privkey, hashedkey, 32);
    return;
}

int xed25519_convert_ed2c_public(uint8_t* curve25519_pubkey, 
        const uint8_t* ed25519_pubkey) {

    /*
        Edwards to Montgomery point conversion.
        Isomorphism is:
            u = (y+1)/(1-y)
        which becomes
            u = (Y+Z)/(Z-Y), but Z=1 so the result is really the same 
        as above even in extended coords
    */
    ge_p3 P;
    fe one;
    fe one_minus_y;
    fe one_over_one_minus_y;
    fe one_plus_y;
    fe u;

    if (ge_frombytes_negate_vartime(&P, ed25519_pubkey) != 0) {
        return XED25519_ERROR_DECODE_FAILURE;
    }

    fe_1(one);
    fe_sub(one_minus_y, one, P.Y);
    fe_invert(one_over_one_minus_y, one_minus_y);
    fe_add(one_plus_y, one, P.Y);
    
    fe_mul(u, one_plus_y, one_over_one_minus_y);

    fe_tobytes(curve25519_pubkey, u);
    return XED25519_RESULT_OK;
}

