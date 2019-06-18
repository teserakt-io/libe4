//	aes256enc.S
//	2018-07-01	Markku-Juhani O. Saarinen <markku@teserakt.io>
//	(c) 2018 Copyright Teserakt AG

//	(Adopted from 2016-09-04 original by same author.)

__SP_H__ = 0x3e
__SP_L__ = 0x3d
__SREG__ = 0x3f
__tmp_reg__ = 0
__zero_reg__ = 1
		.text

.global aes256_enc_exp_key
		.type	aes256_enc_exp_key, @function

// constant time shift macro

.macro	xmul1b	rr=r22, c1b=r19
		clr		__tmp_reg__
		lsl		\rr
		sbc		__tmp_reg__, __zero_reg__
		and		__tmp_reg__, \c1b
		eor		\rr, __tmp_reg__
.endm

// expand key

aes256_enc_exp_key:

		push	r17

		push	r28
		push	r29

		ldi		r27,	hi8(sbox)	//	x		sbox
		movw	r28,	r22			//	y		key
		movw	r30,	r24			//	z		ek

		ldi		r18,	32			// copy first two subkeys
.ek0:	ld		r22,	y+
		st		z+,		r22
		dec		r18
		brne	.ek0

		ldi		r17,	0x01		//	rc
		ldi		r19,	0x1B		//	primitive root for GF(256)

		movw	r28,	r24			//	y = beginning of ek


.ekl:	ld		r20,	y+			//	expandkey()
		ldd		r26,	y + 28
		ld		r22,	x
		eor		r20,	r22
		eor		r20,	r17			// rc
		st		z+,		r20

		ld		r21,	y+
		ldd		r26,	y + 28
		ld		r22,	x
		eor		r21,	r22
		st		z+,		r21

		ld		r24,	y+
		ldd		r26,	y + 28
		ld		r22,	x
		eor		r24,	r22
		st		z+,		r24

		ld		r23,	y+
		ldd		r26,	y + 24
		ld		r22,	x
		eor		r23,	r22
		st		z+,		r23

		ldi		r18,	3

.ek1:	ld		r22,	y+
		eor		r20,	r22
		st		z+,		r20

		ld		r22,	y+
		eor		r21,	r22
		st		z+,		r21

		ld		r22,	y+
		eor		r24,	r22
		st		z+,		r24

		ld		r22,	y+
		eor		r23,	r22
		st		z+,		r23

		dec		r18
		brne	.ek1

		cpi		r17,	0x40		// done ?
		breq	.ekx

		mov		r26,	r20			// upper half
		ld		r20,	x
		ld		r22,	y+
		eor		r20,	r22
		st		z+,		r20

		mov		r26,	r21
		ld		r21,	x
		ld		r22,	y+
		eor		r21,	r22
		st		z+,		r21

		mov		r26,	r24
		ld		r24,	x
		ld		r22,	y+
		eor		r24,	r22
		st		z+,		r24

		mov		r26,	r23
		ld		r23,	x
		ld		r22,	y+
		eor		r23,	r22
		st		z+,		r23

		ldi		r18,	3

.ek2:	ld		r22,	y+
		eor		r20,	r22
		st		z+,		r20

		ld		r22,	y+
		eor		r21,	r22
		st		z+,		r21

		ld		r22,	y+
		eor		r24,	r22
		st		z+,		r24

		ld		r22,	y+
		eor		r23,	r22
		st		z+,		r23

		dec		r18
		brne	.ek2

		lsl		r17					// advance round constant
		jmp		.ekl


.ekx:	pop		r29					// done; exit
		pop		r28

		pop		r17
		ret

.global aes256_encrypt_ecb
		.type	aes256_encrypt_ecb, @function

// encrypt a block

aes256_encrypt_ecb:

		push	r17

		push	r28
		push	r29

		ldi		r27,	hi8(sbox)	//	x		sbox
		movw	r28,	r22			//	y		key
		movw	r30,	r24			//	z		buf

		ldi		r19,	0x1B		//	primitive root for GF(256)

		ldi		r17,	13			// round counter

.encl:	rcall	.ksrws				//	round function
		rcall	.mxcol
		adiw	r28,	16

		dec		r17
		brne	.encl

		rcall	.ksrws				// no mixColumns()
		adiw	r28,	16

		ldi		r18,	16			// final addRoundKey()
.fak:	ld		r22,	y+
		ld		r25,	z
		eor		r22,	r25
		st		z+,		r22
		dec		r18
		brne	.fak

// Done

		pop		r29
		pop		r28

		pop		r17
		ret


