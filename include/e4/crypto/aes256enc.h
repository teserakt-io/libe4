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

#ifndef _AES256ENC_H_
#define _AES256ENC_H_
#ifdef __cplusplus
extern "C" {
#endif

/* expand 256-bit key. ek[] needs 240 bytes of storage */
#define AES256_EXPKEY_LEN 240

void aes256_enc_exp_key(void *ek, const void *key);

/* encrypt a block */
void aes256_encrypt_ecb(void *v, const void *ek);

#ifdef __cplusplus
}
#endif
#endif
