//  f1600_avr.S
//  2018-06-28  Markku-Juhani O. Saarinen <markku@teserakt.io>

//  (c) 2018 Copyright Teserakt AG


__zero_reg__ = 1

        .text

// The C call convention with AVR is that:
// R2  - R17, R28, R29 are call-saved
// R18 - R27, R30, R31 are call-globbered


//  Prototype:
//  void keccak_f1600(uint8_t st[240], uint8_t r)

.global keccak_f1600
        .type   keccak_f1600, @function


keccak_f1600:

        push    r6
        push    r7
        push    r8
        push    r9
        push    r10
        push    r11
        push    r12
        push    r13
        push    r14
        push    r15
        push    r16
        push    r17

        ldi     xl,     lo8(rconst)     // round constant
        ldi     xh,     hi8(rconst)

.iter:

        movw    z,      r24             // input pointer r25:r24

        // ---- Theta transform ----

        clr     zl                      // xor everything at (z+200..z+239)
.xorz1:
        ld      r6,     z
        ldd     r0,     z + 40
        eor     r6,     r0
        ldd     r7,     z + 1
        ldd     r0,     z + 41
        eor     r7,     r0
        ldd     r8,     z + 2
        ldd     r0,     z + 42
        eor     r8,     r0
        ldd     r9,     z + 3
        ldd     r0,     z + 43
        eor     r9,     r0
        subi    zl,     -80
        ld      r0,     z
        eor     r6,     r0
        ldd     r0,     z + 40
        eor     r6,     r0
        ldd     r0,     z + 1
        eor     r7,     r0
        ldd     r0,     z + 41
        eor     r7,     r0
        ldd     r0,     z + 2
        eor     r8,     r0
        ldd     r0,     z + 42
        eor     r8,     r0
        ldd     r0,     z + 3
        eor     r9,     r0
        ldd     r0,     z + 43
        eor     r9,     r0
        subi    zl,     -80
        ld      r0,     z
        eor     r6,     r0
        std     z + 40, r6
        ldd     r0,     z + 1
        eor     r7,     r0
        std     z + 41, r7
        ldd     r0,     z + 2
        eor     r8,     r0
        std     z + 42, r8
        ldd     r0,     z + 3
        eor     r9,     r0
        std     z + 43, r9

        subi    zl,     156
        cpi     zl,     40
        brne    .xorz1

        ldi     zl,     200 + 4 * 8
        rcall   .zget64a
        ldi     zl,     200 + 1 * 8
        rcall   .zget64b
        rcall   .rotbxa
        ldi     zl,     0 * 8
        rcall   .xorz2

        rcall   .movab
        ldi     zl,     200 + 2 * 8
        rcall   .zget64a
        rcall   .rotbxa
        ldi     zl,     3 * 8
        rcall   .xorz2

        rcall   .movab
        ldi     zl,     200 + 0 * 8
        rcall   .zget64a
        rcall   .rotbxa
        ldi     zl,     1 * 8
        rcall   .xorz2

        rcall   .movab
        ldi     zl,     200 + 3 * 8
        rcall   .zget64a
        rcall   .rotbxa
        ldi     zl,     4 * 8
        rcall   .xorz2

        rcall   .movab
        ldi     zl,     200 + 1 * 8
        rcall   .zget64a
        rcall   .rotbxa
        ldi     zl,     2 * 8
        rcall   .xorz2


        // ---- Rho and Pi transforms ----
    
        ldi     zl,     1 * 8
        rcall   .zget64b

        rcall   .rotl1                  // <<< 1
        ldi     zl,     10 * 8
        rcall   .zswp64b
        rcall   .rotl3                  // <<< 3
        ldi     zl,     7 * 8
        rcall   .zswp64b
        rcall   .rotr2                  // <<< 6
        rcall   .rotl8
        ldi     zl,     11 * 8
        rcall   .zswp64
        rcall   .rotl2                  // <<< 10
        rcall   .rotl8
        ldi     zl,     17 * 8
        rcall   .zswp64
        rcall   .rotr1                  // <<< 15
        rcall   .rotl16
        ldi     zl,     18 * 8
        rcall   .zswp64
        rcall   .rotr3                  // <<< 21
        rcall   .rotl24
        ldi     zl,     3 * 8
        rcall   .zswp64
        rcall   .rotl2                  // <<< 28
        rcall   .rotl2
        rcall   .rotl24
        ldi     zl,     5 * 8
        rcall   .zswp64
        rcall   .rotl2                  // <<< 36
        rcall   .rotl2
        rcall   .rotl32
        ldi     zl,     16 * 8
        rcall   .zswp64
        rcall   .rotr3                  // <<< 45
        rcall   .rotr16
        ldi     zl,     8 * 8
        rcall   .zswp64
        rcall   .rotr1                  // <<< 55
        rcall   .rotr8
        ldi     zl,     21 * 8
        rcall   .zswp64
        rcall   .rotl2                  // <<< 2
        ldi     zl,     24 * 8
        rcall   .zswp64b
        rcall   .rotr2                  // <<< 14
        rcall   .rotl16
        ldi     zl,     4 * 8
        rcall   .zswp64
        rcall   .rotl3                  // <<< 27
        rcall   .rotl24
        ldi     zl,     15 * 8
        rcall   .zswp64
        rcall   .rotl1                  // <<< 41
        rcall   .rotr24
        ldi     zl,     23 * 8
        rcall   .zswp64
        rcall   .rotr8                  // <<< 56
        ldi     zl,     19 * 8
        rcall   .zswp64
        rcall   .rotl8                  // <<< 8
        ldi     zl,     13 * 8
        rcall   .zswp64     
        rcall   .rotl1                  // <<< 25
        rcall   .rotl24
        ldi     zl,     12 * 8
        rcall   .zswp64
        rcall   .rotl3                  // <<< 43
        rcall   .rotr24
        ldi     zl,     2 * 8
        rcall   .zswp64
        rcall   .rotr2                  // <<< 62
        ldi     zl,     20 * 8
        rcall   .zswp64b
        rcall   .rotl2                  // <<< 18
        rcall   .rotl16
        ldi     zl,     14 * 8
        rcall   .zswp64
        rcall   .rotr1                  // <<< 39
        rcall   .rotr24
        ldi     zl,     22 * 8
        rcall   .zswp64
        rcall   .rotr3                  // <<< 61
        ldi     zl,     9 * 8
        rcall   .zswp64b
        rcall   .rotl2                  // <<< 20
        rcall   .rotl2
        rcall   .rotl16
        ldi     zl,     6 * 8
        rcall   .zswp64
        rcall   .rotl2                  // <<< 44
        rcall   .rotl2
        rcall   .rotr24
        ldi     zl,     1 * 8
        rcall   .zput64a

        // ---- Chi transform ----

        clr     zl
