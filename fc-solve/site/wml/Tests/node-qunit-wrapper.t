use strict;
use warnings;

use Test::More tests => 1;

my $CMD = q#NODE_PATH="`pwd`"/lib/for-node/js qunit-cli lib/for-node/test-code.js#;

my $output = `$CMD`;

my $count_fails = 0;
pos$output = 0;
while ($output =~ /\G.*?[^0-9]([0-9]+) tests of ([0-9]+) passed\./g)
{
    if ($1 ne $2)
    {
        ++$count_fails;
    }
}

# TEST
if (!is($count_fails, 0, "All QUnit tests passed."))
{
    diag("OUTPUT == <<$output>>");
}

