#!/usr/bin/env perl

use strict;
use warnings;
use autodie;

use bytes;
use integer;

sub f { return $_[0] =~ /Iters: ([0-9]+)/ ? $1 : 1e6 }

my $seed = $ENV{FIRST_SEED} || 1;

my $iters = 100000;
my $old_line = '';
while (1)
{
    my $new_line = "$seed " . `pi-make-microsoft-freecell-board -t 29218 | ../../source/scripts/summarize-fc-solve -- --method random-dfs -to '[01][23468]'  -seed "$seed" -sp r:tf -mi "$iters"`;
    my $new = f($new_line);

    if ($new < $iters)
    {
        $old_line = $new_line;
        $iters = $new;
    }
    print "$seed $old_line";
}
continue
{
    $seed++;
}
