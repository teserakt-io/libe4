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

#ifndef XED25519_H
#define XED25519_H

#ifdef __cplusplus
extern "C" {
#endif


#define XED25519_PRIVKEY_DIGEST_BYTES 32
#define XED25519_RESULT_OK 0
#define XED25519_ERROR_DECODE_FAILURE 1
void xed25519_convert_ed2c_private(uint8_t* curve25519_privkey, 
        const uint8_t* ed25519_privkey);

int xed25519_convert_ed2c_public(uint8_t* curve25519_pubkey, 
        const uint8_t* ed25519_pubkey);

#ifdef __cplusplus
}
#endif

#endif
