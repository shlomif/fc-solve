#!perl -T

use strict;
use warnings;

use Test::More tests => 1;

BEGIN
{
    # TEST
    use_ok('Task::FreecellSolver::Testing');
}

diag(
"Testing Task::FreecellSolver::Testing $Task::FreecellSolver::Testing::VERSION, Perl $], $^X"
);
