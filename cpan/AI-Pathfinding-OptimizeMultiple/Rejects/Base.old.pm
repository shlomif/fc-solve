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

=cut
