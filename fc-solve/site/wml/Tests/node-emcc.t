#!/usr/bin/env perl

use strict;
use warnings;
system(
    q#NODE_PATH="`pwd`"/lib/for-node/js qunit lib/for-node/test-code-emcc.js #);
