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

use Games::Solitaire::Verify::Card ();

my @suits = @{ Games::Solitaire::Verify::Card->get_suits_seq() };
my @R     = @{ Games::Solitaire::Verify::Card->get_ranks_strings() };
my %ranks = map { $R[$_] => $_ } keys @R;

my $r_qr = qr/[HCDS]-([0A23456789TJQK])/;
my $s    = "";
my @alts;
foreach my $parent_rank ( 1 .. $#R )
{
    foreach my $child_rank ( 1 .. $#R )
    {
        foreach my $parent_suit (@suits)
        {
            foreach my $child_suit (@suits)
            {
                if (
                    $parent_rank != $child_rank + 1
                    or Games::Solitaire::Verify::Card->color_for_suit(
                        $parent_suit) eq
                    Games::Solitaire::Verify::Card->color_for_suit($child_suit)
                    )
                {
                    push @alts,
                        sprintf( "%s%s %s%s",
                        $R[$parent_rank], $parent_suit,
                        $R[$child_rank],  $child_suit,
                        );

                }

            }
        }
    }
}
my $par_s  = "(?=(?:" . join( "|", sort @alts ) . "))";
my $par_re = qr/$par_s/;
while ( my $l = <ARGV> )
{
    if ( $l =~ /\AScan:/ .. $l =~ /\A---/ )
    {
        $s .= $l;
    }
    elsif ( length $s )
    {
        my ($founds) = $s =~ /(^Foundations:[^\n]*\n)/ms
            or die "line $.";
        my ($cols) = $s =~ /((?:\n:[^\n]*)+)/ms
            or die "line $.";
        my $count = -( () = $cols =~ /$par_re/g );
        $founds =~ s#$r_qr\K#$count += $ranks{$1};#eg;
        print( $s =~ s/^Scan:[^\n]*\n\K/IrrevRank: $count\n/mrs );
        $s = "";
    }
    else
    {
        print $l;
    }
}
