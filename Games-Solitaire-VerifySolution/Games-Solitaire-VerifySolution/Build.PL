use strict;
use warnings;

use File::Spec;
use lib File::Spec->catdir(File::Spec->curdir(), "inc");

use Test::Run::Builder;

my $builder = Test::Run::Builder->new(
    module_name         => 'Games::Solitaire::VerifySolution',
    license             => 'mit',
    dist_author         => 'Shlomi Fish <shlomif@iglu.org.il>',
    dist_version_from   => 'lib/Games/Solitaire/VerifySolution.pm',
    build_requires => {
        'Test::More' => 0,
    },
    requires => {
        'base' => 0,
        'Class::Accessor' => 0,
        'File::Spec' => 0,
        'Exception::Class' => 0,
    },
    add_to_cleanup      => [ 'Games-Solitaire-VerifySolution-*' ],
    create_makefile_pl => 'traditional',
);

$builder->create_build_script();
