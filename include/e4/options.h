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

#ifndef E4_OPTIONS_H
#define E4_OPTIONS_H

/* Options bits for e4c crypto functions: */

/* Disable checks on message timestamp within acceptable window 
 * Packet timestamps are still covered by authenticated encryption, 
 * but older messages are allowed */
#define E4_OPTION_IGNORE_TIMESTAMP         0x01
/* Disable failure if we do not have a client public key to validate 
 * signatures. Corrupt signatures will ALWAYS fail, this is only 
 * for the case where too many clients exist for us to store all of 
 * their keys. */
#define E4_OPTION_IGNORE_MISSING_PUBKEY    0x02

#endif
