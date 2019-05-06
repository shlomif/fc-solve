#!/usr/bin/env perl

use strict;
use warnings;

s#<(book_info|cpan_dist|cpan_self_dist|cpan_b_self_dist|cpan_self_mod|pdoc_f|wiki_link)\s([^>]*?)/>#
    my ($tag, $args) = ($1, $2);
    "[% $tag( " . ($args =~ s{([a-z]+)="([^"]+)"}{"$1" => "$2",}gmrs) . ") %]"
    #egms;

s#<pdoc_f f="(\w+)">#[%- WRAPPER pdoc_f f = "$1" -%]#g;
s#<cpan_dist d="([^"]*)">#[%- WRAPPER cpan_dist d = "$1" -%]#g;
s#<pdoc d="(\w+)">#[%- WRAPPER pdoc d = "$1" -%]#g;
my $re =
qr#(?:perl_for_newbies_entry|modern_perl_entry|beginning_perl_entry|cpan_dist|pdoc|pdoc_f)#;

s#<($re)>#[%- WRAPPER $1 -%]#g;
s#</$re>#[%- END -%]#g;
s/\$\(ROOT\)\//[% base_path %]/g;
s&<(h[1-9]_section) ([^>]*)>&my ($t, $x)=($1,$2);my @x=$x=~/([^\s=]+=\"[^"]+\")/g; "{% call $t(".join(",",@x).") %}"&egms;
s&</(h[1-9]_section)>&{% endcall %}&g;

s&\A#include "template.wml"(?:#include[^\n]*\n|\n)*<latemp_subject\s+"([^"]*)"\s*/>\n+&{% extends "template.jinja" %}
{% block title %}${1}{% endblock %}
{% block body %}
\n\n&;

s/\$\(PATH_TO_ROOT\)/{{ base_path }}/g;
$_ .= "\n{% endblock %}\n";
