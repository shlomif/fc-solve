package Games::Solitaire::Verify::VariantParams;

use warnings;
use strict;

=head1 NAME

Games::Solitaire::Verify::VariantParams - a class for holding 
the parameters of the variant.

=head1 VERSION

Version 0.03

=cut

our $VERSION = '0.04';

use base 'Games::Solitaire::Verify::Base';

use Games::Solitaire::Verify::Exception;

__PACKAGE__->mk_accessors(qw(
    num_decks
    empty_stacks_filled_by
    num_freecells
    seq_build_by
    sequence_move
    num_columns
    ));

=head1 SYNOPSIS

    use Games::Solitaire::Verify::VariantParams;

    my $freecell_params = 
        Games::Solitaire::Verify::VariantParams->new(
            {
                seq_build_by => "alt_color",
            },
        );
 

=head1 FUNCTIONS

=cut

my %seqs_build_by = (map { $_ => 1 } (qw(alt_color suit rank)));
my %empty_stacks_filled_by_map = (map { $_ => 1 } (qw(kings any none)));
my %seq_moves = (map { $_ => 1 } (qw(limited unlimited)));

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

    return 0;
}

=head2 $state->seq_build_by()

Returns the sequence build by:

=over 4

=item * alt_color

=item * suit

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
            seq_build_by => $self->seq_build_by(),
        }
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

    perldoc Games::Solitaire::Verify::VariantParams


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

1;
