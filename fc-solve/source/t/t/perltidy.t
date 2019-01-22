#!/usr/bin/perl

use strict;
use warnings;

if ( $ENV{FCS_TEST_SKIP_PERLTIDY} )
{
    require Test::More;
    Test::More::plan( 'skip_all' =>
            "Skipping perltidy test because FCS_TEST_SKIP_PERLTIDY was set" );
}
require Test::Code::TidyAll;

Test::Code::TidyAll::tidyall_ok(
    conf_file => "$ENV{FCS_SRC_PATH}/.tidyallrc",
    (
        !$ENV{FCS_TEST_CLANG_FORMAT}
        ? ( selected_plugins => [qw(PerlTidy)], )
        : ()
    ),
);
