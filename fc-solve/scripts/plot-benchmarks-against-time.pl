#!/usr/bin/perl

use strict;
use warnings;

# Load the plugin.
use Git::Repository 'Blame';

my $repository = Git::Repository->new();

# Get the git blame information.
my $blame_lines = $repository->blame(
    'fc-solve/benchmarks/shlomif-core-i3-desktop-machine.txt');

print "Epoch time\tBenchmark duration\tDeals Per Second\n";
foreach my $line (@$blame_lines)
{
    if ( my ($timing) = $line->get_line =~ /\A([0-9]+\.[0-9]+)s\z/ )
    {
        printf "%d\t%s\t%f\n", $line->get_commit_attributes->{'author-time'},
            $timing, ( 32_000 / $timing );
    }
}

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2016 Shlomi Fish

=cut
