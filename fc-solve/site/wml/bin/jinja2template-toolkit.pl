#!/usr/bin/env perl

use strict;
use warnings;

s#\{\%\s*call\s*h(?<depth>[1-6])_section\(\s*id="(?<id>[^"]+)",\s*title="(?<title>[^"]+)"\s*\)\s*\%\}#
qq^[% WRAPPER h$+{depth}_section id = "$+{id}", title = "$+{title}" %]^
#egms
    ;

s#\{\%\s*endcall\s*\%\}#
qq^[% END %]^
#egms
    ;
