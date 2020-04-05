use strict;
use warnings;

use Test::More tests => 4;

# TEST:$run=0;
sub _run
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;
    my $test_fn = shift;
    my $CMD     = q#NODE_PATH="`pwd`"/lib/for-node/js qunit-cli # . $test_fn;

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

    # TEST:$run++;
    if ( !is( $count_fails, 0, "All QUnit tests passed." ) )
    {
        diag("OUTPUT == <<$output>>");
    }

    # TEST:$run++;
    ok( $count_pass, 'Tests were run' );
}

# TEST*$run*2
_run("lib/for-node/test-code-emcc.js");
_run("lib/for-node/test-code.js");
