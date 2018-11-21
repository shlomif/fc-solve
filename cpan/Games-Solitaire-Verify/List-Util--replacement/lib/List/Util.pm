package List::Util;

use strict;
use warnings;

use parent 'Exporter';

our @EXPORT_OK = (qw( first min ));

sub first(&@)
{
    my $cb = shift;

    foreach my $x (@_)
    {
        local $_ = $x;
        if ( $cb->($x) )
        {
            return $x;
        }
    }
    return undef;
}

sub min
{
    my $pivot = shift;

    foreach my $x (@_)
    {
        $pivot = $x < $pivot ? $x : $pivot;
    }

    return $pivot;
}

1;
