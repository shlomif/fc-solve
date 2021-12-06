#!/usr/bin/env perl

use strict;
use warnings;
use Test::More tests => 2;

use Freecell::Deal::MS ();

{

    my $obj = Freecell::Deal::MS->new( deal => 240 );

    # TEST
    ok( $obj, "object" );

    # TEST
    is( $obj->as_str(), <<'EOF', 'as_str 240' );
: JH 9C 5S KC 6S 2H AS
: 5D 3D 9S 2S 3C AD 8C
: 8S 5C KD QC 3H 4D 3S
: 7S AC 9H 6C QH KS 4H
: KH JD 7D 4C 8H 6H
: TS TC 4S 5H QD JS
: 9D JC 2C QS TH 2D
: AH 7C 6D 8D TD 7H
EOF
}
