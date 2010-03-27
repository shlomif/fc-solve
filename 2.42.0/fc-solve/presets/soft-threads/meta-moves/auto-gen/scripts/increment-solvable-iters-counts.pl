#!/usr/bin/perl

use strict;
use warnings;

use IO::All;

foreach my $f (io("data/")->filter(sub { m{\.data\.bin\z} })->all_files)
{
    my @array = unpack("l*", scalar($f->slurp));
    foreach my $elem (@array[3 .. $#array])
    {
        if ($elem > 0)
        {
            $elem++;
        }
    }
    $f->print(pack("l*", @array));
    print "$f\n";
}
