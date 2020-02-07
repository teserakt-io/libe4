#!/bin/sh

export CC=clang 
export LD=clang 
export CSTD=c11 
export CONF=all 
export STORE=none 
export E4_CFLAGS="-O3 -D_FORTIFY_SOURCE=2 -fstack-protector -flto -fsanitize=safe-stack -Wformat -Wformat-security -Werror=format-security" 
export E4_LDFLAGS="-O3 -D_FORTIFY_SOURCE=2 -fstack-protector -flto -fsanitize=safe-stack -fuse-ld=gold -Wl,-z,relro"
make shared

checksec --format=cli --file=build/all/lib/libe4.so.1.0.0
