#!/usr/bin/env perl

# The aim of this program is to run Freecell Solver’s test suite with
# several different build-time configurations of the solver, to make sure
# they are all working fine.

use strict;
use warnings;
use autodie;
use 5.020;

package Games::Solitaire::FC_Solve::Test::Trap::Obj;

use Carp         ();
use Data::Dumper ();

use Term::ANSIColor qw(colored);

use Moo;

my @fields = qw(
    exit
    stderr
    stdout
    warn
);

has [ @fields, 'blurb', 'cmd_line', ] => ( is => 'rw' );

use Text::Sprintf::Named ();

use Test::Trap
    qw( trap $trap :flow:stderr(systemsafe):stdout(systemsafe):warn );

sub _stringify_value
{
    my ( $self, $name ) = @_;

    my $value = $self->$name();

    if ( ( $name eq "return" ) || ( $name eq "warn" ) )
    {
        return Data::Dumper->new( [$value] )->Dump();
    }
    else
    {
        return ( defined($value) ? $value : "" );
    }
}

sub all_info
{
    my $self = shift;

    return Text::Sprintf::Named->new(
        {
            fmt => join( "",
                map { "$_ [%(blurb)s] ===\n{{{{{{\n%($_)s\n}}}}}}\n\n" }
                    (@fields) )
        }
    )->format(
        {
            args => {
                blurb => scalar( $self->blurb ),
                map {
                    my $name = $_;
                    ( $name => $self->_stringify_value($name) )
                } @fields
            }
        }
    );
}

sub emit_all
{
    my $self = shift;

    my $err_s =
        "Error with @{[$self->blurb()]} executing [@{$self->cmd_line()}].\n";

    my $out = sub {
        print $err_s;
    };

    $out->();
    print( $self->all_info() );

    Carp::cluck("Error.");
    print colored(
        "Error!", ( $ENV{'HARNESS_SUMMARY_COLOR_FAIL'} || 'bold red' )
        ),
        "\n";
    $out->();
    exit(-1);
}

sub run_cmd
{
    my ( $blurb, $args ) = @_;

    my @cmd_line = @{ $args->{'cmd'} };

    print "Running: {$blurb} @cmd_line\n";
    STDOUT->flush;

    my $error_code;

    trap { $error_code = ( system { $cmd_line[0] } @cmd_line ); };

    my $self = __PACKAGE__->new( { ( map { $_ => $trap->$_() } @fields ), } );

    $self->blurb($blurb);
    $self->cmd_line( [@cmd_line] );

    if ($error_code)
    {
        $self->emit_all();
    }
    elsif ( $ENV{FC_SOLVE__MULT_CONFIG_TESTS__TRACE} )
    {
        $self->trace_all();
    }

    return $self;
}

sub trace_all
{
    my ($self) = @_;

    print "Trace of @{[$self->blurb()]} executing [@{$self->cmd_line()}].\n";
    print( $self->all_info() );

    return;
}

package main;

use Path::Tiny qw/ path /;

use Term::ANSIColor qw(colored);

local *run_cmd = \&Games::Solitaire::FC_Solve::Test::Trap::Obj::run_cmd;

my $TEST_BASE_IDX  = 1;
my $NUM_PROCESSORS = 4;

my $FALSE = 0;

# Inline::C/etc. leave some files and dirs under restrictive file permissions
# and we need to instruct rmtree to delete them as well.
my $SAFE = $FALSE;

my %skip_indices;
my @tests;

my $IS_TRAVIS_CI = ( exists( $ENV{TRAVIS} ) && ( $ENV{TRAVIS} eq 'true' ) );

my @TRAVIS_CI_SKIP_FAILING_TESTS = (
    $IS_TRAVIS_CI
    ? ( runtest_args => [qw%--exclude-re valgrind--dbm_fc_solver_1%], )
    : ()
);

sub reg_test
{
    my $blurb = shift;
    if ( ref $blurb ne 'HASH' )
    {
        $blurb = { blurb => $blurb, randomly_avoid => $FALSE, };
    }
    push @tests, [ $blurb, @_ ];
}

sub _calc_build_path
{
    my ($idx) = @_;

    return Path::Tiny->cwd->parent->child("build-$idx");
}

