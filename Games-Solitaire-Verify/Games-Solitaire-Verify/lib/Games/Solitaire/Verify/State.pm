package Games::Solitaire::Verify::State;

use warnings;
use strict;

=head1 NAME

Games::Solitaire::Verify::State - a class for Solitaire
states (or positions) of the entire board.

=head1 VERSION

Version 0.0101

=cut

our $VERSION = '0.0901';

use base 'Games::Solitaire::Verify::Base';

use Games::Solitaire::Verify::Exception;
use Games::Solitaire::Verify::Card;
use Games::Solitaire::Verify::Column;
use Games::Solitaire::Verify::Move;
use Games::Solitaire::Verify::Freecells;
use Games::Solitaire::Verify::Foundations;
use Games::Solitaire::Verify::VariantParams;
use Games::Solitaire::Verify::VariantsMap;

use List::Util qw(first);
use POSIX qw();

__PACKAGE__->mk_acc_ref([qw(
    _columns
    _freecells
    _foundations
    _variant
    _variant_params
    _temp_move
    )]);

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


sub _assign_freecells_from_string
{
    my $self = shift;
    my $string = shift;

    $self->_freecells(
        Games::Solitaire::Verify::Freecells->new(
            {
                count => $self->num_freecells(),
                string => $string,
            }
        )
    );

    return;
}

=head2 $self->add_column($columns)

Adds a new column of cards that should be an
L<Games::Solitaire::Verify::Column> object.

=cut

sub add_column
{
    my ($self, $col) = @_;

    push @{$self->_columns()}, $col;

    return;
}

sub _get_suits_seq
{
    my $class = shift;

    return Games::Solitaire::Verify::Card->get_suits_seq();
}

sub _from_string
{
    my ($self, $str) = @_;

    my $rank_re = '[0A1-9TJQK]';

    if ($str !~ m{\A(Foundations:[^\n]*)\n}gms)
    {
        Games::Solitaire::Verify::Exception::Parse::State::Foundations->throw(
            error => "Wrong Foundations",
        );
    }
    my $founds_s = $1;

    $self->_foundations(
        Games::Solitaire::Verify::Foundations->new(
            {
               num_decks => $self->num_decks(),
                string => $founds_s,
            }
        )
    );

    if ($str !~ m{\G(Freecells:[^\n]*)\n}gms)
    {
        Games::Solitaire::Verify::Exception::Parse::State::Freecells->throw(
            error => "Wrong Freecell String",
        );
    }
    $self->_assign_freecells_from_string($1);

    foreach my $col_idx (0 .. ($self->num_columns()-1))
    {
        if ($str !~ m{\G(:[^\n]*)\n}msg)
        {
            Games::Solitaire::Verify::Exception::Parse::State::Column->throw(
                error => "Cannot parse column",
                index => $col_idx,
            );
        }
        my $column_str = $1;

        $self->add_column(
            Games::Solitaire::Verify::Column->new(
                {
                    string => $column_str,
                }
            )
        );
    }

    return;
}

sub _fill_non_custom_variant
{
    my $self = shift;
    my $variant = shift;

    my $variants_map = Games::Solitaire::Verify::VariantsMap->new();

    my $params = $variants_map->get_variant_by_id($variant);

    if (!defined($params))
    {
        Games::Solitaire::Verify::Exception::Variant::Unknown->throw(
            error => "Unknown/Unsupported Variant",
            variant => $variant,
        );
    }
    $self->_variant_params($params);
    $self->_variant($variant);

    return;
}

sub _set_variant
{
    my $self = shift;
    my $args = shift;

    my $variant = $args->{variant};

    if ($variant eq "custom")
    {
        $self->_variant($variant);
        $self->_variant_params($args->{variant_params});
    }
    else
    {
        $self->_fill_non_custom_variant($variant);
    }

    return;
}

sub _init
{
    my ($self, $args) = @_;

    # Set the variant
    $self->_set_variant($args);

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

    return $self->_freecells()->cell($index);
}

=head2 $state->set_freecell($index, $card)

Assigns $card to the contents of the freecell No. $index .

=cut

sub set_freecell
{
    my ($self, $index, $card) = @_;

    return $self->_freecells->assign($index, $card);
}

=head2 $state->get_foundation_value($suit, $index)

