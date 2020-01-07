
# LICENSE

## This project

Code in this repository is licensed under the Apache 2.0 License. This may 
also be found in [LICENSE](LICENSE), except those components described below:

## Open source components and libraries

These components are under their own licenses, which are included in the 
file (where a single file is appropriate) or in the file headers (where 
a single file is used):

### curve25519-donna
   
This is curve25519-donna, available under the following terms: 
[src/crypto/curve25519/LICENSE.TXT](src/crypto/curve25519/LICENSE.TXT)  

### Orson Peter's Portable Ed25519

This is Orson Peter's Portable Ed25519 implementation, licensed under the terms 
in: [src/crypto/ed25519/LICENSE.TXT](src/crypto/ed25519/LICENSE.TXT)

### Tom St Denis' LibTomCrypto

Is used as part of the Ed25519 library to provide sha512. The files are 
`src/crypto/sha512.c` and corresponding include header, and are in the public 
domain.

### Portable stdint.h

[`include/e4/pstdint.h`](include/e4/pstdint.h) 
is Copyright (c) 2005-2016 Paul Hsieh under a two-clause BSD-like license, as 
show in the file.

### aes256enc_ref.c

The code in [`src/crypto/aes256enc_ref.c`] originates from code placed in the 
public domain as part of the optimised reference implementation of AES. The 
headers are left in tact.

