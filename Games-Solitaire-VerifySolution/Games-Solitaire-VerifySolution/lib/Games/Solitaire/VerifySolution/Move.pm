package Games::Solitaire::VerifySolution::Move;

use warnings;
use strict;

=head1 NAME

Games::Solitaire::VerifySolution::Move - a class wrapper for an individual 
Solitaire move.

=head1 VERSION

Version 0.01

=cut

our $VERSION = '0.01';

use base 'Games::Solitaire::VerifySolution::Base';

use Games::Solitaire::VerifySolution::Exception;
__PACKAGE__->mk_accessors(qw(
    source_type
    dest_type
    source
    dest
    num_cards
    _game
    ));

=head1 SYNOPSIS

    use Games::Solitaire::VerifySolution::Move;

    my $move1 = Games::Solitaire::VerifySolution::Move->new(
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
    else
    {
        Games::Solitaire::VerifySolution::Exception::Parse::FCS->throw(
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

=head1 AUTHOR

Shlomi Fish, C<< <shlomif at iglu.org.il> >>

=head1 BUGS

Please report any bugs or feature requests to C<bug-games-solitaire-verifysolution-move at rt.cpan.org>, or through
the web interface at L<http://rt.cpan.org/NoAuth/ReportBug.html?Queue=Games-Solitaire-VerifySolution>.  I will be notified, and then you'll
automatically be notified of progress on your bug as I make changes.

=head1 SUPPORT

You can find documentation for this module with the perldoc command.

    perldoc Games::Solitaire::VerifySolution


You can also look for information at:

=over 4

=item * RT: CPAN's request tracker

L<http://rt.cpan.org/NoAuth/Bugs.html?Dist=Games-Solitaire-VerifySolution>

=item * AnnoCPAN: Annotated CPAN documentation

L<http://annocpan.org/dist/Games-Solitaire-VerifySolution>

=item * CPAN Ratings

L<http://cpanratings.perl.org/d/Games-Solitaire-VerifySolution>

=item * Search CPAN

L<http://search.cpan.org/dist/Games-Solitaire-VerifySolution>

=back


=head1 ACKNOWLEDGEMENTS


=head1 COPYRIGHT & LICENSE

Copyright 2008 Shlomi Fish, all rights reserved.

This program is released under the following license: MIT/X11
( L<http://www.opensource.org/licenses/mit-license.php> ).

=cut

1; # End of Games::Solitaire::VerifySolution::Move
