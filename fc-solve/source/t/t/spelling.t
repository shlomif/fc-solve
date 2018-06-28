#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;

{
    my $to_check_re = join '|',
        ( map { quotemeta join '', @$_ } ( [qw(e x p l i c t)] ) );
    open my $ack_fh, '-|', 'ag', $to_check_re, $ENV{FCS_SRC_PATH}
        or die "Cannot open ag for input - $!";

    my $count_lines = 0;
    while ( my $l = <$ack_fh> )
    {
        ++$count_lines;
        diag($l);
    }

    # TEST
    is( $count_lines, 0, "Count lines is 0." );

    close($ack_fh);
}
