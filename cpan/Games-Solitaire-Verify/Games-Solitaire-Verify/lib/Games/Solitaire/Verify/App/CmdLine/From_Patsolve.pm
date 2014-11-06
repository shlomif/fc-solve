package Games::Solitaire::Verify::App::CmdLine::From_Patsolve;

use strict;
use warnings;

use autodie;

use parent 'Games::Solitaire::Verify::Base';

use Games::Solitaire::Verify::VariantsMap;
use Games::Solitaire::Verify::Solution;
use Games::Solitaire::Verify::State;
use Games::Solitaire::Verify::Move;

use List::MoreUtils qw(firstidx);

use Getopt::Long qw(GetOptionsFromArray);

__PACKAGE__->mk_acc_ref(
    [
        qw(
            _state
            _filename
            _sol_filename
            _variant_params
            _buffer_ref
        )
    ]
);

sub _init
{
    my ($self, $args) = @_;

    my $argv = $args->{'argv'};

    my $variant_map = Games::Solitaire::Verify::VariantsMap->new();

    my $variant_params = $variant_map->get_variant_by_id("freecell");

    GetOptionsFromArray(
        $argv,
        'g|game|variant=s' => sub {
            my (undef, $game) = @_;

            $variant_params = $variant_map->get_variant_by_id($game);

            if (!defined($variant_params))
            {
                die "Unknown variant '$game'!\n";
            }
        },
        'freecells-num=i' => sub {
            my (undef, $n) = @_;
            $variant_params->num_freecells($n);
        },
        'stacks-num=i' => sub {
            my (undef, $n) = @_;
            $variant_params->num_columns($n);
        },
        'decks-num=i' => sub {
            my (undef, $n) = @_;

            if (! ( ($n == 1) || ($n == 2) ) )
            {
                die "Decks should be 1 or 2.";
            }

            $variant_params->num_decks($n);
        },
        'sequences-are-built-by=s' => sub {
            my (undef, $val) = @_;

            my %seqs_build_by =
            (
                (map { $_ => $_ }
                    (qw(alt_color suit rank))
                ),
                "alternate_color" => "alt_color",
            );

            my $proc_val = $seqs_build_by{$val};

            if (! defined($proc_val))
            {
                die "Unknown sequences-are-built-by '$val'!";
            }

            $variant_params->seqs_build_by($proc_val);
        },
        'empty-stacks-filled-by=s' => sub {
            my (undef, $val) = @_;

            my %empty_stacks_filled_by_map =
            (map { $_ => 1 } (qw(kings any none)));

            if (! exists($empty_stacks_filled_by_map{$val}))
            {
                die "Unknown empty stacks filled by '$val'!";
            }

            $variant_params->empty_stacks_filled_by($val);
        },
        'sequence-move=s' => sub {
            my (undef, $val) = @_;

            my %seq_moves = (map { $_ => 1 } (qw(limited unlimited)));

            if (! exists ($seq_moves{$val}) )
            {
                die "Unknown sequence move '$val'!";
            }

            $variant_params->sequence_move($val);
        },
    )
        or die "Cannot process command line arguments";

    my $filename = shift(@$argv);

    if (!defined($filename))
    {
        $filename = "-";
    }

    my $sol_filename = shift(@$argv);

    if (!defined($sol_filename))
    {
        die "Solution filename not specified.";
    }

    $self->_variant_params($variant_params);
    $self->_filename($filename);
    $self->_sol_filename($sol_filename);

    my $s = '';
    $self->_buffer_ref(\$s);

    return;
}

sub _append
{
    my ($self, $text) = @_;

    ${$self->_buffer_ref} .= $text;

    return;
}

sub _get_buffer
{
    my ($self) = @_;

    return ${$self->_buffer_ref};
}

sub _slurp
{
    my $filename = shift;

    open my $in, '<', $filename
        or die "Cannot open '$filename' for slurping - $!";

    local $/;
    my $contents = <$in>;

    close($in);

    return $contents;
}

sub _read_initial_state
{
    my $self = shift;

    $self->_state(
        Games::Solitaire::Verify::State->new(
            {
                string => scalar(_slurp($self->_filename)),
                variant => 'custom',
                'variant_params' => $self->_variant_params(),
            }
        )
    );

    $self->_append("-=-=-=-=-=-=-=-=-=-=-=-\n\n");

    $self->_out_running_state;

    return;
}

