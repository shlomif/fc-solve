#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 38;
use Test::Differences qw/ eq_or_diff /;
use Path::Tiny qw/ path /;

use FC_Solve::Paths
    qw/ $FIND_DEAL_INDEX $GEN_MULTI $MAKE_PYSOL bin_board bin_exe_raw normalize_lf /;
use FC_Solve::Trim qw/trim_trail_ws/;

sub _test_out
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my ($args) = @_;

    my $cmd_line_args = $args->{cmd};

    my $got = `$MAKE_PYSOL @$cmd_line_args`;

    eq_or_diff( $got, $args->{expected}, $args->{blurb} );

    return;
}

sub _test_find_index
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my ($args) = @_;

    my $cmd_line_args = $args->{cmd};

    my $got = `$FIND_DEAL_INDEX @$cmd_line_args`;

    eq_or_diff( $got, $args->{expected}, $args->{blurb} );

    return;
}

my $dir = path("gen-multi-foo");

sub _reset_dir
{
    $dir->remove_tree;
    $dir->mkpath;
}

sub _test_gen_multi
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my ($args) = @_;

    my $cmd_line_args = $args->{cmd};

    _reset_dir;

    # print STDERR "<<$GEN_MULTI @$cmd_line_args>>\n";
    my $got = `$GEN_MULTI @$cmd_line_args`;

    eq_or_diff( $got, $args->{expected}, $args->{blurb} );

    return;
}

my $MAKE_MS_EXE =
    bin_exe_raw( [ 'board_gen', 'pi-make-microsoft-freecell-board' ] );

sub _test_ms
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my ($args) = @_;

    my $cmd_line_args = $args->{cmd};

    my $got = trim_trail_ws( scalar `$MAKE_MS_EXE @$cmd_line_args` );

    eq_or_diff( $got, $args->{expected}, $args->{blurb} );

    return;
}

# TEST
_test_out(
    {
        blurb    => "Testing for good Baker's Dozen",
        cmd      => [qw(-t 24 bakers_dozen)],
        expected => <<'EOF',
KD 8H AC AS
KC 3D 8C 2S
QD TD 7D 9S
2H JH 6S QH
KH 3S 9D 2C
JD 3H TH 7S
7C QS JC AH
8D 4D 6H 7H
6D AD 3C 2D
KS TS 9C 5D
4H TC 6C QC
4S 5C 5S 5H
8S 9H JS 4C
EOF
    }
);

# TEST
_test_out(
    {
        blurb    => "Testing for good Baker's Dozen",
        cmd      => [qw(-t 10800 bakers_dozen)],
        expected => <<'EOF',
KC 3S AC AD
JD 9H 6H JS
KS 8D 8S QH
3D 6C TS 4C
7H 4D 2C 7D
KH TD 4S 3C
2S 3H JC 5C
AS 5S 6D 8C
7C 4H TC 9S
8H 9D QC 9C
7S 5D 5H 2H
2D QD QS TH
KD 6S AH JH
EOF
    }
);

my $BOARD_24 = <<'EOF';
4C 2C 9C 8C QS 4S 2H
5H QH 3C AC 3H 4H QD
QC 9S 6H 9H 3S KS 3D
5D 2S JC 5C JH 6D AS
2D KD 10H 10C 10D 8D
7H JS KH 10S KC 7C
AH 5S 6S AD 8H JD
7S 6C 7D 4D 8S 9D
EOF

my $BOARD_24_T = <<'EOF';
4C 2C 9C 8C QS 4S 2H
5H QH 3C AC 3H 4H QD
QC 9S 6H 9H 3S KS 3D
5D 2S JC 5C JH 6D AS
2D KD TH TC TD 8D
7H JS KH TS KC 7C
AH 5S 6S AD 8H JD
7S 6C 7D 4D 8S 9D
EOF

# TEST
_test_out(
    {
        blurb    => "Testing for Freecell",
        cmd      => [qw(24 freecell)],
        expected => $BOARD_24,
    }
);

# TEST
_test_ms(
    {
        blurb    => "Testing for Freecell",
        cmd      => [qw(24)],
        expected => $BOARD_24,
    }
);

# TEST
_test_ms(
    {
        blurb    => "Testing for -t pi-ms-make Freecell",
        cmd      => [qw(-t 24)],
        expected => $BOARD_24_T,
    }
);

