#!/bin/bash
set -e -x
gmake
NODE_PATH="`pwd`"/lib/for-node/js strace -e openat -f -o q.strace ./qunit.js lib/for-node/test-code-emcc.js
