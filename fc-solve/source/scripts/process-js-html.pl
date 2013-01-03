#!/usr/bin/perl

use strict;
use warnings;

use autodie;
use utf8;

use IO::All;

binmode STDOUT, ':encoding(utf8)';

my $text = io("-")->utf8->slurp;

my $replacement = <<'END_JS';
FS.init((function() {
    var control = $("#stdin");
    var control_input_index = 0;
    return function() {
        var v = control.val();
        return v.charCodeAt(control_input_index++) || null;
}} )())
END_JS

$text =~ s#(</head>)#\n<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.8.3/jquery.min.js"></script>\n$1#;
$text =~ s#(</canvas>)#$1\n<strong>Input:</strong>\n<textarea class="emscripten" id="stdin" rows="10"></textarea>\n#;
$text =~ s/FS.init\(\)/$replacement/;

print $text;

