#!/usr/bin/env perl

use strict;
use warnings;

use HTML::Latemp::GenMakeHelpers;
use File::Find::Object::Rule;

my $generator =
    HTML::Latemp::GenMakeHelpers->new(
        'hosts' =>
        [
            {
                'id' => "common",
                'source_dir' => "common",
                'dest_dir' => "\$(TARGET)",
            },
            {
                'id' => "src",
                'source_dir' => "src",
                'dest_dir' => q/$(D)/,
            },
        ],
    );

eval {
$generator->process_all();
};

my $E = $@;

if ($E)
{
    print "$E\n";
}

1;

