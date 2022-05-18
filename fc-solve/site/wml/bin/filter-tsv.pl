#! /usr/bin/env perl
#
# Short description for filter-tsv.pl
#
# Version 0.0.1
# Copyright (C) 2022 Shlomi Fish < https://www.shlomifish.org/ >
#
# Licensed under the terms of the MIT license.

use strict;
use warnings;
use 5.014;
use autodie;

say if ( ( not /\A[0-9]/ ) or ( $F[0] eq "0" ) or ( $F[0] =~ /000000\z/ ) );