my $MS3E9 = <<'EOF';
8D TS JS TD JH JD JC
4D QS TH AD 4S TC 3C
9H KH QH 4C 5C KD AS
9D 5D 8S 4H KS 6S 9S
6H 2S 7H 3D KC 2C
9C 7C QC 7S QD 7D
6C 3H 8H AC 6D 3S
8C AH 2H 5H 2D 5S
EOF

# TEST
_test_ms(
    {
        blurb    => "Testing pi-ms-make long seed",
        cmd      => [qw(-t 3000000000)],
        expected => $MS3E9,
    }
);

my $MS6E9 = <<'EOF';
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
_test_ms(
    {
        blurb    => "Testing pi-ms-make long seed",
        cmd      => [qw(-t 6000000000)],
        expected => $MS6E9,
    }
);

# TEST
_test_out(
    {
        blurb    => "Testing for Bakers' Game",
        cmd      => [qw(24 bakers_game)],
        expected => <<'EOF',
4C 2C 9C 8C QS 4S 2H
5H QH 3C AC 3H 4H QD
QC 9S 6H 9H 3S KS 3D
5D 2S JC 5C JH 6D AS
2D KD 10H 10C 10D 8D
7H JS KH 10S KC 7C
AH 5S 6S AD 8H JD
7S 6C 7D 4D 8S 9D
EOF
    }
);

# TEST
_test_out(
    {
        blurb    => "Testing for Forecell",
        cmd      => [qw(1977 forecell)],
        expected => <<'EOF',
Freecells: 2D 8H 5S 6H
QH JC 7D 8S 4D 2C
JD JH KS 9C QD 2S
AD 10S 9D 3C 9S AC
10C 7H 4S KD 2H 3S
5C 6C 6D 4C 10D 8C
JS QC 8D 7C 7S 6S
3D AH 3H 10H 4H AS
KC QS 5D 5H 9H KH
EOF
    }
);

# TEST
_test_out(
    {
        blurb    => "Seahaven 100 Output",
        cmd      => [qw(100 seahaven_towers)],
        expected => <<'EOF',
Freecells: - 9S 5S
AD 5C 9C 9H 2C
JS 9D 6H 7H 2H
4S 4H AC 3C KH
QC JD AS KS 8C
7C KD 6S JC 7D
6D AH 5H 3S 8D
JH QS 6C 10C QD
10D 10H 3D 4C 2D
QH 7S 4D 5D 8S
10S 8H 2S 3H KC
EOF
    }
);

# TEST
_test_out(
    {
        blurb    => "Simple Simon 250",
        cmd      => [qw(250 simple_simon)],
        expected => <<'EOF',
6S 6C 10C JC 9H 8D 3C 5S
JH 2S KH 10S 7D 4D 8S 5D
9D 8H 4S KC 7C 4C QS 5C
4H 10D 2C 3S 8C 10H 7S
3H 6D QH 7H 9S AD
2D 2H 6H JS QD
AC 5H QC AS
KD JD KS
9C 3D
AH
EOF
    }
);

# TEST
_test_out(
    {
        blurb    => "Fan 4200",
        cmd      => [qw(-t 4200 fan)],
        expected => <<'EOF',
7H 9H 3S
4C 8H 8C
TH TS 3H
9D QD 5H
5S AS JH
QC 3D 6D
5D KD 9C
TC 4D KC
JD 8S 9S
7S JC 2S
4S KS KH
2H 5C AH
2C QS 3C
4H 2D 6S
TD 6H 7D
7C AC JS
6C 8D QH
AD
EOF
    }
);

# TEST
_test_out(
    {
        blurb    => "Eight Off 314",
        cmd      => [qw(-t 314 eight_off)],
        expected => <<'EOF',
Freecells: TD - 8D - 5D - 6S -
7C QD 8H AS AD 2S
5C 5H 2C QH 8C TS
3H JD 4C 3S AH 9C
5S 6C 4S 8S 3C KD
QS JS 9H QC 6H 2H
TH TC 7H 9D 3D 4H
4D JC KC 7S JH 9S
7D 6D 2D KH KS AC
EOF
    }
);

