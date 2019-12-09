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

#include <stdio.h>

int test_sha3(); /* test_sha3.c */
int test_shake();

int test_aes256(); /* in test_aes_siv.c */
int test_aes_siv();

int main(int argc, char **argv)
{
    printf("test_aes256() = %d\n", test_aes256());
    printf("test_aes_siv() = %d\n", test_aes_siv());

    printf("test_sha3() = %d\n", test_sha3());
    printf("test_shake() = %d\n", test_shake());

    return 0;
}
