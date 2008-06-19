package Games::Solitaire::Verify::State;

use warnings;
use strict;

=head1 NAME

Games::Solitaire::Verify::State - a class for Solitaire
states (or positions) of the entire board.

=head1 VERSION

Version 0.01

=cut

our $VERSION = '0.01';

use base 'Games::Solitaire::Verify::Base';

use Games::Solitaire::Verify::Exception;
use Games::Solitaire::Verify::Card;
use Games::Solitaire::Verify::Column;
use Games::Solitaire::Verify::Move;

use List::Util qw(first);

__PACKAGE__->mk_accessors(qw(
    _columns
    _freecells
    _foundations
    _variant
    ));

=head1 SYNOPSIS

    use Games::Solitaire::Verify::State;

    my $board = <<"EOF";
    Foundations: H-6 C-A D-A S-4
    Freecells:  3D  8H  JH  9H
    : 4C 2C 9C 8C QS JD
    : KS QH
    : QC 9S
    : 5C
    : 2D KD TH TC TD 8D 7C 6D 5S 4D 3C
    : 7H JS KH TS KC QD JC
    : 9D 8S
    : 7S 6C 7D 6S 5D
    EOF

    # Initialise a column
    my $board = Games::Solitaire::Verify::State->new(
        {
            string => $board,
            variant => "freecell",
        },
    );

    # Prints 8.
    print $board->num_columns(), "\n";

    # Prints ": QC 9S"
    print $board->get_column(2)->to_string(), "\n"

=head1 FUNCTIONS

=cut

sub _parse_freecell_card
{
    my ($self, $s) = @_;

    return
    (
        ($s eq q{  })
            ? undef()
            : Games::Solitaire::Verify::Card->new(
                {
                    string => $_,
                }
            )
    );
}

sub _assign_freecells_from_strings
{
    my $self = shift;
    my $string_arr = shift;

    $self->_freecells(
        [ map { $self->_parse_freecell_card($_) } @$string_arr]
    );

    return;
}

sub _add_column
{
    my ($self, $col) = @_;

    push @{$self->_columns()}, $col;

    return;
}

sub _from_string
{
    my ($self, $str) = @_;

    my $rank_re = '[0A1-9TJQK]';

    if ($str !~ m{\AFoundations: H-($rank_re) C-($rank_re) D-($rank_re) S-($rank_re) *\n}gms)
    {
        Games::Solitaire::Verify::Exception::Parse::State::Foundations->throw(
            error => "Wrong Foundations",
        );
    }

    {
        my @founds_strings = ($1, $2, $3, $4);
    
        my %founds;
        foreach my $suit (qw(H C D S))
        {
            $founds{$suit} =
                [
                    Games::Solitaire::Verify::Card->calc_rank_with_0(
                        shift(@founds_strings)
                    )
                ];
        }

        $self->_foundations(\%founds);
    }

    my $fc_card_re = '  (..)';

    if ($str !~ m{\GFreecells:$fc_card_re$fc_card_re$fc_card_re$fc_card_re\n}msg)
    {
        Games::Solitaire::Verify::Exception::Parse::State::Freecells->throw(
            error => "Wrong Freecell String",
        );
    }

    {
        my @freecells = ($1, $2, $3, $4);

        $self->_assign_freecells_from_strings(\@freecells);
    }

    foreach my $col_idx (0 .. ($self->num_columns()-1))
    {
        if ($str !~ m{\G(:[^\n]*)\n}msg)
        {
            Games::Solitaire::Verify::Exception::Parse::State::Columns->throw(
                error => "Cannot parse column",
                index => $col_idx,
            );
        }
        my $column_str = $1;

        $self->_add_column(
            Games::Solitaire::Verify::Column->new(
                {
                    string => $column_str,
                }
            )
        );
    }

    return;
}

sub _set_variant
{
    my $self = shift;
    my $variant = shift;

    if ($variant ne "freecell")
    {
        Games::Solitaire::Verify::Exception::Variant::Unknown->throw(
            error => "Unknown/Unsupported Variant",
            variant => $variant,
        );
    }
    $self->_variant($variant);

    return;
}

sub _init
{
    my ($self, $args) = @_;

    # Set the variant
    $self->_set_variant($args->{variant});

    $self->_columns([]);

    if (exists($args->{string}))
    {
        return $self->_from_string($args->{string});
    }
}

=head2 $state->get_freecell($index)

Returns the contents of the freecell No. $index or undef() if it's empty.

=cut

sub get_freecell
{
    my ($self, $index) = @_;

    return $self->_freecells()->[$index];
}

=head2 $state->get_foundation_value($suit, $index)

Returns the foundation value for the suit $suit of the foundations 
No. $index .

=cut

