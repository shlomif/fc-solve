#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;
use Test::Differences (qw( eq_or_diff ));

use lib './lib';

use FreecellSolver::ExtractGames ();

{
    my $obj = FreecellSolver::ExtractGames->new;

    # TEST
    eq_or_diff(
        [ grep { $_->{name} eq q#Eight Off# } @{$obj->games} ],
        [ { id => 'eight_off', name => q#Eight Off# }, ],
        "Found eight_off",
    );
}
