#!/bin/bash
set -e -x
gmake
export NODE_PATH="`pwd`"/lib/for-node/js
strace -e openat -f -o q.strace node ./qunit.js lib/for-node/test-code-emcc.js
