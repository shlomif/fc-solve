#! /usr/bin/env perl
#
# Short description for perl-msdeal-generator.pl
#
# Copyright (C) 2020 Shlomi Fish ( https://www.shlomifish.org/ )
#
# SPDX-License-Identifier: MIT
#
use strict;
use warnings;
use integer;

use Getopt::Long qw/ GetOptions /;

my $seed;

sub _rand
{
    $seed = ( ( $seed * 214013 + 2531011 ) & (0x7FFF_FFFF) );
    return ( ( $seed >> 16 ) & 0x7fff );
}

sub shuffle
{
    my ($deck) = @_;

    my $i = @$deck;
    while ( --$i )
    {
        my $j = _rand() % ( $i + 1 );
        @$deck[ $i, $j ] = @$deck[ $j, $i ];
    }
    return;
}

my $dir;
my $suffix = '';
GetOptions(
    'dir=s'    => \$dir,
    'suffix=s' => \$suffix,
) or die $!;

for ( my $i = 0 ; $i < @ARGV ; ++$i )
{
    my $arg = $ARGV[$i];
    if ( $arg eq 'seq' )
    {
        my $start = $ARGV[ ++$i ];
        my $end   = $ARGV[ ++$i ];
        foreach my $deal ( $start .. $end )
        {
            my @cards = (
                map {
                    my $s = $_;
                    map { $s . $_ } qw/C D H S/;
                } ( 'A', ( 2 .. 9 ), 'T', 'J', 'Q', 'K' )
            );
            $seed = $deal;
            shuffle( \@cards );
            my @lines = ( map { [] } 0 .. 7 );
            my $i     = 0;
            while (@cards)
            {
                push @{ $lines[$i] }, pop(@cards);
                $i = ( ( $i + 1 ) & 7 );
            }
            open my $o, '>', "$dir/$deal$suffix";
            foreach my $l (@lines)
            {
                print {$o} "@$l\n";
            }
            close $o;
        }
    }
}
