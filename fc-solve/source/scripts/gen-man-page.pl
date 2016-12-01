#!/usr/bin/perl

use strict;
use warnings;

use Getopt::Long;
use Path::Tiny;

=head1 NAME

gen-man-page.pl

=head1 DESCRIPTION

This script generates a man page-ready AsciiDoc document from the more
generic documents such as C<README.txt> or C<USAGE.txt>.

=cut

my ( $readme_path, $usage_path, $out_path );

GetOptions(
    'readme=s' => \$readme_path,
    'usage=s'  => \$usage_path,
    'output=s' => \$out_path,
) or die "Cannot process arguments.";

my $readme = path($readme_path)->slurp_utf8;
my $usage  = path($usage_path)->slurp_utf8;

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

path($out_path)->spew_utf8( $readme, "\n\n", $usage );

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2000 Shlomi Fish

=cut
