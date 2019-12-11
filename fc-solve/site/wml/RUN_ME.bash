#!/bin/bash
set -e -x
gmake
NODE_PATH="`pwd`"/lib/for-node/js ./qunit.js lib/for-node/test-code-emcc.js
