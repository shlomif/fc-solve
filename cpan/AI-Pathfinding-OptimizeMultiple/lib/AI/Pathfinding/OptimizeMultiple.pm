package AI::Pathfinding::OptimizeMultiple;

use strict;
use warnings;

our $VERSION = '0.0.1';

=head1 NAME

AI::Pathfinding::OptimizeMultiple - optimize path finding searches for a large
set of initial conditions (for better average performance).

=head1 VERSION

Version 0.0.1

=head1 SYNOPSIS

    use AI::Pathfinding::OptimizeMultiple

    my $obj = AI::Pathfinding::OptimizeMultiple->new(
        {
            scans =>
            [
                {
                    name => "first_search"
                },
                {
                    name => "second_search",
                },
                {
                    name => "third_search",
                },
            ],
            scans_iters_pdls =>
            {
                first_search => $first_search_pdl,
                second_search => $second_search_pdl,
            },
        }
    );

    my $results = $obj->calc_results();

=head1 DESCRIPTION

This CPAN distribution implements the algorithm described here:

=over 4

=item * L<https://groups.google.com/group/comp.ai.games/msg/41e899e9beea5583?dmode=source&output=gplain&noredirect>

=item * L<http://www.shlomifish.org/lecture/Perl/Lightning/Opt-Multi-Task-in-PDL/>

=back

Given statistics on the performance of several game AI searches (or scans)
across a representative number of initial cases, find a scan
that solves most deals with close-to-optimal performance, by using switch
tasking.

=head1 SEE ALSO

=over 4

=item * L<http://fc-solve.shlomifish.org/,Freecell Solver>

For which this code was first written and used.

=item * L<https://bitbucket.org/shlomif/fc-solve/src/cc5b428ed9bad0132d7a7bc1a14fc6d3650edf45/fc-solve/presets/soft-threads/meta-moves/auto-gen/optimize-seq?at=master,Alternative Implementation in C#/.NET>

An Alternative implementation in C#/.NET, which was written because the
performance of the Perl/PDL code was too slow.

=item * L<PDL> - Perl Data Language

Used here.

=back

=head1 AUTHOR

Shlomi Fish, L<http://www.shlomifish.org/> .

=head1 BUGS

Please report any bugs or feature requests to C<bug-ai-pathfinding-optimizemultiple at rt.cpan.org>, or through
the web interface at L<http://rt.cpan.org/NoAuth/ReportBug.html?Queue=AI-Pathfinding-OptimizeMultiple>.  I will be notified, and then you'll
automatically be notified of progress on your bug as I make changes.

=head1 SUPPORT

You can find documentation for this module with the perldoc command.

    perldoc AI::Pathfinding::OptimizeMultiple

You can also look for information at:

=over 4

=item * RT: CPAN's request tracker

L<http://rt.cpan.org/NoAuth/Bugs.html?Dist=Games-AI-Pathfinding-OptimizeMultiple>

=item * CPAN Ratings

L<http://cpanratings.perl.org/d/Games-AI-Pathfinding-OptimizeMultiple>

=item * MetaCPAN

L<http://search.cpan.org/dist/Games-AI-Pathfinding-OptimizeMultiple>

=back

=head1 ACKNOWLEDGEMENTS

B<popl> from Freenode's #perl for trying to dig some references to an existing
algorithm in the scientific literature.

=head1 COPYRIGHT & LICENSE

Copyright 2008 Shlomi Fish.

This program is released under the following license: MIT/X11
( L<http://www.opensource.org/licenses/mit-license.php> ).

=cut

1; # End of AI::Pathfinding::OptimizeMultiple
