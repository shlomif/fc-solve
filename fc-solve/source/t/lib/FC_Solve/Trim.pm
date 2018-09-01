package FC_Solve::Trim;

use 5.014;
use strict;
use warnings;

use parent 'Exporter';
our @EXPORT_OK = qw(trim_trail_ws);

sub trim_trail_ws
{
    return shift =~ s/[ \t]+$//gmrs;
}

1;
