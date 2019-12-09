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
/*  OpenSSL wrapper */

#include "e4/crypto/aes256enc.h"
#include <openssl/aes.h>

void aes256_enc_exp_key(void *eky, const void *key)
{
    AES_set_encrypt_key(key, 256, eky);
}

void aes256_encrypt_ecb(void *v, const void *eky) { AES_encrypt(v, v, eky); }
