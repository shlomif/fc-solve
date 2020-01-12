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
use 5.014;
use autodie;

use Path::Tiny qw/ path tempdir tempfile cwd /;

use Getopt::Long qw/ GetOptions /;
use Math::RNG::Microsoft ();

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
            Math::RNG::Microsoft->new( seed => $deal )->shuffle( \@cards );
            my @lines = ( map { [] } 0 .. 7 );
            my $l     = 0;
            while (@cards)
            {
                push @{ $lines[$l] }, pop(@cards);
                ( $l += 1 ) &= 7;
            }
            open my $o, '>', "$dir/$deal$suffix";
            print {$o} map { join( " ", @$_ ) . "\n" } @lines;
            close $o;
        }
    }
}
