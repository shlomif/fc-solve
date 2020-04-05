#!/usr/bin/env perl

use strict;
use warnings;
use autodie;

use List::Util qw/max/;
use bytes;
use integer;

sub f { return $_[0] =~ /Iters: ([0-9]+)/ ? $1 : 1e6 }

my $MODE = $ENV{FCS_SEED_MODE};

my $mode_rec = (
    $MODE eq "1"
    ? {
        args       => [ qw#--method random-dfs -to '[01][23468]'#, ],
        deals      => [ 14249, ],
        init_iters => 100000,
        }
    : $MODE eq "2" ? {
        args  => [ qw# --freecells-num 0 --method random-dfs -to '[0AB]' #, ],
        deals => [1003256],
        init_iters => 10_000_000,
        }
    : ( die "Unknown mode" )
);

# my @deals = (14249, 10692);
my @deals = @{ $mode_rec->{deals} };
my @args  = @{ $mode_rec->{args} };

# my @deals = (10692);

my $first_seed = $ENV{FIRST_SEED} || 1;
my $check_seed = $ENV{CHECK_SEED} || $first_seed;

my $LAST_SEED = ( ( 1 << 31 ) - 1 );
my $iters     = ( $mode_rec->{init_iters} + 0 );
my @old_line;

sub handle_seed
{
    my ($seed) = @_;
    my @new_line = map {
              "$seed "
            . `pi-make-microsoft-freecell-board -t $_ | ../scripts/summarize-fc-solve -- @args  -seed "$seed" -sp r:tf -mi "$iters"`
    } @deals;
    my $new = max( map { f($_) } @new_line );

    if ( $new < $iters )
    {
        @old_line = @new_line;
        $iters    = $new;
    }
    print "$seed @old_line" =~ s/[^\n]\K\z/\n/r;

    return;
}

handle_seed($check_seed);
for my $seed ( $first_seed .. $LAST_SEED )
{
    handle_seed($seed);
}
