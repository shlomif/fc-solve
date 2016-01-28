package FC_Solve::SplitTests::ValidateHash;

use strict;
use warnings;

use Carp qw/confess/;

use parent 'Test::Data::Split::Backend::Hash';

sub populate
{
    my ($self, $array_ref) = @_;

    my $l = @$array_ref;

    my $tests = $self->get_hash;

    if ($l & 0x1)
    {
        confess("Input length is not even.");
    }
    foreach my $i (grep { ($_ & 0x1) == 0 } 0 .. ($l-1))
    {
        my ($key, $val) = @$array_ref[$i,$i+1];
        if (exists($tests->{$key}))
        {
            confess("Duplicate key '$key'!");
        }
        $tests->{$key} = $val;
    }

    return;
}

1;
