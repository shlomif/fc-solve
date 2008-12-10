package Games::Solitaire::FC_Solve::ShaAndLen;

use strict;
use warnings;

use Digest::SHA;

use base 'Class::Accessor';

__PACKAGE__->mk_accessors(qw(
    _len
    _hasher
    ));

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
    my $self = shift;
    my $args = shift;

    $self->_len(0);
    $self->_hasher(Digest::SHA->new(256));

    return 0;
}

sub add
{
    my $self = shift;
    my $string = shift;

    $self->_len($self->_len()+length($string));
    $self->_hasher->add($string);

    return;
}

sub len
{
    my $self = shift;

    return $self->_len();
}

sub hexdigest
{
    my $self = shift;

    return $self->_hasher->clone()->hexdigest();
}

sub add_file
{
    my $self = shift;
    my $fh = shift;

    my $buffer;
    {
        local $/;
        $buffer = <$fh>;
    }
    return $self->add($buffer);
}

1;

