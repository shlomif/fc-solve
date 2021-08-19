#!/usr/bin/env perl

use strict;
use warnings;

s#\A\Q{% extends "template.jinja" %}\E##ms;
s#\Q{% block body %}\E#[%- WRAPPER wrap_html -%]#ms;
s#\{\% end\w+ \%\}#[% END %]#gms;

s#\{\% block title \%\}(?<title>[^\{]*)\{\% endblock \%\}#
qq^[%- SET title = "$+{title}" -%]^
#egms;

s#\{\%\s*call\s*h(?<depth>[1-6])_section\(\s*id="(?<id>[^"]+)",\s*title="(?<title>[^"]+)"\s*\)\s*\%\}#
qq^[% WRAPPER h$+{depth}_section id = "$+{id}", title = "$+{title}" %]^
#egms
    ;

s#\{\%\s*else\s*\%\}#
qq^[% ELSE %]^
#egms
    ;
s#\{\%\s*endcall\s*\%\}#
qq^[% END %]^
#egms
    ;

s#\{\{(?<title>[^\}]*)\}\}#
qq^[%$+{title}%]^
#egms;

s#\{\%\s*(?:block|macro)\s*(?<title>[\w]*)(?:\([^\)]*\))?\s*\%\}#
qq^[%- BLOCK $+{title} -%]^
#egms;

s#\{\% (?:if) (?<title>[^%]*)\%\}#
qq^[%- IF $+{title}-%]^
#egms;
