#!/usr/bin/perl

use strict;

my %ranks_map = qw(A 1 2 2 3 3 4 4 5 5 6 6 7 7 8 8 9 9 T 10 J 11 Q 12 K 13 10 10);

my %suits_map = qw(C 0 D 1 S 2 H 3);

sub card_to_num
{
    my $card = shift;
    my ($rank, $suit) = split(//, $card);
    return ($ranks_map{$rank} | ($suits_map{$suit}<<4));
}

my $stack = 0;
while(<>)
{
    my @cards = split(/\s+/, $_);
    print "mypos.tableau[$stack].count = " . scalar(@cards) . ";\n";
    for my $i (0 .. $#cards)
    {
        print "mypos.tableau[$stack].cards[$i] = " . card_to_num($cards[$i]) . ";\n";
    }

    $stack++;
}

my ($i);
print <<"EOF";
mypos.foundations[0] = 0;
mypos.foundations[1] = 0;
mypos.foundations[2] = 0;
mypos.foundations[3] = 0;
mypos.hold[0] = 0;
mypos.hold[1] = 0;
mypos.hold[2] = 0;
mypos.hold[3] = 0;
mypos.hold[4] = 0;
mypos.hold[5] = 0;
mypos.hold[6] = 0;
mypos.hold[7] = 0;
EOF
;

