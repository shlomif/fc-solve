package Games::Solitaire::Verify;

use warnings;
use strict;

use 5.008;

=head1 NAME

Games::Solitaire::Verify - verify solutions for solitaire games.

=cut

=head1 SYNOPSIS

    use Games::Solitaire::Verify::Solution;

    my $verifier = Games::Solitaire::Verify::Solution->new();

    $verifier->verify_solution({ fh => \*STDIN });

=head1 DESCRIPTION

This is a CPAN Perl module that verifies the solutions of various variants
of card Solitaire. It does not aim to try to be a solver for them, because
this is too CPU intensive to be adequately done using perl5 (as of
perl-5.10.0). If you're interested in the latter, look at:

=over 4

=item * http://fc-solve.shlomifish.org/

=item * http://fc-solve.shlomifish.org/links.html#other_solvers

=back

Instead, what Games-Solitaire-Verify does is verify the solutions
and makes sure they are correct.

See L<http://pysolfc.sourceforge.net/> for more about card solitaire.

=head1 SEE ALSO

L<Games::Solitaire::Verify::Solution> , L<Games::Solitaire::Verify::State>

=head1 AUTHOR

Shlomi Fish, L<http://www.shlomifish.org/>.

=cut

1; # End of Games::Solitaire::Verify
