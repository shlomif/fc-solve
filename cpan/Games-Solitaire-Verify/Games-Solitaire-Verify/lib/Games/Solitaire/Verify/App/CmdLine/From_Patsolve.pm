package Games::Solitaire::Verify::App::CmdLine::From_Patsolve;

use strict;
use warnings;
use autodie;

use parent 'Games::Solitaire::Verify::FromOtherSolversBase';

use Games::Solitaire::Verify::VariantsMap      ();
use Games::Solitaire::Verify::Solution         ();
use Games::Solitaire::Verify::State::LaxParser ();
use Games::Solitaire::Verify::Move             ();

use List::MoreUtils qw(firstidx);

__PACKAGE__->mk_acc_ref(
    [
        qw(
            _st
            _filename
            _sol_filename
            _variant_params
            _buffer_ref
            )
    ]
);

sub _perform_move
{
    my ( $self, $move_line ) = @_;

    if ( my ($src_card_s) = $move_line =~ /\A(.[HCDS]) to temp\z/ )
    {
        my $src_col_idx = $self->_find_col_card($src_card_s);
        if ( $src_col_idx < 0 )
        {
            die "Cannot find card.";
        }

        my $dest_fc_idx = firstidx
        {
            !defined( $self->_st->get_freecell($_) )
        }
        ( 0 .. $self->_st->num_freecells - 1 );

        if ( $dest_fc_idx < 0 )
        {
            die "No empty freecell.";
        }

        $self->_perform_and_output_move(
            sprintf(
                "Move a card from stack %d to freecell %d",
                $src_col_idx, $dest_fc_idx,
            ),
        );

    }
    elsif ( ($src_card_s) = $move_line =~ /\A(.[HCDS]) out\z/ )
    {
        my @src_s = $self->_find_card_src_string($src_card_s);
        $self->_perform_and_output_move(
            sprintf( "Move a card from %s to the foundations", $src_s[1] ),
        );
    }
    elsif ( ($src_card_s) = $move_line =~ /\A(.[HCDS]) to empty pile\z/ )
    {
        my $dest_col_idx = $self->_find_empty_col;
        if ( $dest_col_idx < 0 )
        {
            die "Cannot find empty col.";
        }
        my @src_s = $self->_find_card_src_string($src_card_s);

        $self->_perform_and_output_move(
            sprintf( "Move %s from %s to stack %d", @src_s, $dest_col_idx ),
        );
    }
    elsif ( ( $src_card_s, ( my $dest_card_s ) ) =
        $move_line =~ /\A(.[HCDS]) to (.[HCDS])\z/ )
    {
        my $dest_col_idx = $self->_find_col_card($dest_card_s);
        if ( $dest_col_idx < 0 )
        {
            die "Cannot find card <$dest_card_s>.";
        }

        my @src_s = $self->_find_card_src_string($src_card_s);
        $self->_perform_and_output_move(
            sprintf( "Move %s from %s to stack %d", @src_s, $dest_col_idx ) );
    }
    else
    {
        die "Unrecognised move_line <$move_line>";
    }
}

sub _process_main
{
    my $self = shift;

    $self->_read_initial_state;

    open my $in_fh, '<', $self->_sol_filename;

    while ( my $l = <$in_fh> )
    {
        chomp($l);
        $self->_perform_move($l);
    }

    close($in_fh);

    $self->_append("This game is solveable.\n");

    return;
}

1;

=head1 NAME

Games::Solitaire::Verify::App::CmdLine::From_Patsolve - a modulino for
converting from patsolve solutions to fc-solve ones.

=head1 SYNOPSIS

    $ perl -MGames::Solitaire::Verify::App::CmdLine::From_Patsolve -e 'Games::Solitaire::Verify::App::CmdLine::From_Patsolve->new({argv => \@ARGV})->run()' -- [ARGS]

=head1 DESCRIPTION

This is a a modulino for
converting from patsolve solutions to fc-solve ones.

=head1 METHODS

=head2 run()

Actually execute the command-line application.

=cut
