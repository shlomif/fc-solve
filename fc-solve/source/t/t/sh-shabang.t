#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;

use Test::Differences qw/ eq_or_diff /;
use Path::Tiny qw/ path /;

my $data = path( $ENV{FCS_SRC_PATH} )->visit(
    sub {
        my ( $path, $state ) = @_;
        if ( $path->is_file and $path->basename =~ m#\.sh\z# )
        {
            my ($head) = $path->lines( { count => 1 } );
            if (
                not(   ( $head eq "#!/bin/sh\n" )
                    || ( $head eq "#!/bin/bash\n" ) )
                )
            {
                $state->{$path} = $head;
            }
        }
        return;
    },
    { recurse => 1 },
);

# TEST
eq_or_diff( $data, +{}, 'All .sh files have the right sha-bang' );
