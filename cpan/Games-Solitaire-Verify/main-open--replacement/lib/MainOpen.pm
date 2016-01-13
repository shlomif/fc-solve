package MainOpen;

use strict;
use warnings;

my @file_handles;

sub enqueue_file
{
    my ($fh) = @_;
    push @file_handles, (bless $fh, 'MainOpen');
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

sub getline
{
    my ($fh) = @_;

    return $fh->readline();
}

1;

