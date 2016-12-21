package Task::FreecellSolver::Testing;

use warnings;
use strict;

use 5.012;

=head1 NAME

Task::FreecellSolver::Testing - install the CPAN dependencies of the
Freecell Solver test suite.

=cut

our $VERSION = 'v0.0.10';

# Load the dependencies so we'll be sure they are installed.
use Carp;
use Cwd;
use Data::Dumper;
use Digest::SHA;
use Env::Path;
use File::Path;
use File::Spec;
use File::Which ();
use Games::Solitaire::Verify;
use Inline;
use Inline::C ();
use List::MoreUtils;
use List::Util;
use Moo;
use MooX qw(late);
use Path::Tiny 0.077;
use Storable;
use String::ShellQuote;
use Task::Test::Run::AllPlugins;
use Template;
use Test::Data::Split;
use Test::Differences;
use Test::More;
use Test::PerlTidy ();
use Test::Run::CmdLine::Plugin::TrimDisplayedFilenames;
use Test::RunValgrind;
use Test::TrailingSpace ();
use YAML::XS;

=head1 SYNOPSIS

    $ perl -MCPAN -e 'install "Task::FreecellSolver::Testing"'

=head1 AUTHOR

Shlomi Fish, L<http://www.shlomifish.org/>

=cut

1; # End of Task::FreecellSolver::Testing
