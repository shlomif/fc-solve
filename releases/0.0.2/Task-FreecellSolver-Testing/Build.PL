use strict;
use warnings;
use Module::Build;

my $builder = Module::Build->new(
    module_name         => 'Task::FreecellSolver::Testing',
    license             => 'mit',
    dist_author         => q{Shlomi Fish <shlomif@iglu.org.il>},
    dist_version_from   => 'lib/Task/FreecellSolver/Testing.pm',
    requires => {
        "base" => 0,
        "Carp" => 0,
        "Cwd" => 0,
        "Data::Dumper" => 0,
        "Digest::SHA" => 0,
        "Env::Path" => 0,
        "File::Path" => 0,
        "File::Spec" => 0,
        "Games::Solitaire::Verify" => 0,
        "Games::Solitaire::Verify::Solution" => 0,
        "IPC::Open2" => 0,
        "lib" => 0,
        "List::MoreUtils" => 0,
        "Storable" => 0,
        "strict" => 0,
        "String::ShellQuote" => 0,
        "Task::Test::Run::AllPlugins" => 0,
        "Template" => 0,
        "Test::Differences" => 0,
        "Test::More" => 0,
        "warnings" => 0,
        "YAML::XS" => 0,
    },
    build_requires => {
        'Test::More' => 0,
    },
    add_to_cleanup      => [ 'Task-FreecellSolver-Testing-*' ],
    create_makefile_pl => 'traditional',
);

$builder->create_build_script();
