#! /usr/bin/env perl
#
# Short description for calc_irreversible_rank.pl
#
# Author Shlomi Fish <shlomif@cpan.org>
# Version 0.0.1
# Copyright (C) 2020 Shlomi Fish <shlomif@cpan.org>
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
