package Task::FreecellSolver::Testing;

use warnings;
use strict;

=head1 NAME

Task::FreecellSolver::Testing - install the CPAN dependencies of the
Freecell Solver test suite.

=head1 VERSION

Version 0.0.3

=cut

our $VERSION = '0.0.3';

# Load the dependencies so we'll be sure they are installed.
use Carp;
use Data::Dumper;
use Digest::SHA;
use Env::Path;
use File::Path;
use File::Spec;
use Games::Solitaire::Verify;
use IPC::Open2;
use Storable;
use String::ShellQuote;
use Task::Test::Run::AllPlugins;
use Template;
use Test::Differences;
use Test::More;
use Test::Run::CmdLine::Plugin::TrimDisplayedFilenames;
use YAML::XS;

=head1 SYNOPSIS

    $ perl -MCPAN -e 'install "Task::FreecellSolver::Testing"'

=head1 AUTHOR

Shlomi Fish, L<http://www.shlomifish.org/>

=head1 BUGS

Please report any bugs or feature requests to C<bug-task-freecellsolver-testing at rt.cpan.org>, or through
the web interface at L<http://rt.cpan.org/NoAuth/ReportBug.html?Queue=Task-FreecellSolver-Testing>.  I will be notified, and then you'll
automatically be notified of progress on your bug as I make changes.

=head1 SUPPORT

You can find documentation for this module with the perldoc command.

    perldoc Task::FreecellSolver::Testing


You can also look for information at:

=over 4

=item * RT: CPAN's request tracker

L<http://rt.cpan.org/NoAuth/Bugs.html?Dist=Task-FreecellSolver-Testing>

=item * AnnoCPAN: Annotated CPAN documentation

L<http://annocpan.org/dist/Task-FreecellSolver-Testing>

=item * CPAN Ratings

L<http://cpanratings.perl.org/d/Task-FreecellSolver-Testing>

=item * Search CPAN

L<http://search.cpan.org/dist/Task-FreecellSolver-Testing/>

=back


=head1 ACKNOWLEDGEMENTS


=head1 COPYRIGHT & LICENSE

Copyright 2009 Shlomi Fish.

This program is distributed under the MIT (X11) License:
L<http://www.opensource.org/licenses/mit-license.php>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

=cut

1; # End of Task::FreecellSolver::Testing
