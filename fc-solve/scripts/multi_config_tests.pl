#!/usr/bin/perl

# The aim of this program is to run Freecell Solver’s test suite with
# several different  build-time configurations of the solver, to make sure
# they are all working fine.

use strict;
use warnings;
use autodie;

package Games::Solitaire::FC_Solve::Test::Trap::Obj;

use strict;
use warnings;

use Carp         ();
use Data::Dumper ();
use IO::Handle;

use Term::ANSIColor qw(colored);

use parent 'Games::Solitaire::Verify::Base';

my @fields = qw(
    die
    exit
    leaveby
    return
    stderr
    stdout
    wantarray
    warn
);

__PACKAGE__->mk_acc_ref( [@fields] );
__PACKAGE__->mk_acc_ref( [ 'blurb', 'cmd_line', ] );

use Text::Sprintf::Named ();

use Test::Trap
    qw( trap $trap :flow:stderr(systemsafe):stdout(systemsafe):warn );

sub _init
{
    my ( $self, $args ) = @_;

    %$self = ( %$self, %$args );

    return $self;
}

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

use strict;
use warnings;

use Cwd qw(getcwd);
use File::Path qw(rmtree);
use File::Spec ();

use Term::ANSIColor qw(colored);

local *run_cmd = \&Games::Solitaire::FC_Solve::Test::Trap::Obj::run_cmd;

my $test_index     = 1;
my $NUM_PROCESSORS = 4;

my $FALSE = 0;

# Inline::C/etc. leave some files and dirs under restrictive file permissions
# and we need to instruct rmtree to delete them as well.
my $SAFE = $FALSE;

my @tests;

sub reg_test
{
    push @tests, [@_];
}

sub _calc_build_path
{
    my ($idx) = @_;

    return File::Spec->rel2abs(
        File::Spec->catdir(
            File::Spec->curdir(), File::Spec->updir(), "build-$idx"
        )
    );
}

sub run_tests
{
    my ( $blurb_base_base, $args ) = @_;

    my $idx = $test_index++;
    my $blurb_base = sprintf "%s [ idx = %d / %d ]", $blurb_base_base, $idx,
        scalar(@tests);

    my $tatzer_args       = $args->{'tatzer_args'};
    my $cmake_args        = $args->{'cmake_args'};
    my $prepare_dist_args = $args->{'prepare_dist_args'};

    if ( 1 != grep { $_ } ( $tatzer_args, $cmake_args, $prepare_dist_args ) )
    {
        die
"One and only one of tatzer_args or cmake_args or prepare_dist_args must be specified.";
    }

    my $cwd        = getcwd();
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
        run_cmd(
            "$blurb_base : prepare dist",
            {
                cmd => [
                    $^X,
"../scripts/prepare-self-contained-dbm-etc-solvers-packages-for-hpc-machines/$prepare_dist_args->{base}",
                    @{ $prepare_dist_args->{args} }
                ],
            },
        );
        run_cmd( "$blurb_base : untar",
            { cmd => [ "tar", "-xvf", "dbm_fcs_for_sub.tar.xz" ], } );
        chdir('dbm_fcs_for_sub');
        run_cmd( "$blurb_base : make", { cmd => ['make'], } );
        chdir($cwd);
        rmtree( 'dbm_fcs_for_sub', 0, $SAFE );
        unlink('dbm_fcs_for_sub.tar.xz');
    }
    else
    {
        mkdir($build_path);
        chdir($build_path);
        if ($tatzer_args)
        {
            run_cmd( "$blurb_base : Tatzer",
                { cmd => [ '../source/Tatzer', @$tatzer_args ] } );
        }
        else
        {
            run_cmd( "$blurb_base : cmake",
                { cmd => [ 'cmake', @$cmake_args, '../source' ] } );
        }
        run_cmd( "$blurb_base : make",
            { cmd => [ 'make', "-j$NUM_PROCESSORS" ] } );
        if ( not $args->{do_not_test} )
        {
            run_cmd( "$blurb_base : test",
                { cmd => [ $^X, "$cwd/run-tests.pl" ] } );
        }

        chdir($cwd);
        rmtree( $build_path, 0, $SAFE );
    }

    return;
}

