#!/usr/bin/env perl

use strict;
use warnings;

s#\A\Q{% extends "template.jinja" %}\E##ms;
s#\Q{% block body %}\E#[%- WRAPPER wrap_html -%]#ms;
s#\Q{% endblock %}\E(?<trail_space>\s*)\z#[% END %]$+{trail_space}#ms;

s#\{\% block title \%\}(?<title>[^\{]*)\{\% endblock \%\}#
qq^[%- SET title = "$+{title}" -%]^
#egms;

s#\{\%\s*call\s*h(?<depth>[1-6])_section\(\s*id="(?<id>[^"]+)",\s*title="(?<title>[^"]+)"\s*\)\s*\%\}#
qq^[% WRAPPER h$+{depth}_section id = "$+{id}", title = "$+{title}" %]^
#egms
    ;

s#\{\%\s*endcall\s*\%\}#
qq^[% END %]^
#egms
    ;
