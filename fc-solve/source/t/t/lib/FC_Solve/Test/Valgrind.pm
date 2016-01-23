package FC_Solve::Test::Valgrind;

use strict;
use warnings;

use Test::More ();
use Carp ();
use File::Spec ();
use File::Temp qw( tempdir );

sub _expand_catfile_arg
{
    my $hash_ref = shift;

    my $type = $hash_ref->{type};

    if ($type eq 'ENV')
    {
        return $ENV{ $hash_ref->{arg} };
    }
    else
    {
        Carp::confess("Unknown catfile arg type '$type'!");
    }
}

sub _expand_arg
{
    my $hash_ref = shift;

    my $type = $hash_ref->{type};

    if ($type eq 'tempdir')
    {
        return tempdir(CLEANUP => 1),
    }
    elsif ($type eq 'catfile')
    {
        my $prefix = exists($hash_ref->{prefix}) ? $hash_ref->{prefix} : '';
        return $prefix . File::Spec->catfile(
            map { (ref($_) eq 'HASH') ? _expand_catfile_arg($_) : $_ }
            @{$hash_ref->{args}}
        );
    }
    else
    {
        Carp::confess("Unknown arg type '$type'!");
    }
}

sub _run_test
{
    my ($id, $args) = @_;

    if (ref($args) ne "HASH")
    {
        Carp::confess("args must be a HASH (wrong ID)?.");
    }

    # $args = { argv => $args, prog => "freecell-solver-range-parallel-solve", };

    my $log_fn = "valgrind--$id.log";

    if (!defined($id))
    {
        Carp::confess("Missing id");
    }

    my $cmd_line_args = [
        map { (ref($_) eq 'HASH') ? _expand_arg($_) : $_ } @{$args->{argv}}
    ];

    my $prog = $args->{prog};

    my $blurb = $args->{blurb};
    if ((!defined($blurb)) or ref($blurb) ne '' or length($blurb) == 0)
    {
        Carp::confess("Blurb is wrong.");
    }

    system(
        "valgrind",
        "--track-origins=yes",
        "--leak-check=yes",
        "--log-file=$log_fn",
        $ENV{'FCS_PATH'} . "/$prog",
        @$cmd_line_args,
    );

    open my $read_from_valgrind, "<", $log_fn
        or die "Cannot open valgrind.log for reading";
    my $found_error_summary = 0;
    my $found_malloc_free = 0;
    LINES_LOOP:
    while (my $l = <$read_from_valgrind>)
    {
        if (index($l, q{ERROR SUMMARY: 0 errors from 0 contexts}) >= 0)
        {
            $found_error_summary = 1;
        }
        elsif (index($l, q{in use at exit: 0 bytes}) >= 0)
        {
            $found_malloc_free = 1;
        }
    }
    close ($read_from_valgrind);

    if (Test::More::ok (($found_error_summary && $found_malloc_free), $blurb))
    {
        unlink($log_fn);
    }
    else
    {
        die "Valgrind failed";
    }
}

sub run_id
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my (undef, $args) = @_;

    return _run_test($args->{id}, $args->{data});
}

1;

=head1 COPYRIGHT AND LICENSE

Copyright (c) 2009 Shlomi Fish

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

