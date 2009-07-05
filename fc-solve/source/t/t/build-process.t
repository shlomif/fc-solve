#!/usr/bin/perl

use strict;
use warnings;

use Test::More;
use Carp;
use Data::Dumper;
use String::ShellQuote;
use File::Spec;
use List::MoreUtils qw(none);

if (! $ENV{'FCS_TEST_BUILD'} )
{
    plan skip_all => "Skipping because FCS_TEST_BUILD is not set";
}

plan tests => 7;

# Change directory to the Freecell Solver base distribution directory.
chdir($ENV{"FCS_PATH"});

sub test_cmd
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;
    my ($cmd, $blurb) = @_;

    my $sys_ret = 
        ((ref($cmd) eq "ARRAY")
            ? system(@$cmd)
            : system($cmd)
        );

    if (!ok (!$sys_ret, $blurb))
    {
        die "Command failed! $!.";
    }
}

{
    # TEST
    test_cmd(["cmake", "."], "cmake succeeded");

    # TEST
    test_cmd(["make", "package_source"], "make package_source is successful");

    open my $ver_fh, "<", "ver.txt";
    my $version = <$ver_fh>;
    close($ver_fh);
    chomp($version);

    my $arc_name = "freecell-solver-$version.tar.gz";
    # TEST
    ok (scalar(-e $arc_name), 
        "Archive exists."
    );

    open my $tar_fh, "-|", "tar", "-tzvf", $arc_name
        or die "Could not open Tar '$arc_name' for opening.";

    my @tar_lines = (<$tar_fh>);
    close($tar_fh);

    chomp(@tar_lines);

    # TEST
    ok ((none { m{/config\.h\z} } @tar_lines),
        "Archive does not contain config.h files");

    # TEST
    ok ((none { m{/freecell-solver-range-parallel-solve\z} } @tar_lines),
        "Archive does not contain the range solver executable");

    # TEST
    ok ((none { m{/libfreecell-solver\.a\z} } @tar_lines),
        "Archive does not contain libfreecell-solver.a");

    # TEST
    test_cmd ("rpmbuild -tb $arc_name 2>/dev/null", 
        "rpmbuild -tb is successful."
    );
}

=head1 COPYRIGHT AND LICENSE

Copyright (c) 2009 Shlomi Fish

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

=cut


