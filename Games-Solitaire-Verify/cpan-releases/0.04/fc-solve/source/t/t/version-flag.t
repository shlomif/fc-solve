#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 2;
use String::ShellQuote;

{
    open my $good_ver_fh, "<", "../ver.txt";
    my $good_ver = <$good_ver_fh>;
    close($good_ver_fh);
    chomp($good_ver);

    my $fc_solve_exe = shell_quote($ENV{'FCS_PATH'} . "/fc-solve");

    my $got_output = `$fc_solve_exe --version`;

    my $got_status = $?;
    # TEST
    ok (!$got_status, "fc-solve --version ran fine");

    # TEST
    like (
        $got_output,
        qr{^libfreecell-solver version \Q$good_ver\E}ms,
        "Version is contained in the --version display",
    );
}