sub _out_running_state
{
    my ($self) = @_;

    $self->_append(
        $self->_state->to_string(). "\n\n====================\n\n"
    );

    return;
}

sub _perform_and_output_move
{
    my ($self, $move_s) = @_;

    $self->_append( "$move_s\n\n");

    $self->_state->verify_and_perform_move(
        Games::Solitaire::Verify::Move->new(
            {
                fcs_string => $move_s,
                game => $self->_state->_variant(),
            },
        )
    );
    $self->_out_running_state;

    return;
}

sub _find_col_card
{
    my ($self, $card_s) = @_;

    return firstidx {
        my $col = $self->_state->get_column($_);
        ($col->len == 0) ? 0 : $col->top->fast_s eq $card_s
    } (0 .. $self->_state->num_columns - 1);
}

sub _find_empty_col
{
    my ($self) = @_;

    return firstidx {
        $self->_state->get_column($_)->len == 0
    } (0 .. $self->_state->num_columns - 1);
}

sub _find_fc_card
{
    my ($self, $card_s) = @_;
    my $dest_fc_idx = firstidx {
        my $card = $self->_state->get_freecell($_);
        defined ($card) ? ($card->fast_s eq $card_s) : 0;
    } (0 .. $self->_state->num_freecells - 1);
}

sub _find_card_src_string
{
    my ($self, $src_card_s) = @_;

    my $src_col_idx = $self->_find_col_card($src_card_s);
    # TODO : try to find a freecell card.
    if ($src_col_idx < 0)
    {
        my $src_fc_idx = $self->_find_fc_card($src_card_s);
        if ($src_fc_idx < 0)
        {
            die "Cannot find card <$src_card_s>.";
        }
        return ("a card", "freecell $src_fc_idx");
    }
    else
    {
        return ("1 cards", "stack $src_col_idx");
    }
}

sub _perform_move
{
    my ($self, $move_line) = @_;

    if (my ($src_card_s) = $move_line =~ /\A(.[HCDS]) to temp\z/)
    {
        my $src_col_idx = $self->_find_col_card($src_card_s);
        if ($src_col_idx < 0)
        {
            die "Cannot find card.";
        }

        my $dest_fc_idx = firstidx {
            ! defined ($self->_state->get_freecell($_))
        } (0 .. $self->_state->num_freecells - 1);

        if ($dest_fc_idx < 0)
        {
            die "No empty freecell.";
        }

        $self->_perform_and_output_move(
            sprintf("Move a card from stack %d to freecell %d", $src_col_idx, $dest_fc_idx,),
        );

    }
    elsif (($src_card_s) = $move_line =~ /\A(.[HCDS]) out\z/)
    {
        my @src_s = $self->_find_card_src_string($src_card_s);
        $self->_perform_and_output_move(
            sprintf("Move a card from %s to the foundations", $src_s[1]),
        );
    }
    elsif (($src_card_s) = $move_line =~ /\A(.[HCDS]) to empty pile\z/)
    {
        my $dest_col_idx = $self->_find_empty_col;
        if ($dest_col_idx < 0)
        {
            die "Cannot find empty col.";
        }
        my @src_s = $self->_find_card_src_string($src_card_s);

        $self->_perform_and_output_move(
            sprintf("Move %s from %s to stack %d", @src_s, $dest_col_idx),
        );
    }
    elsif (($src_card_s, (my $dest_card_s)) = $move_line =~ /\A(.[HCDS]) to (.[HCDS])\z/)
    {
        my $dest_col_idx = $self->_find_col_card($dest_card_s);
        if ($dest_col_idx < 0)
        {
            die "Cannot find card <$dest_card_s>.";
        }

        my @src_s = $self->_find_card_src_string($src_card_s);
        $self->_perform_and_output_move(
            sprintf("Move %s from %s to stack %d",
                @src_s, $dest_col_idx
            )
        );
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

    while (my $l = <$in_fh>)
    {
        chomp($l);
        $self->_perform_move($l);
    }

    close($in_fh);

    $self->_append("This game is solveable.\n");

    return;
}

sub run
{
    my ($self) = @_;

    $self->_process_main;

    print $self->_get_buffer;

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

=head1 COPYRIGHT & LICENSE

Copyright 2012 by Shlomi Fish

This program is distributed under the MIT (X11) License:
L<http://www.opensource.org/licenses/mit-license.php>

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
