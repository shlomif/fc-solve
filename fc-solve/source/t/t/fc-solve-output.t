#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;
use Carp;
use Data::Dumper;
use String::ShellQuote;
use File::Spec;

sub trap_board
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my $args = shift;

    my $board = $args->{board};
    my $deal = $args->{deal};
    my $msdeals = $args->{msdeals};

    if (! defined($board))
    {
        if (!defined($deal))
        {
            confess "Neither Deal nor board are specified";
        }
        if ($deal !~ m{\A[1-9][0-9]*\z})
        {
            confess "Invalid deal $deal";
        }
    }

    my $theme = $args->{theme} || ["-l", "gi"];

    my $variant = $args->{variant}  || "freecell";
    my $is_custom = ($variant eq "custom");
    my $variant_s = $is_custom ? "" : "-g $variant";

    my $fc_solve_exe = shell_quote($ENV{'FCS_PATH'} . "/fc-solve");

    open my $fc_solve_output,
        ($msdeals ?
            "pi-make-microsoft-freecell-board $deal | " :
            ($board ? "" : "make_pysol_freecell_board.py $deal $variant | ")
        ) .
        "$fc_solve_exe $variant_s " . shell_quote(@$theme) . " -p -t -sam " .
        ($board ? shell_quote($board) : "") .
        " |"
        or Carp::confess "Error! Could not open the fc-solve pipeline";

    my @lines = <$fc_solve_output>;

    close($fc_solve_output);

    return { out_lines => \@lines };
}

{
    my $fc_solve_output = trap_board({ deal => 1941, theme => ['--show-exceeded-limits', '--max-iters', '10'], });

    my $output_text = join('', @{$fc_solve_output->{out_lines}});

    # TEST
    like(
        $output_text,
        qr/
            ^Iterations\ count\ exceeded\.\n
            Total\ number\ of\ states\ checked\ is\ 10\.\n
        /msx,
        "Checking that --show-exceeded-limits is working properly.",
    );
}

=head1 COPYRIGHT AND LICENSE

Copyright (c) 2008 Shlomi Fish

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

