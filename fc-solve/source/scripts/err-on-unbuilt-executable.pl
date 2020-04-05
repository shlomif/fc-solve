#!/usr/bin/env perl

use strict;
use warnings;

print {*STDERR} <<"EOF";
The executable '$0' was not built by cmake.

You need to run cmake with different arguments or dependencies.
EOF
