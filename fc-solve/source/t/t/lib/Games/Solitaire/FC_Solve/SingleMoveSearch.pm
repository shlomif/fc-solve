package Games::Solitaire::FC_Solve::SingleMoveSearch;

use strict;
use warnings;

use base 'Games::Solitaire::Verify::Base';

sub calc_move_half
{
    my ($self, $move, $which) = @_;

    return (($move >> ($which * 4)) & 0xF);
}

sub compile_move_spec
{
    my ($self, $move_spec) = @_;

    my $t = $move_spec->{type};
    my $idx = $move_spec->{idx};

    return
    (
          ($t eq "stack") ? $idx
        : ($t eq "freecell") ? ($idx+8)
        : ($t eq "found") ? ($idx+8+4)
        : (die "Unknown type '$t'")
    )
}

1;

