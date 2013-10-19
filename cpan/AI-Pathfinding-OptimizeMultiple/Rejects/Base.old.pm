package AI::Pathfinding::OptimizeMultiple::Base;

use strict;
use warnings;


use vars qw($VERSION);

$VERSION = '0.0.1';

use Class::XSAccessor;



sub new
{
    my $class = shift;
    my $self = {};
    bless $self, $class;

    # May throw an exception.
    $self->_init(@_);

    return $self;
}


sub mk_accessors
{
    my $package = shift;
    return $package->mk_acc_ref([@_]);
}


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


1; # End of AI::Pathfinding::OptimizeMultiple::Move

__END__

=pod

=head1 NAME

AI::Pathfinding::OptimizeMultiple::Base

=head1 VERSION

version 0.0.1

=head1 SYNOPSIS

    use base 'AI::Pathfinding::OptimizeMultiple::Base';

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

This is the base class for L<AI::Pathfinding::OptimizeMultiple> classes.
Everything is subject to change.

=head1 NAME

AI::Pathfinding::OptimizeMultiple::Base

=head1 VERSION

version 0.0.1

=head1 NAME

AI::Pathfinding::OptimizeMultiple::Base - a base class.

=head1 FUNCTIONS

=head2 new($args)

The constructor. Blesses and calls _init() .

=head2 __PACKAGE__->mk_accessors(qw(method1 method2 method3))

Equivalent to L<Class::Accessor>'s mk_accessors only using Class::XSAccessor.
It beats running an ugly script on my code, and can be done at run-time.

Gotta love dynamic languages like Perl 5.

=head2 __PACKAGE__->mk_acc_ref([qw(method1 method2 method3)])

Creates the accessors in the array-ref of names at run-time.

=head1 AUTHOR

Shlomi Fish, L<http://www.shlomifish.org>.

=head1 SUPPORT

You can find documentation for this module with the perldoc command.

    perldoc AI::Pathfinding::OptimizeMultiple

You can also look for information at:

=over 4

=item * RT: CPAN's request tracker

L<http://rt.cpan.org/NoAuth/Bugs.html?Dist=Shlomif-FCS-CalcMetaScan>

=item * AnnoCPAN: Annotated CPAN documentation

L<http://annocpan.org/dist/Shlomif-FCS-CalcMetaScan>

=item * CPAN Ratings

L<http://cpanratings.perl.org/d/Shlomif-FCS-CalcMetaScan>

=item * Search CPAN

L<http://search.cpan.org/dist/Shlomif-FCS-CalcMetaScan>

=back

=head1 ACKNOWLEDGEMENTS

=head1 COPYRIGHT AND LICENSE

Copyright (c) 2010 Shlomi Fish

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