Returns the foundation value for the suit $suit of the foundations 
No. $index .

=cut

sub get_foundation_value
{
    my ($self, $suit, $idx) = @_;

    return $self->_foundations()->value($suit, $idx);
}

=head2 $state->increment_foundation_value($suit, $index)

Increments the foundation value for the suit $suit of the foundations
No. $index .

=cut

sub increment_foundation_value
{
    my ($self, $suit, $idx) = @_;

    $self->_foundations()->increment($suit, $idx);

    return;
}

=head2 $board->num_decks()

Returns the number of decks that the variant has. Useful when querying
the foundations.

=cut

sub num_decks
{
    my $self = shift;

    return $self->_variant_params->num_decks();
}

=head2 $board->num_freecells()

Returns the number of Freecells in the board.

=cut

sub num_freecells
{
    my $self = shift;

    return $self->_variant_params->num_freecells();
}

=head2 $board->num_empty_freecells()

Returns the number of empty Freecells on the board.

=cut

sub num_empty_freecells
{
    my $self = shift;

    return $self->_freecells->num_empty();
}

=head2 $board->num_columns()

The number of columns in the board.

=cut

sub num_columns
{
    my $self = shift;

    return $self->_variant_params->num_columns();
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
        $copy->add_column(
            $self->get_column($idx)->clone()
        );
    }

    $copy->_foundations(
        $self->_foundations()->clone()
    );

    $copy->_freecells(
        $self->_freecells()->clone()
    );

    return $copy;
}

=head2 my $verdict = $board->verify_and_perform_move($move);

Performs $move on the board. If successful, returns 0. Else returns a non-zero
value. See L<Games::Solitaire::Verify::Move> for more information.

=cut

# Returns 0 on success, else the error.
sub _can_put_into_empty_column
{
    my ($self, $card) = @_;

    if ($self->_variant_params->empty_stacks_filled_by() eq "kings")
    {
        if ($card->rank() != 13)
        {
            return Games::Solitaire::Verify::Exception::Move::Dest::Col::OnlyKingsCanFillEmpty->new(
                error => "Non-king on an empty stack",
                move => $self->_temp_move(),
            );
        }
    }
    return 0;
}

sub _is_matching_color
{
    my ($self, $parent, $child) = @_;

    my $rules = $self->_variant_params()->rules();
    my $sbb = $self->_variant_params()->seq_build_by();

    my $verdict = 
    (
          ($rules eq "simple_simon")
        ? 0
        : ($sbb eq "alt_color")
        ? ($parent->color() eq $child->color())
        : ($sbb eq "suit")
        ? ($parent->suit() ne $child->suit())
        : 0
    );

    if ($verdict)
    {
        return Games::Solitaire::Verify::Exception::Move::Dest::Col::NonMatchSuits->new(
            seq_build_by => $sbb,
            move => $self->_temp_move(),
        );
    }
    
    return 0;
}

sub _can_put_on_top
{
    my ($self, $parent, $child) = @_;

    if ($parent->rank() != $child->rank()+1)
    {
        return Games::Solitaire::Verify::Exception::Move::Dest::Col::RankMismatch->new(
            error => "Rank mismatch between parent and child cards",
            move => $self->_temp_move(),
        );
    }
    
    if (my $ret = $self->_is_matching_color($parent, $child) )
    {
        return $ret;
    }
    
    return 0;
}

sub _can_put_on_column
{
    my ($self, $col_idx, $card) = @_;

    return
        (($self->get_column($col_idx)->len() == 0)
            ? $self->_can_put_into_empty_column($card)
            : $self->_can_put_on_top(
                $self->get_column($col_idx)->top(),
                $card
              )
        );
}

sub _calc_freecell_max_seq_move
{
    my ($self, $args) = @_;
    my $to_empty = (defined($args->{to_empty}) ? $args->{to_empty} : 0);

    return (($self->num_empty_freecells()+1) << ($self->num_empty_columns()-$to_empty))
}

sub _calc_empty_stacks_filled_by_any_card_max_seq_move
{
    my ($self, $args) = @_;

    return $self->_calc_freecell_max_seq_move($args);
}