# TEST
_test_out(
    {
        blurb    => "der_katzenschwantz 103",
        cmd      => [qw(-t 103 der_katzenschwantz)],
        expected => <<'EOF',
JC JS AH 2H 3H 2S 2D 9D 9C 4C QD TH 6H 6S 9C 3C 5H JD 4H JH 9S 7D 4D 4C JH QH 9D 2D
KC
KH 6D 9S 2C 3H
KS QS
KD TS AS 5S TD 5C 5H 3S 7C TH 6S 7H 5D 6C 4D 5S AC QS 8S 4S AH 2H JD TC 8H 3D 8S 5D 9H 6H AS 2S 8D QC AD AC 8D
KD 6D
KS 8C 7S QC QH 5C JS 7C 2C TS 7S 4H 9H 7D 4S 6C QD AD TD 8C 3C
KC 7H TC 3S 3D 8H JC
KH
EOF
    }
);

# TEST
_test_out(
    {
        blurb    => "yukon 973",
        cmd      => [qw(-t 973 yukon)],
        expected => <<'EOF',
TD
<QC> 5H TH 9C 7H 4D
<6S> <KS> 9D 4H KH TC 8D
<5S> <7D> <3H> 5C 5D 6H QH 3D
<JC> <KD> <4C> <AS> 4S JD 8C 8S KC
<7C> <6C> <2C> <TS> <QS> 2D AD AC 7S AH
<6D> <9H> <JS> <2S> <3S> <2H> 9S QD 3C JH 8H
EOF
    }
);

# TEST
_test_out(
    {
        blurb    => "beleaguered_castle 99",
        cmd      => [qw(-t 99 beleaguered_castle)],
        expected => <<'EOF',
Foundations: H-A C-A D-A S-A
KD 3D JS 8C JC 6C
8H TH 9C 8D 6S TD
5S QH 2S 8S QD 9S
7S QS 3S 6H 7D 2H
KH 4H 4S 5D KS 3C
JH KC 2C 7C 7H 9D
4C QC 5C 4D JD 6D
TS 9H 2D 3H TC 5H
EOF
    }
);

# TEST
_test_out(
    {
        blurb    => "streets_and_alleys 1080",
        cmd      => [qw(-t 1080 streets_and_alleys)],
        expected => <<'EOF',
8D 5C KS KC 2C 7C AS
3H 8S 8C AD 4D TC 9C
JD AC 5D 6D 7S 6S QC
3D 3C 6C 8H JC 7H TD
9S JS QH JH 4C 5S
7D QD 2D TS 9D 2S
4H QS 5H 6H 3S AH
KD 2H 4S KH 9H TH
EOF
    }
);

# TEST
_test_out(
    {
        blurb    => "streets_and_alleys 1080",
        cmd      => [qw(-t 1080 citadel)],
        expected => <<'EOF',
Foundations: H-2 C-2 D-2 S-2
8D 5C 8C 6D 7S 6S
3H 8S 5D 8H JC 7H
JD 3C 6C JH 4C 5S
3D JS QH TS 9D
9S QD 6H 3S TH
7D QS 5H KH 9H 9C
4H 4S 7C QC
KD KS KC 4D TC TD
EOF
    }
);

# TEST
_test_out(
    {
        blurb    => "gypsy 200",
        cmd      => [qw(-t 200 gypsy)],
        expected => <<'EOF',
Talon: 9C AD 6S KH 8D 3D TC QC AD QC 7S AH 6C QH 8S 5C KD 4H 2D 8C JD 9S 7H 4C JS JH 9D JC 4D 9S AC QS 5S KS TD 2H 4H KC TC 2C JD 3H 3D KD 7C 9C 8C 6D 4C 3S TH 2S 5D 7D 6H TH 8H JS 6S 2C 4S 3C JH 2D TD 3H 5C QS AH AC 4D 5H 2H 2S JC 9D AS QD 9H 6C
<3S> <7H> 7D
<5H> <8D> 8S
<TS> <4S> 6D
<AS> <QD> QH
<9H> <8H> 5S
<7S> <KS> KC
<6H> <3C> 5D
<7C> <TS> KH
EOF
    }
);

