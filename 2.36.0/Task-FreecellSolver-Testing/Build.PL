use strict;
use warnings;
use Module::Build;

my $builder = Module::Build->new(
    module_name         => 'Task::FreecellSolver::Testing',
    license             => 'mit',
    dist_author         => q{Shlomi Fish <shlomif@iglu.org.il>},
    dist_version_from   => 'lib/Task/FreecellSolver/Testing.pm',
    requires => {
        "Games::Solitaire::Verify" => 0,
        "String::ShellQuote" => 0,
        "Task::Test::Run::AllPlugins" => 0,
        "Template" => 0,
        "Test::Differences" => 0,
        "YAML::XS",
    },
    build_requires => {
        'Test::More' => 0,
    },
    add_to_cleanup      => [ 'Task-FreecellSolver-Testing-*' ],
    create_makefile_pl => 'traditional',
);

$builder->create_build_script();