if ( $ENV{FC_SOLVE_GIT_CHECKOUT} )
{
    $ENV{LIBAVL2_SOURCE_DIR} = "$ENV{HOME}/Download/unpack/prog/c/avl-2.0.3/";

    run_cmd( 'git checkout', { cmd => [qw(git checkout master)], } );
    run_cmd( 'git pull', { cmd => [qw(git pull --ff-only origin master)], } );
}
elsif ( not exists $ENV{LIBAVL2_SOURCE_DIR} )
{
    if ( !-d "avl-2.0.3" )
    {
        run_cmd(
            'wget avl',
            {
                cmd => [qw(wget ftp://ftp.gnu.org/pub/gnu/avl/avl-2.0.3.tar.gz)]
            }
        );
        run_cmd( 'untar avl', { cmd => [qw(tar -xvf avl-2.0.3.tar.gz)] } );
        system(
q#find avl-2.0.3 -type f | xargs -d '\n' perl -i -lp -E 's/[\t ]+\z//'#
        );
    }
    $ENV{LIBAVL2_SOURCE_DIR} = getcwd() . "/avl-2.0.3";
}

# This is just to test that the reporting is working fine.
# run_cmd('false', {cmd => [qw(false)],});
# my $ARCH = 'x64';
my $ARCH = 'n2';

# Load the b or t suffixes.
my @LB = ( '-l', $ARCH . 'b' );
my @LT = ( '-l', $ARCH . 't' );

sub reg_tatzer_test
{
    my $blurb = shift;
    return reg_test( $blurb, { tatzer_args => [@_] } );
}

sub reg_lt_test
{
    my $blurb = shift;
    return reg_tatzer_test( $blurb, @LT, @_ );
}

sub reg_prep
{
    my ( $blurb, $base ) = @_;
    return reg_test( $blurb,
        { prepare_dist_args => { base => $base, args => [] } } );
}

reg_tatzer_test( "--fc-only wo break back compat", qw(--fc-only) );
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
reg_lt_test( "-l n2t with --disable-patsolve", '--disable-patsolve', );
reg_test(
    "build_only: maximum speed preset",
    {
        do_not_test => 1,
        tatzer_args => [ @LB, qw(-l extra_speed --disable-err-strs) ]
    }
);
reg_test(
    "build_only: no iter-handler",
    {
        do_not_test => 1,
        tatzer_args => [ @LB, qw(-l extra_speed --without-iter-handler) ]
    }
);
reg_test( "Plain CMake Default", { cmake_args => [], run_perltidy => 1, } );
reg_test( "Non-Debondt Delta States",
    { cmake_args => ['-DFCS_DISABLE_DEBONDT_DELTA_STATES=1'] } );
reg_tatzer_test( "Default", () );
reg_tatzer_test( "--rcs", qw(--rcs) );

reg_lt_test( "libavl2 with COMPACT_STATES",
    qw(--states-type=COMPACT_STATES --libavl2-p=prb) );

reg_lt_test(
    "libavl2 with COMPACT_STATES and --rcs",
    qw(--states-type=COMPACT_STATES --libavl2-p=prb --rcs),
);

reg_lt_test( "libavl2 with INDIRECT_STATES", qw(--libavl2-p=prb), );

reg_tatzer_test( "without-depth-field", qw(--without-depth-field) );
reg_tatzer_test( "without-depth-field and rcs",
    qw(--without-depth-field --rcs) );
reg_lt_test( "No FCS_SINGLE_HARD_THREAD", '--nosingle-ht' );

reg_lt_test( "Break Backward Compatibility #1", '--break-back-compat-1' );

reg_lt_test(
    "Freecell-only (as well as Break Backcompat)",
    qw(--break-back-compat-1 --fc-only),
);

{
    my @found =
        grep { -e $_ }
        map  { _calc_build_path( $test_index + $_ ); } keys @tests;
    if (@found)
    {
        die
"The following build dirs exist and interfere with the build - [ @found ]. Please remove them!";
    }
}

foreach my $run (@tests)
{
    run_tests(@$run);
}

print colored( "All tests successful.",
    ( $ENV{'HARNESS_SUMMARY_COLOR_SUCCESS'} || 'bold green' ) ),
    "\n";

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
