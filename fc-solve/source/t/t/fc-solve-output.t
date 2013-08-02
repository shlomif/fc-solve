#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 7;
use Carp;
use Data::Dumper;
use String::ShellQuote;
use File::Spec;
use File::Temp qw( tempdir );

sub trap_board
{
    my $args = shift;

    my $board = $args->{board};
    my $deal = $args->{deal};
    my $msdeals = $args->{msdeals};
    my $pysolfc_deals = $args->{pysolfc_deals};

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
            ($board ? "" :
                ("make_pysol_freecell_board.py" .
                    ($pysolfc_deals ? " -F " : "") .
                    " $deal $variant | ")
            )
        ) .
        "$fc_solve_exe $variant_s " . shell_quote(@$theme) . " -p -t -sam " .
        ($board ? shell_quote($board) : "") .
        " |"
        or Carp::confess "Error! Could not open the fc-solve pipeline";

    my @lines = <$fc_solve_output>;

    close($fc_solve_output);

    return { out_lines => \@lines };
}

sub trap_dbm
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my $args = shift;

    my $board_fn = $args->{board_fn};

    my $dbm_solve_exe = shell_quote($ENV{'FCS_PATH'} . "/dbm_fc_solver");

    my $temp_dir = tempdir (CLEANUP => 1);

    open my $fc_solve_output,
        "$dbm_solve_exe " . shell_quote(
            "--offload-dir-path", $temp_dir, "--num-threads", 1,
            $board_fn
        ) .
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

{
    my $fc_solve_output = trap_board({ deal => 1941, theme => ['-sel', '--max-iters', '10'], });

    my $output_text = join('', @{$fc_solve_output->{out_lines}});

    # TEST
    like(
        $output_text,
        qr/
            ^Iterations\ count\ exceeded\.\n
            Total\ number\ of\ states\ checked\ is\ 10\.\n
        /msx,
        "Checking that '-sel' (shortened option) is working properly.",
    );
}

{
    my $GOOD_ITERS_COUNT = 60817;

    # TEST:$num_choices=2;
    foreach my $choice (qw(fc_solve fcpro))
    {
        my $fc_solve_output = trap_board({ deal => '22215757927177568630',
                pysolfc_deals => 1,
                theme => [qw(-s -i -p -t -sam -sel -l qsi -fif 1 --flares-choice), $choice], });

        my $output_text = join('', @{$fc_solve_output->{out_lines}});

        # TEST*$num_choices
        is(
            scalar(() = $output_text =~ /^Iteration: /gms),
            $GOOD_ITERS_COUNT,
            "Verifying existing iterations count for qsi --flares-choice $choice",
        );

        # TEST*$num_choices
        like(
            $output_text,
            qr/^Total number of states checked is \Q$GOOD_ITERS_COUNT\E\.$/ms,
            "Total number of states checked is reported as the same as the actual one for --flares-choice $choice.",
        );
    }
}

{

    my $WS = ' ';
    my $needle = <<"EOF";
Success!
--------
Foundations: H-T C-8 D-A S-J$WS
Freecells:  7D$WS$WS$WS$WS
: KH QC JD
: KC QH
: 3D
: KS QD JC TD 9C 8D
: 2D KD QS JH TC 9D
: 5D
: 4D
: 6D

==
Column 4 -> Freecell 1
--------
Foundations: H-T C-8 D-A S-J$WS
Freecells:  7D  9D
: KH QC JD
: KC QH
: 3D
: KS QD JC TD 9C 8D
: 2D KD QS JH TC
: 5D
: 4D
: 6D

==
Column 4 -> Column 0
--------
Foundations: H-T C-8 D-A S-J$WS
Freecells:  7D  9D
: KH QC JD TC
: KC QH
: 3D
: KS QD JC TD 9C 8D
: 2D KD QS JH
: 5D
: 4D
: 6D

==
Freecell 1 -> Column 0
--------
Foundations: H-T C-8 D-A S-J$WS
Freecells:  7D$WS$WS$WS$WS
: KH QC JD TC 9D
: KC QH
: 3D
: KS QD JC TD 9C 8D
: 2D KD QS JH
: 5D
: 4D
: 6D

==
Column 4 -> Foundation 0
--------
Foundations: H-J C-8 D-A S-J$WS
Freecells:  7D$WS$WS$WS$WS
: KH QC JD TC 9D
: KC QH
: 3D
: KS QD JC TD 9C 8D
: 2D KD QS
: 5D
: 4D
: 6D

==
Column 4 -> Foundation 3
--------
Foundations: H-J C-8 D-A S-Q$WS
Freecells:  7D$WS$WS$WS$WS
: KH QC JD TC 9D
: KC QH
: 3D
: KS QD JC TD 9C 8D
: 2D KD
: 5D
: 4D
: 6D

==
Column 4 -> Freecell 1
--------
Foundations: H-K C-K D-K S-K$WS
Freecells:$WS$WS$WS$WS$WS$WS$WS$WS
:$WS
:$WS
:$WS
:$WS
:$WS
:$WS
:$WS
:$WS

==
END
EOF

    my $dbm_output = trap_dbm(
        {
            board_fn =>
                File::Spec->catfile(
                    $ENV{FCS_SRC_PATH}, 't', 't', 'data',
                    'sample-boards', '2freecells-24-mid-with-colons.board'
                ),
        }
    );

    my $output_text = join('', @{$dbm_output->{out_lines}});

    # TEST
    ok (
        (index( $output_text, $needle) >= 0),
        "dbm_fc_solver invocation contains the solution's output."
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

