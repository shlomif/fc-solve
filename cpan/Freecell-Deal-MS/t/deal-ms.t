#!/usr/bin/env perl

use strict;
use warnings;
use Test::More tests => 5;

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

{
    my $obj = Freecell::Deal::MS->new( deal => 6000000000 );

    # TEST
    ok( $obj, "object" );

    # TEST
    is( $obj->as_str(), <<'EOF', 'as_str 6e9' );
: 2D 2C QS 8D KD 8C 4C
: 3D AH 2H 4H TS 6H QD
: 4D JS AD 6S JH JC JD
: KH 3H KS AS TC 5D AC
: TD 7C 9C 7H 3C 3S
: QH 9H 9D 5S 7S 6C
: 5C 5H 2S KC 9S 4S
: 6D QC 8S TH 7D 8H
EOF

    my $BOARD = <<'EOF';
2D 2C QS 8D KD 8C 4C
3D AH 2H 4H TS 6H QD
4D JS AD 6S JH JC JD
KH 3H KS AS TC 5D AC
TD 7C 9C 7H 3C 3S
QH 9H 9D 5S 7S 6C
5C 5H 2S KC 9S 4S
6D QC 8S TH 7D 8H
EOF

    # TEST
    is_deeply(
        $obj->as_columns_array()->{'array_of_arrays_of_strings'},
        [ map { [ split / /ms, $_ ] } ( $BOARD =~ /([^\n\r]+)/gms ) ],
        'as_columns_array 6e9'
    );
}
