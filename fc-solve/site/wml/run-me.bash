#!/usr/bin/env bash
export NODE_PATH="`pwd`/lib/for-node/js"
# node bin/node_inspect_debugger_exec.js
expect -f bin/expect--node-inspect--auto.expect
