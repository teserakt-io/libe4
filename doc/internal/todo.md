
# E4 C Library TODO LIST

## Essential Backlog

These are items that should be done as fast as possible.

 1. Unit tests for file storage functionality.
 1. AVR integration (minimise duplication of functionality/code):
     1. EEPROM storage target instead of file.
     1. Integration of AVR crypto (Sha3, SIV).
 1. Make system to cope with this (see BearSSL).
 1. Remove memory leak due to strdup usage / do not depend on malloc anywhere.
 1. C89 everywhere (remove stdint or typedef stdint in this project).

## Wishlist backlog

These items are wishlist/future todos, roughly in order:

 1. Deploy test on ARM boards to understand abstractions needed for hardware.
    Works cross purpose with EPFL HALUCINATOR project (we have a firmware 
    image to analyze for these boards).
 1. ARM-optimized crypto.
 1. Constant-time crypto across the board (AES not bitsliced) (see / use BearSSL).
 1. Pubkey crypto
 1. Fuzzing using AFL etc.