sub _calc_max_sequence_move
{
    my ($self, $args) = @_;

    return
        +($self->_variant_params->sequence_move() eq "unlimited")
            ? POSIX::INT_MAX()
            : ($self->_variant_params->empty_stacks_filled_by() eq "any")
            ? $self->_calc_empty_stacks_filled_by_any_card_max_seq_move($args)
            : ($self->num_empty_freecells() + 1)
        ;
}

sub _is_sequence_in_column
{
    my ($self, $source_idx, $num_cards, $num_seq_components_ref) = @_;

    my $col = $self->get_column($source_idx);
    my $len = $col->len();

    my $rules = $self->_variant_params()->rules();

    my $num_comps = 1;

    foreach my $card_idx (0 .. ($num_cards-2))
    {
        my $parent = $col->pos($len-1-$card_idx-1);
        my $child = $col->pos($len-1-$card_idx);

        if ($self->_can_put_on_top($parent, $child))
        {
            return
                Games::Solitaire::Verify::Exception::Move::Src::Col::NonSequence->new
                (
                    move => $self->_temp_move(),
                    pos => $card_idx,
                )
                ;
        }
        
        $num_comps +=
        (
              ($rules eq "simple_simon")
            ? (($parent->suit() ne $child->suit()) ? 1 : 0)
            : 1
        );
    }

    ${$num_seq_components_ref} = $num_comps;

    return 0;
}

=head2 $self->clear_freecell($index)

Clears/empties the freecell at position $pos .

=cut

sub clear_freecell
{
    my ($self, $index) = @_;

    return $self->_freecells->clear($index);
}

sub verify_and_perform_move
{
    my ($self, $move) = @_;

    $self->_temp_move($move);

    my $ret = $self->_verify_and_perform_move_main();

    $self->_temp_move(undef());

    return $ret;
}

sub _perform_move__stack_to_foundation
{
    my $self = shift;

    my $move = $self->_temp_move();

    my $col_idx = $move->source();
    my $card = $self->get_column($col_idx)->top();

    my $rank = $card->rank();
    my $suit = $card->suit();

    my $f_idx = 
        first 
        { $self->get_foundation_value($suit, $_) == $rank-1 }
        (0 .. ($self->num_decks()-1))
        ;

    if (defined($f_idx))
    {
        $self->get_column($col_idx)->pop();
        $self->increment_foundation_value($suit, $f_idx);
        return 0;
    }
    else
    {
        return
            Games::Solitaire::Verify::Exception::Move::Dest::Foundation->new(
                move => $move
            );
    }
}

sub _perform_move__stack_seq_to_foundation
{
    my $self = shift;

    my $move = $self->_temp_move();

    my $rules = $self->_variant_params()->rules();

    if ($rules ne "simple_simon")
    {
        return Games::Solitaire::Verify::Exception::Move::Variant::Unsupported
            ->new(
                move => $move
            );
    }

    my $col_idx = $move->source();

    my $num_seq_components;
    my $verdict =
        $self->_is_sequence_in_column(
            $col_idx,
            13,
            \$num_seq_components,
        );

    if ($verdict)
    {
        return $verdict;
    }

    if ($num_seq_components != 1)
    {
        return Games::Solitaire::Verify::Exception::Move::Src::Col::NotTrueSeq->new(
            move => $move
        );
    }
    
    my $card = $self->get_column($col_idx)->top();

    my $suit = $card->suit();

    my $f_idx =
        first
        { $self->get_foundation_value($suit, $_) == 0 }
        (0 .. ($self->num_decks()-1))
        ;

    if (defined($f_idx))
    {
        foreach my $card_idx (1 .. 13)
        {
            $self->get_column($col_idx)->pop();
            $self->increment_foundation_value($suit, $f_idx);
        }
        return 0;
    }
    else
    {
        return
            Games::Solitaire::Verify::Exception::Move::Dest::Foundation->new(
                move => $move
            );
    }
}

sub _perform_move__stack_to_freecell
{
    my $self = shift;
    my $move = $self->_temp_move();

    my $col_idx = $move->source();
    my $fc_idx = $move->dest();
    
    if (! $self->get_column($col_idx)->len())
    {
        return
            Games::Solitaire::Verify::Exception::Move::Src::Col::NoCards->new(
                move => $move,
            );
    }

    if (defined($self->get_freecell($fc_idx)))
    {
        return
            Games::Solitaire::Verify::Exception::Move::Dest::Freecell->new(
                move => $move,
            );
    }

    $self->set_freecell($fc_idx, $self->get_column($col_idx)->pop());

    return 0;
}

