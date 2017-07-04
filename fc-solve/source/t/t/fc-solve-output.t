#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 10;
use FC_Solve::GetOutput ();
use Carp                ();
use String::ShellQuote qw/ shell_quote /;
use File::Temp qw( tempdir );
use Test::Differences qw/ eq_or_diff /;
use FC_Solve::Paths
    qw/ $IS_WIN bin_board bin_exe_raw is_without_dbm normalize_lf samp_board /;

sub _get
{
    return normalize_lf( join( '', @{ shift->{out_lines} } ) );
}

sub trap_board
{
    my $args = shift;

    my $fc_solve_output = FC_Solve::GetOutput->new($args)->open_cmd_line->{fh};
    my @lines           = <$fc_solve_output>;

    close($fc_solve_output);

    return { out_lines => \@lines };
}

sub trap_dbm
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my $args = shift;

    open my $fc_solve_output,
        shell_quote(
        bin_exe_raw( ['dbm_fc_solver'] ),
        "--offload-dir-path", tempdir( CLEANUP => 1 ),
        "--num-threads", 1, $args->{board_fn}
        )
        . " |"
        or Carp::confess "Error! Could not open the fc-solve pipeline";

    my @lines = <$fc_solve_output>;

    close($fc_solve_output);

    return { out_lines => \@lines };
}

sub trap_depth_dbm
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my $args = shift;

    open my $fc_solve_output,
        shell_quote(
        bin_exe_raw( ['depth_dbm_fc_solver'] ), "--offload-dir-path",
        ( tempdir( CLEANUP => 1 ) . '/' ), "--num-threads",
        1,                  "--iters-delta-limit",
        $args->{max_iters}, $args->{board_fn}
        )
        . " |"
        or Carp::confess
        "Error! Could not open the depth_dbm_fc_solver pipline!";

    my @lines = <$fc_solve_output>;

    close($fc_solve_output);

    return { out_lines => \@lines };
}

{
    my $fc_solve_output = trap_board(
        {
            deal  => 1941,
            theme => [ '--show-exceeded-limits', '--max-iters', '10' ],
        }
    );

    my $output_text = _get($fc_solve_output);

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
    my $trap = sub {
        return trap_board(@_)->{out_lines};
    };

    my $want_output =
        $trap->( { deal => 24, theme => [ '-to', '0123456789' ], } );
    my $have_output = $trap->(
        {
            deal  => 24,
            theme => [ '-to', '0123456789', '-dto2', '9,0123456789', ],
        }
    );
    my $long_have_output = $trap->(
        {
            deal  => 24,
            theme => [
                '-to', '0123456789', '--depth-tests-order2', '9,0123456789',
            ],
        }
    );

    # TEST
    eq_or_diff( $have_output, $want_output,
        '-dto2 yields the pristine tests order.',
    );

    # TEST
    eq_or_diff( $long_have_output, $want_output,
        '--depth-tests-order2 yields the pristine tests order.',
    );
}

{
    my $fc_solve_output = trap_board(
        { deal => 1941, theme => [ '-sel', '--max-iters', '10' ], } );

    my $output_text = _get($fc_solve_output);

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
        my $fc_solve_output = trap_board(
            {
                deal          => '22215757927177568630',
                pysolfc_deals => 1,
                theme         => [
                    qw(-s -i -p -t -sam -sel -l qsi -fif 1 --flares-choice),
                    $choice
                ],
            }
        );

        my $output_text = _get($fc_solve_output);

        # TEST*$num_choices
        is(
            scalar( () = $output_text =~ /^Iteration: /gms ),
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
SKIP:
    {

        if ( is_without_dbm() || $IS_WIN )
        {
            Test::More::skip( "without the dbm fc_solvers or win32", 1 );
        }
        my $needle = <<"EOF";
Success!
--------
Foundations: H-T C-8 D-A S-J
Freecells:  7D
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
Foundations: H-T C-8 D-A S-J
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
Foundations: H-T C-8 D-A S-J
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
Foundations: H-T C-8 D-A S-J
Freecells:  7D
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
Foundations: H-J C-8 D-A S-J
Freecells:  7D
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
Foundations: H-J C-8 D-A S-Q
Freecells:  7D
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
Foundations: H-K C-K D-K S-K
Freecells:
:
:
:
:
:
:
:
:

==
END
EOF

        my $needle_non_debondt = <<"EOF";
Success!
--------
Foundations: H-T C-8 D-A S-J
Freecells:      7D
: 3D
: 4D
: 5D
: 6D
: 2D KD QS JH TC 9D
: KH QC JD
: KC QH
: KS QD JC TD 9C 8D

==
Column 4 -> Freecell 0
--------
Foundations: H-T C-8 D-A S-J
Freecells:  7D  9D
: 3D
: 4D
: 5D
: 6D
: 2D KD QS JH TC
: KH QC JD
: KC QH
: KS QD JC TD 9C 8D

==
Column 4 -> Column 5
--------
Foundations: H-T C-8 D-A S-J
Freecells:  7D  9D
: 3D
: 4D
: 5D
: 6D
: 2D KD QS JH
: KH QC JD TC
: KC QH
: KS QD JC TD 9C 8D

==
Freecell 1 -> Column 5
--------
Foundations: H-T C-8 D-A S-J
Freecells:      7D
: 3D
: 4D
: 5D
: 6D
: 2D KD QS JH
: KH QC JD TC 9D
: KC QH
: KS QD JC TD 9C 8D

==
Column 4 -> Foundation 0
--------
Foundations: H-J C-8 D-A S-J
Freecells:      7D
: 3D
: 4D
: 5D
: 6D
: 2D KD QS
: KH QC JD TC 9D
: KC QH
: KS QD JC TD 9C 8D

==
Column 4 -> Foundation 3
--------
Foundations: H-J C-8 D-A S-Q
Freecells:      7D
: 3D
: 4D
: 5D
: 6D
: 2D KD
: KH QC JD TC 9D
: KC QH
: KS QD JC TD 9C 8D

==
Column 4 -> Freecell 0
--------
Foundations: H-K C-K D-K S-K
Freecells:
:
:
:
:
:
:
:
:

==
END
EOF

        my $dbm_output = trap_dbm(
            {
                board_fn => samp_board('2freecells-24-mid-with-colons.board'),
            }
        );

        my $output_text = _get($dbm_output);

        $output_text =~ s# +(\n|\z)#$1#g;

        # TEST
        ok(
            scalar(
                       index( $output_text, $needle ) >= 0
                    or index( $output_text, $needle_non_debondt ) >= 0
            ),
            "dbm_fc_solver invocation contains the solution's output."
        );
    }
}

{
SKIP:
    {
        if ( is_without_dbm() || $IS_WIN )
        {
            Test::More::skip( "without the dbm fc_solvers or win32", 1 );
        }

        my $dbm_output = trap_depth_dbm(
            {
                board_fn  => bin_board('981.board'),
                max_iters => 400_000,
            }
        );

        my $output_text = _get($dbm_output);

        # TEST
        like(
            $output_text,
            qr/^Mark\+Sweep Progress - 100000/ms,
            "depth_dbm_fc_solver bug with infinite run is fixed."
        );
    }
}

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2008 Shlomi Fish

=cut
