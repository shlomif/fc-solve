#!/usr/bin/perl

use strict;
use warnings;
use autodie;

use Cwd        ();
use File::Spec ();
use File::Copy qw/ copy /;
use File::Path qw/ mkpath /;
use Getopt::Long qw/ GetOptions /;
use Env::Path ();
use Path::Tiny qw/ path /;
use File::Basename qw/ basename dirname /;

my $bindir     = dirname(__FILE__);
my $abs_bindir = File::Spec->rel2abs($bindir);

# Whether to use prove instead of runprove.
my $use_prove = $ENV{FCS_USE_TEST_RUN} ? 0 : 1;
my $num_jobs = $ENV{TEST_JOBS};

sub _is_parallized
{
    return ( $use_prove && $num_jobs );
}

sub _calc_prove
{
    return [ 'prove',
        ( defined($num_jobs) ? sprintf( "-j%d", $num_jobs ) : () ) ];
}

sub run_tests
{
    my $tests = shift;

    my @cmd = ( ( $use_prove ? @{ _calc_prove() } : 'runprove' ), @$tests );
    if ( $ENV{RUN_TESTS_VERBOSE} )
    {
        print "Running [@cmd]\n";
    }

    # Workaround for Windows spawning-SNAFU.
    my $exit_code = system(@cmd);
    exit( $exit_code ? (-1) : 0 );
}

my $tests_glob = "*.{t.exe,py,t}";
my $exclude_re_s;

my @execute;
GetOptions(
    '--exclude-re=s' => \$exclude_re_s,
    '--execute|e=s'  => \@execute,
    '--glob=s'       => \$tests_glob,
    '--prove!'       => \$use_prove,
    '--jobs|j=n'     => \$num_jobs,
) or die "--glob='tests_glob'";

{
    my $fcs_path = Cwd::getcwd();
    local $ENV{FCS_PATH}     = $fcs_path;
    local $ENV{FCS_SRC_PATH} = $abs_bindir;

    local $ENV{FCS_TEST_TAGS} = join ' ',
        sort { $a cmp $b }
        ( path( File::Spec->catdir( File::Spec->curdir(), "t", "TAGS.txt" ) )
            ->slurp_utf8 =~ /([a-zA-Z_0-9]+)/g );

    my $testing_preset_rc;
    {
        my $preset_dest =
            File::Spec->catdir( File::Spec->curdir(), "t", "Presets" );
        my $preset_src = File::Spec->catfile( $abs_bindir, "Presets" );
        my $pset_files_dest = File::Spec->catdir( $preset_dest, "presets" );
        my $pset_files_src  = File::Spec->catdir( $preset_src,  "presets" );
        mkpath($pset_files_dest);
        opendir( my $d, $pset_files_src );
        while ( defined( my $f = readdir($d) ) )
        {

            if ( $f =~ m{\.sh\z} )
            {
                copy(
                    File::Spec->catfile( $pset_files_src,  $f ),
                    File::Spec->catfile( $pset_files_dest, $f )
                );
            }
        }
        closedir($d);

        my $files_dest_abs =
            File::Spec->rel2abs(
            File::Spec->catfile( $pset_files_dest, "STUB.TXT" ) );

        $files_dest_abs =~ s{STUB\.TXT\z}{};

        $testing_preset_rc =
            File::Spec->rel2abs(
            File::Spec->catfile( $preset_dest, "presetrc" ) );

        open my $in, "<",
            File::Spec->catfile( $fcs_path, "Presets", "presetrc" );
        open my $out, ">", $testing_preset_rc;
        while ( my $line = <$in> )
        {
            $line =~ s{\A(dir=)(.*)}{$1$files_dest_abs\n}ms;
            print {$out} $line;
        }
        close($in);
        close($out);
    }

    local $ENV{FREECELL_SOLVER_PRESETRC} = $testing_preset_rc;
    local $ENV{FREECELL_SOLVER_QUIET}    = 1;
    Env::Path->PATH->Prepend(
        File::Spec->catdir( Cwd::getcwd(), "board_gen" ),
        File::Spec->catdir( $abs_bindir, "t", "scripts" ),
    );

    Env::Path->CPATH->Prepend( $abs_bindir, );

    Env::Path->LD_LIBRARY_PATH->Prepend($fcs_path);

    my $foo_lib_dir = File::Spec->catdir( $abs_bindir, "t", "lib" );
    foreach my $add_lib ( Env::Path->PERL5LIB(), Env::Path->PYTHONPATH() )
    {
        $add_lib->Append($foo_lib_dir);
    }

    my $get_config_fn = sub {
        my $basename = shift;

        return File::Spec->rel2abs(
            File::Spec->catdir( $bindir, "t", "config", $basename ),
        );
    };
    my $IS_WIN = ( $^O eq "MSWin32" );

    local $ENV{HARNESS_ALT_INTRP_FILE} = $get_config_fn->(
        $IS_WIN
        ? "alternate-interpreters--mswin.yml"
        : "alternate-interpreters.yml"
    );

    local $ENV{HARNESS_TRIM_FNS} = 'keep:1';

    local $ENV{HARNESS_PLUGINS} = join(
        ' ', qw(
            BreakOnFailure ColorSummary ColorFileVerdicts AlternateInterpreters
            TrimDisplayedFilenames
            )
    );

    my $is_ninja = ( -e "build.ninja" );
    my $MAKE = $IS_WIN ? 'gmake' : 'make';
    if ($is_ninja)
    {
        system( "ninja", "boards" );
    }
    else
    {
        if ( system( $MAKE, "-s", "boards" ) )
        {
            die "$MAKE failed";
        }
    }

    chdir("t");

    if ( !$is_ninja )
    {
        if ( system( $MAKE, "-s" ) )
        {
            die "$MAKE failed";
        }
    }

# Cancelled because it is done by the build system.
# if (! glob('t/valgrind--*.t'))
# {
# my $start = time();
# system($^X, "$abs_bindir/scripts/generate-individual-valgrind-test-scripts.pl");
# my $end = time();
# print "StartGen=$start\nEndGen=$end\n";
# }

    # Put the valgrind tests last, because they take a long time.
    my @tests =
        sort {
        ( ( ( $a =~ /valgrind/ ) <=> ( $b =~ /valgrind/ ) ) *
                ( _is_parallized() ? -1 : 1 ) )
            || ( basename($a) cmp basename($b) )
            || ( $a cmp $b )
        } (
        glob("t/$tests_glob"),
        glob("./$tests_glob"),
        (
              ( $fcs_path ne $abs_bindir )
            ? ( glob("$abs_bindir/t/t/$tests_glob") )
            : ()
        ),
        );

    if ( defined($exclude_re_s) )
    {
        my $re = qr/$exclude_re_s/ms;
        @tests = grep { basename($_) !~ $re } @tests;
    }

    if ( !$ENV{FCS_TEST_BUILD} )
    {
        @tests = grep { !/build-process/ } @tests;
    }

    if ( $ENV{FCS_TEST_WITHOUT_VALGRIND} )
    {
        @tests = grep { !/valgrind/ } @tests;
    }

    {
        print STDERR "FCS_PATH = $ENV{FCS_PATH}\n";
        print STDERR "FCS_SRC_PATH = $ENV{FCS_SRC_PATH}\n";
        print STDERR "FCS_TEST_TAGS = <$ENV{FCS_TEST_TAGS}>\n";
        if ( $ENV{FCS_TEST_SHELL} )
        {
            system("bash");
        }
        elsif (@execute)
        {
            system(@execute);
        }
        else
        {
            run_tests( \@tests );
        }
    }
}

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2000 Shlomi Fish

=cut
