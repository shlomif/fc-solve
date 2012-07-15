#!/usr/bin/perl

# The aim of this program is to run Freecell Solver’s test suite with
# several different  build-time configurations of the solver, to make sure
# they are all working fine.

use strict;
use warnings;

package Games::Solitaire::FC_Solve::Test::Trap::Obj;

use strict;
use warnings;

use Carp;
use Data::Dumper;
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

__PACKAGE__->mk_acc_ref([@fields]);
__PACKAGE__->mk_acc_ref(['blurb', 'cmd_line', ]);

use Text::Sprintf::Named;

use Test::Trap qw( trap $trap :flow:stderr(systemsafe):stdout(systemsafe):warn );

sub _init
{
    my ($self, $args) = @_;

    %$self = (%$self, %$args);

    return $self;
}

sub _stringify_value
{
    my ($self, $name) = @_;

    my $value = $self->$name();

    if (($name eq "return") || ($name eq "warn"))
    {
        return Data::Dumper->new([$value])->Dump();
    }
    else
    {
        return (defined($value) ? $value : "");
    }
}

=head2 $trapper->diag_all()

Calls L<Test::More>'s diag() with alll the trapped fields, like stdout,
stderr, etc.

=cut

sub all_info
{
    my $self = shift;

    return Text::Sprintf::Named->new(
            {
                fmt =>
            join( "",
            map { "$_ ===\n{{{{{{\n%($_)s\n}}}}}}\n\n" }
            (@fields))
            }
        )->format({args => { map { my $name = $_;
                        ($name => $self->_stringify_value($name)) }
                    @fields
                }});
}

sub emit_all
{
    my $self = shift;

    print "Error with @{[$self->blurb()]} executing [@{$self->cmd_line()}].\n";
    print ($self->all_info());

    Carp::cluck("Error.");
    print colored("Error!", ($ENV{'HARNESS_SUMMARY_COL_FAIL'} || 'bold red')),
        "\n";
    exit(-1);
}

=head2 my $got = Test::Run::Trap::Obj->trap_run({class => $class, args => \@args, run_func => $func})

Runs C<$class->$func()> with the arguments @args placed into a hash-ref,
traps the results and returns a results object.

=cut

sub run_cmd
{
    my ($blurb, $args) = @_;

    my @cmd_line = @{$args->{'cmd'}};

    print "Running: {$blurb} @cmd_line\n";
    STDOUT->flush;

    my $error_code;

    trap { $error_code = (system {$cmd_line[0] } @cmd_line); };

    my $self = __PACKAGE__->new({
        ( map { $_ => $trap->$_() } @fields),
    });

    $self->blurb($blurb);
    $self->cmd_line([@cmd_line]);

    if ($error_code)
    {
        $self->emit_all();
    }

    return $self;
}

package main;

use strict;
use warnings;

use Cwd;
use File::Path qw(rmtree);

use Term::ANSIColor qw(colored);

local *run_cmd = \&Games::Solitaire::FC_Solve::Test::Trap::Obj::run_cmd;

sub run_tests
{
    my ($blurb_base, $args) = @_;

    my $tatzer_args = $args->{'tatzer_args'};

    my $cwd = getcwd();
    my $build_path = 'build';

    mkdir($build_path);
    chdir($build_path);

    run_cmd("$blurb_base : Tatzer", {cmd => ['../Tatzer', @$tatzer_args, '..']});
    run_cmd("$blurb_base : make", {cmd => [qw(make -j2)]});
    run_cmd("$blurb_base : test", {cmd => [$^X, "$cwd/run-tests.pl"]});

    chdir($cwd);
    rmtree($build_path, 0, 1);

    return;
}

local $ENV{LIBAVL2_SOURCE_DIR} = "$ENV{HOME}/Download/unpack/prog/c/avl-2.0.3/";

run_cmd('git checkout', {cmd => [qw(git checkout master)],});
run_cmd('git pull', {cmd => [qw(git pull --ff-only origin master)],});

# This is just to test that the reporting is working fine.
# run_cmd('false', {cmd => [qw(false)],});

run_tests("Default", { tatzer_args => [] });
run_tests("--rcs", { tatzer_args => [qw(--rcs)] });

run_tests("libavl2 with COMPACT_STATES",
    {
        tatzer_args =>
        [qw(-l x64t --states-type=COMPACT_STATES --libavl2-p=prb)]
    }
);

run_tests("libavl2 with COMPACT_STATES and --rcs",
    {
        tatzer_args =>
        [qw(-l x64t --states-type=COMPACT_STATES --libavl2-p=prb --rcs)]
    }
);

run_tests("libavl2 with INDIRECT_STATES",
    {
        tatzer_args =>
        [qw(-l x64t --libavl2-p=prb)]
    }
);

run_tests("without-depth-field",
    {
        tatzer_args =>
        [qw(--without-depth-field)],
    }
);

run_tests("without-depth-field and rcs",
    {
        tatzer_args =>
        [qw(--without-depth-field --rcs)],
    }
);

print colored("All tests successful.",
        ($ENV{'HARNESS_SUMMARY_COL_SUC'} || 'bold green')
    ), "\n";

exit(0);

=head1 COPYRIGHT & LICENSE

Copyright 2012 by Shlomi Fish

This program is distributed under the MIT (X11) License:
L<http://www.opensource.org/licenses/mit-license.php>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

=cut
