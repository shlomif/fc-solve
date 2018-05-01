package Games::Solitaire::Verify::State::LaxParser;

use warnings;
use strict;

=head1 NAME

Games::Solitaire::Verify::State - a class for Solitaire
states (or positions) of the entire board.

=cut

use parent 'Games::Solitaire::Verify::State';

sub _from_string
{
    my ( $self, $str ) = @_;

    my $rank_re = '[0A1-9TJQK]';

    my $founds_s;
    if ( $str =~ m{\A(Foundations:[^\n]*)\n}cgms )
    {
        $founds_s = $1;
    }
    else
    {
        $founds_s = "Foundations: H-0 C-0 D-0 S-0";
    }

    $self->set_foundations(
        Games::Solitaire::Verify::Foundations->new(
            {
                num_decks => $self->num_decks(),
                string    => $founds_s,
            }
        )
    );

    my $fc = 'Freecells:';
    if ( $str =~ m{\G(Freecells:[^\n]*)\n}cgms )
    {
        $fc = $1;
    }
    $self->_assign_freecells_from_string($fc);

    foreach my $col_idx ( 0 .. ( $self->num_columns() - 1 ) )
    {
        if ( $str !~ m{\G([^\n]*)\n}msg )
        {
            Games::Solitaire::Verify::Exception::Parse::State::Column->throw(
                error => "Cannot parse column",
                index => $col_idx,
            );
        }
        my $column_str = $1;
        if ( $column_str !~ /\A:/ )
        {
            $column_str =~ s/\A\s*/: /;
        }

        $self->add_column(
            Games::Solitaire::Verify::Column->new(
                {
                    string => $column_str,
                }
            )
        );
    }

    return;
}

1;    # End of Games::Solitaire::Verify::State
