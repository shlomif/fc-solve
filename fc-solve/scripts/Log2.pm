package Log2;

# Silly module I've written (and I think it would be better as a DSL):
#
# Sample invocation and result:
#
# $ perl -MLog2 -e 'plog2(4 => 14, 44 => 5, 4 => 1, 4 => 3)'
# log2(14) * 4 + log2(5) * 44 + log2(1) * 4 + log2(3) * 4 =
#    123.734105866159 bits (bytes: 16)

use strict;
use warnings;

use POSIX qw(ceil);

our @EXPORT = (qw(plog2));

use base 'Exporter';

sub plog2
{
    my @specs = @_;

    my $total = 0;

    my @clauses;

    while (@specs)
    {
        my $count = shift(@specs);
        my $base  = shift(@specs);

        push @clauses, "log2($base) * $count";
        $total += log($base) * $count;
    }

    my $num_bits  = $total / log(2);
    my $num_bytes = ceil( $num_bits / 8 );
    print join( ' + ', @clauses ), " = $num_bits bits (bytes: $num_bytes)\n";
}

1;

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2012 Shlomi Fish

=cut
