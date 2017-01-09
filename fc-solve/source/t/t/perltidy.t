#!/usr/bin/perl

use strict;
use warnings;

if ( $ENV{FCS_TEST_SKIP_PERLTIDY} )
{
    require Test::More;
    Test::More::plan( 'skip_all' =>
            "Skipping perltidy test because FCS_TEST_SKIP_PERLTIDY was set" );
}
else
{
    require Test::PerlTidy;

    Test::PerlTidy::run_tests(
        path       => $ENV{FCS_SRC_PATH},
        perltidyrc => "$ENV{FCS_SRC_PATH}/.perltidyrc",
        exclude    => [ qr/--.*?\.t\z/, qr/Makefile\.PL/ ],
    );
}