.nonl2:
        ldi     r21,    8
.nonl1:
        ld      r11,    z
        ldd     r12,    z + 8
        ldd     r13,    z + 16
        ldd     r14,    z + 24
        ldd     r15,    z + 32

        ldi     r16,    0xFF
        eor     r16,    r12
        and     r16,    r13
        eor     r16,    r11

        ldi     r17,    0xFF
        eor     r17,    r13
        and     r17,    r14
        eor     r17,    r12

        ldi     r18,    0xFF
        eor     r18,    r14
        and     r18,    r15
        eor     r18,    r13

        ldi     r19,    0xFF
        eor     r19,    r15
        and     r19,    r11
        eor     r19,    r14

        ldi     r20,    0xFF
        eor     r20,    r11
        and     r20,    r12
        eor     r20,    r15

        std     z + 8,  r17
        std     z + 16, r18
        std     z + 24, r19
        std     z + 32, r20
        st      z+,     r16

        dec     r21
        brne    .nonl1

        subi    zl,     -32
        cpi     zl,     200
        brlo    .nonl2

        // ---- Iota ----

        movw    z,      r24

        ld      r21,    x+              // "compressed" round constant
        ldi     r20,    0x8F
        and     r20,    r21
        ld      r19,    z
        eor     r19,    r20
        st      z,      r19

        ldi     r20,    0x10
        and     r20,    r21
        breq    .bit4z
        ldd     r19,    z + 1
        subi    r19,    0x80
        std     z + 1,  r19

