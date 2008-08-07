package Games::Solitaire::Verify;

use warnings;
use strict;

=head1 NAME

Games::Solitaire::Verify - verify solutions for solitaire games.

=head1 VERSION

Version 0.02

=cut

our $VERSION = '0.04';

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

=item * http://fc-solve.berlios.de/

=item * http://fc-solve.berlios.de/links.html#other_solvers

=back

Instead, what Games-Solitaire-Verify does is verify the solutions
and makes sure they are correct.

See L<http://pysolfc.sourceforge.net/> for more about card solitaire.

=head1 SEE ALSO

L<Games::Solitaire::Verify::Solution> , L<Games::Solitaire::Verify::State>

=head1 AUTHOR

Shlomi Fish, C<< <shlomif at iglu.org.il> >>

=head1 BUGS

Please report any bugs or feature requests to C<bug-games-solitaire-verifysolution at rt.cpan.org>, or through
the web interface at L<http://rt.cpan.org/NoAuth/ReportBug.html?Queue=Games-Solitaire-Verify>.  I will be notified, and then you'll
automatically be notified of progress on your bug as I make changes.




=head1 SUPPORT

You can find documentation for this module with the perldoc command.

    perldoc Games::Solitaire::Verify

You can also look for information at:

=over 4

=item * RT: CPAN's request tracker

L<http://rt.cpan.org/NoAuth/Bugs.html?Dist=Games-Solitaire-Verify>

=item * AnnoCPAN: Annotated CPAN documentation

L<http://annocpan.org/dist/Games-Solitaire-Verify>

=item * CPAN Ratings

L<http://cpanratings.perl.org/d/Games-Solitaire-Verify>

=item * Search CPAN

L<http://search.cpan.org/dist/Games-Solitaire-Verify>

=back


=head1 ACKNOWLEDGEMENTS


=head1 COPYRIGHT & LICENSE

Copyright 2008 Shlomi Fish, all rights reserved.

This program is released under the following license: MIT/X11
( L<http://www.opensource.org/licenses/mit-license.php> ).

=cut

1; # End of Games::Solitaire::Verify
