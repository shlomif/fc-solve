#!/usr/bin/env perl

use 5.014;
use strict;
use warnings;
use autodie;

use Getopt::Long qw/ GetOptions /;
use Env::Path ();
use Path::Tiny qw/ path /;
use File::Basename qw/ basename /;

my $bindir     = path(__FILE__)->parent;
my $abs_bindir = $bindir->absolute;

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

my $exit_success;

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
    exit( $exit_success ? 0 : $exit_code ? (-1) : 0 );
}

my $tests_glob = "*.{t.exe,py,t}";
my $exclude_re_s;

my @execute;
GetOptions(
    '--exclude-re=s' => \$exclude_re_s,
    '--execute|e=s'  => \@execute,
    '--exit0!'       => \$exit_success,
    '--glob=s'       => \$tests_glob,
    '--prove!'       => \$use_prove,
    '--jobs|j=n'     => \$num_jobs,
) or die "Wrong opts - $!";

sub myglob
{
    return glob( shift . "/$tests_glob" );
}

{
    my $fcs_path = Path::Tiny->cwd;
    local $ENV{FCS_PATH}                = $fcs_path;
    local $ENV{FCS_SRC_PATH}            = $abs_bindir;
    local $ENV{PYTHONDONTWRITEBYTECODE} = '1';

    local $ENV{FCS_TEST_TAGS} = join ' ',
        sort { $a cmp $b }
        ( path('.')->child(qw/t TAGS.txt/)->slurp_utf8 =~ /([a-zA-Z_0-9]+)/g );

    my $preset_dest     = path('.')->child( "t", "Presets" );
    my $preset_src      = $abs_bindir->child("Presets");
    my $pset_files_dest = $preset_dest->child("presets");
    my $pset_files_src  = $preset_src->child("presets");
    $pset_files_dest->mkpath;
    foreach my $f ( path($pset_files_src)->children(qr{\.sh\z}) )
    {
        $f->copy( $pset_files_dest->child( $f->basename ) );
    }

    my $files_dest_abs = $pset_files_dest->child("STUB.TXT")->absolute;

    $files_dest_abs =~ s{STUB\.TXT\z}{};

    my $testing_preset_rc = $preset_dest->child("presetrc")->absolute;
    $testing_preset_rc->spew_utf8(
        $fcs_path->child( "Presets", "presetrc" )->slurp_utf8 =~
            s{^(dir=)([^\n]*)}{$1$files_dest_abs}gmrs );

    local $ENV{FREECELL_SOLVER_PRESETRC} = $testing_preset_rc;
    local $ENV{FREECELL_SOLVER_QUIET}    = 1;
    Env::Path->PATH->Prepend(
        Path::Tiny->cwd->child("board_gen"),
        $abs_bindir->child( "t", "scripts" ),
    );

    my $IS_WIN = ( $^O eq "MSWin32" );
    Env::Path->CPATH->Prepend( $abs_bindir, );

    Env::Path->LD_LIBRARY_PATH->Prepend($fcs_path);
    if ($IS_WIN)
    {
        # For the shared objects.
        Env::Path->PATH->Append($fcs_path);
    }

    my $foo_lib_dir = $abs_bindir->child( "t", "lib" );
    foreach my $add_lib ( Env::Path->PERL5LIB(), Env::Path->PYTHONPATH() )
    {
        $add_lib->Append($foo_lib_dir);
    }

    my $get_config_fn = sub {
        my $basename = shift;

        return $bindir->child( "t", "config", $basename )->absolute;
    };

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

    # Put the valgrind tests last, because they take a long time.
    my @tests =
        sort {
        ( ( ( $a =~ /valgrind/ ) <=> ( $b =~ /valgrind/ ) ) *
                ( _is_parallized() ? -1 : 1 ) )
            || ( basename($a) cmp basename($b) )
            || ( $a cmp $b )
        } (
        myglob('t'),
        myglob('.'),
        (
              ( $fcs_path ne $abs_bindir )
            ? ( myglob("$abs_bindir/t/t") )
            : ()
        ),
        );

    if ( defined($exclude_re_s) )
    {
        my $re = qr/$exclude_re_s/ms;
        @tests = grep { basename($_) !~ $re } @tests;
    }
    @tests = grep { basename($_) !~ /\A(?:lextab|yacctab)\.py\z/ } @tests;

    if ( !$ENV{FCS_TEST_BUILD} )
    {
        @tests = grep { !/build-process/ } @tests;
    }

    if ( $ENV{FCS_TEST_WITHOUT_VALGRIND} )
    {
        @tests = grep { !/valgrind/ } @tests;
    }

    print STDERR <<"EOF";
FCS_PATH = $ENV{FCS_PATH}
FCS_SRC_PATH = $ENV{FCS_SRC_PATH}
FCS_TEST_TAGS = <$ENV{FCS_TEST_TAGS}>
EOF

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

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2000 Shlomi Fish

=cut
