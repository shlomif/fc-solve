package FC_Solve::Trim;

use strict;
use warnings;

use parent 'Exporter';

our @EXPORT_OK=qw(trim_trail_ws);

sub trim_trail_ws
{
    my $trim_trail_ws = shift;
    $trim_trail_ws =~ s/[ \t]+$//gms;
    return $trim_trail_ws;
}

1;

