package Games::Solitaire::Verify::Solution::ExpandMultiCardMoves::Lax;

use strict;
use warnings;

use parent 'Games::Solitaire::Verify::Solution::ExpandMultiCardMoves';

=head1 NAME

Games::Solitaire::Verify::Solution::ExpandMultiCardMoves::Lax - faster and
laxer expansion.

=head1 VERSION

Version 0.1300

=cut

our $VERSION = '0.1300';

sub _assign_read_new_state
{
    my ($self, $str) = @_;

    if (!defined($self->_state()))
    {
        $self->_state(
            Games::Solitaire::Verify::State->new(
                {
                    string => $str,
                    @{$self->_calc_variant_args()},
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
=head1 AUTHOR

Shlomi Fish, L<http://www.shlomifish.org/>.

=head1 BUGS

Please report any bugs or feature requests to C<bug-games-solitaire-verify at rt.cpan.org>, or through
the web interface at L<http://rt.cpan.org/NoAuth/ReportBug.html?Queue=Games-Solitaire-Verify>.  I will be notified, and then you'll
automatically be notified of progress on your bug as I make changes.

=head1 SUPPORT

You can find documentation for this module with the perldoc command.

    perldoc Games::Solitaire::Verify::Solution

You can also look for information at:

=over 4

=item * RT: CPAN's request tracker

L<http://rt.cpan.org/NoAuth/Bugs.html?Dist=Games-Solitaire-Verify>

=item * AnnoCPAN: Annotated CPAN documentation

L<http://annocpan.org/dist/Games-Solitaire-Verify>

=item * CPAN Ratings

L<http://cpanratings.perl.org/d/Games-Solitaire-Verify>

=item * MetaCPAN

L<http://metacpan.org/release/Games-Solitaire-Verify>

=back


=head1 ACKNOWLEDGEMENTS


=head1 COPYRIGHT & LICENSE

Copyright 2008 Shlomi Fish.

This program is released under the following license: MIT/X11
( L<http://www.opensource.org/licenses/mit-license.php> ).

=cut

1; # End of Games::Solitaire::Verify::Solution::ExpandMultiCardMoves::Lax
