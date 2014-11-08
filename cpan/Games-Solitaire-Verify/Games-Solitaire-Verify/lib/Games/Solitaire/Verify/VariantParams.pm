package Games::Solitaire::Verify::VariantParams;

use warnings;
use strict;

=head1 NAME

Games::Solitaire::Verify::VariantParams - a class for holding
the parameters of the variant.

=cut

our $VERSION = '0.1300';

use parent 'Games::Solitaire::Verify::Base';

use Games::Solitaire::Verify::Exception;

__PACKAGE__->mk_acc_ref([qw(
    empty_stacks_filled_by
    num_columns
    num_decks
    num_freecells
    rules
    seq_build_by
    sequence_move
    )]);

=head1 SYNOPSIS

    use Games::Solitaire::Verify::VariantParams;

    my $freecell_params =
        Games::Solitaire::Verify::VariantParams->new(
            {
                seq_build_by => "alt_color",
            },
        );


=head1 METHODS

=cut

my %seqs_build_by = (map { $_ => 1 } (qw(alt_color suit rank)));
my %empty_stacks_filled_by_map = (map { $_ => 1 } (qw(kings any none)));
my %seq_moves = (map { $_ => 1 } (qw(limited unlimited)));
my %rules_collection = (map { $_ => 1 } (qw(freecell simple_simon)));

sub _init
{
    my ($self, $args) = @_;

    # Set the variant
    #

    {
        my $seq_build_by = $args->{seq_build_by};

        if (!exists($seqs_build_by{$seq_build_by}))
        {
            Games::Solitaire::Verify::Exception::VariantParams::Param::SeqBuildBy->throw(
                    error => "Unrecognised seq_build_by",
                    value => $seq_build_by,
            );
        }
        $self->seq_build_by($seq_build_by);
    }

    {
        my $esf = $args->{empty_stacks_filled_by};

        if (!exists($empty_stacks_filled_by_map{$esf}))
        {
            Games::Solitaire::Verify::Exception::VariantParams::Param::EmptyStacksFill->throw(
                    error => "Unrecognised empty_stacks_filled_by",
                    value => $esf,
            );
        }

        $self->empty_stacks_filled_by($esf);
    }

    {
        my $num_decks = $args->{num_decks};

        if (! (($num_decks == 1) || ($num_decks == 2)) )
        {
            Games::Solitaire::Verify::Exception::VariantParams::Param::NumDecks->throw(
                    error => "Wrong Number of Decks",
                    value => $num_decks,
            );
        }
        $self->num_decks($num_decks);
    }

    {
        my $num_columns = $args->{num_columns};

        if (($num_columns =~ /\D/)
                ||
            ($num_columns == 0))
        {
            Games::Solitaire::Verify::Exception::VariantParams::Param::Stacks->throw(
                    error => "num_columns is not a number",
                    value => $num_columns,
            );
        }
        $self->num_columns($num_columns)
    }

    {
        my $num_freecells = $args->{num_freecells};

        if ($num_freecells =~ /\D/)
        {
            Games::Solitaire::Verify::Exception::VariantParams::Param::Freecells->throw(
                    error => "num_freecells is not a number",
                    value => $num_freecells,
            );
        }
        $self->num_freecells($num_freecells);
    }

    {
        my $seq_move = $args->{sequence_move};

        if (!exists($seq_moves{$seq_move}))
        {
            Games::Solitaire::Verify::Exception::VariantParams::Param::SeqMove->throw(
                    error => "Unrecognised sequence_move",
                    value => $seq_move,
            );
        }

        $self->sequence_move($seq_move);
    }

    {
        my $rules = $args->{rules} || "freecell";

        if (!exists($rules_collection{$rules}))
        {
            Games::Solitaire::Verify::Exception::VariantParams::Param::Rules->throw(
                    error => "Unrecognised rules",
                    value => $rules,
            );
        }
        $self->rules($rules);
    }

    return 0;
}

=head2 $variant_params->empty_stacks_filled_by()

What empty stacks can be filled by:

=over 4

=item * any

=item * none

=item * kings

=back

=head2 $variant_params->num_columns()

The number of columns the variant has.

=head2 $variant_params->num_decks()

The numbe of decks the variant has.

=head2 $variant_params->num_freecells()

The number of freecells the variant has.

=head2 $variant_params->rules()

The rules by which the variant obides:

=over 4

=item * freecell

=item * simple_simon

=back

=head2 $variant_params->seq_build_by()

Returns the sequence build by:

=over 4

=item * alt_color

=item * suit

=back

=head2 $variant_params->sequence_move()

=over 4

=item * limited

=item * unlimited

=back

=cut

=head2 $self->clone()

Returns a clone.

=cut

sub clone
{
    my $self = shift;

    return __PACKAGE__->new(
        {
            empty_stacks_filled_by => $self->empty_stacks_filled_by(),
            num_columns => $self->num_columns(),
            num_decks => $self->num_decks(),
            num_freecells => $self->num_freecells(),
            rules => $self->rules(),
            seq_build_by => $self->seq_build_by(),
            sequence_move => $self->sequence_move(),
        }
    );
}

1;