sub _perform_move__stack_to_stack
{
    my $self = shift;
    my $move = $self->_temp_move();

    my $source = $move->source();
    my $dest = $move->dest();
    my $num_cards = $move->num_cards();

    my $source_len = $self->get_column($source)->len() ;

    if ($source_len < $num_cards)
    {
        return
            Games::Solitaire::Verify::Exception::Move::Src::Col::NotEnoughCards->new(
                move => $move,
            );
    }

    my $num_seq_components;
    if (my $verdict = $self->_is_sequence_in_column(
            $source,
            $num_cards,
            \$num_seq_components,
        ))
    {
        return $verdict;
    }

    if (my $verdict = $self->_can_put_on_column(
            $dest, 
            $self->get_column($source)->pos($source_len-$num_cards)
        ))
    {
        return $verdict;
    }

    # Now let's see if we have enough resources
    # to move the cards.

    if ($num_seq_components > $self->_calc_max_sequence_move(
            {
                to_empty => ($self->get_column($dest)->len() == 0),
            }
            ))
    {
        return
            Games::Solitaire::Verify::Exception::Move::NotEnoughEmpties->new(
                move => $move,
            );
    }

    # Now let's actually move them.
    my @cards;
    foreach my $i (1 .. $num_cards)
    {
        push @cards, $self->get_column($source)->pop();
    }
    $self->get_column($dest)->append(
        Games::Solitaire::Verify::Column->new(
            {
                cards => [reverse @cards],
            }
        )
    );

    return 0;
}

sub _perform_move__freecell_to_foundation
{
    my $self = shift;
    my $move = $self->_temp_move();

    my $fc_idx = $move->source();
    my $card = $self->get_freecell($fc_idx);

    if (!defined($card))
    {
        return
            Games::Solitaire::Verify::Exception::Move::Src::Freecell::Empty->new(
                move => $move,
            );
    }

    my $rank = $card->rank();
    my $suit = $card->suit();

    my $f_idx =
        first
        { $self->get_foundation_value($suit, $_) == $rank-1 }
        (0 .. ($self->num_decks()-1))
        ;

    if (defined($f_idx))
    {
        $self->clear_freecell($fc_idx);
        $self->increment_foundation_value($suit, $f_idx);
        return 0;
    }
    else
    {
        return
            Games::Solitaire::Verify::Exception::Move::Dest::Foundation->new(
                move => $move
            );
    }
}

sub _perform_move__freecell_to_stack
{
    my $self = shift;
    my $move = $self->_temp_move();

    my $fc_idx = $move->source();
    my $col_idx = $move->dest();

    my $card = $self->get_freecell($fc_idx);

    if (!defined($card))
    {
        return Games::Solitaire::Verify::Exception::Move::Src::Freecell::Empty->new(
            move => $move,
        );
    }

    if (my $verdict = $self->_can_put_on_column($col_idx, $card))
    {
        return $verdict;
    }

    $self->get_column($col_idx)->push($card);
    $self->clear_freecell($fc_idx);
    
    return 0;
}

sub _verify_and_perform_move_main
{
    my $self = shift;

    my $move = $self->_temp_move();

    my $src = $move->source_type();
    my $dest = $move->dest_type();

    my $method = $self->can("_perform_move__${src}_to_${dest}");

    if ($method)
    {
        return $method->($self);
    }
    else
    {
        die "Cannot handle this move type";
    }
}

sub _stringify_freecells
{
    my $self = shift;

    return $self->_freecells()->to_string();
}


sub _stringify_foundations
{
    my $self = shift;

    return $self->_foundations()->to_string();
}

=head2 $self->to_string()

Stringifies the board into the Freecell Solver solution display notation.

=cut

sub to_string
{
    my $self = shift;

    return join("", 
        map { "$_\n" }
        (
            $self->_stringify_foundations(),
            $self->_stringify_freecells(),
            (
                map { $self->get_column($_)->to_string() } 
                (0 .. ($self->num_columns()-1))
            )
        )
    );
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

Copyright 2008 Shlomi Fish.

This program is released under the following license: MIT/X11
( L<http://www.opensource.org/licenses/mit-license.php> ).

=cut

1; # End of Games::Solitaire::Verify::Move
