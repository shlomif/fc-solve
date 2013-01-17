package Games::Solitaire::Verify::VariantsMap;

use strict;
use warnings;

require Games::Solitaire::Verify::VariantParams;

use base 'Games::Solitaire::Verify::Base';

=head1 NAME

Games::Solitaire::Verify::VariantsMap - a mapping from the variants to
their parameters.

=head1 VERSION

Version 0.03

=cut

our $VERSION = '0.1103';

sub _init
{
    return 0;
}

my %variants_map =
(
    "bakers_dozen" =>
        Games::Solitaire::Verify::VariantParams->new(
            {
                'num_decks' => 1,
                'num_columns' => 13,
                'num_freecells' => 0,
                'sequence_move' => "limited",
                'seq_build_by' => "rank",
                'empty_stacks_filled_by' => "none",
            }
        ),
    "bakers_game" =>
        Games::Solitaire::Verify::VariantParams->new(
            {
                'num_decks' => 1,
                'num_columns' => 8,
                'num_freecells' => 4,
                'sequence_move' => "limited",
                'seq_build_by' => "suit",
                'empty_stacks_filled_by' => "any",
            }
        ),
    "beleaguered_castle" =>
        Games::Solitaire::Verify::VariantParams->new(
            {
                'num_decks' => 1,
                'num_columns' => 8,
                'num_freecells' => 0,
                'sequence_move' => "limited",
                'seq_build_by' => "rank",
                'empty_stacks_filled_by' => "any",
            }
        ),
    "cruel" =>
        Games::Solitaire::Verify::VariantParams->new(
            {
                'num_decks' => 1,
                'num_columns' => 12,
                'num_freecells' => 0,
                'sequence_move' => "limited",
                'seq_build_by' => "suit",
                'empty_stacks_filled_by' => "none",
            }
        ),
    "der_katzenschwanz" =>
        Games::Solitaire::Verify::VariantParams->new(
            {
                'num_decks' => 2,
                'num_columns' => 9,
                'num_freecells' => 8,
                'sequence_move' => "unlimited",
                'seq_build_by' => "alt_color",
                'empty_stacks_filled_by' => "none",
            }
        ),
    "die_schlange" =>
        Games::Solitaire::Verify::VariantParams->new(
            {
                'num_decks' => 2,
                'num_columns' => 9,
                'num_freecells' => 8,
                'sequence_move' => "limited",
                'seq_build_by' => "alt_color",
                'empty_stacks_filled_by' => "none",
            }
        ),
    "eight_off" =>
        Games::Solitaire::Verify::VariantParams->new(
            {
                'num_decks' => 1,
                'num_columns' => 8,
                'num_freecells' => 8,
                'sequence_move' => "limited",
                'seq_build_by' => "suit",
                'empty_stacks_filled_by' => "kings",
            }
        ),
    "fan" =>
        Games::Solitaire::Verify::VariantParams->new(
            {
                'num_decks' => 1,
                'num_columns' => 18,
                'num_freecells' => 0,
                'sequence_move' => "limited",
                'seq_build_by' => "suit",
                'empty_stacks_filled_by' => "kings",
            }
        ),
    "forecell" =>
        Games::Solitaire::Verify::VariantParams->new(
            {
                'num_decks' => 1,
                'num_columns' => 8,
                'num_freecells' => 4,
                'sequence_move' => "limited",
                'seq_build_by' => "alt_color",
                'empty_stacks_filled_by' => "kings",
            }
        ),
    "freecell" =>
        Games::Solitaire::Verify::VariantParams->new(
            {
                'num_decks' => 1,
                'num_columns' => 8,
                'num_freecells' => 4,
                'sequence_move' => "limited",
                'seq_build_by' => "alt_color",
                'empty_stacks_filled_by' => "any",
            }
        ),
    "good_measure" =>
        Games::Solitaire::Verify::VariantParams->new(
            {
                'num_decks' => 1,
                'num_columns' => 10,
                'num_freecells' => 0,
                'sequence_move' => "limited",
                'seq_build_by' => "rank",
                'empty_stacks_filled_by' => "none",
            }
        ),
    "kings_only_bakers_game" =>
        Games::Solitaire::Verify::VariantParams->new(
            {
                'num_decks' => 1,
                'num_columns' => 8,
                'num_freecells' => 4,
                'sequence_move' => "limited",
                'seq_build_by' => "suit",
                'empty_stacks_filled_by' => "kings",
            }
        ),
    "relaxed_freecell" =>
        Games::Solitaire::Verify::VariantParams->new(
            {
                'num_decks' => 1,
                'num_columns' => 8,
                'num_freecells' => 4,
                'sequence_move' => "unlimited",
                'seq_build_by' => "alt_color",
                'empty_stacks_filled_by' => "any",
            }
        ),
    "relaxed_seahaven_towers" =>
        Games::Solitaire::Verify::VariantParams->new(
            {
                'num_decks' => 1,
                'num_columns' => 10,
                'num_freecells' => 4,
                'sequence_move' => "unlimited",
                'seq_build_by' => "suit",
                'empty_stacks_filled_by' => "kings",
            }
        ),
    "seahaven_towers" =>
        Games::Solitaire::Verify::VariantParams->new(
            {
                'num_decks' => 1,
                'num_columns' => 10,
                'num_freecells' => 4,
                'sequence_move' => "limited",
                'seq_build_by' => "suit",
                'empty_stacks_filled_by' => "kings",
            }
        ),
    "simple_simon" =>
        Games::Solitaire::Verify::VariantParams->new(
            {
                'num_decks' => 1,
                'num_columns' => 10,
                'num_freecells' => 0,
                'sequence_move' => "limited",
                'seq_build_by' => "suit",
                'empty_stacks_filled_by' => "any",
                'rules' => "simple_simon",
            }
        ),
);

=head1 FUNCTIONS

=head2 $self->calc_variants_map()

Returns the variants map - a hash reference mapping the variant ID to its
parameters.

=cut

sub calc_variants_map
{
    my $self = shift;

    return \%variants_map;
}

=head2 my $variant_params = $self->get_variant_by_id($id)

Returns the variant from its ID or undef if it does not exist.

=cut

sub get_variant_by_id
{
    my $self = shift;
    my $id = shift;

    my $map = $self->calc_variants_map();

    if (!exists($map->{$id}))
    {
        return;
    }
    else
    {
        return $map->{$id}->clone();
    }
}

=head1 PARAMETERS

=head2 Variants IDs

This is a list of the available variant IDs.

=over 4

=item * bakers_dozen

=item * bakers_game

=item * beleaguered_castle

=item * cruel

=item * der_katzenschwanz

=item * die_schlange

=item * eight_off

=item * fan

=item * forecell

=item * freecell

=item * good_measure

=item * kings_only_bakers_game

=item * relaxed_freecell

=item * relaxed_seahaven_towers

=item * seahaven_towers

=item * simple_simon

=back

=head1 SEE ALSO

L<Games::Solitaire::Verify::VariantParams> .

=head1 AUTHOR

Shlomi Fish, C<< <shlomif at iglu.org.il> >>

=head1 BUGS

Please report any bugs or feature requests to C<bug-games-solitaire-verifysolution-move at rt.cpan.org>, or through
the web interface at L<http://rt.cpan.org/NoAuth/ReportBug.html?Queue=Games-Solitaire-Verify>.  I will be notified, and then you'll
automatically be notified of progress on your bug as I make changes.

=head1 SUPPORT

You can find documentation for this module with the perldoc command.

    perldoc Games::Solitaire::Verify::VariantsMap


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

1;
