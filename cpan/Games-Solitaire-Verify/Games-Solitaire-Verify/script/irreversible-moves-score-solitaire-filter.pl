#! /usr/bin/env perl
#
# irreversible-moves-score-solitaire-filter.pl
#
# Filters the output of "fc-solve -s -i -p -t -sam -sel" to
# display an extra header for each state with the "rank"/"score"
# of irreversible moves, which are:
#
# 1. Moving a card to the foundation.
# 2. Moving a card away from a non-parent (where it was lying at the
# start of the game).
#
# Author Shlomi Fish <shlomif@cpan.org>
# Copyright (C) 2020 Shlomi Fish <shlomif@cpan.org>.
# Under the MIT / Expat license.
#
use strict;
use warnings;
use 5.014;
use autodie;

use Games::Solitaire::Verify::Card  ();
use Games::Solitaire::Verify::State ();

my @suits = @{ Games::Solitaire::Verify::Card->get_suits_seq() };
my $s     = "";
while ( my $l = <ARGV> )
{
    if ( $l =~ /\AScan:/ .. $l =~ /\A---/ )
    {
        $s .= $l;
    }
    elsif ( length $s )
    {
        my ($str) = $s =~ /(^Foundations:.*?\n)\n/ms
            or die "line $.";
        my $state = Games::Solitaire::Verify::State->new(
            {
                string  => $str,
                variant => "freecell",
            }
        );
        my $count = 0;
        foreach my $i ( 0 .. $state->num_columns - 1 )
        {
            my $col = $state->get_column($i);
            foreach my $h ( 0 .. $col->len - 2 )
            {
                my $parent = $col->pos($h);
                my $child  = $col->pos( $h + 1 );
                if (
                    not(    ( $parent->color ne $child->color )
                        and ( $parent->rank == $child->rank + 1 ) )
                    )
                {
                    --$count;
                }
            }
        }
        foreach my $suit (@suits)
        {
            $count += $state->get_foundation_value( $suit, 0 );
        }
        print( $s =~ s/^Scan:[^\n]*\n\K/IrrevRank: $count\n/mrs );
        $s = "";
    }
    else
    {
        print $l;
    }
}
