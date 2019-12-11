set -e -x
gmake
export NODE_PATH="`pwd`"/lib/for-node/js NODE_NO_READLINE=1
node-inspect ./qunit.js lib/for-node/test-code-emcc.js < "./in.txt"
