#!/usr/bin/perl

# This script takes a freecell-solver-* range solve dump (such as:
#
#    ./freecell-solver-multi-thread-solve 1 32000 500 -l gi > dump.txt
#
# And calculates the time elapsing from start to finish.

use strict;
use warnings;
use Path::Tiny qw/ path /;

sub get_time
{
    my $l = shift;
    if ( $l =~ m{ at (\d+\.\d+)} )
    {
        return $1;
    }
    else
    {
        die "Cannot find a number at line '$l'";
    }
}

my $output = '';
foreach my $fn (@ARGV)
{
    my @lines = path($fn)->lines_utf8;
    if ( not @lines )
    {
        die qq#File "$fn" is empty.#;
    }
    if ( @ARGV > 1 )
    {
        $output .= $fn . ':';
    }
    $output .= +( get_time( $lines[-1] ) - get_time( $lines[0] ) ) . "\n";
}

print $output;

=head1 COPYRIGHT AND LICENSE

Copyright (c) 2000 Shlomi Fish

Licensed under L<https://opensource.org/licenses/mit-license.php> .

=cut
