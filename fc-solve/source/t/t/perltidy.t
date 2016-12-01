#!/usr/bin/perl

use strict;
use warnings;

use Test::PerlTidy (qw( run_tests ));

run_tests(
    path       => $ENV{FCS_SRC_PATH},
    perltidyrc => "$ENV{FCS_SRC_PATH}/.perltidyrc",
    exclude    => [ qr/--.*?\.t\z/, qr/Makefile\.PL/ ],
);
