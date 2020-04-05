#!perl -T

use Test::More tests => 1;

BEGIN
{
    use_ok('Task::FreecellSolver::Testing::MultiConfig');
}

diag(
"Testing Task::FreecellSolver::Testing::MultiConfig $Task::FreecellSolver::Testing::MultiConfig::VERSION, Perl $], $^X"
);
