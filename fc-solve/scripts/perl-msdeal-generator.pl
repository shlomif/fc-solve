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

use Freecell::Deal::MS ();
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
