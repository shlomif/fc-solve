#!/usr/bin/perl

use strict;

my %ranks_map = qw(A 1 2 2 3 3 4 4 5 5 6 6 7 7 8 8 9 9 T 10 J 11 Q 12 K 13 10 10);

my %suits_map = qw(C 0 D 1 S 2 H 3);

sub card_to_num
{
    my $card = shift;
    if ($card eq "-")
    {
        return 0;
    }
    my ($rank, $suit) = split(//, $card);
    return ($ranks_map{$rank} | ($suits_map{$suit}<<4));
}

my @foundation = (0,0,0,0);
my @freecells = ();

my $stack = 0;
while(<>)
{
    if (/^Foundations:/)
    {
        s/^Foundations:\s*//;
        my @f_s = split(/\s+/, $_);
        foreach my $f (@f_s)
        {
            $f =~ m/([CHDS])-([1-9ATJKQ])/;
            $foundation[$suits_map{$1}] = $ranks_map{$2};
        }

        next;
    }
    if (/^Freecells:/)
    {
        s/^Freecells:\s*//;
        my $string = $_;
        @freecells = (map { &card_to_num($_) } split(/\s+/, $string));
        next;
    }
    my @cards = split(/\s+/, $_);
    print "mypos.tableau[$stack].count = " . scalar(@cards) . ";\n";
    for my $i (0 .. $#cards)
    {
        print "mypos.tableau[$stack].cards[$i] = " . card_to_num($cards[$i]) . ";\n";
    }

    $stack++;
}

push @freecells, ((0) x (8 - scalar(@freecells)));

my ($i);
print join("", (map { "mypos.foundations[$_] = $foundation[$_]\n" } (0 .. 3)));
print join("", (map { "mypos.hold[$_] = $freecells[$_]\n" } (0 .. 7)));

