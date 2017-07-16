use strict;
use warnings;

use Test::More tests => 1;

my $CMD = q#NODE_PATH="`pwd`"/lib/for-node/js qunit-cli lib/for-node/test-code.js#;

my $output = `$CMD`;

my $count_fails = 0;
my $destroyed_output = $output;
$destroyed_output=~ s/[^0-9]([0-9]+) tests of ([0-9]+) passed\./if ($1 ne $2) { ++$count_fails; }/eg;
# TEST
if (!is($count_fails, 0, "All QUnit tests passed."))
{
    diag("OUTPUT == <<$output>>");
}

