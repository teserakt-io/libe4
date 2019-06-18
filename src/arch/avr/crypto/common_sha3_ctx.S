//  common_sha3_ctx.S
//  2018-06-29  Markku-Juhani O. Saarinen <markku@teserakt.io>

//  (c) 2018 Copyright Teserakt AG

//  Just an aligned buffer. avr-gcc kept crashing on a C struct aligned at 256.
//  (Yep, the compiler always just segfaulted! Hope that this is temporary..)

    .comm   common_sha3_ctx, 256, 256
.global __do_clear_bss

