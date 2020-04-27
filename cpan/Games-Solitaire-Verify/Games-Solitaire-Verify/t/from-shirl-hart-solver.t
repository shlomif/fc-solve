#!/usr/bin/env perl

use strict;
use warnings;
use autodie;
use Path::Tiny qw/ path /;

use Test::Differences (qw( eq_or_diff ));

require 5.008;

use Test::More tests => 2;

use Games::Solitaire::Verify::App::CmdLine::From_ShirlHartSolver ();

my $sol_fn = path('t/data/sample-solutions/1024.shirl-hart-solver.sol.txt');

my $board_fn = path('t/data/boards/1024.fc.board');

{
    my $obj = Games::Solitaire::Verify::App::CmdLine::From_ShirlHartSolver->new(
        {
            argv => [ qw(-g freecell), $board_fn, $sol_fn, ],
        }
    );

    $obj->_process_main();

    # TEST
    eq_or_diff(
        scalar( $obj->_get_buffer ),
        path("./t/data/sample-solutions/from-shirl-hart-1024--want.txt")
            ->slurp_utf8,
        "After KS to empty pile move.",
    );
}

$sol_fn = path('t/data/sample-solutions/4.shirl-hart-solver.sol.txt');

$board_fn = path('t/data/boards/4.fc.board');

{
    my $obj = Games::Solitaire::Verify::App::CmdLine::From_ShirlHartSolver->new(
        {
            argv => [ qw(-g freecell), $board_fn, $sol_fn, ],
        }
    );

    $obj->_process_main();

    # TEST
    pass("No error thrown on deal #4");
}
