package MainOpen;

use strict;
use warnings;

my @file_handles;

sub enqueue_file
{
    my ($fh) = @_;
    push @file_handles, $fh;
}

sub my_open
{
    return shift(@file_handles);
}

sub readline
{
    my ($fh) = @_;

    return shift(@$fh);
}

1;

