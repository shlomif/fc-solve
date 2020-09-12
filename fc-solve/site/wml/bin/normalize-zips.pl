#! /usr/bin/env perl

# Copyright 2014 Andrew Ayer
#
# This file is part of strip-nondeterminism.
#
# strip-nondeterminism is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# strip-nondeterminism is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with strip-nondeterminism.  If not, see <http://www.gnu.org/licenses/>.
#

# Modified by Shlomi Fish <shlomif@cpan.org>
# Based on strip-nondeterminism. GPLed.

use strict;
use warnings;

use File::StripNondeterminism::handlers::zip ();

File::StripNondeterminism::init();

$File::StripNondeterminism::canonical_time =
    $File::StripNondeterminism::clamp_time = 0;
for my $fn (@ARGV) { File::StripNondeterminism::handlers::zip::normalize($fn) }
