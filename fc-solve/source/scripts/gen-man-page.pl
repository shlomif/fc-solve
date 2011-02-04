#!/usr/bin/perl

use strict;
use warnings;

=head1 NAME

gen-man-page.pl

=head1 DESCRIPTION

This script generates man pages ready AsciiDoc document from the more
generic documents such as C<README.txt> or C<USAGE.txt>.

=cut

sub _slurp
{
    my $filename = shift;

    open my $in, "<", $filename
        or die "Cannot open '$filename' for slurping - $!";

    local $/;
    my $contents = <$in>;

    close($in);

    return $contents;
}

my $readme = _slurp("README.txt");
my $usage = _slurp("USAGE.txt");

$usage =~ s{\A.*?(^The programs *$)}{$1}ms;

my $manify_text = <<'EOF';
:doctype: manpage

NAME
----
fc-solve - automated solver for Freecell and related Solitiare variants
EOF

my $man_title = 'fc-solve(6)';

$readme =~ s/\AFreecell Solver Readme File\n(=+)\n/
    $man_title . "\n" . '=' x length($man_title) . "\n"/ge;
$readme =~ s/(:Revision[^\n]*\n)/$1$manify_text/ms;

open my $out, ">", "fc-solve.txt";
print {$out} $readme, "\n\n", $usage;
close($out);

=head1 COPYRIGHT AND LICENSE

Copyright (c) 2000 Shlomi Fish

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

