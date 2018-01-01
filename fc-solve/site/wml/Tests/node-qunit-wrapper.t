use strict;
use warnings;

use Test::More tests => 2;

my $CMD =
    q#NODE_PATH="`pwd`"/lib/for-node/js qunit-cli lib/for-node/test-code.js#;

my $output = `$CMD`;

my $count_fails = 0;
my $count_pass  = 0;
pos $output = 0;
while ( $output =~ /\G.*?[^0-9]([0-9]+) tests of ([0-9]+) passed\./gms )
{
    if ( $1 ne $2 )
    {
        ++$count_fails;
    }
    else
    {
        ++$count_pass;
    }
}

# TEST
if ( !is( $count_fails, 0, "All QUnit tests passed." ) )
{
    diag("OUTPUT == <<$output>>");
}

# TEST
ok( $count_pass, 'Tests were run' );
