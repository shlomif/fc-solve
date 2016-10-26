use strict;
use warnings;

use Test::More tests => 1;

my $CMD = q#NODE_PATH="`pwd`"/lib/for-node/js qunit-cli lib/for-node/test-code.js#;

my $output = `$CMD`;

# TEST
if (!like($output, qr/([0-9]+) tests of \1 passed\./, "All QUnit tests passed."))
{
    diag("OUTPUT == <<$output>>");
}

