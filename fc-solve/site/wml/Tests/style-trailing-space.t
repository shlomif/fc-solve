#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;

{
    open my $ack_fh, '-|', 'ack', '-l', q/[ \t]+$/, '.'
        or die "Cannot open ack for input - $!";

    my $count_lines = 0;
    ACK_OUTPUT:
    while (my $l = <$ack_fh>)
    {
        chomp($l);

        if ($l =~ m{\A(?:dest/t2-homepage|t2)/(?:lecture/CMake|(?:humour/fortunes/fortunes-shlomif-ids-data\.yaml\z))})
        {
            next ACK_OUTPUT;
        }
        $count_lines++;
        diag("$l\n");
    }

    # TEST
    is ($count_lines, 0, "Count lines is 0.");

    close($ack_fh);
}
