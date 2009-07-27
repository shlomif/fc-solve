use strict;
use warnings;
use Module::Build;

my $builder = Module::Build->new(
    module_name         => 'Task::FreecellSolver::Testing',
    license             => 'mit',
    dist_author         => q{Shlomi Fish <shlomif@iglu.org.il>},
    dist_version_from   => 'lib/Task/FreecellSolver/Testing.pm',
    build_requires => {
        'Test::More' => 0,
    },
    add_to_cleanup      => [ 'Task-FreecellSolver-Testing-*' ],
    create_makefile_pl => 'traditional',
);

$builder->create_build_script();
