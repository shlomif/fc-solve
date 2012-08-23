package Games::Solitaire::Verify::Move;

use warnings;
use strict;

=head1 NAME

Games::Solitaire::Verify::Move - a class wrapper for an individual
Solitaire move.

=head1 VERSION

Version 0.01

=cut

our $VERSION = '0.1000';

use base 'Games::Solitaire::Verify::Base';

use Games::Solitaire::Verify::Exception;

__PACKAGE__->mk_acc_ref([qw(
    source_type
    dest_type
    source
    dest
    num_cards
    _game
    )]);

=head1 SYNOPSIS

    use Games::Solitaire::Verify::Move;

    my $move1 = Games::Solitaire::Verify::Move->new(
        {
            fcs_string => "Move a card from stack 0 to the foundations",
            game => "freecell",
        },
    );

=head1 FUNCTIONS

=cut

sub _from_fcs_string
{
    my ($self, $str) = @_;

    if ($str =~ m{\AMove a card from stack (\d+) to the foundations\z})
    {
        my $source = $1;

        $self->source_type("stack");
        $self->dest_type("foundation");

        $self->source($source);
    }
    elsif ($str =~ m{\AMove a card from freecell (\d+) to the foundations\z})
    {
        my $source = $1;

        $self->source_type("freecell");
        $self->dest_type("foundation");

        $self->source($source);
    }
    elsif ($str =~ m{\AMove a card from freecell (\d+) to stack (\d+)\z})
    {
        my ($source, $dest) = ($1, $2);

        $self->source_type("freecell");
        $self->dest_type("stack");

        $self->source($source);
        $self->dest($dest);
    }
    elsif ($str =~ m{\AMove a card from stack (\d+) to freecell (\d+)\z})
    {
        my ($source, $dest) = ($1, $2);

        $self->source_type("stack");
        $self->dest_type("freecell");

        $self->source($source);
        $self->dest($dest);
    }
    elsif ($str =~ m{\AMove (\d+) cards from stack (\d+) to stack (\d+)\z})
    {
        my ($num_cards, $source, $dest) = ($1, $2, $3);

        $self->source_type("stack");
        $self->dest_type("stack");

        $self->source($source);
        $self->dest($dest);
        $self->num_cards($num_cards);
    }
    elsif ($str =~ m{\AMove the sequence on top of Stack (\d+) to the foundations\z})
    {
        my $source = $1;

        $self->source_type("stack_seq");
        $self->dest_type("foundation");

        $self->source($source);
    }
    else
    {
        Games::Solitaire::Verify::Exception::Parse::FCS->throw(
            error => "Cannot parse 'FCS' String",
        );
    }
}

sub _init
{
    my ($self, $args) = @_;

    $self->_game($args->{game});

    if (exists($args->{fcs_string}))
    {
        return $self->_from_fcs_string($args->{fcs_string});
    }
}

=head1 METHODS

=head2 $move->source_type()

Accessor for the solitaire card game's board layout's type -
C<"stack">, C<"freecell">, etc. used in the layout.

=head2 $move->dest_type()

Accessor for the destination type - C<"stack">, C<"freecell">,
C<"destination">.

=head2 $move->source()

The index number of the source.

=head2 $move->dest()

The index number of the destination.

=head2 $move->num_cards()

Number of cards affects - only relevant for a stack-to-stack move usually.

=head1 AUTHOR

Shlomi Fish, C<< <shlomif at iglu.org.il> >>

=head1 BUGS

Please report any bugs or feature requests to C<bug-games-solitaire-verifysolution-move at rt.cpan.org>, or through
the web interface at L<http://rt.cpan.org/NoAuth/ReportBug.html?Queue=Games-Solitaire-Verify>.  I will be notified, and then you'll
automatically be notified of progress on your bug as I make changes.

=head1 SUPPORT

You can find documentation for this module with the perldoc command.

    perldoc Games::Solitaire::Verify


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
