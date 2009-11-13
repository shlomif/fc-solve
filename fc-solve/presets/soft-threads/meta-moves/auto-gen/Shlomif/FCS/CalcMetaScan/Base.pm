package Shlomif::FCS::CalcMetaScan::Base;

use strict;
use warnings;

use base 'Class::Accessor';

sub new
{
    my $class = shift;
    my $self = {};
    bless $self, $class;
    $self->_init(@_);
    return $self;
}

sub _init
{
    die "you must override _init().";
}

1;

