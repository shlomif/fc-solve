package FC_Solve::GetOutput;

use strict;
use warnings;

use parent 'Games::Solitaire::Verify::Base';

use Carp qw/confess/;
use String::ShellQuote qw/shell_quote/;

use FC_Solve::Paths qw( $FC_SOLVE_EXE $IS_WIN $MAKE_PYSOL );

__PACKAGE__->mk_acc_ref(
    [
        qw(
            board
            deal
            is_custom
            msdeals
            pysolfc_deals
            theme
            variant
            variant_s
            )
    ]
);

sub _init
{
    my ( $self, $args ) = @_;

    my $board = $self->board( $args->{board} );
    my $deal  = $self->deal( $args->{deal} );

    $self->theme( $args->{theme} || [ "-l", "gi" ] );

    if ( !defined($board) )
    {
        if ( !defined($deal) )
        {
            confess "Neither Deal nor board are specified";
        }
        if ( $deal !~ m{\A[1-9][0-9]*\z} )
        {
            confess "Invalid deal $deal";
        }
    }

    my $variant = $self->variant( $args->{variant} || "freecell" );
    $self->variant_s(
        $self->is_custom( scalar( $variant eq "custom" ) )
        ? ""
        : "-g $variant"
    );

    $self->msdeals( $args->{msdeals} );
    $self->pysolfc_deals( $args->{pysolfc_deals} );

    return;
}

sub board_gen_prefix
{
    my ($self) = @_;

    my $deal = $self->deal;

    return (
        $self->board ? ""
        : (
            (
                $self->msdeals ? "pi-make-microsoft-freecell-board -t $deal | "
                : (       "$MAKE_PYSOL -t"
                        . ( $self->pysolfc_deals ? " -F " : "" )
                        . " $deal @{[$self->variant]} | " )
            )
        )
    );
}

sub _win_quote
{
    my $ret = shell_quote(@_);

    if ($IS_WIN)
    {
        $ret =~ tr#'#"#;
    }

    return $ret;
}

sub fc_solve_params_suffix
{
    my ($self) = @_;

    my $board = $self->board;

    return
          $self->variant_s . ' '
        . _win_quote( @{ $self->theme } )
        . " -p -t -sam "
        . ( $board ? _win_quote($board) : "" );
}

sub calc_cmd_line
{
    my ($self) = @_;

    my $cmd_line =
        (     $self->board_gen_prefix()
            . " $FC_SOLVE_EXE "
            . $self->fc_solve_params_suffix() );

    return +{ cmd_line => $cmd_line, };
}

sub open_cmd_line
{
    my ($self) = @_;

    my $cmd_line_args = $self->calc_cmd_line;
    open my $fc_solve_output, "$cmd_line_args->{cmd_line} |"
        or Carp::confess "Error! Could not open the fc-solve pipeline";
    $cmd_line_args->{fh} = $fc_solve_output;

    return $cmd_line_args;
}

1;

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2000 Shlomi Fish

=cut
