#!/bin/sh

CC=clang LD=clang E4_OUTPUT_DIR=qa/aubsan/symkey E4_CFLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" E4_LDFLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" CSTD=c89 CONF=symkey make test
CC=clang LD=clang E4_OUTPUT_DIR=qa/memsan/symkey E4_CFLAGS="-fsanitize=memory -fno-omit-frame-pointer" E4_LDFLAGS="-fsanitize=memory -fno-omit-frame-pointer" CSTD=c89 CONF=pubkey make test
CC=clang LD=clang E4_OUTPUT_DIR=qa/aubsan/pubkey E4_CFLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" E4_LDFLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" CSTD=c89 CONF=pubkey make test
CC=clang LD=clang E4_OUTPUT_DIR=qa/memsan/pubkey E4_CFLAGS="-fsanitize=memory -fno-omit-frame-pointer" E4_LDFLAGS="-fsanitize=memory -fno-omit-frame-pointer" CSTD=c89 CONF=pubkey make test

export E4_CFLAGS="-O3 -D_FORTIFY_SOURCE=2 -fstack-protector -flto -fsanitize=safe-stack,address,undefined -fno-omit-frame-pointer -Wformat -Wformat-security -Werror=format-security" 
export E4_LDFLAGS="-O3 -D_FORTIFY_SOURCE=2 -fstack-protector -flto -fsanitize=safe-stack,address,undefined -fuse-ld=gold -Wl,-z,relro"
CC=clang LD=clang E4_OUTPUT_DIR=qa/aubsan/all CSTD=c11 CONF=pubkey make test

export E4_CFLAGS="-O3 -D_FORTIFY_SOURCE=2 -fstack-protector -flto -fsanitize=memory -fno-omit-frame-pointer -Wformat -Wformat-security -Werror=format-security" 
export E4_LDFLAGS="-O3 -D_FORTIFY_SOURCE=2 -fstack-protector -flto -fsanitize=memory -Wl,-z,relro"
CC=clang LD=clang E4_OUTPUT_DIR=qa/memsan/all CSTD=c11 CONF=pubkey make test

export E4_CFLAGS=
export E4_LDFLAGS=

CC=clang LD=clang E4_OUTPUT_DIR=qa/tidy/symkey CSTD=c89 CONF=symkey make tidy
CC=clang LD=clang E4_OUTPUT_DIR=qa/tidy/pubkey CSTD=c89 CONF=pubkey make tidy
CC=clang LD=clang E4_OUTPUT_DIR=qa/tidy/all CSTD=c11 CONF=all make tidy