# TEST
_test_out(
    {
        blurb    => "klondike 460",
        cmd      => [qw(-t 460 klondike)],
        expected => <<'EOF',
Talon: 3C AD AH KH KS 2H 3S 4D QS TD 8H AS 8D 6C 5D 4S 3H 6D 2C 7S AC 9S 7C TC
QC
<4H> 6S
<KD> <9H> 2S
<8S> <TH> <JD> 5H
<TS> <JH> <5C> <KC> 2D
<JC> <6H> <7D> <3D> <JS> 8C
<9C> <5S> <QH> <9D> <QD> <4C> 7H
EOF
    }
);

# TEST
_test_out(
    {
        blurb    => "small_harp 468",
        cmd      => [qw(-t 468 small_harp)],
        expected => <<'EOF',
Talon: 8S AD 6S 7H AC 3S 9H 9D 8D TD 2S QH 4S JH 9C 4C QS AS AH 3H 4H KD 2D 5D
<2H> <5C> <TH> <6C> <5S> <TS> 7D
<JC> <2C> <3C> <3D> <TC> 7C
<8H> <KS> <JD> <KC> 9S
<QD> <QC> <4D> 6D
<8C> <7S> 5H
<6H> KH
JS
EOF
    }
);

# TEST
_test_out(
    {
        blurb    => "Simple Simon 7537454 (Long seed)",
        cmd      => [qw(-t 7537454 simple_simon)],
        expected => <<'EOF',
KD TH 7H 2H 3S 7D KC 6S
QD JC 5C TS 8H TD 5D 4C
JS AH KS 6H 5S 9S 9D 2D
7C QS 8D QC JD 4S 6C
TC 2S 8C 3C 8S 9C
7S KH QH 3D 4H
2C 6D AC 5H
AD AS JH
3H 4D
9H
EOF
    }
);

# TEST
_test_out(
    {
        blurb    => "Freecell PySolFC 800600",
        cmd      => [qw(-F 800600)],
        expected => <<'EOF',
3H 10S QH 10D 8D JC AS
3S KC 2D 6D 5S 10H 5C
JH 6S 4D 8S 7H 7D 6H
4C 7C 9D AH 4H 5D 8C
7S KS QD 3D 5H 2H
8H 9H 2C AC KD AD
JS 9C 4S 9S 10C 2S
JD QS QC KH 3C 6C
EOF
    }
);

# TEST
_test_out(
    {
        blurb    => "Freecell PySolFC 800600 (Long flag).",
        cmd      => [qw(--pysolfc 800600)],
        expected => <<'EOF',
3H 10S QH 10D 8D JC AS
3S KC 2D 6D 5S 10H 5C
JH 6S 4D 8S 7H 7D 6H
4C 7C 9D AH 4H 5D 8C
7S KS QD 3D 5H 2H
8H 9H 2C AC KD AD
JS 9C 4S 9S 10C 2S
JD QS QC KH 3C 6C
EOF
    }
);

# TEST
_test_out(
    {
        blurb    => "Freecell PySolFC 800600",
        cmd      => [qw(-F -t 800600)],
        expected => <<'EOF',
3H TS QH TD 8D JC AS
3S KC 2D 6D 5S TH 5C
JH 6S 4D 8S 7H 7D 6H
4C 7C 9D AH 4H 5D 8C
7S KS QD 3D 5H 2H
8H 9H 2C AC KD AD
JS 9C 4S 9S TC 2S
JD QS QC KH 3C 6C
EOF
    }
);

# TEST
_test_out(
    {
        blurb    => "Freecell PySolFC 2400240 Simple Simon",
        cmd      => [qw(-F -t 2400240 simple_simon)],
        expected => <<'EOF',
9D 5C 7D 4H TH 7S KH AC
TC KD AD 5S 9C 5D AH QC
2C 3H JH 5H 8D JC 9H JD
4D QD KS 8H QH 2D QS
6D 3D 6H TS 3C 2H
7C JS 9S 2S 6S
8S 4C AS 4S
KC 8C TD
6C 7H
3S
EOF
    }
);

# TEST
_test_out(
    {
        blurb    => "PySolFC 45508856405861261758 black_hole",
        cmd      => [qw(--pysolfc -t 45508856405861261758 black_hole)],
        expected => <<'EOF',
Foundations: AS
2S 3D 6S
2C 5D 7H
3C 7C 3S
4C AC JS
9D QC 4S
QD 6C 8H
TC JC TS
8D 7S 8S
7D 3H 5H
JD 9C 2H
TD 6D QH
2D KH KC
4D KD AH
4H JH 5C
AD 9S QS
6H KS TH
5S 9H 8C
EOF
    }
);

