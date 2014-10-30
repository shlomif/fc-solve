package Class::XSAccessor;

use strict;
use warnings;

sub import
{
    my ($pkg) = caller();

    my $caller_pkg = shift;

    my %what = @_;

    my $mapping = delete ( $what{'accessors'} );

    if (defined ($mapping))
    {
        foreach my $meth_name (keys %$mapping)
        {
            my $slot = $mapping->{$meth_name};

            {
                no strict;
                *{"${pkg}::$meth_name"} = sub {
                    my $self = shift;

                    if (@_)
                    {
                        $self->{$slot} = shift;
                    }
                    return $self->{$slot};
                };
            }
        }
    }
}

1;

