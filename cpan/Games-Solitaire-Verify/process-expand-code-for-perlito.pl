#!/usr/bin/perl

use strict;
use warnings;

my @lines = `pi-make-microsoft-freecell-board -t 24 | fc-solve -sam -sel -p -t`;
my $s = join(',', map { "q#$_#" } @lines);
print "use MainOpen;MainOpen::enqueue_file([$s]);\n\n";
print while (<STDIN>);
