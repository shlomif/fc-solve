#!/usr/bin/perl

use strict;
use warnings;

# use File::Which;
# use File::Basename;
use Cwd;
use FindBin;

{
    local $ENV{FCS_PATH} = Cwd::getcwd();
    chdir("$FindBin::Bin/t");
    my @tests = glob("t/*.{exe,t}");

    {
        # local $ENV{FCS_PATH} = dirname(which("fc-solve"));
        print STDERR "FCS_PATH = $ENV{FCS_PATH}\n";
        exec("runprove", @tests);
    }
}
