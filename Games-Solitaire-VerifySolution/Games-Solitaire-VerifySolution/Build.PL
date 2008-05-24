use strict;
use warnings;
use Module::Build;

my $builder = Module::Build->new(
    module_name         => 'Games::Solitaire::VerifySolution',
    license             => 'mit',
    dist_author         => 'Shlomi Fish <shlomif@iglu.org.il>',
    dist_version_from   => 'lib/Games/Solitaire/VerifySolution.pm',
    build_requires => {
        'Test::More' => 0,
    },
    add_to_cleanup      => [ 'Games-Solitaire-VerifySolution-*' ],
    create_makefile_pl => 'traditional',
);

$builder->create_build_script();
