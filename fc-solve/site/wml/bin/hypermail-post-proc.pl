#! /usr/bin/env perl
#
# Short description for hypermail-post-proc.pl
#
# Version 0.0.1
# Copyright (C) 2022 Shlomi Fish < https://www.shlomifish.org/ >
#
# Licensed under the terms of the MIT license.

use strict;
use warnings;
use 5.014;
use autodie;

s#\A(<li><dfn>Archived on</dfn>: )[^<]+(</li>)\z#$1 . "12 June 2022" . $2#ems;
