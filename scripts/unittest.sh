#!/bin/sh

make test

./build/test/crypto; echo "== RETURNCODE=$?"
./build/test/util; echo "== RETURNCODE=$?"
./build/test/e4file; echo "== RETURNCODE=$?"

