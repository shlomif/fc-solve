#!/usr/bin/perl

use strict;
use warnings;

unlink("CMakeCache.txt");

my $c_fn = "CMakeLists.txt";
open my $in, "<", $c_fn;
open my $out, ">", "$c_fn.new";
while (my $l = <$in>)
{
    if ($l =~ m/\ASET *\( *COMPILER_FLAGS_TO_CHECK/ms)
    {
        $l =~ s{ *"-Werror[^"]*"}{}g;
    }
    print {$out} $l;
}
close($in);
close($out);
rename("$c_fn.new", $c_fn);

system("c:\\Program Files\\CMake 2.6\\bin\\cmake.exe",
    "-G", "MinGW Makefiles"
);

system("c:\\strawberry\\c\\bin\\mingw32-make.exe", "package");
