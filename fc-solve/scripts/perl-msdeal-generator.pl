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

package Freecell::Deal::MS;

use strict;
use warnings;

use Math::RNG::Microsoft ();

use Class::XSAccessor {
    constructor => 'new',
    accessors   => [qw(deal)],
};

sub as_str
{
    my ($self) = @_;

    my @cards = (
        map {
            my $s = $_;
            map { $s . $_ } qw/C D H S/;
        } ( 'A', ( 2 .. 9 ), 'T', 'J', 'Q', 'K' )
    );
    Math::RNG::Microsoft->new( seed => scalar( $self->deal ) )
        ->shuffle( \@cards );
    my @lines = ( map { [ ':', ] } 0 .. 7 );
    my $i     = 0;
    while (@cards)
    {
        push @{ $lines[$i] }, pop(@cards);
        $i = ( ( $i + 1 ) & 7 );
    }
    my $str = join "", map { "@$_\n" } @lines;
    return $str;
}

package main;

use strict;
use warnings;
use Getopt::Long qw/ GetOptions /;

my $dir;
my $suffix = '';
GetOptions(
    'dir=s'    => \$dir,
    'suffix=s' => \$suffix,
) or die $!;

sub gen
{
    my ($deal) = @_;

    open my $o, '>', "$dir/$deal$suffix";
    $o->print( Freecell::Deal::MS->new( deal => $deal )->as_str() );
    close $o;

    return;
}

for ( my $i = 0 ; $i < @ARGV ; ++$i )
{
    my $arg = $ARGV[$i];
    if ( $arg eq 'seq' )
    {
        my $start = $ARGV[ ++$i ];
        my $end   = $ARGV[ ++$i ];
        foreach my $deal ( $start .. $end )
        {
            gen($deal);
        }
    }
    else
    {
        gen($arg);
    }
}
