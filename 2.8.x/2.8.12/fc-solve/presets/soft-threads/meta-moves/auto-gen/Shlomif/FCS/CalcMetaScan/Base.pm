package Shlomif::FCS::CalcMetaScan::Base;

use strict;
use warnings;

use base 'Class::Accessor';

sub new
{
    my $class = shift;
    my $self = {};
    bless $self, $class;
    $self->initialize(@_);
    return $self;
}

sub initialize
{
    die "you must override initialize().";
}

1;