# TEST
_test_out(
    {
        blurb    => "make_pysol --ms board No. 100 - the FC-Pro board.",
        cmd      => [qw(--ms -t 100000)],
        expected => <<'EOF',
5C 6S JC 2D 2C 8S 3C
QS 7S 9C QD JD 9D AS
8C TH 3H 4C TS 2H QC
AD 7C JS QH 5D 6D 4H
7D 4S KD 7H 5S 3D
TD 4D TC 6H 8D 2S
KC AH JH 8H KH 6C
3S AC 9H KS 5H 9S
EOF
    }
);

# TEST
_test_out(
    {
        blurb    => "make_pysol -M board No. 100 - the FC-Pro board.",
        cmd      => [qw(-M -t 100000)],
        expected => <<'EOF',
5C 6S JC 2D 2C 8S 3C
QS 7S 9C QD JD 9D AS
8C TH 3H 4C TS 2H QC
AD 7C JS QH 5D 6D 4H
7D 4S KD 7H 5S 3D
TD 4D TC 6H 8D 2S
KC AH JH 8H KH 6C
3S AC 9H KS 5H 9S
EOF
    }
);

# TEST
_test_out(
    {
        blurb    => "PySolFC black_hole 2",
        cmd      => [qw(--pysolfc -t 2 all_in_a_row)],
        expected => <<'EOF',
Foundations: -
QD 6D KS 2H
QC 2D 7C 3H
KC 9C 6S AD
3C TD 4H 7H
4C JC AC 3D
2C 8C 8H 5H
KD 6C AH 8D
5C 8S 9S KH
4D 4S TC 7D
JD 5D 2S 5S
JS QH 3S TH
6H 7S TS JH
QS 9D 9H AS
EOF
    }
);

# TEST
_test_out(
    {
        blurb    => "make_pysol --ms board long seed.",
        cmd      => [qw(--ms -t 3000000000)],
        expected => $MS3E9,
    }
);

# TEST
_test_out(
    {
        blurb    => "make_pysol --ms board even longer (> 4 G) seed.",
        cmd      => [qw(--ms -t 6000000000)],
        expected => $MS6E9,
    }
);

# TEST
_test_find_index(
    {
        blurb    => "find-deal-index for deal No. 1941",
        cmd      => [ '--ms', bin_board("1941.board") ],
        expected => "Found deal = 1941\n",
    }
);

# TEST
_test_find_index(
    {
        blurb => "find-deal-index with -o",
        cmd =>
            [ '--ms', '-o', bin_board("24.find.txt"), bin_board("24.board") ],
        expected => q{},
    }
);

# TEST
eq_or_diff(
    [ normalize_lf( scalar path( bin_board("24.find.txt") )->slurp_utf8 ) ],
    ["Found deal = 24\n"], "-o flag output to file.",
);

# TEST
_test_find_index(
    {
        blurb    => "find-deal-index for no newline board",
        cmd      => [ '--ms', bin_board("24.no-newline.board") ],
        expected => "Found deal = 24\n",
    }
);

# TEST
_test_gen_multi(
    {
        blurb => "gen-multi for black_hole",
        cmd   => [
            '--game',   'black_hole',
            '--dir',    $dir . '',
            '--prefix', 'pys',
            '--suffix', '.board',
            '--',       '45508856405861261758'
        ],
        expected => '',
    }
);

# TEST
eq_or_diff(
    [
        normalize_lf(
            scalar $dir->child('pys45508856405861261758.board')->slurp_utf8
        )
    ],
    [<<'EOF'], "gen-multi",
Foundations: AS
2S 3D 6S
2C 5D 7H
3C 7C 3S
4C AC JS
9D QC 4S
QD 6C 8H
TC JC TS
8D 7S 8S
7D 3H 5H
JD 9C 2H
TD 6D QH
2D KH KC
4D KD AH
4H JH 5C
AD 9S QS
6H KS TH
5S 9H 8C
EOF
);

$dir->remove_tree;
__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2008 Shlomi Fish

=cut
