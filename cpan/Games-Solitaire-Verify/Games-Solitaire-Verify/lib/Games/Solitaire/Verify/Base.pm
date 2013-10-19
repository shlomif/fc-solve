package Games::Solitaire::Verify::Base;

use strict;
use warnings;

=head1 NAME

Games::Solitaire::Verify::Base - a base class.

=cut

use vars qw($VERSION);

$VERSION = '0.1202';

use Class::XSAccessor;

=head1 SYNOPSIS

    use parent 'Games::Solitaire::Verify::Base';

    sub _init
    {
        my ($self, $args) = @_;

        $self->address($args->{address});

        if (!exists($args->{name}))
        {
            die "No name - no cry.";
        }

        return;
    }

=head1 DESCRIPTION

This is the base class for L<Games::Solitaire::Verify> classes.
Everything is subject to change.

=cut

=head1 FUNCTIONS

=head2 new($args)

The constructor. Blesses and calls _init() .

=cut

sub new
{
    my $class = shift;
    my $self = {};
    bless $self, $class;

    # May throw an exception.
    $self->_init(@_);

    return $self;
}

=head2 __PACKAGE__->mk_accessors(qw(method1 method2 method3))

Equivalent to L<Class::Accessor>'s mk_accessors only using Class::XSAccessor.
It beats running an ugly script on my code, and can be done at run-time.

Gotta love dynamic languages like Perl 5.

=cut

sub mk_accessors
{
    my $package = shift;
    return $package->mk_acc_ref([@_]);
}

=head2 __PACKAGE__->mk_acc_ref([qw(method1 method2 method3)])

Creates the accessors in the array-ref of names at run-time.

=cut

sub mk_acc_ref
{
    my $package = shift;
    my $names = shift;

    my $mapping = +{ map { $_ => $_ } @$names };

    eval <<"EOF";
package $package;

Class::XSAccessor->import(
    accessors => \$mapping,
);
EOF

}

=head1 AUTHOR

Shlomi Fish, L<http://www.shlomifish.org/>.

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