.bit4z: ldi     r20,    0x20
        and     r20,    r21
        breq    .bit5z
        ldd     r19,    z + 3
        subi    r19,    0x80
        std     z + 3,  r19

.bit5z: ldi     r20,    0x40
        and     r20,    r21
        breq    .bit6z
        ldd     r19,    z + 7
        subi    r19,    0x80
        std     z + 7,  r19
.bit6z:

        dec     r22                     //  r22 = "r", round count
        breq    .done                   //  iterate
        jmp     .iter
.done:
        pop     r17
        pop     r16
        pop     r15
        pop     r14
        pop     r13
        pop     r12
        pop     r11
        pop     r10
        pop     r9
        pop     r8
        pop     r7
        pop     r6

        ret

// Get a 64-bit word from (z..z+7) to (r6..r13)

.zget64a:
        ld      r6,     z+
        ld      r7,     z+
        ld      r8,     z+
        ld      r9,     z+
        ld      r10,    z+
        ld      r11,    z+
        ld      r12,    z+
        ld      r13,    z
        ret

// Get a 64-bit word from (z..z+7) to (r14..r21)

.zget64b:
        ld      r14,    z+
        ld      r15,    z+
        ld      r16,    z+
        ld      r17,    z+
        ld      r18,    z+
        ld      r19,    z+
        ld      r20,    z+
        ld      r21,    z
        ret

// Move (r6..r13) to (r14..r21)

.movab:
        mov     r14,    r6
        mov     r15,    r7
        mov     r16,    r8
        mov     r17,    r9
        mov     r18,    r10
        mov     r19,    r11
        mov     r20,    r12
        mov     r21,    r13
        ret

// Put a 64-bit word (r6..r13) to (z..z+7)

.zput64a:
        st      z+,     r6
        st      z+,     r7
        st      z+,     r8
        st      z+,     r9
        st      z+,     r10
        st      z+,     r11
        st      z+,     r12
        st      z,      r13
        ret

// get (z..z+7) to (r14..r21), replace with (r6..r14)

.zswp64:
        ld      r14,    z
        st      z+,     r6
        ld      r15,    z
        st      z+,     r7
        ld      r16,    z
        st      z+,     r8
        ld      r17,    z
        st      z+,     r9
        ld      r18,    z
        st      z+,     r10
        ld      r19,    z
        st      z+,     r11
        ld      r20,    z
        st      z+,     r12
        ld      r21,    z
        st      z,      r13
        ret

// swap (z..z+7) with (r14..r21) without messing up (r6..r13)

.zswp64b:
        mov     r0,     r14
        ld      r14,    z
        st      z+,     r0
        mov     r0,     r15
        ld      r15,    z
        st      z+,     r0
        mov     r0,     r16
        ld      r16,    z
        st      z+,     r0
        mov     r0,     r17
        ld      r17,    z
        st      z+,     r0
        mov     r0,     r18
        ld      r18,    z
        st      z+,     r0
        mov     r0,     r19
        ld      r19,    z
        st      z+,     r0
        mov     r0,     r20
        ld      r20,    z
        st      z+,     r0
        mov     r0,     r21
        ld      r21,    z
        st      z+,     r0
        ret

.rotl8:
        mov     r6,     r21
        mov     r7,     r14
        mov     r8,     r15
        mov     r9,     r16
        mov     r10,    r17
        mov     r11,    r18
        mov     r12,    r19
        mov     r13,    r20
        ret

.rotl16:
        mov     r6,     r20
        mov     r7,     r21
        mov     r8,     r14
        mov     r9,     r15
        mov     r10,    r16
        mov     r11,    r17
        mov     r12,    r18
        mov     r13,    r19
        ret

.rotl24:
        mov     r6,     r19
        mov     r7,     r20
        mov     r8,     r21
        mov     r9,     r14
        mov     r10,    r15
        mov     r11,    r16
        mov     r12,    r17
        mov     r13,    r18
        ret

.rotl32:
        mov     r6,     r18
        mov     r7,     r19
        mov     r8,     r20
        mov     r9,     r21
        mov     r10,    r14
        mov     r11,    r15
        mov     r12,    r16
        mov     r13,    r17
        ret

