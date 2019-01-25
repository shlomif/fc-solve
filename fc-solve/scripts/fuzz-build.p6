#!/usr/bin/env perl6
#
# fuzz-build.p6
# Copyright (C) 2018 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.
#

sub MAIN(Bool :$g=False, Bool :$t=False, Bool :$rb=False)
{
    my $seed;
    if ($g)
    {
        %*ENV{"FCS_GCC"}=1;
        $seed=1;
    }
    else
    {
        %*ENV{"CC"}="/usr/bin/clang";
        %*ENV{"CXX"}="/usr/bin/clang++";
        %*ENV{"FCS_CLANG"}=1;
        $seed=1;
    }
    %*ENV{"HARNESS_BREAK"}="1";
    %*ENV{"CFLAGS"}="-Werror";
    my $cmd ="../source/Tatzer && make";
    $cmd ~= " && perl ../source/run-tests.pl" if $t;
    if $rb
    {
        $cmd = Q:qq (bash -c ". ~/.bashrc && Theme fcs && _reprb_diff_builds");
    }
    while True
    {
        say "Checking seed=$seed";
        %*ENV{"FCS_THEME_RAND"}="$seed";
        if shell($cmd)
        {
            ++$seed;
        }
        else
        {
            say "seed=$seed failed";
            last;
        }
    }
}
