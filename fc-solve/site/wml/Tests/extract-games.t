#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 3;
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

    # TEST
    eq_or_diff(
        [ grep { $_->{name} =~ m#\A(?:Eight Off|Forecell)\z# } @{$obj->games} ],
        [
            { id => 'eight_off', name => q#Eight Off# },
            { id => 'forecell', name => q#Forecell# },
        ],
        "Found eight_off",
    );

    # TEST
    like(
        scalar( $obj->calc_html() ),
        qr#<option value="freecell" selected="selected">Freecell \(default\)</option>#,
        "selected freecell.",
    );
}