.rotr8:
        mov     r6,     r15
        mov     r7,     r16
        mov     r8,     r17
        mov     r9,     r18
        mov     r10,    r19
        mov     r11,    r20
        mov     r12,    r21
        mov     r13,    r14
        ret

.rotr16:
        mov     r6,     r16
        mov     r7,     r17
        mov     r8,     r18
        mov     r9,     r19
        mov     r10,    r20
        mov     r11,    r21
        mov     r12,    r14
        mov     r13,    r15
        ret

.rotr24:
        mov     r6,     r17
        mov     r7,     r18
        mov     r8,     r19
        mov     r9,     r20
        mov     r10,    r21
        mov     r11,    r14
        mov     r12,    r15
        mov     r13,    r16
        ret

// individual bit-shifts

.rotl3:                                 // rotate (r14--r21) left by 3 bits
        lsl     r14
        rol     r15
        rol     r16
        rol     r17
        rol     r18
        rol     r19
        rol     r20
        rol     r21
        adc     r14,     __zero_reg__
.rotl2:
        lsl     r14
        rol     r15
        rol     r16
        rol     r17
        rol     r18
        rol     r19
        rol     r20
        rol     r21
        adc     r14,     __zero_reg__
.rotl1:
        lsl     r14
        rol     r15
        rol     r16
        rol     r17
        rol     r18
        rol     r19
        rol     r20
        rol     r21
        adc     r14,    __zero_reg__    // wrap-around bit
        ret

.rotr3:                                 // rotate (r14--r21) right by 3 bits
        lsr     r21
        ror     r20
        ror     r19
        ror     r18
        ror     r17
        ror     r16
        ror     r15
        ror     r14
        brcc    .rotr2                  // 2 cycles w/o carry, 1 cycle with
        ori     r21,    0x80            // 1 cycle -- constant time
.rotr2:
        lsr     r21
        ror     r20
        ror     r19
        ror     r18
        ror     r17
        ror     r16
        ror     r15
        ror     r14
        brcc    .rotr1                  // 2 cycles w/o carry, 1 cycle with
        ori     r21,    0x80            // 1 cycle -- constant time
.rotr1:
        lsr     r21
        ror     r20
        ror     r19
        ror     r18
        ror     r17
        ror     r16
        ror     r15
        ror     r14
        brcc    .rotr0                  // 2 cycles w/o carry, 1 cycle with
        ori     r21,    0x80            // 1 cycle -- constant time
.rotr0:
        ret


.xorz2: 
        ld      r0,     z
        eor     r0,     r14
        st      z+,     r0
        ld      r0,     z
        eor     r0,     r15
        st      z+,     r0
        ld      r0,     z
        eor     r0,     r16
        st      z+,     r0
        ld      r0,     z
        eor     r0,     r17
        st      z+,     r0
        ld      r0,     z
        eor     r0,     r18
        st      z+,     r0
        ld      r0,     z
        eor     r0,     r19
        st      z+,     r0
        ld      r0,     z
        eor     r0,     r20
        st      z+,     r0
        ld      r0,     z
        eor     r0,     r21
        st      z,      r0

        subi    zl,     -33
        cpi     zl,     200
        brlo    .xorz2
        ret

.rotbxa:                                // B = (B <<< 1) ^ A
        lsl     r14
        rol     r15
        rol     r16
        rol     r17
        rol     r18
        rol     r19
        rol     r20
        rol     r21
        adc     r14,    __zero_reg__    // wrap-around bit
        eor     r14,    r6
        eor     r15,    r7
        eor     r16,    r8
        eor     r17,    r9
        eor     r18,    r10
        eor     r19,    r11
        eor     r20,    r12
        eor     r21,    r13
        ret


        // round constants
        .section    .rodata
        .type       rconst, @object
        .size       rconst, 24
rconst: .byte       0x01, 0x92, 0xDA, 0x70, 0x9B, 0x21, 0xF1, 0x59
        .byte       0x8A, 0x88, 0x39, 0x2A, 0xBB, 0xCB, 0xD9, 0x53
        .byte       0x52, 0xC0, 0x1A, 0x6A, 0xF1, 0xD0, 0x21, 0x78

.global __do_copy_data
.global __do_clear_bss


