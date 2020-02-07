#!/bin/sh

CC=clang LD=clang E4_OUTPUT_DIR=qa/aubsan/symkey E4_CFLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" E4_LDFLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" CSTD=c89 CONF=symkey make test
CC=clang LD=clang E4_OUTPUT_DIR=qa/memsan/symkey E4_CFLAGS="-fsanitize=memory -fno-omit-frame-pointer" E4_LDFLAGS="-fsanitize=memory -fno-omit-frame-pointer" CSTD=c89 CONF=pubkey make test
CC=clang LD=clang E4_OUTPUT_DIR=qa/aubsan/pubkey E4_CFLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" E4_LDFLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" CSTD=c89 CONF=pubkey make test
CC=clang LD=clang E4_OUTPUT_DIR=qa/memsan/pubkey E4_CFLAGS="-fsanitize=memory -fno-omit-frame-pointer" E4_LDFLAGS="-fsanitize=memory -fno-omit-frame-pointer" CSTD=c89 CONF=pubkey make test

CC=clang LD=clang E4_OUTPUT_DIR=qa/tidy/symkey CSTD=c89 CONF=symkey make tidy
CC=clang LD=clang E4_OUTPUT_DIR=qa/tidy/pubkey CSTD=c89 CONF=pubkey make tidy
