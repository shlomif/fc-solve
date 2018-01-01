#!/usr/bin/perl

use strict;
use warnings;
use autodie;

use File::Spec ();

use CGI ();

my $dir_name = shift || ".";
my $filename = shift || "index.html";

opendir my $dh, $dir_name;
my @files = File::Spec->no_upwards( readdir(D) );
closedir($dh);

@files = grep { ( $_ ne "index.html" ) && ( $_ ne "CVS" ) } @files;
@files = sort { $a cmp $b } @files;
@files = map { $_ . ( ( -d "$dir_name/$_" ) ? "/" : "" ) } @files;

open my $out, '>', $filename;
print {$out} <<"EOF" ;
<!DOCTYPE html
    PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<title>Index for $dir_name</title>
</head>
<body style="background-color: white">

<h1>Index for $dir_name</h1>
<ul>
EOF

print {$out} join(
    "",
    map {
        my $escaped = CGI::escapeHTML($_);
        "<li><a href=\"$escaped\">$escaped</a></li>\n"
    } @files
);

print {$out} <<"EOF" ;
</ul>
<h3><a href="../">Up one level</a></h3>
</body>
</html>
EOF

close($out);
