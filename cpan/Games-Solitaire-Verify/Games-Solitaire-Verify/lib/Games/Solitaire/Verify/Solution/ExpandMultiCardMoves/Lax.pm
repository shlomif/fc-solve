package Games::Solitaire::Verify::Solution::ExpandMultiCardMoves::Lax;

use strict;
use warnings;

use parent 'Games::Solitaire::Verify::Solution::ExpandMultiCardMoves';

=head1 NAME

Games::Solitaire::Verify::Solution::ExpandMultiCardMoves::Lax - faster and
laxer expansion.

=cut

our $VERSION = '0.1701';

sub _assign_read_new_state
{
    my ($self, $str) = @_;

    if (!defined($self->_st()))
    {
        $self->_st(
            Games::Solitaire::Verify::State->new(
                {
                    string => $str,
                    @{$self->_V},
                }
            )
        );
    }

    return;
}

=head1 SYNOPSIS

    use Games::Solitaire::Verify::Solution::ExpandMultiCardMoves::Lax;

    my $input_filename = "freecell-24-solution.txt";

    open (my $input_fh, "<", $input_filename)
        or die "Cannot open file $!";

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution::ExpandMultiCardMoves::Lax->new(
        {
            input_fh => $input_fh,
            variant => "freecell",
            output_fh => \*STDOUT,
        },
    );

    my $ret = $solution->verify();

    close($input_fh);

    if ($ret)
    {
        die $ret;
    }
    else
    {
        print "Solution is OK";
    }

=cut

1; # End of Games::Solitaire::Verify::Solution::ExpandMultiCardMoves::Lax
