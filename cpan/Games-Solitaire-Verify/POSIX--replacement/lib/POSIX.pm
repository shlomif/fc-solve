package POSIX;

use strict;
use warnings;

use Exporter;

our @ISA = qw(Exporter);

our @EXPORT_OK = qw(ceil);

sub INT_MAX
{
    return 0x7FFFFFFF;
}

sub ceil
{
    my ($x) = @_;

    my $i = int($x);

    if ($i < $x)
    {
        $i += 1;
    }
    return $i;
}

1;