// mixColumns()
// z = buf

.mxcol: ldi		r18,	4

.mxclp: ld		r20,	z
		ldd		r21,	z + 1
		ldd		r24,	z + 2
		ldd		r23,	z + 3

		mov		r25,	r20
		eor		r25,	r21
		mov		r22,	r25
		eor		r25,	r24
		eor		r25,	r23
		xmul1b	r22
		eor		r22,	r20
		eor		r22,	r25
		st		z+,		r22

		mov		r22,	r21
		eor		r22,	r24
		xmul1b	r22
		eor		r22,	r21
		eor		r22,	r25
		st		z+,		r22

		mov		r22,	r24
		eor		r22,	r23
		xmul1b	r22
		eor		r22,	r24
		eor		r22,	r25
		st		z+,		r22

		mov		r22,	r23
		eor		r22,	r20
		xmul1b	r22
		eor		r22,	r23
		eor		r22,	r25
		st		z+,		r22

		dec		r18
		brne	.mxclp

		sbiw	r30,	16

		ret

// addRoundKeys(), subBytes(), and shiftRows() in one nice swoop
// Assumes x = sbox, y = key, z = buf

.ksrws: ld		r22,	z
		ld		r26,	y
		eor		r26,	r22
		ld		r22,	x
		st		z,		r22

		ldd		r22,	z + 1
		ldd		r26,	y + 1
		eor		r26,	r22
		ld		r25,	x

		ldd		r22,	z + 4
		ldd		r26,	y + 4
		eor		r26,	r22
		ld		r22,	x
		std		z + 4,	r22

		ldd		r22,	z + 5
		ldd		r26,	y + 5
		eor		r26,	r22
		ld		r22,	x
		std		z + 1,	r22

		ldd		r22,	z + 8
		ldd		r26,	y + 8
		eor		r26,	r22
		ld		r22,	x
		std		z + 8,	r22

		ldd		r22,	z + 9
		ldd		r26,	y + 9
		eor		r26,	r22
		ld		r22,	x
		std		z + 5,	r22

		ldd		r22,	z + 12
		ldd		r26,	y + 12
		eor		r26,	r22
		ld		r22,	x
		std		z + 12, r22

		ldd		r22,	z + 13
		ldd		r26,	y + 13
		eor		r26,	r22
		ld		r22,	x
		std		z + 9,	r22

		std		z + 13, r25

		ldd		r22,	z + 10
		ldd		r26,	y + 10
		eor		r26,	r22
		ld		r25,	x

		ldd		r22,	z + 2
		ldd		r26,	y + 2
		eor		r26,	r22
		ld		r22,	x
		std		z + 10, r22

		std		z + 2,	r25

		ldd		r22,	z + 3
		ldd		r26,	y + 3
		eor		r26,	r22
		ld		r25,	x

		ldd		r22,	z + 15
		ldd		r26,	y + 15
		eor		r26,	r22
		ld		r22,	x
		std		z + 3,	r22

		ldd		r22,	z + 11
		ldd		r26,	y + 11
		eor		r26,	r22
		ld		r22,	x
		std		z + 15, r22

		ldd		r22,	z + 7
		ldd		r26,	y + 7
		eor		r26,	r22
		ld		r22,	x
		std		z + 11, r22

		std		z + 7,	r25

		ldd		r22,	z + 14
		ldd		r26,	y + 14
		eor		r26,	r22
		ld		r25,	x

		ldd		r22,	z + 6
		ldd		r26,	y + 6
		eor		r26,	r22
		ld		r22,	x
		std		z + 14, r22

		std		z + 6,	r25
		ret

		.size		aes256_encrypt_ecb, .-aes256_encrypt_ecb


		// mucho importante! sbox must be aligned at 256 bytes
		.section	.rodata
		.type		sbox, @object
		.balign		256

sbox:	.byte	0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5
		.byte	0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76
		.byte	0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0
		.byte	0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0
		.byte	0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC
		.byte	0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15
		.byte	0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A
		.byte	0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75
		.byte	0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0
		.byte	0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84
		.byte	0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B
		.byte	0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF
		.byte	0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85
		.byte	0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8
		.byte	0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5
		.byte	0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2
		.byte	0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17
		.byte	0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73
		.byte	0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88
		.byte	0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB
		.byte	0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C
		.byte	0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79
		.byte	0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9
		.byte	0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08
		.byte	0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6
		.byte	0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A
		.byte	0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E
		.byte	0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E
		.byte	0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94
		.byte	0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF
		.byte	0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68
		.byte	0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16

.global __do_copy_data
