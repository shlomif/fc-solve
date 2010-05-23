use strict;
use warnings;

use File::Spec;
use lib File::Spec->catdir(File::Spec->curdir(), "inc");

use Test::Run::Builder;

my $builder = Test::Run::Builder->new(
    module_name         => 'Games::Solitaire::Verify',
    license             => 'mit',
    dist_author         => 'Shlomi Fish <shlomif@iglu.org.il>',
    dist_version_from   => 'lib/Games/Solitaire/Verify.pm',
    build_requires => {
        'Test::More' => 0,
    },
    requires => {
        'base' => 0,
        'Class::XSAccessor' => 0,
        'File::Spec' => 0,
        'Exception::Class' => 0,
        'List::Util' => 0,
    },
    add_to_cleanup      => [ 'Games-Solitaire-Verify-*' ],
    create_makefile_pl => 'traditional',
    script_files => [],
    meta_merge =>
    {
        resources =>
        {
            repository => "http://svn.berlios.de/svnroot/repos/fc-solve/trunk/Games-Solitaire-Verify/",
            homepage => "http://fc-solve.berlios.de/verify-code/",
            MailingList => "http://tech.groups.yahoo.com/group/fc-solve-discuss/",
        },
        keywords =>
        [
            "baker's dozen",
            "baker's game",
            "card",
            "card games",
            "cards",
            "eight off",
            "forecell",
            "foundations",
            "freecell",
            "french",
            "games",
            "moves",
            "patience",
            "seahaven",
            "seahaven towers",
            "simple simon",
            "single-player",
            "solitaire",
            "solutions",
            "solve",
            "solving",
            "verification",
            "verify",
        ],
    },

);

$builder->create_build_script();