sub get_foundation_value
{
    my ($self, $suit, $idx) = @_;

    return $self->_foundations()->{$suit}->[$idx];
}

=head2 $state->increment_foundation_value($suit, $index)

Increments the foundation value for the suit $suit of the foundations
No. $index .

=cut

sub increment_foundation_value
{
    my ($self, $suit, $idx) = @_;

    $self->_foundations()->{$suit}->[$idx]++;

    return;
}

=head2 $board->num_freecells()

Returns the number of Freecells in the board.

=cut

sub num_freecells
{
    my $self = shift;

    return 4;
}

=head2 $board->num_empty_freecells()

Returns the number of empty Freecells on the board.

=cut

sub num_empty_freecells
{
    my $self = shift;

    my $count = 0;

    foreach my $fc_idx (0 .. ($self->num_freecells()-1) )
    {
        if (!defined($self->get_freecell($fc_idx)))
        {
            $count++;
        }
    }
    return $count;
}

=head2 $board->num_columns()

The number of columns in the board.

=cut

sub num_columns
{
    my $self = shift;

    return 8;
}

=head2 $board->get_column($index)

Gets the column object for column No. $index.

=cut

sub get_column
{
    my $self = shift;
    my $index = shift;

    return $self->_columns->[$index];
}

=head2 $board->num_empty_columns()

Returns the number of completely unoccupied columns in the board.

=cut

sub num_empty_columns
{
    my $self = shift;

    my $count = 0;

    foreach my $idx (0 .. ($self->num_columns()-1) )
    {
        if (! $self->get_column($idx)->len())
        {
            $count++;
        }
    }
    return $count;
}

=head2 $board->clone()

Returns a clone of the board, with all of its element duplicated.
=cut

sub clone
{
    my $self = shift;

    my $copy = Games::Solitaire::Verify::State->new(
        {
            variant => $self->_variant(),
        }
    );

    foreach my $idx (0 .. ($self->num_columns()-1))
    {
        $copy->_add_column(
            $self->get_column($idx)->clone()
        );
    }

    {
        my $founds = $self->_foundations();
        my $founds_copy = +{ 
            map { $_ => [ @{$founds->{$_}} ] } keys(%$founds) 
        };
        $copy->_foundations(
            $founds_copy
        );
    }

    $copy->_freecells(
        [
            map
            { defined($_) ? $_->clone() : $_ }
            @{$self->_freecells()}
        ]
    );

    return $copy;
}

=head2 my $verdict = $board->verify_and_perform_move($move);

Performs $move on the board. If successful, returns 0. Else returns a non-zero
value. See L<Games::Solitaire::Verify::Move> for more information.

=cut

sub verify_and_perform_move
{
    my ($self, $move) = @_;

    if (   ($move->source_type() eq "stack")
        && ($move->dest_type() eq "foundation"))
    {
        my $col_idx = $move->source();
        my $card = $self->get_column($col_idx)->top();

        my $rank = $card->rank();
        my $suit = $card->suit();

        my $f_idx = 
            first 
            { $self->get_foundation_value($suit, $_) == $rank-1 }
            (0 .. 0)
            ;

        if (defined($f_idx))
        {
            $self->get_column($col_idx)->pop();
            $self->increment_foundation_value($suit, $f_idx);
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        die "Cannot handle this move type";
    }
}

=head1 AUTHOR

Shlomi Fish, C<< <shlomif at iglu.org.il> >>

=head1 BUGS

Please report any bugs or feature requests to C<bug-games-solitaire-verifysolution-move at rt.cpan.org>, or through
the web interface at L<http://rt.cpan.org/NoAuth/ReportBug.html?Queue=Games-Solitaire-Verify>.  I will be notified, and then you'll
automatically be notified of progress on your bug as I make changes.

=head1 SUPPORT

You can find documentation for this module with the perldoc command.

    perldoc Games::Solitaire::Verify::Column


You can also look for information at:

=over 4

=item * RT: CPAN's request tracker

L<http://rt.cpan.org/NoAuth/Bugs.html?Dist=Games-Solitaire-Verify>

=item * AnnoCPAN: Annotated CPAN documentation

L<http://annocpan.org/dist/Games-Solitaire-Verify>

=item * CPAN Ratings

L<http://cpanratings.perl.org/d/Games-Solitaire-Verify>

=item * Search CPAN

L<http://search.cpan.org/dist/Games-Solitaire-Verify>

=back


=head1 ACKNOWLEDGEMENTS


=head1 COPYRIGHT & LICENSE

Copyright 2008 Shlomi Fish, all rights reserved.

This program is released under the following license: MIT/X11
( L<http://www.opensource.org/licenses/mit-license.php> ).

=cut

1; # End of Games::Solitaire::Verify::Move
