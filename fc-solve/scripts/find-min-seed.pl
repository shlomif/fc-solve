#!/usr/bin/env perl

use strict;
use warnings;
use autodie;

use List::Util qw/max/;
use bytes;
use integer;

sub f { return $_[0] =~ /Iters: ([0-9]+)/ ? $1 : 1e6 }

# my @deals = (14249, 10692);
my @deals = (14249);

# my @deals = (10692);

my $first_seed = $ENV{FIRST_SEED} || 1;
my $check_seed = $ENV{CHECK_SEED} || $first_seed;

my $LAST_SEED = ( ( 1 << 31 ) - 1 );
my $iters     = 100000;
my @old_line;

sub handle_seed
{
    my ($seed) = @_;
    my @new_line = map {
              "$seed "
            . `pi-make-microsoft-freecell-board -t $_ | ../../source/scripts/summarize-fc-solve -- --method random-dfs -to '[01][23468]'  -seed "$seed" -sp r:tf -mi "$iters"`
    } @deals;
    my $new = max( map { f($_) } @new_line );

    if ( $new < $iters )
    {
        @old_line = @new_line;
        $iters    = $new;
    }
    print "$seed @old_line";

    return;
}

handle_seed($check_seed);
for my $seed ( $first_seed .. $LAST_SEED )
{
    handle_seed($seed);
}
