#!/usr/bin/perl -w

use strict;

use CGI;

my $dir_name = shift || ".";

opendir D, ".";
my @files = <D>;
closedir(D);

@files = grep { ($_ ne "index.html") && (! /\.\.?/) } @files;

open O, ">index.html";
print O, <<"EOF" ;
<html>
<head>
<title>Index for $dir_name</title>
</head>
<body style="background-color: white">

<h1>Index for $dir_name</h1>
<ul>
EOF

print O join("", map { my $escaped = CGI::escapeHTML($_); "<li><a href=\"$escaped\">$escaped</a></li>\n" });

print O, <<"EOF" ;
</ul>
</body>
</html>
EOF
;