my $CWD         = Path::Tiny->cwd;
my $WEBSITE_DIR = "$CWD/../site/wml";

sub _chdir_run
{
    my ( $DIR_PATH_NAME, $cb ) = @_;

    chdir($DIR_PATH_NAME);
    $cb->();
    chdir($CWD);

    return;
}

my $GENERATOR = ( ( delete $ENV{CMAKE_GEN} ) || 'make' );

my $filtered_idx = 0;
my $total_filtered;

sub run_tests
{
    my ( $idx, $blurb_rec, $args ) = @_;
    my $IS_NINJA             = ( $GENERATOR eq 'ninja' );
    my $calc_generator_flags = sub {
        my $flag = shift;
        return [
            (
                $IS_NINJA
                ? ( $flag, ucfirst($GENERATOR), )
                : ()
            ),
        ];
    };

    $args = +{ @TRAVIS_CI_SKIP_FAILING_TESTS, %$args };
    if ( not defined($total_filtered) )
    {
        Carp::confess("\$total_filtered undefined.");
    }
    my $blurb_base_base = $blurb_rec->{blurb};
    my $is_filtered     = ( exists $skip_indices{ $idx - $TEST_BASE_IDX } );
    my $filtered_idx_snapshot = $filtered_idx + 0;

    my $displayed_filtered_idx_snapshot =
        $filtered_idx_snapshot + $TEST_BASE_IDX;

    if ( $displayed_filtered_idx_snapshot > $total_filtered )
    {
        if ( $displayed_filtered_idx_snapshot == 1 + $total_filtered )
        {
            --$displayed_filtered_idx_snapshot;
        }
        else
        {
            Carp::confess(
"\$displayed_filtered_idx_snapshot > \$total_filtered undefined."
            );
        }
    }
    my $blurb_base = sprintf "%s [ idx = %d / %d ; filtered-idx = %d / %d ]",
        $blurb_base_base, $idx,
        scalar(@tests),
        $displayed_filtered_idx_snapshot,
        $total_filtered;
    my $run = sub {
        my ( $DESC, $cmd ) = @_;
        run_cmd( "$blurb_base : $DESC", { cmd => [@$cmd] } );
    };

    if ( not $is_filtered )
    {
        ++$filtered_idx;
        if ( $filtered_idx > $total_filtered )
        {
            Carp::confess("\$filtered_idx > \$total_filtered .");
        }
    }
    if ($is_filtered)
    {
        if ( not $blurb_rec->{randomly_avoid} )
        {
            die "foo $idx";
        }
        $run->( "SKIPPING shuffled index $idx.", ["true"] );
        return;
    }
    if ( $idx < ( $ENV{MIN} // 0 ) )
    {
        return;
    }
    my $tatzer_args       = $args->{'tatzer_args'};
    my $tatzer_theme      = $args->{'tatzer_theme'};
    my $cmake_args        = $args->{'cmake_args'};
    my $prepare_dist_args = $args->{'prepare_dist_args'};
    my $website_args      = $args->{'website_args'};

    if (
        1 != grep { $_ } (
            $tatzer_args,       $tatzer_theme, $cmake_args,
            $prepare_dist_args, $website_args
        )
        )
    {
        die
"One and only one of tatzer_args or cmake_args or prepare_dist_args must be specified.";
    }
    if ($tatzer_theme)
    {
        $tatzer_args = [ '-l', $tatzer_theme, ];
    }

    my $build_path = _calc_build_path($idx);
    local %ENV = %ENV;
    delete( $ENV{FCS_USE_TEST_RUN} );
    $ENV{TEST_JOBS} = $NUM_PROCESSORS;
    if ( not $args->{run_perltidy} )
    {
        $ENV{FCS_TEST_SKIP_PERLTIDY} = 1;
    }

    if ($prepare_dist_args)
    {
        $run->(
            "prepare dist",
            [
                $^X,
"../scripts/prepare-self-contained-dbm-etc-solvers-packages-for-hpc-machines/$prepare_dist_args->{base}",
                @{ $prepare_dist_args->{args} }
            ],
        );
        my $DIR = path('dbm_fcs_dist');
        my $ARC = "$DIR.tar.xz";
        $run->( "untar", [ "tar", "-xvf", $ARC ] );
        _chdir_run(
            $DIR,
            sub {
                $run->( 'make', ['make'], );
            }
        );
        $DIR->remove_tree( { safe => $SAFE } );
        unlink($ARC);
    }
    elsif ($website_args)
    {
        my $DIR = $WEBSITE_DIR;
        my $PATH_PREFIX =
            $ENV{FC_SOLVE__MULT_CONFIG_TESTS__DOCKER}
            ? "$CWD/../scripts/dockerized-emscripten/:"
            : "";
        my $NODE_BIN_DIR = "$DIR/node_modules/.bin/";
        if ($FALSE)
        {
            local $ENV{PATH} = $PATH_PREFIX . $ENV{PATH} . ":$NODE_BIN_DIR";
        }
        local $ENV{PATH} = $NODE_BIN_DIR . ":" . $PATH_PREFIX . $ENV{PATH};
        _chdir_run(
            $DIR,
            sub {
                local $ENV{PWD} = $DIR;
                run_cmd(
                    "$blurb_base : ./gen-helpers",
                    { cmd => [ $^X, 'gen-helpers' ] }
                );

                if ( $ENV{FC_SOLVE__MULT_CONFIG_TESTS__DOCKER} )
                {
                    foreach my $component (qw# travis emscripten #)
                    {
                        my $P =
qq#/home/$component/build/shlomif/fc-solve/fc-solve/source/../site/wml/../../source#;
                        while ( my $x = $P =~ m#(/)#g )
                        {
                            my $path = substr( $P, 0, pos($P) );
                            print "<$path>\n",
                                scalar(
`docker exec -it emscripten bash -c 'echo $path/*'`
                                ),
                                "\n";
                        }
                    }
                    exit(-1);
                }
                my $run_make = sub {
                    my ($args) = @_;
                    my @cmd = @$args;

                    unshift @cmd,
                        ( $ENV{DBTOEPUB} ? "DBTOEPUB=\"$ENV{DBTOEPUB}\"" : () ),
                        (
                        $ENV{DOCBOOK5_XSL_STYLESHEETS_PATH}
                        ? "DOCBOOK5_XSL_STYLESHEETS_PATH=\"$ENV{DOCBOOK5_XSL_STYLESHEETS_PATH}\""
                        : ()
                        );
                    foreach my $x ( $args, \@cmd )
                    {
                        unshift @$x, "make";
                    }
                    return run_cmd(
                        "$blurb_base : @$args",
                        {
                            cmd => [
                                $ENV{FC_SOLVE__MULT_CONFIG_TESTS__DOCKER}
                                ? (@cmd)
                                : (
                                    'bash',
                                    '-c',
qq#src_me="\$HOME/bin/Dev-Path-Configs-Source-Me.bash"; if test -e "\$src_me"; then . "\$src_me" ; fi ; set -o pipefail ; @cmd 2>&1 | tail -300#
                                )
                            ]
                        }
                    );
                };
                $run_make->( [] );
                if ( not $args->{do_not_test} )
                {
                    $run_make->( ['test'] );
                }
            }
        );
    }
    else
    {
        mkdir($build_path);
        if ( $args->{'cmake-B'} )
        {
            my $inner_build_path = "inner_build_dir";
            my $bpath            = "$build_path/$inner_build_path";
            mkdir($bpath);
            _chdir_run(
                $build_path,
                sub {
                    $run->(
                        "cmake config stage",
                        [
                            'cmake',           '-B',
                            $inner_build_path, '-S',
                            '../source',       @$cmake_args,
                        ]
                    );
                    my @CMAKE_VER = (
                        scalar(`cmake --version`) =~
                            /\Acmake\s+version\s+([0-9]+)\.([0-9]+)/i );

                    # Fails with old cmake on travis.
                    if (
                        ( not $IS_TRAVIS_CI )
                        or (   ( $CMAKE_VER[0] > 3 )
                            or ( $CMAKE_VER[0] == 3 and $CMAKE_VER[1] >= 15 ) )
                        )
                    {
                        $run->(
                            "cmake build stage",
                            [ 'cmake', '--build', $inner_build_path, ]
                        );
                    }
                    return;
                }
            );
        }
        else
        {
            _chdir_run(
                $build_path,
                sub {
                    if ($tatzer_args)
                    {
                        $run->(
                            "Tatzer",
                            [
                                '../scripts/Tatzer',
                                @{ $calc_generator_flags->("--gen") },
                                @$tatzer_args
                            ]
                        );
                    }
                    else
                    {
                        $run->(
                            "cmake",
                            [
                                'cmake', @{ $calc_generator_flags->("-G") },
                                @$cmake_args, '../source'
                            ]
                        );
                    }
                    my $builder = ( $IS_NINJA ? "ninja" : "make" );
                    $run->( $builder, [ $builder, "-j$NUM_PROCESSORS" ] );
                    my $run_test = sub {
                        my ($cmd_line) = @_;
                        $run->(
                            "test", [ $^X, "$CWD/run-tests.pl", @$cmd_line, ]
                        );
                    };
                    if ( not $args->{do_not_test} )
                    {
                        $run_test->( [ @{ $args->{runtest_args} // [] } ] );
                    }
                    if ( my $exe = $args->{extra_test_command} )
                    {
                        $run_test->($exe);
                    }
                }
            );
        }
        $build_path->remove_tree( { safe => $SAFE } );
    }

    return;
}

my $AVLV = "avl-2.0.3";
if ( $ENV{FC_SOLVE_GIT_CHECKOUT} )
{
    $ENV{LIBAVL2_SOURCE_DIR} = "$ENV{HOME}/Download/unpack/prog/c/$AVLV/";

    run_cmd( 'git checkout', { cmd => [qw(git checkout master)], } );
    run_cmd( 'git pull', { cmd => [qw(git pull --ff-only origin master)], } );
}
elsif ( not exists $ENV{LIBAVL2_SOURCE_DIR} )
{
    if ( !( -d $AVLV and -f "$AVLV/prb.h" ) )
    {
        my $AVLT = "$AVLV.tar.gz";
        run_cmd(
            'wget avl',
            {
                cmd => [ 'wget', "https://ftp.gnu.org/pub/gnu/avl/$AVLT" ]
            }
        );
        run_cmd( 'untar avl', { cmd => [ qw(tar -xvf), $AVLT ] } );
        path($AVLV)->visit(
            sub {
                my $p = shift;
                $p->edit_raw( sub { s/[\t ]+$//gms; } ) if $p->is_file;
            },
            { recurse => 1, }
        );
    }
    $ENV{LIBAVL2_SOURCE_DIR} = Path::Tiny->cwd->child($AVLV);
    print "LIBAVL2_SOURCE_DIR = $ENV{LIBAVL2_SOURCE_DIR}\n";
}
elsif ( ( !-d $ENV{LIBAVL2_SOURCE_DIR} )
    or grep { !-f } glob( $ENV{LIBAVL2_SOURCE_DIR} . '/*.[ch]' ) )
{
    die "LIBAVL2_SOURCE_DIR is invalid.";
}

sub reg_tatzer_test
{
    my $blurb = shift;
    return reg_test( $blurb, { tatzer_args => [@_] } );
}

sub reg_theme_test
{
    my $blurb = shift;
    return reg_test( $blurb, { tatzer_theme => shift(), }, );
}

sub reg_prep
{
    my ( $blurb, $base ) = @_;
    return reg_test( $blurb,
        { prepare_dist_args => { base => $base, args => [] } } );
}

reg_test(
    "cmake -B -S ( GH#47 )",
    {
        'cmake-B'  => 1,
        cmake_args => [ '-DFCS_AVOID_INT128=1', '-DFCS_ENABLE_DBM_SOLVER=1', ]
    }
);
reg_test(
    {
        blurb => "zero freecells mode",
    },
    {
        tatzer_theme => 'zerofcmode',
        runtest_args => [
            '--custom-tests-suite', '--tests-dir',
            '../scripts/zerofc-mode-tests/t/'
        ],
    },
);
reg_test(
    {
        blurb => "zero freecells mode sample solution",
    },
    {
        tatzer_theme => 'zerofcmode_with_moves',
        runtest_args => [
            '--custom-tests-suite', '--tests-dir',
            '../scripts/zerofc-mode-tests/solution-t/',
        ],
    },
);
reg_theme_test( "Default", 'empty', );

sub enable_website_build_for_now
{
    my $NODE_DIR = "$WEBSITE_DIR/node_modules";
    if ( !-d "$NODE_DIR/param-case" )
    {
        die
"node dir $NODE_DIR appears to be missing! Tests will probably fail.";
    }
    reg_test( 'Website #1', { website_args => [] } );
}
enable_website_build_for_now();

reg_test(
    "No int128",
    { cmake_args => [ '-DFCS_AVOID_INT128=1', '-DFCS_ENABLE_DBM_SOLVER=1', ] }
);
reg_prep( "prepare_dist fcc_solver",
    'prepare_fcc_solver_self_contained_package.pl' );
reg_prep( "prepare_dist AWS",
    'prepare_aws_depth_dbm_fc_solver_self_contained_package.pl' );
reg_prep( "prepare_dist vendu",
    'prepare_vendu_depth_dbm_fc_solver_self_contained_package.pl' );
reg_prep( "prepare_dist pbs",
    'prepare_pbs_dbm_solver_self_contained_package.pl' );
reg_tatzer_test( "--fc-only wo break back compat", qw(--fc-only) );
reg_test(
    "-l n2t with --disable-patsolve",
    {
        tatzer_theme       => 'citest-disable-patsolve',
        extra_test_command => [
            (
                map { ; ( "--execute", $_ ) } (
                    $^X,
"$CWD/../../cpan/Games-Solitaire-Verify/benchmark/sanity-test.pl",
                )
            ),
        ],
    },
);
reg_test(
    "build_only: maximum speed preset",
    {
        do_not_test  => 1,
        tatzer_theme => 'citest-max-speed',
    }
);
my $TRUE = 1;
reg_test(
    {
        blurb          => "build_only: no iter-handler",
        randomly_avoid => $TRUE,
    },
    {
        do_not_test  => 1,
        tatzer_theme => 'citest-speed-noiter',
    }
);
reg_test(
    "Plain CMake Default",
    {
        cmake_args   => [],
        run_perltidy => 1,
    }
);
reg_test(
    {
        blurb          => "Non-Debondt Delta States",
        randomly_avoid => $TRUE,
    },
    { cmake_args => ['-DFCS_DISABLE_DEBONDT_DELTA_STATES=1'] }
);
reg_tatzer_test(
    {
        blurb          => "--rcs",
        randomly_avoid => $TRUE,
    },
    qw(--rcs)
);

reg_theme_test(
    {
        blurb          => "libavl2 with COMPACT_STATES",
        randomly_avoid => $TRUE,
    },
    'citest-avl-COMPACT_STATES',
);

reg_theme_test(
    {
        blurb          => "libavl2 with COMPACT_STATES and --rcs",
        randomly_avoid => $TRUE,
    },
    'citest-avl-COMPACT_STATES-rcs',
);

reg_theme_test(
    {
        blurb          => "libavl2 with INDIRECT_STACK_STATES",
        randomly_avoid => $TRUE,
    },
    'citest-avl-INDIRECT_STACK_STATES',
);

reg_tatzer_test(
    {
        blurb          => "without-depth-field",
        randomly_avoid => $TRUE,
    },
    qw(--without-depth-field)
);
reg_tatzer_test(
    {
        blurb          => "without-depth-field and rcs",
        randomly_avoid => $TRUE,
    },
    qw(--without-depth-field --rcs)
);
reg_theme_test(
    {
        blurb          => "No FCS_SINGLE_HARD_THREAD",
        randomly_avoid => $TRUE,
    },
    'citest-nosingle-ht',
);

reg_theme_test(
    {
        blurb          => "dbm apr_hash",
        randomly_avoid => $TRUE,
    },
    'citest-apr_hash',
);
reg_theme_test(
    {
        blurb          => "Break Backward Compatibility #1",
        randomly_avoid => $TRUE,
    },
    'citest-bbc',
);

reg_test(
    {
        blurb          => "Freecell-only (as well as Break Backcompat)",
        randomly_avoid => $TRUE,
    },
    { tatzer_theme => 'citest-fc-nocompat', },
);

{
    my @found =
        grep { -e $_ }
        map { _calc_build_path( $TEST_BASE_IDX + $_ ); } keys @tests;
    if (@found)
    {
        die
"The following build dirs exist and interfere with the build - [ @found ]. Please remove them!";
    }
}
my @l;
while ( my ( $idx, $run ) = each @tests )
{
    if ( $run->[0]->{randomly_avoid} )
    {
        push @l, $idx;

        # say Data::Dumper->new( [ $idx, $run ] )->Dump;
    }
}
use List::Util qw/ shuffle /;
my $AVOID_NUM = int( delete( $ENV{AVOID_NUM} ) // 0 );
if ( $AVOID_NUM > @l )
{
    $AVOID_NUM = @l;
}
if ( $AVOID_NUM > 0 )
{
    %skip_indices =
        map { $_ => $TRUE } ( ( shuffle @l )[ 0 .. ( $AVOID_NUM - 1 ) ] );
}

# say Data::Dumper->new( [ \%skip_indices ] )->Dump;
$total_filtered = @tests - keys(%skip_indices);

_chdir_run(
    '../../',
    sub {
        run_cmd( "root tests",
            { cmd => [ qw(prove), glob('root-tests/t/*.t') ] } );
    }
);

use Getopt::Long qw/ GetOptions /;

my $include_filter;
my $only_check_ready;
my $skip_dzil_tests;
my $skip_opencl_tests;

GetOptions(
    "include=s"          => \$include_filter,
    "only-check-ready!"  => \$only_check_ready,
    "skip-dzil-tests!"   => \$skip_dzil_tests,
    "skip-opencl-tests!" => \$skip_opencl_tests,
) or die $!;

if ( not $skip_opencl_tests )
{
    _chdir_run(
        '../',
        sub {
            my $base_path      = path("B-opencl")->absolute();
            my $board_gen_path = $base_path->child("board_gen")->absolute();
            my $inst_path      = path("INST-opencl")->absolute();
            $board_gen_path->mkdir();

            local $ENV{'CC'}      = 'clang';
            local $ENV{'REBUILD'} = '1';
            local $ENV{PATH}      = $ENV{PATH} . ":${inst_path}/bin";

            _chdir_run(
                $base_path,
                sub {
                    warn qx#pwd#;
                    run_cmd(
                        "OpenCL tests setup",
                        {
                            cmd => [
                                "bash",
                                "-c",
"../scripts/Tatzer --prefix $inst_path && make install",
                            ]
                        }
                    );
                }
            );

            _chdir_run(
                $board_gen_path,
                sub {
                    warn qx#pwd#;
                    run_cmd(
                        "OpenCL tests",
                        {
                            cmd => [
                                qw(prove), glob('../../scripts/opencl-test.t')
                            ]
                        }
                    );
                }
            );
        }
    );
}
if ( not $skip_dzil_tests )
{
    foreach my $dir_path_name (
        '../../cpan/Games-Solitaire-Verify/Games-Solitaire-Verify/',
        '../../cpan/Freecell-Deal-MS/',
        )
    {
        _chdir_run(
            $dir_path_name,
            sub {
                my ($mod) = ( $dir_path_name =~ m#/([^/]+)/\z#ms )
                    or Carp::confess(
                    "Cannot extract basename from '$dir_path_name'");
                return run_cmd( "$mod dzil", { cmd => [qw(dzil test --all)] } );
            }
        );
    }
}

if ($only_check_ready)
{
    exit(0);
}
RUN_TESTS:
while ( my ( $idx, $run ) = each @tests )
{
    if ( $include_filter and $run->[0]->{blurb} !~ /$include_filter/ )
    {
        next RUN_TESTS;
    }
    run_tests( $TEST_BASE_IDX + $idx, @$run );
}

my $COL  = $ENV{'HARNESS_SUMMARY_COLOR_SUCCESS'};
my $TEXT = "All tests successful.";
print $COL ? colored( $TEXT, $COL ) : $TEXT;
print "\n";
exit(0);

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2012 Shlomi Fish

=cut
