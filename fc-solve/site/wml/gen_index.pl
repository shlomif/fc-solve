#!/usr/bin/perl -w

use strict;

use CGI;

my $dir_name = shift || ".";
my $filename = shift || "index.html";

opendir D, $dir_name;
my @files = readdir(D);
closedir(D);

@files = grep { ($_ ne "index.html") && (! /^\.\.?$/) && ($_ ne "CVS")} @files;
@files = sort { $a cmp $b } @files;
@files = map { $_ . ((-d "$dir_name/$_") ? "/" : "") } @files;

open O, ">$filename";
print O <<"EOF" ;
<html>
<head>
<title>Index for $dir_name</title>
</head>
<body style="background-color: white">

<h1>Index for $dir_name</h1>
<ul>
EOF

print O join("", map { my $escaped = CGI::escapeHTML($_); "<li><a href=\"$escaped\">$escaped</a></li>\n" } @files);

print O <<"EOF" ;
</ul>
<h3><a href="../">Up one level</a></h3>
</body>
</html>
EOF
;

close(O);

